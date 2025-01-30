#version 460 core

layout (location = 0) in vec2 in_position;
layout (location = 1) in vec2 in_uv;
layout (location = 2) in vec4 in_color;

layout (push_constant) uniform u_push_constant { 
  vec2 scale; 
  vec2 translate;
} matrices;

out gl_PerVertex { 
  vec4 gl_Position; 
};

layout (location = 0) out struct { 
  vec4 color; 
  vec2 uv; 
} out_vertex;

// TEST

layout(set = 3, binding = 4) uniform B1 {
  vec4 v;
} b1;

layout(set = 2, binding = 2) uniform B2 {
  vec4 v;
} b2;

layout(set = 4, binding = 6) uniform B3 {
  vec4 v;
} b3;

layout(set = 5, binding = 8) uniform B4 {
  vec4 v;
} b4;

layout(set = 1, binding = 10) uniform B5 {
  vec4 v;
} b5;


void main() {
  out_vertex.color = in_color;
  out_vertex.uv = in_uv;
  gl_Position = vec4(in_position * matrices.scale + matrices.translate, 0, 1);
}