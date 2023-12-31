// Sam Collier 2023
#pragma once

#include <goop/sys/Vertex.h>
#include <map>
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
	const VkBuffer* getInstanceBuffer(uint32_t currentFrame) const
	{
		return &instanceBuffers[currentFrame];
	}
	VkBuffer getIndexBuffer(uint32_t currentFrame) const { return indexBuffers[currentFrame]; }
	uint32_t getVertexCount(uint32_t currentFrame) const { return vertexCounts[currentFrame]; }
	uint32_t getIndexCount(uint32_t currentFrame, uint32_t index) const
	{
		if (indexCounts.size() <= currentFrame)
		{
			return 0;
		}
		if (indexCounts[currentFrame].size() <= index)
		{
			return 0;
		}

		return indexCounts[currentFrame][index];
	}
	size_t getIndexCountSize(uint32_t currentFrame) const
	{
		return indexCounts[currentFrame].size();
	}
	uint32_t getIndexOffset(uint32_t currentFrame, uint32_t index) const
	{
		return indexOffsets[currentFrame][index];
	}
	uint32_t getInstanceOffsets(uint32_t currentFrame, uint32_t index) const
	{
		return instanceOffsets[currentFrame][index];
	}
	uint32_t getMeshID(uint32_t currentFrame, uint32_t index) const
	{
		return meshIDs[currentFrame][index];
	}
	uint32_t getInstanceCount(uint32_t currentFrame, uint32_t index) const
	{
		return instanceCounts[currentFrame][index];
	}
	size_t getNumUniqueInstances(uint32_t currentFrame) const
	{
		return instanceOffsets[currentFrame].size();
	}

	void swapBuffers(uint32_t currentFrame);
	void updateBuffers(uint32_t currentFrame, const Vertex* vertices, uint32_t vertexCount,
					   const uint32_t* indices, uint32_t indexCount,
					   const std::vector<uint32_t>* indexOffsets,
					   const std::vector<uint32_t>* indexCounts,
					   const std::map<uint32_t, std::vector<Instance>>* instances,
					   uint32_t instanceCount, bool forceUpdate);
	bool isReadyToSwap() const { return bReadyToSwap; }

  private:
	void createVertexBuffer(uint32_t currentFrame, const Vertex* vertices, uint32_t vertexCount);
	void createInstanceBuffer(uint32_t currentFrame, const Instance* instances,
							  uint32_t instanceCount);
	void createIndexBuffer(uint32_t currentFrame, const uint32_t* indices, uint32_t indexCount);
	void clearBuffers();

	// TODO driver developers recommend storing vertex and index buffers into a single VkBuffer and
	// use offsets. That way the data is more cache friendly
	std::vector<VkBuffer> vertexBuffers;
	std::vector<VkDeviceMemory> vertexBufferMemories;
	std::vector<VkBuffer> indexBuffers;
	std::vector<VkDeviceMemory> indexBufferMemories;
	std::vector<VkBuffer> instanceBuffers;
	std::vector<VkDeviceMemory> instanceBufferMemories;

	std::vector<std::vector<uint32_t>> indexOffsets;
	std::vector<std::vector<uint32_t>> instanceOffsets;
	std::vector<std::vector<uint32_t>> meshIDs;

	std::vector<VkBuffer> vertexBufferStaging;
	std::vector<VkDeviceMemory> vertexBufferMemoryStaging;
	std::vector<VkBuffer> indexBufferStaging;
	std::vector<VkDeviceMemory> indexBufferMemoryStaging;
	std::vector<VkBuffer> instanceBufferStaging;
	std::vector<VkDeviceMemory> instanceBufferMemoryStaging;

	std::vector<uint32_t> vertexCounts;
	std::vector<std::vector<uint32_t>> instanceCounts;
	std::vector<std::vector<uint32_t>> indexCounts;
	std::vector<uint32_t> oldVertexCounts;
	std::vector<uint32_t> oldIndexCounts;
	std::vector<uint32_t> oldInstanceCounts;

	VkDeviceSize vertexBufferSize = 0;
	VkDeviceSize instanceBufferSize = 0;
	VkDeviceSize indexBufferSize = 0;

	Context* ctx;

	bool bReadyToSwap = false;
};
} // namespace goop::sys::platform::vulkan
