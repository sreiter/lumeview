#version 330 core
out vec4 FragColor;

void main ()
{
	float ia = clamp(1.0f - gl_FragCoord.z, 0.0f, 1.0f);
	FragColor = vec4 (ia * vec3(1.0f, 0.5f, 0.2f), 1.0f);
}
