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

void main() {
  out_vertex.color = in_color;
  out_vertex.uv = in_uv;
  gl_Position = vec4(in_position * matrices.scale + matrices.translate, 0, 1);
}