// Sam Collier 2023
#pragma once

#include <cstdint>
#include <functional>
#include <goop/sys/ResourceSubsystem.h>
#include <goop/sys/Vertex.h>
#include <map>
#include <memory>
#include <vector>
#include <goop/Primitives.h>

namespace goop::sys
{

struct MeshImportData
{
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
};

class MeshLoader : public ResourceSubsystem
{
  public:
	virtual int initialize() override;
	const std::map<uint32_t, MeshImportData>* getData() const { return data.get(); }
	int loadPrimitive(Primitive primitive);

  protected:
	std::unique_ptr<std::map<uint32_t, MeshImportData>> data;
	std::map<std::string, uint32_t> loadedMeshes;
	std::vector<int> unloadedSlots;
};
} // namespace goop::sys
