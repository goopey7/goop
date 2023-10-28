// Sam Collier 2023
#pragma once

#include <goop/sys/Vertex.h>
#include <goop/sys/MeshLoader.h>
#include <vector>
#include <volk.h>

#include <assimp/Importer.hpp>

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

  private:
	void createVertexBuffer(const MeshImportData* mid);
	void createIndexBuffer(const MeshImportData* mid);

	// TODO driver developers recommend storing vertex and index buffers into a single VkBuffer and
	// use offsets. That way the data is more cache friendly
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;

	uint32_t vertexCount = 0;
	uint32_t indexCount = 0;

	Context* ctx;
};
} // namespace goop::sys::platform::vulkan
