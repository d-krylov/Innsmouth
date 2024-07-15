#version 450

layout (location = 0) in vec3 in_normal;
layout (location = 1) in vec2 in_uv;
layout (location = 2) in vec3 in_fragment_position;

layout (location = 0) out vec4 out_color;

layout (binding = 0) uniform sampler2D u_ambient_texture;
layout (binding = 1) uniform sampler2D u_diffuse_texture;

void main() {

  vec3 light_ambient = vec3(0.2);
  vec3 light_diffuse = vec3(0.3, 0.6, 0.2);
  vec3 light_direction = normalize(vec3(1.0));

  vec3 ambient = light_ambient * texture(u_diffuse_texture, in_uv).rgb;

  float kd = max(dot(in_normal, light_direction), 0.0);

  vec3 diffuse = light_diffuse * kd * texture(u_diffuse_texture, in_uv).rgb;  
    
  vec3 color = ambient + diffuse;

  out_color = vec4(color, 1.0);
}