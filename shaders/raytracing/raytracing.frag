#version 460 core

#include "common.frag"
#include "sd.frag"

// IN
layout (location = 0) in vec2 iv_uv;

// OUT
layout (location = 0) out vec4 out_color;

// UNFORM
layout(set = 0, binding = 0) uniform sampler2D textures[3];

// PUSH CONSTANTS
layout (push_constant) uniform u_inputs {
  vec2 i_mouse;
  vec2 i_resolution; 
  float i_time;
};

vec2 scene(vec3 p) {
  vec2 q = (i_mouse - i_resolution.xy) / i_resolution.xy;
  q.y *= i_resolution.y / i_resolution.x;

  float ret = sdRoundBox(p, vec3(0.5, 0.5, 0.5), 0.0);

  return vec2(ret, 0);
}

vec3 getNormal(in vec3 p) {
  const float h = 0.01; 
  const vec2 k = vec2(1.0, -1.0);
  return normalize(k.xyy * scene(p + k.xyy * h).x + 
                   k.yyx * scene(p + k.yyx * h).x + 
                   k.yxy * scene(p + k.yxy * h).x + 
                   k.xxx * scene(p + k.xxx * h).x);
}

Hit trace(in Ray ray, float near) {
  Hit hit;
  hit.id = -1.0;
  float t = near;
  for (int i = 0; i < MAXIMUM_STEPS; i++) {
    vec3 p = ray.origin + ray.direction * t;
    vec2 d = scene(p);
    if (d.x < EPSILON) {
      hit.id = d.y;
      hit.position = p; hit.normal = getNormal(hit.position);
      break;
    }
    t += STEP_SIZE * d.x;
    if (t > FAR) { break; }
  }
  return hit;
}

void main() {
  vec2 uv = (2.0 * gl_FragCoord.xy - i_resolution.xy) / i_resolution.xy;
  
  uv.y *= i_resolution.y / i_resolution.x;

  Ray ray;
  ray.origin = vec3(0.0, 0.0, 4.0);
  ray.direction = vec3(normalize(vec3(uv, -1.0)));
  
  vec3 color = vec3(0.0, 0.0, 0.0);

  Hit hit = trace(ray, 0.0);

  if (hit.id != -1.0) {
    vec2 texture_uv = hit.position.xy * abs(hit.normal.z) +
                      hit.position.xz * abs(hit.normal.y) +
                      hit.position.yz * abs(hit.normal.x);
    color = texture(textures[2], texture_uv).xyz;
  }


  out_color = vec4(color, 1.0);
}