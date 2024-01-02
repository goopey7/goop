#include "Physics_Bullet.h"

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

	dynamicsWorld->setGravity(btVector3(0, -9.81f, 0));

	return 0;
}

int Physics_Bullet::destroy()
{
	delete dynamicsWorld;
	delete solver;
	delete overlappingPairCache;
	delete dispatcher;
	delete collisionConfiguration;
	return 0;
}

Physics_Bullet::~Physics_Bullet() { destroy(); }

void Physics_Bullet::simulate(float dt)
{
	dynamicsWorld->stepSimulation(dt, 10);
	for (auto& [rbc, rb] : rigidBodies)
	{
		btTransform transform;
		rb->getMotionState()->getWorldTransform(transform);
		btVector3 pos = transform.getOrigin();
		btQuaternion rot = transform.getRotation();
		transforms[rbc]->position = glm::vec3(pos.x(), pos.y(), pos.z());
		// convert to euler angles
		float yaw, pitch, roll;
		btMatrix3x3(rot).getEulerYPR(yaw, pitch, roll);
		transforms[rbc]->rotation = glm::vec3(pitch, yaw, roll);
	}
}

void Physics_Bullet::addRigidBody(RigidbodyComponent* rbc, TransformComponent* tc)
{
	btCollisionShape* shape = new btBoxShape(btVector3(rbc->box[0], rbc->box[1], rbc->box[2]));
	collisionShapes[rbc] = shape;

	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(btVector3(tc->position.x, tc->position.y, tc->position.z));
	transforms[rbc] = tc;

	btScalar mass(rbc->mass);

	btVector3 localInertia(0, 0, 0);

	if (mass != 0.0f)
	{
		shape->calculateLocalInertia(mass, localInertia);
	}

	btDefaultMotionState* motionState = new btDefaultMotionState(transform);
	motionStates[rbc] = motionState;

	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);
	rigidBodies[rbc] = new btRigidBody(rbInfo);
	dynamicsWorld->addRigidBody(rigidBodies[rbc]);
}

void Physics_Bullet::removeRigidBody(RigidbodyComponent* rbc) {}
