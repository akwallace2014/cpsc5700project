#version 330
layout (location = 0) in vec3 point;
layout (location = 1) in vec3 normal;
in vec2 uv;
out vec2 vUv;
out vec3 vPoint, vNormal;
uniform mat4 modelview, persp;
void main() {
	vUv = uv;
	vPoint = (modelview*vec4(point, 1)).xyz;
	vNormal = (modelview*vec4(normal, 0)).xyz;
	gl_Position = 0.1 * persp*vec4(vPoint, 1);
}