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

	Context* ctx;
	VkImage textureImage;
	VkDeviceMemory textureImageMemory;
};
} // namespace goop::sys::platform::vulkan
