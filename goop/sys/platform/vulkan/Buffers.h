// Sam Collier 2023
#pragma once

#include "Vertex.h"
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
	Buffers(Context* ctx);
	~Buffers();

	const VkBuffer* getVertexBuffer() const { return &vertexBuffer; }
	VkBuffer getIndexBuffer() const { return indexBuffer; }

	uint32_t getVertexCount() const { return vertices.size(); }
	uint32_t getIndexCount() const { return indices.size(); }

  private:
	void loadModel();
	void createVertexBuffer();
	void createIndexBuffer();

	// TODO driver developers recommend storing vertex and index buffers into a single VkBuffer and
	// use offsets. That way the data is more cache friendly
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;

	Assimp::Importer importer;

	Context* ctx;

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
};
} // namespace goop::sys::platform::vulkan
