#ifndef COMMON_SHADER
#define COMMON_SHADER

struct Material {
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;
  float shininess;
};

struct PointLight {
  vec4 position;
  vec4 color;
  vec4 attenuation;
};

#endif // COMMON_SHADER