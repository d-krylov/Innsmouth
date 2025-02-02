#version 460 core

// IN
layout (location = 0) in vec2 iv_uv;

// OUT
layout (location = 0) out vec4 out_color;


void main() {
  //vec2 uv = (2.0 * gl_FragCoord.xy - i_resolution.xy) / i_resolution.x;
  
  vec3 color = vec3(0.5, 0.2, 0.1);

  out_color = vec4(color, 1.0);
}