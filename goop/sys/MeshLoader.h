// Sam Collier 2023
#pragma once

#include <cstdint>
#include <functional>
#include <goop/sys/Subsystem.h>
#include <memory>
#include <string>
#include <vector>

namespace goop::sys
{

struct Vert
{
	float x;
	float y;
	float z;
	float u;
	float v;

	bool operator==(const Vert& other) const
	{
		return x == other.x && y == other.y & z == other.z && u == other.u && v == other.v;
	}
};

struct MeshImportData
{
	std::vector<Vert> vertices;
	std::vector<uint32_t> indices;
};

class MeshLoader : public Subsystem
{
  public:
	virtual void loadModel(const std::string& path) = 0;
	const MeshImportData& getData() const;

  protected:
	MeshImportData data;
};

// Global pointer to goop's window
extern std::unique_ptr<MeshLoader> gMeshLoader;

} // namespace goop::sys

namespace std
{
using namespace goop::sys;
template <> struct hash<Vert>
{
	size_t operator()(Vert const& vertex) const
	{
		return ((hash<float>()(vertex.x) ^ (hash<float>()(vertex.y) << 1)) >> 1) ^
			   (hash<float>()(vertex.z) << 1);
	}
};
} // namespace std
