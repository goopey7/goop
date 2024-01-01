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
	Texture(Context* ctx, unsigned char* pixels, int width, int height, const char* path);
	~Texture();

	VkImageView getImageView() const { return textureImageView; }

  private:
	void createTextureImage(unsigned char* pixels, int width, int height);
	void createTextureImageView();

	Context* ctx;
	VkImage textureImage;
	VkDeviceMemory textureImageMemory;
	VkImageView textureImageView;
};
} // namespace goop::sys::platform::vulkan
