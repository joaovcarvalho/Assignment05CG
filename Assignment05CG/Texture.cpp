#include "Texture.h"
#include "SOIL.h"
#include <cassert>

Texture::Texture(const char* fileName)
{
	int width, height;

	unsigned char * data;

	this->id = SOIL_load_OGL_texture
		(
			fileName,
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_TEXTURE_REPEATS
			);

	if (0 == this->id)
	{
		printf("SOIL loading error: '%s'\n", SOIL_last_result());
	}
}

void Texture::Bind(unsigned int unit) {
	assert(unit >= 0 && unit <= 31);

	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, this->id);
}


Texture::~Texture()
{
}
