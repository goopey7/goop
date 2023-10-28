// Sam Collier 2023
#pragma once
#include <cstdint>
#include <optional>
#include <vector>
#include <volk.h>

namespace goop::sys::platform::vulkan
{
struct QueueFamilyIndices
{
	// it's possible that the device uses a separate queue for drawing and presentation
	// but this approach will at least handle both cases
	// TODO prefer that graphics and presentation are in same queue for improved performance
	// TODO investigate separating transfers to a separate queue
	std::optional<uint32_t> graphics; // graphics queues support transfer operations
	std::optional<uint32_t> present;
	// std::optional<uint32_t> transfer;
	bool isComplete() { return graphics.has_value() && present.has_value(); }
};

inline QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
{
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	for (int i = 0; i < queueFamilies.size(); i++)
	{
		const auto& queueFamily = queueFamilies[i];
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.graphics = i;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

		if (presentSupport)
		{
			indices.present = i;
		}

		if (indices.isComplete())
		{
			break;
		}
	}

	return indices;
}
} // namespace goop::sys::platform::vulkan
