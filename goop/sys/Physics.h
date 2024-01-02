#include "Subsystem.h"
#include <memory>
#include <goop/Components.h>

namespace goop::sys
{
class Physics : public Subsystem
{
	public:
		Physics() = default;
		virtual void simulate(float dt) = 0;
		virtual void addRigidBody(RigidbodyComponent* rbc, TransformComponent* tc) = 0;
		virtual void removeRigidBody(RigidbodyComponent* rbc) = 0;
};

extern const std::unique_ptr<Physics> gPhysics;
} // namespace goop::sys
