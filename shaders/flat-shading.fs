#version 330 core

uniform vec4 color = vec4 (1.f, 1.f, 1.f, 1.f);
uniform float zfacNear = 1.f;
uniform float zfacFar = 1.f;

out vec4 fragColor;
in float elemLightIntensity;

void main ()
{
	fragColor = vec4 (elemLightIntensity * color.rgb, color.a);
	gl_FragDepth = gl_FragCoord.z * (mix(zfacNear, zfacFar, gl_FragCoord.z));
}
