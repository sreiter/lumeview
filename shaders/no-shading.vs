#version 330 core

layout (location = 0) in vec3 pos;
uniform mat4 view;

out VS_OUT {
    vec3 viewPos;
    float lightIntensity;
} vs_out;

void main()
{
	gl_Position = view * vec4 (pos, 1.0);
	vs_out.lightIntensity = 1.0f;
	vs_out.viewPos = gl_Position.xyz;
}