#pragma once
#include "Object.h"
class Planet :
	public Object
{
public:
	Planet(Shader* shader, char* meshName, Texture* texture = NULL);
	~Planet();

private:
	void createProceduralBump();
};

