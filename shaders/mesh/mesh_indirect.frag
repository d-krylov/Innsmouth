#version 460

#extension GL_EXT_nonuniform_qualifier : require
#extension GL_EXT_ray_query : require

struct Primitive {
  int color_texture_index;
  int normal_texture_index;
  uint vertices_offset;
  uint indices_offset;
  uint indices_size;
};

layout (binding = 1, set = 0) uniform accelerationStructureEXT tlas;

layout (binding = 2, set = 0) readonly buffer Primitives {
	Primitive primitives[];
};

layout (binding = 0, set = 1) uniform sampler2D textures[];

// INPUT
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_uv;
layout (location = 3) in flat uint in_draw_id;

// OUTPUT
layout (location = 0) out vec4 out_color;

void main() {

  Primitive primitive = primitives[nonuniformEXT(in_draw_id)];

  vec4 ambient = texture(textures[primitive.color_texture_index], in_uv);

  vec3 normal_texture = vec3(0.0);

  vec3 normal = in_normal;

  vec3 flash = vec3(0.0, 500.0, 0.0);
  vec3 sun = normalize(flash - in_position);
  float steps = distance(flash, in_position);

  rayQueryEXT rq;
	rayQueryInitializeEXT(rq, tlas, gl_RayFlagsTerminateOnFirstHitEXT, 0xff, in_position, 0.1f, sun, steps + 1.0);
	rayQueryProceedEXT(rq);

  float NdotL = clamp(dot(sun, normal), 0.0, 1.0);
  bool hit = rayQueryGetIntersectionTypeEXT(rq, true) == gl_RayQueryCommittedIntersectionNoneEXT;
  NdotL *= hit ? 1.0 : 0.01;

  vec3 color = 0.5 * ambient.rgb + vec3(0.4, 0.4, 0.1) * NdotL;

  out_color = vec4(color, ambient.a);
}