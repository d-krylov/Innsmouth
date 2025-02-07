#version 460

// IN
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;

// OUT
layout (location = 0) out vec4 out_color;



void main() {

  vec3 color = vec3(0.5);



  out_color = vec4(color, 1.0);
}