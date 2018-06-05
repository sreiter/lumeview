#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;

uniform float ambient = 0.2;
uniform mat4 view;
uniform mat4 projection;

out VS_OUT {
    vec3 viewPos;
    float lightIntensity;
} vs_out;

void main()
{
	vec4 viewPos = view * vec4 (pos, 1.0);
	vs_out.lightIntensity = ambient + (1.f - ambient) * abs(normalize(view*vec4(normal, 0)).z);
	vs_out.viewPos = viewPos.xyz;
	viewPos[3] = 1.f;
	gl_Position = projection * viewPos;
}
