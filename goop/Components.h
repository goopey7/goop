#pragma once

#include <glm/glm.hpp>
#include <string>

namespace goop
{
struct TransformComponent
{
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
	TransformComponent() : position(0.0f), rotation(0.0f), scale(1.0f) {}
	TransformComponent(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
		: position(position), rotation(rotation), scale(scale)
	{
	}
};

struct TagComponent
{
	std::string tag;
	TagComponent() : tag("Entity") {}
	TagComponent(const std::string& tag) : tag(tag) {}
};

struct MeshComponent
{
	std::string path;
	uint32_t id;
	MeshComponent(const std::string& path) : path(path) {}
};

} // namespace goop
