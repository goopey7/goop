// Sam Collier 2023

#include "Sampler.h"
#include "Context.h"
#include <stdexcept>

using namespace goop::sys::platform::vulkan;

Sampler::Sampler(Context* ctx) : ctx(ctx)
{
	createSampler();
}

Sampler::~Sampler()
{
	vkDestroySampler(*ctx, sampler, nullptr);
}

void Sampler::createSampler()
{
	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;

	// VK_FILTER_NEAREST will disable interpolation if we want pixel-art
	// VK_FILTER_LINEAR will interpolate between texels if we want smoothness
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;

	// How to handle texture coordinates outside of [0, 1]
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

	VkPhysicalDeviceProperties properties{};
	vkGetPhysicalDeviceProperties(*ctx, &properties);
	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;

	// Which color to use when sampling beyond the image with
	// VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;

	// stick to [0, 1] for texture coordinates
	samplerInfo.unnormalizedCoordinates = VK_FALSE;

	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.f;
	samplerInfo.minLod = 0.f;
	samplerInfo.maxLod = 0.f;

	if (vkCreateSampler(*ctx, &samplerInfo, nullptr, &sampler) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create texture sampler!");
	}
}
