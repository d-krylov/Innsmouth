#version 460

// IN
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_uv;

// PUSH CONSTANTS
layout (push_constant) uniform Transformations {
  mat4 projection;
  mat4 view;
  mat4 model;
} transformations;

layout (location = 0) out vec3 out_position;
layout (location = 1) out vec3 out_normal;
layout (location = 2) out vec2 out_uv;

out gl_PerVertex { 
  vec4 gl_Position; 
};

void main() {
  out_normal = normalize(mat3(transpose(inverse(transformations.model))) * in_normal);
  out_position = vec3(transformations.model * vec4(in_position, 1.0));
  gl_Position = transformations.projection * 
                transformations.view *
                transformations.model * vec4(in_position, 1.0);

  gl_Position.y = -gl_Position.y;	
}