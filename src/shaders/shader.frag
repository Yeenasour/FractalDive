#version 430 core

precision highp float;

in vec3 fragmentColor;
in vec2 position;

uniform vec2 u_resolution;
uniform vec2 u_center;
uniform float u_zoom;
uniform int u_MAX_ITERATIONS;
uniform int u_BASE_ITERATIONS;

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

vec3 computeFragColor(vec2 uv) {
	vec3 color;
	float aspectRatio = u_resolution.x / u_resolution.y;
	vec2 c = u_center + (uv) * vec2(4.0 * aspectRatio, 4.0) / u_zoom;
	vec2 z = vec2(0.0);
	int iter = 0;
	while (length(z) < 2.0 && iter < u_MAX_ITERATIONS) {
		z = compMul(z,z);
		z = compAdd(z,c);
		iter++;
	}
	if (iter == u_MAX_ITERATIONS) {
		color = vec3(0.0);
	} else {
        float t = 2 * 3.14159265 * float(iter) / float(u_BASE_ITERATIONS);
        
        color = vec3(0.5 + 0.5 * cos(t * 6.28), 0.5 + 0.5 * sin(t * 6.28), 0.5);
	}
	return color;
}

void main() {
	/*if (abs(position.x) + abs(position.y) < 0.25)
	{
		screenColor = vec4(1.0, 0.0, 0.0, 1.0);
		return;
	}*/

	vec3 color = vec3(0.0);

	// between 1.5 and 2.0 seems to be a good constant
	vec2 pixelSize = 2.0 / u_resolution;
	const int numSamples = 4;
	vec2 offsets[numSamples] = vec2[](
    	vec2(-0.25, -0.25) * pixelSize,
    	vec2( 0.25, -0.25) * pixelSize,
    	vec2(-0.25,  0.25) * pixelSize,
    	vec2( 0.25,  0.25) * pixelSize
	);

	// Quincunx sample pattern for anti-aliasing
	for (int i = 0; i < numSamples; i++) {
		vec2 uvSample = position + offsets[i];
		color += computeFragColor(uvSample) * 0.125;
	}
	color += computeFragColor(position) * 0.5;

	screenColor = vec4(color, 1.0);
}