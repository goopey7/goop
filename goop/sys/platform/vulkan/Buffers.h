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

	const VkBuffer* getVertexBuffer(uint32_t currentFrame) const
	{
		return &vertexBuffers[currentFrame];
	}
	VkBuffer getIndexBuffer(uint32_t currentFrame) const { return indexBuffers[currentFrame]; }
	uint32_t getVertexCount(uint32_t currentFrame) const { return vertexCounts[currentFrame]; }
	uint32_t getIndexCount(uint32_t currentFrame) const { return indexCounts[currentFrame]; }

	void swapBuffers(uint32_t currentFrame);
	void updateBuffers(uint32_t currentFrame, const Vertex* vertices, uint32_t vertexCount,
					   const uint32_t* indices, uint32_t indexCount);
	bool isReadyToSwap() const { return bReadyToSwap; }

  private:
	void createVertexBuffer(uint32_t currentFrame, const Vertex* vertices, uint32_t vertexCount);
	void createIndexBuffer(uint32_t currentFrame, const uint32_t* indices, uint32_t indexCount);
	void clearBuffers();

	// TODO driver developers recommend storing vertex and index buffers into a single VkBuffer and
	// use offsets. That way the data is more cache friendly
	std::vector<VkBuffer> vertexBuffers;
	std::vector<VkDeviceMemory> vertexBufferMemories;
	std::vector<VkBuffer> indexBuffers;
	std::vector<VkDeviceMemory> indexBufferMemories;

	std::vector<VkBuffer> vertexBufferStaging;
	std::vector<VkDeviceMemory> vertexBufferMemoryStaging;
	std::vector<VkBuffer> indexBufferStaging;
	std::vector<VkDeviceMemory> indexBufferMemoryStaging;

	std::vector<uint32_t> vertexCounts;
	std::vector<uint32_t> indexCounts;
	std::vector<uint32_t> oldVertexCounts;
	std::vector<uint32_t> oldIndexCounts;

	VkDeviceSize vertexBufferSize = 0;
	VkDeviceSize indexBufferSize = 0;

	Context* ctx;

	bool bReadyToSwap = false;
};
} // namespace goop::sys::platform::vulkan
