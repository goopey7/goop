// Sam Collier 2023
#pragma once

#include "goop/sys/platform/vulkan/Sampler.h"
#include "goop/sys/platform/vulkan/Utils.h"
#include <array>
#include <glm/glm.hpp>
#include <goop/sys/Renderer.h>
#include <goop/sys/Vertex.h>
#include <goop/sys/platform/vulkan/Buffers.h>
#include <goop/sys/platform/vulkan/Context.h>
#include <goop/sys/platform/vulkan/Pipeline.h>
#include <goop/sys/platform/vulkan/Swapchain.h>
#include <goop/sys/platform/vulkan/Sync.h>
#include <goop/sys/platform/vulkan/Texture.h>
#include <memory>
#include <mutex>
#include <optional>
#include <thread>
#include <vector>

#include <volk.h>

#include <goop/sys/platform/vulkan/QueueFamilyIndices.h>
#include <goop/sys/platform/vulkan/SwapchainSupportInfo.h>

struct ImGui_ImplVulkan_InitInfo;

namespace goop::sys::platform::vulkan
{
class Renderer_Vulkan : public Renderer
{
  public:
	// Subsystem interface
	int initialize() final;
	int destroy() final;

	// Renderer interface
	void beginFrame() final;
#ifdef GOOP_APPTYPE_EDITOR
	void render(float width = -1.f, float height = -1.f) final;
#else
	void render() final;
#endif
	void endFrame() final;
	void addToRenderQueue(uint32_t mesh, MeshLoader* meshLoader) final;
	void updateBuffers();

	void addTexture(unsigned char* pixels, int width, int height, const char* path) final;
	void removeTexture(const char* path) final;

#ifdef GOOP_APPTYPE_EDITOR
	ImTextureID getViewTexture() const final { return imgSet; }
#else
	ImTextureID getViewTexture() const final { return nullptr; }
#endif

  private:
	void initImGui();
	std::vector<const char*> getRequiredExtensions();
	VkExtent2D selectExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
	void updateUniformBuffer(Scene* scene, uint32_t currentFrame);

	void recreateSwapchain();

#ifdef GOOP_APPTYPE_EDITOR
	bool renderScene(uint32_t width, uint32_t height, uint32_t imageIndex);
	void renderFrame(uint32_t imageIndex);
#endif

	Context* ctx;
	UniformBuffer* uniformBuffer;
	Descriptor* descriptor;
	Pipeline* pipeline;
	Swapchain* swapchain;
	Sampler* sampler;
	Buffers* buffers;
	Sync* sync;

	VkDescriptorPool imguiPool;

	uint32_t currentFrame = 0;
	uint32_t imageIndex = 0;
	bool frameBufferResized = false;

	std::queue<uint32_t> oldQueue;
	std::vector<std::map<uint32_t, uint32_t>> oldInstanceCounts;

	std::map<std::string, Texture*> textures;

#ifdef GOOP_APPTYPE_EDITOR
	float oldWidth = -1.f;
	float oldHeight = -1.f;
	VkDescriptorSet imgSet;
	Texture* viewTexture;
#endif
};
} // namespace goop::sys::platform::vulkan

