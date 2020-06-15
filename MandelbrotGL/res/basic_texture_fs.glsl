#version 430 core

in vec2 vTexCoord;
out vec4 color;//requirement of a fragment shader

uniform sampler2D uPositionTexture;
uniform sampler1D uColorTexture;

void main()
{
	color = vec4(
		texture(uPositionTexture, vTexCoord).x, 0.0, 0.0,
        1.0
	);
}
