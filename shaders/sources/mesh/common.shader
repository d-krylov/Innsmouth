#ifndef COMMON_SHADER
#define COMMON_SHADER

struct Material {
  float shininess;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

struct PointLight {
  vec3 position;
  vec3 color;
  vec3 attenuation;
};

#endif // COMMON_SHADER