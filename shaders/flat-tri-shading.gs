#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

uniform float ambient = 0.2;

in VS_OUT {
    vec3 viewPos;
    float lightIntensity;
} vrts[];

out float elemLightIntensity;

void main ()
{
	vec3 v0 = (vrts[1].viewPos - vrts[0].viewPos).xyz;
	vec3 v1 = (vrts[2].viewPos - vrts[0].viewPos).xyz;

	elemLightIntensity = ambient + (1.0f - ambient) * abs(normalize(cross (v0, v1)).z);
	
	for (int i = 0; i < 3; ++i) {
		gl_Position = gl_in[i].gl_Position; 
	    EmitVertex();
	}

    EndPrimitive();
}