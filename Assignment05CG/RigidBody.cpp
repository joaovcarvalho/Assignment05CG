#include "RigidBody.h"

RigidBody::RigidBody(Shader* shader, char* meshName, Texture* texture):Object(shader, meshName, texture)
{
	acceleration = vec3(0, 0, 0);
	velocity = vec3(0, 0, 0);

	angularVelocity = vec3(0, 0, 0);
	angularAcceleration = vec3(0, 0, 0);
}

void RigidBody::applyForce(vec3 f) {
	acceleration += f;
}

void RigidBody::update() {
	velocity += acceleration;
	this->setPosition(this->getPosition() + velocity);
}


RigidBody::~RigidBody()
{
}
