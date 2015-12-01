#pragma once
#include "Object.h"
class RigidBody :
	public Object
{
public:
	RigidBody(Shader* shader, char* meshName, Texture* texture = NULL);
	~RigidBody();

	void applyForce(vec3);
	void update();

private: 
	vec3 acceleration;
	vec3 velocity;

	vec3 angularVelocity;
	vec3 angularAcceleration;
};

