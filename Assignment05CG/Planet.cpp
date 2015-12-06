#include "Planet.h"


Planet::Planet(Shader* shader, char* meshName, Texture* texture):Object(shader, meshName, texture)
{
	//createProceduralBump();
	generateObjectBufferMesh(shader->getProgramID(), this->vertexCounter);
}

void Planet::createProceduralBump() {
	for (int j = 0; j < this->vertexCounter; j++)
	{
		int i = j * 3;
		float randomAspect = rand() % 10;
		vec3 vertex = vec3(this->g_vp.at(i), this->g_vp.at(i + 1), this->g_vp.at(i + 2));
		vec3 normal = vec3(this->g_vn.at(i), this->g_vn.at(i + 1), this->g_vn.at(i + 2));
		normal = normal + (normalise(normal)*randomAspect);
		this->g_vn[i] = normal.v[0];
		this->g_vn[i + 1] = normal.v[1];
		this->g_vn[i + 2] = normal.v[2];
	}
}

Planet::~Planet()
{
}
