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
} // namespace goop::sys::platform::vulkan
