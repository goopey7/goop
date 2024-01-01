//Sam Collier 2023

#include "ImageLoader_STB.h"
#include "goop/sys/Renderer.h"
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace goop::sys::platform::stb;

uint32_t ImageLoader_STB::load(const std::string& path)
{
	int width, height, channels;
	unsigned char* pixels = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
	std::cout << "Loaded " << path << " with " << channels << " channels" << std::endl;
	goop::sys::gRenderer->addTexture(pixels, width, height, path.c_str());
	stbi_image_free(pixels);
	return 0;
}

uint32_t ImageLoader_STB::unload(const std::string& path)
{
	std::cout << "Unloaded " << path << std::endl;
	goop::sys::gRenderer->removeTexture(path.c_str());
	return 0;
}

int ImageLoader_STB::initialize()
{
	return 0;
}

int ImageLoader_STB::destroy()
{
	return 0;
}

