#include <glm/glm.hpp>

namespace goop::sys::platform::vulkan
{
struct UniformBufferObject
{
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
};
} // namespace goop::sys::platform::vulkan
