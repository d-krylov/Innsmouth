#ifndef COMMON_SHADER
#define COMMON_SHADER

struct Material {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float shininess;
};

struct PointLight {
  vec3 position;
  vec3 color;
  vec3 attenuation;
};

#endif // COMMON_SHADER