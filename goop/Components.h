#include <glm/glm.hpp>
#include <string>

namespace goop
{
struct TransformComponent
{
	glm::mat4 transform;
	TransformComponent() : transform(glm::mat4(1.0f)) {}
	TransformComponent(const glm::mat4& transform) : transform(transform) {}
};

struct TagComponent
{
	std::string tag;
	TagComponent() : tag("Entity") {}
	TagComponent(const std::string& tag) : tag(tag) {}
};
} // namespace goop
