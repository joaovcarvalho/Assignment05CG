#include "RigidBody.h"

RigidBody::RigidBody(Shader* shader, char* meshName, Texture* texture):Object(shader, meshName, texture)
{
	velocity = 0.5f;

	angularVelocity = vec3(0, 0, 0);
	angularAcceleration = vec3(0, 0, 0);
}

RigidBody::RigidBody() :Object() {
	
}

void RigidBody::applyForce(float f){
	velocity += f;
}

void RigidBody::setVelocity(float f) {
	this->velocity = f;
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

RigidBody* RigidBody::clone() {
	RigidBody* obj = new RigidBody();
	obj->setPosition(this->position);
	for (std::vector<Object*>::iterator it = this->childs.begin(); it != this->childs.end(); ++it) {
		obj->addChild(*it);
	}
	this->bindVBO();

	obj->setRotation(this->rotation);
	obj->setVAO(this->vao);
	obj->setShader(this->shader);
	obj->setVertexCounter(this->vertexCounter);

	obj->setVertices(this->g_vp);
	obj->setVPVBO(this->vp_vbo);
	obj->setBoundingBox(this->vbo_vertices_bounding_box, 
						this->ibo_elements,
						this->sizeBoundingBox,
						this->centerBoundingBox,
						this->minBoundingBox,
						this->maxBoundingBox);

	return obj;

}
