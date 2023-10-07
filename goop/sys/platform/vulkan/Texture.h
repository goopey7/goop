// Sam Collier 2023
#pragma once

#include <vulkan/vulkan.h>

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

  private:
	void createTextureImage(const char* path);
	void createTextureImageView();
	void createTextureSampler();

	Context* ctx;
	VkImage textureImage;
	VkDeviceMemory textureImageMemory;
	VkImageView textureImageView;
	VkSampler textureSampler;
};
} // namespace goop::sys::platform::vulkan
