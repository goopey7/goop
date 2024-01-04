#include <goop/sys/Physics.h>

namespace goop
{
static void applyImpulse(RigidbodyComponent& rbc, glm::vec3 impulse)
{
	sys::gPhysics->applyImpulse(&rbc, impulse);
}
} // namespace goop
