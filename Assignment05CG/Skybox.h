#pragma once
#include "stdafx.h"
#include "Texture.h"
#include "Shader.h"

class Skybox
{
private:
	GLuint vao = 0;
	Shader* shader;

	void generateCubeVAO();
	void generateCubeTexture();
	void bindVBO();

	GLuint ibo_cube_elements;
	GLuint vp_vbo;
	int NumVertices;

	std::string textureName;
	void bindTexture(unsigned int unit);

	GLuint idTexture;
public:
	Skybox(char* dir, char* image, Shader* shader);
	~Skybox();

	void display(mat4 perspective, mat4 view);
};

