#version 460

// IN
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_uv;

// PUSH CONSTANTS
layout (push_constant) uniform MVP {
  mat4 projection_matrix;
  mat4 view_matrix;
  mat4 model_matrix;
} pc_mvp;

layout (location = 0) out vec3 out_position;
layout (location = 1) out vec3 out_normal;
layout (location = 2) out vec2 out_uv;


void main() {
  out_uv = in_uv;
  out_normal = normalize(mat3(transpose(inverse(pc_mvp.model_matrix))) * in_normal);
  out_position = vec3(pc_mvp.model_matrix * vec4(in_position, 1.0));
  gl_Position = pc_mvp.projection_matrix * 
                pc_mvp.view_matrix * 
                pc_mvp.model_matrix * vec4(in_position, 1.0);

  gl_Position.y = -gl_Position.y;	
}