#version 460 core

layout(location = 0) out vec4 fragColor;

layout(set = 0, binding = 0) uniform sampler2D u_texture;

layout (location = 0) in struct { 
  vec4 color; 
  vec2 uv; 
} in_vertex;

void main() {
  fragColor = in_vertex.color * texture(u_texture, in_vertex.uv.st);
}