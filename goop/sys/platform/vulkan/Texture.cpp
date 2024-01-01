// Sam Collier 2023

#include "Texture.h"
#include "Context.h"
#include "Utils.h"
#include <cstring>
#include <stdexcept>

using namespace goop::sys::platform::vulkan;

Texture::Texture(Context* ctx, unsigned char* pixels, int width, int height, const char* path)
	: ctx(ctx)
{
	createTextureImage(pixels, width, height);
	createTextureImageView();
}

Texture::~Texture()
{
	vkDestroyImageView(*ctx, textureImageView, nullptr);
	vkDestroyImage(*ctx, textureImage, nullptr);
	vkFreeMemory(*ctx, textureImageMemory, nullptr);
}

void Texture::createTextureImage(unsigned char* pixels, int width, int height)
{
	VkDeviceSize imageSize = width * height * 4;

	if (!pixels)
	{
		throw std::runtime_error("failed to load texture image!");
	}

	// copy pixel data to staging buffer
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	createBuffer(ctx, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				 stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(*ctx, stagingBufferMemory, 0, imageSize, 0, &data);
	memcpy(data, pixels, static_cast<size_t>(imageSize));
	vkUnmapMemory(*ctx, stagingBufferMemory);

	createImage(ctx, width, height, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
				VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, textureImageMemory);

	// transition image layout to transfer destination
	transitionImageLayout(ctx, textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED,
						  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	copyBufferToImage(ctx, stagingBuffer, textureImage, static_cast<uint32_t>(width),
					  static_cast<uint32_t>(height));

	// transition image layout to shader read only
	transitionImageLayout(ctx, textureImage, VK_FORMAT_R8G8B8A8_SRGB,
						  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
						  VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	vkDestroyBuffer(*ctx, stagingBuffer, nullptr);
	vkFreeMemory(*ctx, stagingBufferMemory, nullptr);
}

void Texture::createTextureImageView()
{
	textureImageView =
		createImageView(ctx, textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
}

