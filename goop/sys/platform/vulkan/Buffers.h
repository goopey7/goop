// Sam Collier 2023
#pragma once

#include "Vertex.h"
#include <goop/sys/MeshLoader.h>
#include <vector>
#include <vulkan/vulkan.h>

#include <assimp/Importer.hpp>

namespace goop::sys::platform::vulkan
{

class Context;

class Buffers
{
  public:
	Buffers(const Buffers&) = delete;
	Buffers& operator=(const Buffers&) = delete;
	Buffers(Context* ctx, const MeshLoader* ml);
	~Buffers();

	const VkBuffer* getVertexBuffer() const { return &vertexBuffer; }
	VkBuffer getIndexBuffer() const { return indexBuffer; }

	uint32_t getVertexCount() const { return ml->getData().vertices.size(); }
	uint32_t getIndexCount() const { return ml->getData().indices.size(); }

  private:
	void createVertexBuffer();
	void createIndexBuffer();

	// TODO driver developers recommend storing vertex and index buffers into a single VkBuffer and
	// use offsets. That way the data is more cache friendly
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;

	std::vector<Vertex> vertices;

	Context* ctx;
	const MeshLoader* ml;
};
} // namespace goop::sys::platform::vulkan
