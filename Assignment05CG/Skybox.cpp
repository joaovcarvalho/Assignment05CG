#include "Skybox.h"
#include "SOIL.h"
#include "Object.h"
#include <cassert>

Skybox::Skybox(char* dir, char* image, Shader* shader)
{
	this->shader = shader;
	std::string textureLocation;
	textureLocation.append(dir);
	textureLocation.append(image);
	this->textureName = textureLocation;
	this->generateCubeVAO();
	this->generateCubeTexture();
}

void Skybox::generateCubeTexture() {
	this->idTexture = SOIL_load_OGL_cubemap
		(
			"skybox.jpg",
			"skybox.jpg",
			"skybox.jpg",
			"skybox.jpg",
			"skybox.jpg",
			"skybox.jpg",
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS
			);

	if (0 == this->idTexture)
	{
		printf("SOIL loading error: '%s'\n", SOIL_last_result());
	}
}

void Skybox::generateCubeVAO() {
	float points[] = {
  		-10.0f,  10.0f, -10.0f,
  		-10.0f, -10.0f, -10.0f,
   		10.0f, -10.0f, -10.0f,
   		10.0f, -10.0f, -10.0f,
   		10.0f,  10.0f, -10.0f,
  		-10.0f,  10.0f, -10.0f,
  
  		-10.0f, -10.0f,  10.0f,
  		-10.0f, -10.0f, -10.0f,
  		-10.0f,  10.0f, -10.0f,
		-10.0f,  10.0f, -10.0f,
		-10.0f,  10.0f,  10.0f,
		-10.0f, -10.0f,  10.0f,
  
		10.0f, -10.0f, -10.0f,
		 10.0f, -10.0f,  10.0f,
		 10.0f,  10.0f,  10.0f,
		 10.0f,  10.0f,  10.0f,
		 10.0f,  10.0f, -10.0f,
		 10.0f, -10.0f, -10.0f,
   
		-10.0f, -10.0f,  10.0f,
		-10.0f,  10.0f,  10.0f,
		 10.0f,  10.0f,  10.0f,
		 10.0f,  10.0f,  10.0f,
		 10.0f, -10.0f,  10.0f,
		-10.0f, -10.0f,  10.0f,
  
		-10.0f,  10.0f, -10.0f,
		 10.0f,  10.0f, -10.0f,
		 10.0f,  10.0f,  10.0f,
		 10.0f,  10.0f,  10.0f,
		-10.0f,  10.0f,  10.0f,
		-10.0f,  10.0f, -10.0f,
  
		-10.0f, -10.0f, -10.0f,
		-10.0f, -10.0f,  10.0f,
		 10.0f, -10.0f, -10.0f,
		 10.0f, -10.0f, -10.0f,
		-10.0f, -10.0f,  10.0f,
		 10.0f, -10.0f,  10.0f
	};

	for(int i = 0; i < (3*36); i++){
		points[i] *= 100;
	}	
	
	int numVertices = 36;

	glGenBuffers(1, &this->vp_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);
	glBufferData(GL_ARRAY_BUFFER, numVertices * 3 * sizeof(float), &points, GL_STATIC_DRAW);
}

void Skybox::bindTexture(unsigned int unit) {
	assert(unit >= 0 && unit <= 31);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->idTexture);
}

void Skybox::display(mat4 perspective, mat4 view) {

	//glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	glDepthMask(GL_FALSE);
	GLuint shaderProgramID = this->shader->getProgramID();
	glUseProgram(shaderProgramID);
	this->bindVBO();
	this->bindTexture(0);

	int matrix_location = glGetUniformLocation(shaderProgramID, "model");
	int view_mat_location = glGetUniformLocation(shaderProgramID, "view");
	int proj_mat_location = glGetUniformLocation(shaderProgramID, "proj");
	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, perspective.m);
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, view.m);
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, identity_mat4().m);

	glDrawArrays(GL_TRIANGLES, 0, 36);

	glDepthMask(GL_TRUE);
}

void Skybox::bindVBO() {
	GLuint shaderProgramID = this->shader->getProgramID();
	GLuint loc1 = glGetAttribLocation(shaderProgramID, "vertex_position");

	if(this->vao == 0){
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glEnableVertexAttribArray(loc1);
		glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);
		glVertexAttribPointer(loc1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	}else{
		glBindVertexArray(vao);
	}
}

Skybox::~Skybox()
{
}
