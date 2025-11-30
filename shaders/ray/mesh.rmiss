#version 460
#extension GL_EXT_ray_tracing : require
#extension GL_GOOGLE_include_directive : require

#include "common.glsl"

layout(location = 0) rayPayloadInEXT Hit payload;

void main() {
  payload.color = vec4(0.6, 0.1, 0.1, 1.0);
}