#pragma once

#include "stdafx.h"
#include "Shader.h"
#include "Texture.h"

class Object
{
protected:
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
	GLuint ibo_elements;
	GLuint vbo_vertices_bounding_box;

	vec3 sizeBoundingBox;
	vec3 centerBoundingBox;

public:
	Object(Shader* shader, char* meshName, Texture* texture = NULL);
	Object();

	void Object::setVAO(float v);
	void Object::setShader(Shader* s);
	void Object::setVertexCounter(int n);
	void Object::setSizeBoudingBox(vec3);
	void Object::setCenterBoudingBox(vec3);
	void Object::setVertices(std::vector<float>);
	void generateBoundingBox();
	void Object::setBoundingBox(GLuint, GLuint, vec3, vec3);
	void Object::setVPVBO(GLuint);

	std::vector<float> getVertices();

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
	void setRotation(vec3);

	static void setViewMatrix(mat4 v);
	static void setProjectionMatrix(mat4 v);

	void resetState();

	void addChild(Object*);

	Object* clone();
	void drawBoundingBox();

	~Object();
};

