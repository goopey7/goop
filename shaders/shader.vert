#version 450

layout(binding = 0) uniform UniformBufferObject
{
	mat4 view;
	mat4 proj;
} ubo;

layout(push_constant) uniform PushConstantObject
{
	mat4 model;
} pc;

// vertex data
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

// instance data
layout (location = 3) in uint inInstanceID;
layout(location = 0) out vec3 outColor;
layout(location = 1) out vec2 outTexCoord;

void main()
{
	gl_Position = ubo.proj * ubo.view * pc.model * vec4(inPosition, 1.0);
	outColor = inColor;
	outTexCoord = inTexCoord;
}

