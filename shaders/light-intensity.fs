#version 330 core

in float lightIntensity;

out vec4 fragColor;


void main ()
{
	float ia = lightIntensity; //clamp(1.0f - gl_FragCoord.z, 0.0f, 1.0f);
	fragColor = vec4 (ia * vec3(1.0f, 1.0f, 0.9f), 1.0f);
}
