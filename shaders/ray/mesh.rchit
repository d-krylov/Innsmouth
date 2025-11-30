#version 460
#extension GL_EXT_ray_tracing : require
#extension GL_GOOGLE_include_directive : require

#include "common.glsl"

layout(location = 0) rayPayloadInEXT Hit payload;

hitAttributeEXT vec2 barycentric;

void main() {
  payload.color = vec4(barycentric, 1 - barycentric.x - barycentric.y, 1.0);
}