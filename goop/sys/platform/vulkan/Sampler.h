// Sam Collier 2023
#pragma once

#include <volk.h>

namespace goop::sys::platform::vulkan
{
class Context;
class Sampler
{
  public:
	Sampler(const Sampler&) = delete;
	Sampler& operator=(const Sampler&) = delete;
	Sampler(Context* ctx);
	~Sampler();
	operator VkSampler() const { return sampler; }

	VkSampler getSampler() const { return sampler; }

  private:
	void createSampler();

	Context* ctx;
	VkSampler sampler;
};
} // namespace goop::sys::platform::vulkan
