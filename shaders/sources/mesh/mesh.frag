#version 460

#include "common.shader"

// IN
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_uv;

// OUT
layout (location = 0) out vec4 out_color;

layout (binding = 0) uniform sampler2D u_ambient_map;
layout (binding = 1) uniform sampler2D u_diffuse_map;
layout (binding = 2) uniform sampler2D u_specular_map;

layout (set = 0, binding = 3) uniform UBO {
  PointLight point_light;
  vec4 camera_position;
} ubo;

vec3 GetPointLight(Material material, PointLight point_light, vec3 camera_position, vec3 fragment_position) {
  vec3 L_direction = normalize(point_light.position.xyz - fragment_position);
  vec3 V_direction = normalize(camera_position      - fragment_position);
  vec3 H_direction = normalize(L_direction + V_direction);
  float D = distance(point_light.position.xyz, fragment_position);
  float attenuation = 1.0 / dot(point_light.attenuation.xyz, vec3(D * D, D, 1.0));  
  float NdotL = max(dot(in_normal, L_direction), 0.0);
  float NdotH = max(dot(in_normal, H_direction), 0.0);
  vec3 diffuse  = point_light.color.rgb * NdotL * material.diffuse.rgb;   
  vec3 specular = point_light.color.rgb * pow(NdotH, material.shininess) * material.specular.rgb;
  return (material.ambient.rgb + diffuse + specular) * attenuation;
}

void main() {

  vec3 color = vec3(0.0);

  Material material;

  material.ambient  = texture(u_ambient_map,  in_uv);
  material.diffuse  = texture(u_diffuse_map,  in_uv);
  material.specular = texture(u_specular_map,  in_uv);

  material.shininess = 32.0;

  color += GetPointLight(material, ubo.point_light, ubo.camera_position.xyz, in_position);

  out_color = vec4(color, 1.0);
}