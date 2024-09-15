#version 460

// IN
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_uv;

// OUT
layout (location = 0) out vec4 out_color;

void main() {
  vec3 color = vec3(0.5);

  vec3 L_direction = normalize(vec3(1.0, 1.0, 1.0));

  float NdotL = max(dot(in_normal, L_direction), 0.0);

  color *= NdotL;

  out_color = vec4(color, 1.0);
}