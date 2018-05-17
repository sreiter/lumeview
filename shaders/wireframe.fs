#version 330 core

out vec4 fragColor;

void main ()
{
	fragColor = vec4 (vec3(0.6f, 0.6f, 0.6f), 1.0f);
	gl_FragDepth = gl_FragCoord.z * 0.99;
}
