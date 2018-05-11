#version 330 core

layout (location = 0) in vec3 pos;

uniform mat4 view;

void main()
{
	gl_Position = view * vec4 (pos, 1.0);
}
