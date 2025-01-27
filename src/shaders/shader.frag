#version 330 core

in vec3 fragmentColor;
in vec2 position;

out vec4 screenColor;

void main() {
	screenColor = vec4(fragmentColor, 1.0);
}