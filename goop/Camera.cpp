// Sam Collier 2023

#include "Camera.h"

using namespace goop;

Camera::Camera()
{
	forward = glm::vec3(0, 0, -1);
	up = glm::vec3(0, 1, 0);
	rotate(rotation);
	updateRotation();
	right = -right;
}

void Camera::translate(const glm::vec3& translation) { position += translation; }

void Camera::rotate(const glm::vec3& rotation) { this->rotation += rotation; }

void Camera::updateRotation()
{
	float cosP, cosY, cosR;
	float sinP, sinY, sinR;

	cosP = cosf(glm::radians(rotation.x));
	cosY = cosf(glm::radians(rotation.y));
	cosR = cosf(glm::radians(rotation.z));
	sinP = sinf(glm::radians(rotation.x));
	sinY = sinf(glm::radians(rotation.y));
	sinR = sinf(glm::radians(rotation.z));

	forward = glm::vec3(sinY * cosP, sinP, cosP * -cosY);

	up = glm::vec3(-cosY * sinR - sinY * sinP * cosR, cosP * cosR,
				   -sinY * sinR - sinP * cosR * -cosY);

	right = glm::cross(forward, up);
}

void Camera::setPosition(const glm::vec3& position) { this->position = position; }

void Camera::setRotation(const glm::vec3& rotation) { this->rotation = rotation; }

const glm::vec3& Camera::getPosition() const { return position; }

const glm::vec3& Camera::getRotation() const { return rotation; }

const glm::vec3& Camera::getForward() const { return forward; }

const glm::vec3& Camera::getUp() const { return up; }

const glm::vec3& Camera::getRight() const { return right; }
