#version 450

layout(location = 0) in vec3 inColor;
layout(location = 1) in vec2 inTexCoord;

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) out vec4 outColor;

void main()
{
	outColor = texture(texSampler, inTexCoord);
	outColor = vec4(pow(outColor.rgb, vec3(1.0/2.2)), outColor.a);
}

