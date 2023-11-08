// Sam Collier 2023
#pragma once

#include <goop/sys/ResourceSubsystem.h>
#include <cstdint>
#include <functional>
#include <goop/sys/Vertex.h>
#include <memory>
#include <vector>

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
	const std::vector<MeshImportData>* getData() const { return data.get(); }

  protected:
	std::unique_ptr<std::vector<MeshImportData>> data;

};
} // namespace goop::sys