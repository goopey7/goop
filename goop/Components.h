#pragma once

#include <cstring>
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
	std::string texturePath;
	uint32_t textureId;
	MeshComponent(const std::string& path, const std::string& texturePath)
		: path(path), texturePath(texturePath)
	{
	}
};

struct RigidbodyComponent
{
	float mass;
	float box[3];
	RigidbodyComponent() : mass(1.0f)
	{
		box[0] = 1.0f;
		box[1] = 1.0f;
		box[2] = 1.0f;
	}
	RigidbodyComponent(float mass, float box[3]) : mass(mass)
	{
		memcpy(this->box, box, sizeof(float) * 3);
	}
};

} // namespace goop
