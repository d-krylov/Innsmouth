#version 460

// OUT
layout (location = 0) out vec2 out_uv;

out gl_PerVertex {
  vec4 gl_Position;
};

void main() {
  int vertex_index = gl_VertexIndex;

  float position_x = vertex_index % 2;
  float position_y = vertex_index / 2;

  vec2 position = vec2(position_x, position_y) * 4.0 - 1;

  float uv_x = (vertex_index << 1) & 2;
  float uv_y = (vertex_index << 0) & 2;

  out_uv = vec2(uv_x, uv_y);
  
  gl_Position = vec4(position, 0.0, 1.0);
}