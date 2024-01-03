#include "Physics_Bullet.h"
#include <glm/gtc/quaternion.hpp>

#ifdef GOOP_PHYSICS_BULLET
const std::unique_ptr<goop::sys::Physics> goop::sys::gPhysics =
	std::make_unique<goop::sys::platform::bullet::Physics_Bullet>();
#endif

using namespace goop::sys::platform::bullet;

int Physics_Bullet::initialize()
{
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);
	overlappingPairCache = new btDbvtBroadphase();
	solver = new btSequentialImpulseConstraintSolver;

	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver,
												collisionConfiguration);

	dynamicsWorld->setGravity(btVector3(0, -0.81f, 0));

	bIsInitialized = true;
	return 0;
}

int Physics_Bullet::destroy()
{
	for (auto& [rbc, rb] : rigidBodies)
	{
		dynamicsWorld->removeRigidBody(rb);
		delete rb->getMotionState();
		delete rb;
		delete collisionShapes[rbc];
	}
	delete dynamicsWorld;
	delete solver;
	delete overlappingPairCache;
	delete dispatcher;
	delete collisionConfiguration;
	rigidBodies.clear();
	collisionShapes.clear();
	transforms.clear();
	bIsInitialized = false;
	return 0;
}

void Physics_Bullet::simulate(float dt)
{
	dynamicsWorld->stepSimulation(dt, 10);
	for (auto& [rbc, rb] : rigidBodies)
	{
		btTransform transform;
		rb->getMotionState()->getWorldTransform(transform);
		btVector3 pos = transform.getOrigin();
		transforms[rbc]->position = glm::vec3(pos.x(), pos.y(), pos.z());
		auto rot = transform.getRotation();
		glm::vec3 euler =
			glm::degrees(glm::eulerAngles(glm::quat(rot.w(), rot.x(), rot.y(), rot.z())));
		transforms[rbc]->rotation = euler;
	}
}

void Physics_Bullet::addRigidBody(RigidbodyComponent* rbc, TransformComponent* tc)
{
	btCollisionShape* shape =
		new btBoxShape(btVector3(rbc->box[0] / 2.f, rbc->box[1] / 2.f, rbc->box[2] / 2.f));
	collisionShapes[rbc] = shape;

	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(btVector3(tc->position.x, tc->position.y, tc->position.z));
	auto rot = glm::quat(glm::radians(tc->rotation));
	transform.setRotation(btQuaternion(rot.x, rot.y, rot.z, rot.w));
	transforms[rbc] = tc;

	btScalar mass(rbc->mass);

	btVector3 localInertia(0, 0, 0);

	if (mass != 0.0f)
	{
		shape->calculateLocalInertia(mass, localInertia);
	}

	btDefaultMotionState* motionState = new btDefaultMotionState(transform);

	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);
	rigidBodies[rbc] = new btRigidBody(rbInfo);
	dynamicsWorld->addRigidBody(rigidBodies[rbc]);
}

void Physics_Bullet::removeRigidBody(RigidbodyComponent* rbc)
{
	if (bIsInitialized)
	{
		dynamicsWorld->removeRigidBody(rigidBodies[rbc]);
		delete rigidBodies[rbc]->getMotionState();
		delete rigidBodies[rbc];
		delete collisionShapes[rbc];
		rigidBodies.erase(rbc);
		collisionShapes.erase(rbc);
		transforms.erase(rbc);
	}
}

Physics_Bullet::~Physics_Bullet() { destroy(); }
