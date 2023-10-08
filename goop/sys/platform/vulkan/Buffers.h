// Sam Collier 2023
#pragma once

#include "Vertex.h"
#include <vector>
#include <vulkan/vulkan.h>

namespace goop::sys::platform::vulkan
{

class Context;

const std::vector<Vertex> vertices = {
	{{-0.5f, -0.5f, 0.f}, {1.f, 0.f, 0.f}, {1.f, 0.f}},
	{{0.5f, -0.5f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 0.f}},
	{{0.5f, 0.5f, 0.f}, {0.f, 0.f, 1.f}, {0.f, 1.f}},
	{{-0.5f, 0.5f, 0.f}, {1.f, 1.f, 0.f}, {1.f, 1.f}},

	{{-0.5f, -0.5f, -0.5f}, {1.f, 0.f, 0.f}, {1.f, 0.f}},
	{{0.5f, -0.5f, -0.5f}, {0.f, 1.f, 0.f}, {0.f, 0.f}},
	{{0.5f, 0.5f, -0.5f}, {0.f, 0.f, 1.f}, {0.f, 1.f}},
	{{-0.5f, 0.5f, -0.5f}, {1.f, 1.f, 0.f}, {1.f, 1.f}},
};

const std::vector<uint32_t> indices = {
	0, 1, 2, 2, 3, 0,
	4, 5, 6, 6, 7, 4,
};

class Buffers
{
  public:
	Buffers(const Buffers&) = delete;
	Buffers& operator=(const Buffers&) = delete;
	Buffers(Context* ctx);
	~Buffers();

	const VkBuffer* getVertexBuffer() const { return &vertexBuffer; }
	VkBuffer getIndexBuffer() const { return indexBuffer; }

  private:
	void createVertexBuffer();
	void createIndexBuffer();

	// TODO driver developers recommend storing vertex and index buffers into a single VkBuffer and
	// use offsets. That way the data is more cache friendly
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;

	Context* ctx;
};
} // namespace goop::sys::platform::vulkan
