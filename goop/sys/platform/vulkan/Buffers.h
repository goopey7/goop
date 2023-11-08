// Sam Collier 2023
#pragma once

#include <goop/sys/Vertex.h>
#include <vector>
#include <volk.h>

namespace goop::sys::platform::vulkan
{

class Context;

class Buffers
{
  public:
	Buffers(const Buffers&) = delete;
	Buffers& operator=(const Buffers&) = delete;
	Buffers(Context* ctx);
	~Buffers();

	const VkBuffer* getVertexBuffer() const { return &vertexBuffer; }
	VkBuffer getIndexBuffer() const { return indexBuffer; }
	uint32_t getVertexCount() const { return vertexCount; }
	uint32_t getIndexCount() const { return indexCount; }

	void swapBuffers();
	void updateBuffers(const Vertex* vertices, uint32_t vertexCount, const uint32_t* indices,
					   uint32_t indexCount);
	bool isReadyToSwap() const { return bReadyToSwap; }

  private:
	void createVertexBuffer(const Vertex* vertices, uint32_t vertexCount);
	void createIndexBuffer(const uint32_t* indices, uint32_t indexCount);
	void clearBuffers();

	// TODO driver developers recommend storing vertex and index buffers into a single VkBuffer and
	// use offsets. That way the data is more cache friendly
	VkBuffer vertexBuffer = VK_NULL_HANDLE;
	VkDeviceMemory vertexBufferMemory = VK_NULL_HANDLE;
	VkBuffer indexBuffer = VK_NULL_HANDLE;
	VkDeviceMemory indexBufferMemory = VK_NULL_HANDLE;

	VkBuffer vertexBufferStaging = VK_NULL_HANDLE;
	VkDeviceMemory vertexBufferMemoryStaging = VK_NULL_HANDLE;
	VkBuffer indexBufferStaging = VK_NULL_HANDLE;
	VkDeviceMemory indexBufferMemoryStaging = VK_NULL_HANDLE;
	uint32_t vertexCount = 0;
	uint32_t indexCount = 0;

	VkDeviceSize vertexBufferSize = 0;
	VkDeviceSize indexBufferSize = 0;

	Context* ctx;

	bool bReadyToSwap = false;
};
} // namespace goop::sys::platform::vulkan
