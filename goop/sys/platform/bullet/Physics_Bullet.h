// Sam Collier 2023
#pragma once

#include <btBulletDynamicsCommon.h>
#include <goop/sys/Physics.h>
#include <map>

namespace goop::sys::platform::bullet
{
class Physics_Bullet : public Physics
{
  public:
	Physics_Bullet() = default;
	~Physics_Bullet();

	// subsystem interface
	int initialize() final;
	int destroy() final;

	// physics interface
	void simulate(float dt) final;
	void addRigidBody(RigidbodyComponent* rbc, TransformComponent* tc) final;
	void removeRigidBody(RigidbodyComponent* rbc) final;
	void applyImpulse(RigidbodyComponent* rbc, glm::vec3 impulse) final;

  private:
	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btBroadphaseInterface* overlappingPairCache;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* dynamicsWorld;

	std::map<RigidbodyComponent*, btRigidBody*> rigidBodies;
	std::map<RigidbodyComponent*, btCollisionShape*> collisionShapes;
	std::map<RigidbodyComponent*, TransformComponent*> transforms;
};
} // namespace goop::sys::platform::bullet
