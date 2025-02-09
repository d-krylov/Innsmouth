#version 460

#include "../common/light.frag"

// IN
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;

// OUT
layout (location = 0) out vec4 out_color;

layout (set = 0, binding = 0) uniform UBO {
	vec3 view_position;
} scene;

void main() {

  vec3 material_color = vec3(0.3, 0.8, 0.4);
  vec3 light_color = vec3(0.6);
  vec3 L = vec3(0.0, 1.0, 0.0);

  vec3 color = phong(material_color, light_color, in_normal, L, scene.view_position, in_position);

  out_color = vec4(color, 1.0);
}