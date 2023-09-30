// Sam Collier 2023
#pragma once
#include <vector>
#include <vulkan/vulkan.h>

namespace goop::sys::platform::vulkan
{
struct SwapchainSupportInfo
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

inline SwapchainSupportInfo getSwapchainSupportInfo(VkPhysicalDevice device, VkSurfaceKHR surface)
{
	SwapchainSupportInfo details;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

	if (formatCount != 0)
	{
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

	if (presentModeCount != 0)
	{
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount,
												  details.presentModes.data());
	}

	return details;
}

} // namespace goop::sys::platform::vulkan
