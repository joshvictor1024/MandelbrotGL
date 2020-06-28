#version 430 core

layout(local_size_x = 32, local_size_y = 32) in;

layout(r8) uniform image2D uImage;
uniform vec4 uRangeRect;
uniform vec2 uImageDim;
uniform int uIteration;

void main() {
	vec2 z = vec2(0.0, 0.0);
	vec2 c = vec2(uRangeRect.xy) + vec2(uRangeRect.zw) * vec2(gl_GlobalInvocationID.xy) / uImageDim;

	uint it = 0;
	for (; it < uIteration && (z.x * z.x + z.y * z.y < 2.0 * 2.0); it++)
	{
		z += c;
		z = vec2(z.x * z.x - z.y * z.y, 2.0 * z.x * z.y);
	}

	imageStore(uImage, ivec2(gl_GlobalInvocationID.xy), vec4(float(it) / float(uIteration), 0.0, 0.0, 0.0));
}




































