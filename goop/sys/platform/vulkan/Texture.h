// Sam Collier 2023
#pragma once

#include <volk.h>

namespace goop::sys::platform::vulkan
{
class Context;
class Texture
{
  public:
	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;
	Texture(Context* ctx, const char* path);
	~Texture();

	VkImageView getImageView() const { return textureImageView; }
	VkSampler getSampler() const { return textureSampler; }

  private:
	void createTextureImage(const char* path);
	void createTextureImageView();
	void createTextureSampler();

	Context* ctx;
	VkImage textureImage;
	VkDeviceMemory textureImageMemory;
	VkImageView textureImageView;
	VkSampler textureSampler;

	VkSampler viewportSampler;
};
} // namespace goop::sys::platform::vulkan
