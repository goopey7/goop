// Sam Collier 2023
#include <goop/sys/platform/vulkan/Renderer_Vulkan.h>

using namespace goop::sys;

void Renderer::addToRenderQueue(goop::res::mesh mesh, MeshLoader* meshLoader)
{
	this->meshLoader = meshLoader;
	meshQueue.push(mesh);
}

