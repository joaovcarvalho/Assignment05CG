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
	void setViewDir(vec3);

private: 
	float velocity;

	vec3 viewDir;

	vec3 angularVelocity;
	vec3 angularAcceleration;
};

