#version 450

layout(binding = 0) uniform UniformBufferObject
{
	mat4 model;
	mat4 view;
	mat4 proj;
} ubo;

// vertex data
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

// instance data
layout(location = 3) in mat4 inTransform;

layout(location = 0) out vec3 outColor;
layout(location = 1) out vec2 outTexCoord;

void main()
{
	gl_Position = ubo.proj * ubo.view * inTransform * ubo.model * vec4(inPosition, 1.0);
	outColor = inColor;
	outTexCoord = inTexCoord;
}

