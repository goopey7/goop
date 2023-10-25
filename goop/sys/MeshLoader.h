// Sam Collier 2023
#pragma once

#include <cstdint>
#include <functional>
#include <goop/sys/Subsystem.h>
#include <goop/sys/Vertex.h>
#include <memory>
#include <string>
#include <vector>

namespace goop::sys
{

struct MeshImportData
{
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
};

class MeshLoader : public Subsystem
{
  public:
	virtual int initialize() override;
	virtual void loadModel(const std::string& path) = 0;
	const MeshImportData* getData() const { return data.get(); }

  protected:
	std::unique_ptr<MeshImportData> data;
};

// Global pointer to goop's window
extern std::unique_ptr<MeshLoader> gMeshLoader;

} // namespace goop::sys
