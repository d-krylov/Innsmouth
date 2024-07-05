#version 450

layout (location = 0) in vec3 in_normal;
layout (location = 1) in vec2 in_uv;
layout (location = 2) in vec3 in_fragment_position;

layout (location = 0) out vec4 out_color;

layout (binding = 0) uniform sampler2D texture_sampler;

void main() {
  //out_color = vec4(0.2, 0.3, 0.4, 1.0);

  out_color = texture(texture_sampler, in_uv);
}