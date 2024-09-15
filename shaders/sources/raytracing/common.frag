#ifndef COMMON_FRAG
#define COMMON_FRAG

#define STEP_SIZE      0.5
#define EPSILON        0.001
#define MAXIMUM_STEPS  5000
#define FAR            100.0

struct Ray {
  vec3 origin;
  vec3 direction;
};

struct Hit {
  vec3 position;
  vec3 normal;
  float id;
  float t;
};

#endif