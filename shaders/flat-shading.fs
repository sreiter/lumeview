#version 330 core

uniform vec4 color = vec4 (1.f, 1.f, 1.f, 1.f);
uniform float zfac = 1.f;

out vec4 fragColor;
in float elemLightIntensity;

void main ()
{
	fragColor = vec4 (elemLightIntensity * color.rgb, color.a);
	gl_FragDepth = gl_FragCoord.z * zfac;
}
