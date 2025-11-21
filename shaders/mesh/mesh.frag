#version 460

#extension GL_EXT_nonuniform_qualifier : require
#extension GL_EXT_ray_query : require

// INPUT
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_uv;
layout (location = 3) in flat uint in_draw_id;

// OUTPUT
layout (location = 0) out vec4 out_color;

layout (binding = 1, set = 0) uniform accelerationStructureEXT tlas;

void main() {

  vec3 flash = vec3(0.0, 900.0, 0.0);
  vec3 sun = normalize(flash - in_position);
  float steps = distance(flash, in_position);

  float NdotL = clamp(dot(in_normal, sun), 0.0, 1.0);

  rayQueryEXT rq;
	rayQueryInitializeEXT(rq, tlas, gl_RayFlagsTerminateOnFirstHitEXT, 0xff, in_position, 0.1f, sun, steps + 1);
  rayQueryProceedEXT(rq);
  
  bool hit = rayQueryGetIntersectionTypeEXT(rq, true) == gl_RayQueryCommittedIntersectionNoneEXT;
  NdotL *= hit ? 1.0 : 0.01;

  vec3 color = vec3(0.2) + vec3(0.4, 0.4, 0.1) * NdotL;

  out_color = vec4(color, 1.0);

}