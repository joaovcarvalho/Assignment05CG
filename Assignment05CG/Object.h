#pragma once

#include "stdafx.h"
#include "Shader.h"
#include "Texture.h"

class Object
{
private:
	static int vboId;
	unsigned int vao = NULL;
	unsigned int vp_vbo = 0;
	unsigned int vn_vbo = 0;
	unsigned int vt_vbo = 0;

	vec3 position;
	vec3 rotation;
	vec3 scaleVec;

	vec3 rotationAroundDegree;
	vec3 rotationAroundOrigin;

	Shader* shader;
	Texture* texture;

	char* meshName;

	GLuint loc1, loc2, loc3;
	int vertexCounter = 0;
	std::vector<float> g_vp, g_vn, g_vt;

	mat4 model;
	static mat4 view, projection;

	std::vector<Object*> childs;

	void bindVBO();

public:
	Object(Shader* shader, char* meshName, Texture* texture = NULL);

	void generateObjectBufferMesh(GLuint shaderProgramID, int numVertices);
	bool load_mesh(const char* file_name);

	void display(mat4 fatherMatrix = identity_mat4());

	void rotate(float x, float y, float z);
	void rotateAround(float x, float y, float z, vec3 origin);
	void move(float x, float y, float z);
	void scaleAll(float x);

	vec3 getPosition();
	vec3 getRotation();

	void setPosition(vec3);

	static void setViewMatrix(mat4 v);
	static void setProjectionMatrix(mat4 v);

	void resetState();

	void addChild(Object*);

	~Object();
};

