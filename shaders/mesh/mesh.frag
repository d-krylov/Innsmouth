#version 460

// INPUT
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_uv;
layout (location = 3) in flat uint in_draw_id;

// OUTPUT
layout (location = 0) out vec4 out_color;

void main() {

  vec3 sun = normalize(vec3(1.0));
  float NdotL = clamp(dot(in_normal, sun), 0.0, 1.0);

  vec3 color = vec3(0.5) + vec3(0.4, 0.4, 0.1) * NdotL;

  out_color = vec4(color, 1.0);

}