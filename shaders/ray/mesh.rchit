#version 460
#extension GL_EXT_ray_tracing : require
#extension GL_GOOGLE_include_directive : require

#include "common.glsl"

layout (binding = 2, set = 0) readonly buffer Vertices {
	Vertex vertices[];
};

layout (binding = 3, set = 0) readonly buffer Indices {
	uint indices[];
};

layout(location = 0) rayPayloadInEXT Hit payload;

hitAttributeEXT vec2 barycentric;

void main() {

  uint triangle_index = gl_PrimitiveID * 3;

  uint i0 = indices[triangle_index + 0];
  uint i1 = indices[triangle_index + 1];
  uint i2 = indices[triangle_index + 2];

  Vertex v0 = vertices[i0];
  Vertex v1 = vertices[i1];
  Vertex v2 = vertices[i2];

  vec3 n0 = vec3(v0.nx, v0.ny, v0.nz);
  vec3 n1 = vec3(v1.nx, v1.ny, v1.nz);
  vec3 n2 = vec3(v2.nx, v2.ny, v2.nz);

  vec3 p0 = vec3(v0.px, v0.py, v0.pz);
  vec3 p1 = vec3(v1.px, v1.py, v1.pz);
  vec3 p2 = vec3(v2.px, v2.py, v2.pz);

  vec3 position = (1.0 - barycentric.x - barycentric.y) * p0 + barycentric.x * p1 + barycentric.y * p2;
  vec3 normal = normalize((1.0 - barycentric.x - barycentric.y) * n0 + barycentric.x * n1 + barycentric.y * n2);

  vec3 sun = normalize(vec3(1.0, 1.0, 0.0));

  float NdotL = clamp(dot(sun, normal), 0.0, 1.0);

  vec3 color = vec3(0.3) + NdotL * vec3(0.4, 0.4, 0.1);

  payload.color = vec4(color, 1.0);
}