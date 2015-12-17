#pragma once
#include "Object.h"
class RigidBody :
	public Object
{
public:
	RigidBody(Shader* shader, char* meshName, Texture* texture = NULL);
	RigidBody();
	~RigidBody();

	void update();
	void setViewDir(vec3);
	void setVelocity(float);
	virtual RigidBody* clone();

	void RigidBody::applyForce(float f);
private: 
	float velocity;

	vec3 viewDir;

	vec3 angularVelocity;
	vec3 angularAcceleration;
};

