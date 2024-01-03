// Sam Collier 2023
#pragma once

#include <glm/glm.hpp>

namespace goop
{
class Camera
{
  public:
	Camera(const Camera&) = delete;
	Camera& operator=(const Camera&) = delete;
	Camera();

	void updateRotation();

	void translate(const glm::vec3& translation);
	void rotate(const glm::vec3& rotation);

	void setPosition(const glm::vec3& position);
	void setRotation(const glm::vec3& rotation);

	const glm::vec3& getPosition() const;
	const glm::vec3& getRotation() const;
	const glm::vec3& getForward() const;
	const glm::vec3& getUp() const;
	const glm::vec3& getRight() const;

  private:
	glm::vec3 position;
	glm::vec3 forward;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 rotation;
};
}
