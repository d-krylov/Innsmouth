#version 460

struct Light {
  vec4 position;
  vec4 color;
  vec4 attenuation;
};

struct Material {
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;
  float shininess;
};

// IN
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_uv;

// OUT
layout (location = 0) out vec4 out_color;

layout (set = 0, binding = 0) uniform Scene {
  Light light;
  vec4 camera_position;
} scene;

vec3 GetPointLight(Material material, Light light, vec3 camera_position, vec3 fragment_position) {
  vec3 L_direction = normalize(light.position.xyz - fragment_position);
  vec3 V_direction = normalize(camera_position - fragment_position);
  vec3 H_direction = normalize(L_direction + V_direction);
  float D = distance(light.position.xyz, fragment_position);
  float attenuation = 1.0 / dot(light.attenuation.rgb, vec3(D * D, D, 1.0));  
  float NdotL = max(dot(in_normal, L_direction), 0.0);
  float NdotH = max(dot(in_normal, H_direction), 0.0);
  vec3 diffuse  = light.color.rgb * NdotL * material.diffuse.rgb;   
  vec3 specular = light.color.rgb * pow(NdotH, material.shininess) * material.specular.rgb;
  return (material.ambient.rgb + diffuse.rgb + specular.rgb) * attenuation;
}

void main() {
  vec3 color = vec3(0.0);

  Material material;

  material.ambient  = vec4(0.5, 0.1, 0.1, 0.0);
  material.diffuse  = vec4(0.1, 0.5, 0.1, 0.0);
  material.specular = vec4(0.1, 0.1, 0.5, 0.0);
  material.shininess = 32.0;

  color += GetPointLight(material, scene.light, scene.camera_position.rgb, in_position);

  out_color = vec4(color, 1.0);
}