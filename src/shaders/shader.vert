#version 430 core

precision highp float;

layout (location=0) in vec2 vertexPos;

out vec2 position;

void main() {
	gl_Position = vec4(vertexPos, 0.0, 1.0);
	position = vertexPos;
}