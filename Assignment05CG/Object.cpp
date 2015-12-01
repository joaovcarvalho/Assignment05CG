#include "Object.h"

Object::Object(Shader* shader, char* meshName, Texture* texture)
{
	this->texture = texture;
	this->meshName = meshName;

	load_mesh(meshName);
	generateObjectBufferMesh(shader->getProgramID(), this->vertexCounter);

	this->shader = shader;

	this->model = identity_mat4();
	position = vec3(0, 0, 0);
	rotation = vec3(0, 0, 0);
	scaleVec = vec3(1,1,1);

	this->childs = std::vector<Object*>();
}


Object::~Object()
{
}

vec3 Object::getPosition() {
	return this->position;
}

void Object::setPosition(vec3 v) {
	this->position = v;
}

vec3 Object::getRotation() {
	return this->rotation;
}

void Object::rotate(float x, float y, float z) {
	this->rotation += vec3(x, y, z);
}

void Object::scaleAll(float i) {
	this->scaleVec = vec3(i, i, i);
}

void Object::rotateAround(float x, float y, float z, vec3 origin) {
	this->rotationAroundDegree += vec3(x, y, z);
	this->rotationAroundOrigin = origin;
}

void Object::move(float x, float y, float z) {
	this->position += vec3(x, y, z);
}

void Object::setViewMatrix(mat4 v) {
	Object::view = v;
}

void Object::setProjectionMatrix(mat4 v) {
	Object::projection = v;
}

void Object::resetState() {
	this->model = identity_mat4();
	position = vec3(0, 0, 0);
	rotation = vec3(0, 0, 0);
}

void Object::addChild(Object* c) {
	this->childs.push_back(c);
}

void Object::display(mat4 fatherMatrix) {
	this->bindVBO();
	if(this->texture != NULL)
		this->texture->Bind(0);

	GLuint shaderProgramID = this->shader->getProgramID();
	glUseProgram(shaderProgramID);

	int matrix_location = glGetUniformLocation(shaderProgramID, "model");
	int view_mat_location = glGetUniformLocation(shaderProgramID, "view");
	int proj_mat_location = glGetUniformLocation(shaderProgramID, "proj");

	this->model = identity_mat4();

	this->model = scale(model, this->scaleVec);

	this->model = rotate_x_deg(this->model, rotation.v[0]);
	this->model = rotate_y_deg(this->model, rotation.v[1]);
	this->model = rotate_z_deg(this->model, rotation.v[2]);

	this->model = translate(this->model, position);

	mat4 resultMatrix = fatherMatrix*this->model;
	// update uniforms & draw
	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, this->projection.m);
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, this->view.m);
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, resultMatrix.m);

	glDrawArrays(GL_TRIANGLES, 0, this->vertexCounter);

	int i = 0;
	for (std::vector<Object*>::iterator it = this->childs.begin(); it != this->childs.end(); ++it) {
		(*it)->display(resultMatrix);
	}
}

void Object::bindVBO() {
	GLuint shaderProgramID = this->shader->getProgramID();
	loc1 = glGetAttribLocation(shaderProgramID, "vertex_position");
	loc2 = glGetAttribLocation(shaderProgramID, "vertex_normal");
	loc3 = glGetAttribLocation(shaderProgramID, "vertex_texture");
	
	if (this->vao == NULL) {
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		glEnableVertexAttribArray(loc1);
		glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);
		glVertexAttribPointer(loc1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

		glEnableVertexAttribArray(loc2);
		glBindBuffer(GL_ARRAY_BUFFER, vn_vbo);
		glVertexAttribPointer(loc2, 3, GL_FLOAT, GL_FALSE, 0, NULL);

		if (this->texture != NULL) {
			glEnableVertexAttribArray(loc3);
			glBindBuffer(GL_ARRAY_BUFFER, vt_vbo);
			glVertexAttribPointer(loc3, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		}
	}else{
		glBindVertexArray(vao);	
	}


}

bool Object::load_mesh(const char* file_name) {
	const aiScene* scene = aiImportFile(file_name, aiProcess_Triangulate);

	if (!scene) {
		fprintf(stderr, "ERROR: reading mesh %s\n", file_name);
		return false;
	}

	/*
	printf("  %i animations\n", scene->mNumAnimations);
	printf("  %i cameras\n", scene->mNumCameras);
	printf("  %i lights\n", scene->mNumLights);
	printf("  %i materials\n", scene->mNumMaterials);
	printf("  %i meshes\n", scene->mNumMeshes);
	printf("  %i textures\n", scene->mNumTextures);
	*/

	if (scene->HasMaterials())
	{
		for (unsigned int i = 0; i < scene->mNumMaterials; i++)
		{
			const aiMaterial* material = scene->mMaterials[i];

			if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
				aiString Path;
			}
		}
	}



	for (unsigned int m_i = 0; m_i < scene->mNumMeshes; m_i++) {
		const aiMesh* mesh = scene->mMeshes[m_i];
		//printf("    %i vertices in mesh\n", mesh->mNumVertices);
		this->vertexCounter = mesh->mNumVertices;

		for (unsigned int v_i = 0; v_i < mesh->mNumVertices; v_i++) {
			if (mesh->HasPositions()) {
				const aiVector3D* vp = &(mesh->mVertices[v_i]);
				//printf ("      vp %i (%f,%f,%f)\n", v_i, vp->x, vp->y, vp->z);
				g_vp.push_back(vp->x);
				g_vp.push_back(vp->y);
				g_vp.push_back(vp->z);
			}
			if (mesh->HasNormals()) {
				const aiVector3D* vn = &(mesh->mNormals[v_i]);
				//printf ("      vn %i (%f,%f,%f)\n", v_i, vn->x, vn->y, vn->z);
				g_vn.push_back(vn->x);
				g_vn.push_back(vn->y);
				g_vn.push_back(vn->z);
			}

			if (mesh->HasTextureCoords(0)) {
				const aiVector3D* vt = &(mesh->mTextureCoords[0][v_i]);
				//printf ("      vt %i (%f,%f)\n", v_i, vt->x, vt->y);
				g_vt.push_back(vt->x);
				g_vt.push_back(vt->y);
			}



			if (mesh->HasTangentsAndBitangents()) {
				// NB: could store/print tangents here
			}
		}
	}

	aiReleaseImport(scene);
	return true;
}

void Object::generateObjectBufferMesh(GLuint shaderProgramID, int numVertices) {
	/*----------------------------------------------------------------------------
	LOAD MESH HERE AND COPY INTO BUFFERS
	----------------------------------------------------------------------------*/
	if (numVertices < 1) {
		std::cerr << "Error: The mesh dont have positions or normals" << std::endl;
		return;
	}
	
	//Note: you may get an error "vector subscript out of range" if you are using this code for a mesh that doesnt have positions and normals
	//Might be an idea to do a check for that before generating and binding the buffer.
	glGenBuffers(1, &this->vp_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);
	glBufferData(GL_ARRAY_BUFFER, numVertices * 3 * sizeof(float), &g_vp[0], GL_STATIC_DRAW);

	glGenBuffers(1, &this->vn_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vn_vbo);
	glBufferData(GL_ARRAY_BUFFER, numVertices * 3 * sizeof(float), &g_vn[0], GL_STATIC_DRAW);

	if (this->texture != NULL) {
		glGenBuffers(1, &this->vt_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vt_vbo);
		glBufferData(GL_ARRAY_BUFFER, numVertices * 2 * sizeof(float), &g_vt[0], GL_STATIC_DRAW);
	}
}
