#version 430 core

precision highp float;

in vec3 fragmentColor;
in vec2 position;

// make these into uniforms
uniform float u_xMin;
uniform float u_xMax;
uniform float u_yMin;
uniform float u_yMax;
uniform int u_MAX_ITERATIONS;
//vec2 resolution = vec2() / might be a QOL improvement later

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

	float normalizedX = (position.x * 0.5) + 0.5;
	float normalizedY = (position.y * 0.5) + 0.5;
	
	float Re = mix(u_xMin, u_xMax, normalizedX);
	float Im = mix(u_yMin, u_yMax, normalizedY);

	vec2 c = vec2(Re, Im);
	vec2 z = vec2(0.0);
	int iter = 0;
	while (length(z) < 2.0 && iter < u_MAX_ITERATIONS) {
		z = compMul(z,z);
		z = compAdd(z,c);
		iter++;
	}
	if (iter == u_MAX_ITERATIONS) {
		screenColor = vec4(fragmentColor, 1.0);
	} else {	// perhaps use a better/clearer visualization
		screenColor = vec4(vec3(map(iter, 0.0, u_MAX_ITERATIONS, 0.0, 1.0)), 1.0);
	}
}