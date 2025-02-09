#version 460

#include "../common/light.frag"

// IN
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_uv;

// OUT
layout (location = 0) out vec4 out_color;

layout (set = 0, binding = 0) uniform UBO {
	vec3 view_position;
} scene;

layout (set = 0, binding = 1) uniform sampler2D u_albedo;
//layout (set = 1, binding = 1) uniform sampler2D u_diffuse;
//layout (set = 1, binding = 2) uniform sampler2D u_specular;

void main() {

  vec4 material_color = texture(u_albedo, in_uv);
  vec3 light_color = vec3(0.6);
  vec3 L = vec3(0.0, 1.0, 0.0);

  vec3 color = phong(material_color.rgb, light_color, in_normal, L, scene.view_position, in_position);

  out_color = vec4(color, material_color.a);
}