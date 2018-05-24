#version 330 core

layout (lines) in;
layout (line_strip, max_vertices = 2) out;

uniform float ambient = 0.2;
uniform mat4 view;

in VS_OUT {
    vec3 viewPos;
    float lightIntensity;
} vrts[];

out float elemLightIntensity;

void main ()
{
	elemLightIntensity = 0.5f * (vrts[0].lightIntensity + vrts[1].lightIntensity);
	
	for (int i = 0; i < 2; ++i) {
		gl_Position = gl_in[i].gl_Position; 
	    EmitVertex();
	}

    EndPrimitive();
}