#version 460

// OUT
layout (location = 0) out vec2 out_uv;

out gl_PerVertex {
  vec4 gl_Position;
};

void main() {
  vec2 position = vec2(gl_VertexIndex % 2, gl_VertexIndex / 2) * 4.0 - 1;

  out_uv = vec2((gl_VertexIndex << 1) & 2, 
                (gl_VertexIndex << 0) & 2);
  
  gl_Position = vec4(position, 0.0, 1.0);
}