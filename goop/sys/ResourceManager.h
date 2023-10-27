// Sam Collier 2023
#pragma once

#include <goop/sys/MeshLoader.h>
#include <goop/sys/Subsystem.h>
#include <string>

#ifdef RENDERER_VULKAN
#include <goop/sys/platform/vulkan/Buffers.h>
#endif

namespace goop::res
{
	enum mesh
	{
		VIKING_ROOM,
	};
}

namespace goop::sys
{
class ResourceManager : public Subsystem
{
  public:
	virtual int initialize() final;
	virtual int destroy() final;

	virtual bool loadMesh(const std::string& path);

  private:
	std::unique_ptr<MeshLoader> meshLoader;

	const MeshLoader* getMeshLoader() const { return meshLoader.get(); }

#ifdef RENDERER_VULKAN
	friend class goop::sys::platform::vulkan::Buffers;
#endif
};
} // namespace goop::sys
