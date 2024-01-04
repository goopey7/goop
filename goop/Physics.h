#pragma once
#include <goop/sys/Physics.h>

namespace goop
{
static void applyImpulse(RigidbodyComponent& rbc, glm::vec3 impulse)
{
	sys::gPhysics->applyImpulse(&rbc, impulse);
}
static void setVelocity(goop::Entity& e, glm::vec3 velocity, bool relative = false)
{
	if (!e.hasComponent<RigidbodyComponent>())
	{
		throw std::runtime_error("Entity does not have a RigidbodyComponent");
		return;
	}
	if (relative)
	{
		Scene* scene = e.getScene();
		glm::vec3 forward = scene->getCurrentCamera()->getForward();
		forward.y = 0;
		glm::vec3 right = scene->getCurrentCamera()->getRight();
		glm::vec3 up = scene->getCurrentCamera()->getUp();
		velocity = forward * -velocity.z + right * velocity.x + up * velocity.y;
	}
	auto& rbc = e.getComponent<RigidbodyComponent>();
	sys::gPhysics->setVelocity(&rbc, velocity);
}
} // namespace goop
