// Sam Collier 2023
#pragma once

#include <goop/sys/ImageLoader.h>

namespace goop::sys::platform::stb
{
class ImageLoader_STB : public ImageLoader
{
  public:
	ImageLoader_STB(const ImageLoader_STB&) = delete;
	ImageLoader_STB& operator=(const ImageLoader_STB&) = delete;
	ImageLoader_STB() = default;
	~ImageLoader_STB() { destroy(); }

	// subsytem interface
	int initialize() final;
	int destroy() final;

	// resource interface
	uint32_t load(const std::string& path) final;
	uint32_t unload(const std::string& path) final;

  protected:
};
} // namespace goop::sys::platform::stb
