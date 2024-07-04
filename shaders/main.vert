#version 450

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_uv;

layout (push_constant) uniform constants {
  mat4 u_projection_matrix;
  mat4 u_view_matrix;
  mat4 u_model_matrix;
} PushConstants;

layout (location = 0) out vec3 out_normal;
layout (location = 1) out vec2 out_uv;
layout (location = 2) out vec3 out_fragment_position;

void main() {
  out_uv = in_uv;
  out_normal = normalize(mat3(transpose(inverse(PushConstants.u_model_matrix))) * in_normal);
  out_fragment_position = vec3(PushConstants.u_model_matrix * vec4(in_position, 1.0));
  gl_Position = PushConstants.u_projection_matrix * 
                PushConstants.u_view_matrix * 
                PushConstants.u_model_matrix * vec4(in_position, 1.0);
}