#version 430 core

precision highp float;

in vec3 fragmentColor;
in vec2 position;

uniform vec2 u_resolution;
uniform vec2 u_center;
uniform float u_zoom;
uniform int u_MAX_ITERATIONS;

out vec4 screenColor;

vec2 compAdd(vec2 z1, vec2 z2) {
	return z1 + z2;
}

vec2 compMul(vec2 z1, vec2 z2) {
	return vec2((z1.x * z2.x) - (z1.y * z2.y), (z1.x * z2.y) + (z1.y * z2.x));
}

float map(float x, float inMin, float inMax, float outMin, float outMax) {
	return (x - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
}

void main() {
	float aspectRatio = u_resolution.x / u_resolution.y;
	vec2 c = u_center + (position) * vec2(4.0 * aspectRatio, 4.0) / u_zoom;
	vec2 z = vec2(0.0);
	int iter = 0;
	while (length(z) < 2.0 && iter < u_MAX_ITERATIONS) {
		z = compMul(z,z);
		z = compAdd(z,c);
		iter++;
	}
	if (iter == u_MAX_ITERATIONS) {
		screenColor = vec4(vec3(0.0), 1.0);
	} else {
        float t = 2 * 3.14159265 * float(iter) / float(u_MAX_ITERATIONS);	//TODO swap MAX_ITERATIONS for a BASE_ITERATIONS to preserve the color distribution
        
        screenColor = vec4(vec3(0.5 + 0.5 * cos(t * 6.28), 0.5 + 0.5 * sin(t * 6.28), 0.5), 1.0);
	}
}