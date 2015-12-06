#include "RigidBody.h"

RigidBody::RigidBody(Shader* shader, char* meshName, Texture* texture):Object(shader, meshName, texture)
{
	velocity = 0.5f;

	angularVelocity = vec3(0, 0, 0);
	angularAcceleration = vec3(0, 0, 0);
}

void RigidBody::applyForce(vec3 f) {

}

void RigidBody::update() {
	this->setPosition(this->getPosition()+ this->viewDir*velocity);
}

void RigidBody::setViewDir(vec3 d) {
	this->viewDir = d;
}

RigidBody::~RigidBody()
{
}
