#version 430 core

layout(location = 0) in vec2 position;//"location" correspond to index of vao
layout(location = 1) in vec2 texCoord;
out vec2 vTexCoord;//v for varying

uniform mat4 uMVP;

void main()
{
	gl_Position = uMVP * vec4(position, 0.0, 1.0);
	vTexCoord = texCoord;
}
