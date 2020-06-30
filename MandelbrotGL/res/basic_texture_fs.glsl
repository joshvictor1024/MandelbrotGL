#version 430 core

in vec2 vTexCoord;
out vec4 color;     //requirement of a fragment shader

uniform sampler2D uPositionTexture;
uniform sampler1D uColorTexture;
uniform int uIteration;

void main()
{
	color = vec4(
		vec3(texture(
                uColorTexture, texture(uPositionTexture, vTexCoord).x * float(uIteration) / 2048.0
        )),
        1.0
	);
}
