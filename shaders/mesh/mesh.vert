#version 460
#extension GL_ARB_shader_draw_parameters: require

layout (push_constant) uniform PushConstants {
	mat4 projection;
	mat4 view;
  mat4 model;
} pc;

struct Vertex {
	float px, py, pz;
	float nx, ny, nz;
	float uvx, uvy;
};

layout (binding = 0, set = 0) readonly buffer Vertices {
	Vertex vertices[];
};

// OUTPUT
layout (location = 0) out vec3 out_position;
layout (location = 1) out vec3 out_normal;
layout (location = 2) out vec2 out_uv;
layout (location = 3) out flat uint out_drawid; 

void main() {
	Vertex vertex = vertices[gl_VertexIndex];

	vec3 position = vec3(vertex.px, vertex.py, vertex.pz);
	vec3 normal = vec3(vertex.nx, vertex.ny, vertex.nz);

	out_position = vec3(pc.model * vec4(position, 1.0));
	out_normal = mat3(transpose(inverse(pc.model))) * normal;
	out_uv = vec2(vertex.uvx, vertex.uvy);
	out_drawid = gl_DrawIDARB;
  
  gl_Position = pc.projection * pc.view * pc.model * vec4(position, 1.0);
}