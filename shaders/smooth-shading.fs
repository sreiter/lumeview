#version 330 core

uniform vec4 color = vec4 (1.f, 1.f, 1.f, 1.f);

in VS_OUT {
    vec3 viewPos;
    float lightIntensity;
} vrt;

out vec4 fragColor;


void main ()
{
	fragColor = vec4 (vrt.lightIntensity * color.rgb, color.a);
	gl_FragDepth = gl_FragCoord.z * zfac;
}
