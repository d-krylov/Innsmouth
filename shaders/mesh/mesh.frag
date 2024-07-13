#version 450

layout (location = 0) in vec3 in_normal;
layout (location = 1) in vec2 in_uv;
layout (location = 2) in vec3 in_fragment_position;

layout (location = 0) out vec4 out_color;

layout (binding = 0) uniform sampler2D texture_sampler;

void main() {

  vec3 material_ambient = vec3(0.5);
  vec3 material_diffuse = vec3(0.5);
  vec3 light_ambient = vec3(0.3, 0.6, 0.5);

  vec3 ambient = light_ambient * material_ambient;

  // diffuse 
  vec3 light_direction = normalize(vec3(1.0));

  float NdotL = max(dot(in_normal, light_direction), 0.0);

  vec3 diffuse = light_ambient * NdotL * material_diffuse;  

  vec3 color = ambient + diffuse;

  out_color = vec4(color, 1.0);
}