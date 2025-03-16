#version 430 core

precision highp float;

in vec2 position;

uniform ivec2 u_resolution;
uniform vec2 u_center;
uniform float u_zoom;
uniform int u_MAX_ITERATIONS;
uniform int u_BASE_ITERATIONS;
uniform float u_saturation;
uniform float u_brightness;
uniform vec2 u_julia_c;

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

vec3 hsvToRgb(float h, float s, float v) {
    float p = v * (1.0 - s);
    float q = v * (1.0 - s * fract(h * 6.0));
    float t = v * (1.0 - s * (1.0 - fract(h * 6.0)));

    if (h < 1.0/6.0) return vec3(v, t, p);
    if (h < 2.0/6.0) return vec3(q, v, p);
    if (h < 3.0/6.0) return vec3(p, v, t);
    if (h < 4.0/6.0) return vec3(p, q, v);
    if (h < 5.0/6.0) return vec3(t, p, v);
    return vec3(v, p, q);
}

vec3 computeFragColor(vec2 uv) {
	vec3 color;
	float aspectRatio = float(u_resolution.x) / float(u_resolution.y);
	vec2 c;
	vec2 z;
	if (!isnan(u_julia_c.x) && !isnan(u_julia_c.y)) {
		c = u_julia_c;
		z = u_center + (uv) * vec2(4.0 * aspectRatio, 4.0) / u_zoom;
	} else {
		c = u_center + (uv) * vec2(4.0 * aspectRatio, 4.0) / u_zoom;
		z = vec2(0.0);
	}
	int iter = 0;
	while (length(z) < 2.0 && iter < u_MAX_ITERATIONS) {
		z = compMul(z,z);
		z = compAdd(z,c);
		iter++;
	}
	if (iter == u_MAX_ITERATIONS) {
		color = vec3(0.0);
	} else {
        float t = float(iter) / float(u_BASE_ITERATIONS);
		float hue = mod(t * 5.0, 1.0);
        color = hsvToRgb(hue, u_saturation, u_brightness);
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