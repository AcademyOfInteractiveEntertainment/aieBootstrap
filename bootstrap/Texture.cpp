//----------------------------------------------------------------------------
// This class is used to load textures for drawing as sprites.
//----------------------------------------------------------------------------
#include "gl_core_4_4.h"
#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <assert.h>

namespace aie 
{

Texture::Texture() 
{
	m_filename = nullptr;
	m_width = 0;
	m_height = 0;
	m_glHandle = 0;
	m_format = 0;
	m_loadedPixels = nullptr;
}

Texture::Texture(const char* filename, Filtering filter) : Texture()
{
	Load(filename, filter);
}

Texture::Texture(unsigned int width, unsigned int height, Format format, unsigned char* pixels, Filtering filter) : Texture()
{
	Create(width, height, format, pixels, filter);
}

Texture::~Texture() 
{
	Unload();
}

bool Texture::Load(const char* filename, Filtering filter)
{
	// If there is already a texture loaded in this object, delete it first.
	Unload();

	int x = 0;
	int y = 0;
	int comp = 0;
	m_loadedPixels = stbi_load(filename, &x, &y, &comp, STBI_default);

	if (m_loadedPixels) 
	{
		glGenTextures(1, &m_glHandle);
		glBindTexture(GL_TEXTURE_2D, m_glHandle);

		switch (comp) 
		{
		case STBI_grey:
			m_format = RED;
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, x, y, 0, GL_RED, GL_UNSIGNED_BYTE, m_loadedPixels);
			break;

		case STBI_grey_alpha:
			m_format = RG;
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, x, y, 0, GL_RG, GL_UNSIGNED_BYTE, m_loadedPixels);
			break;

		case STBI_rgb:
			m_format = RGB;
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, m_loadedPixels);
			break;

		case STBI_rgb_alpha:
			m_format = RGBA;
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_loadedPixels);
			break;

		default:
			break;
		};

		switch (filter)
		{
		case FILTER_LINEAR:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			break;

		case FILTER_NEAREST:
		default:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			break;
		}

		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);

		m_width = (unsigned int)x;
		m_height = (unsigned int)y;

		//Textures must be a multiple of 2 or they will have artifacts.
		//Procedural textures can have a width/height of 1.
		assert((m_width == 1 || (m_width % 2) == 0) && "Texture width must be a multiple of 2");
		assert((m_height == 1 || (m_height % 2) == 0) && "Texture height must be a multiple of 2");

		size_t length = strlen(filename) + 1;
		m_filename = new char[length];
		strcpy_s(m_filename, length, filename);

		return true;
	}
	return false;
}

// Unload the texture and delete all allocations.
void Texture::Unload()
{
	if (m_glHandle != 0)
	{
		glDeleteTextures(1, &m_glHandle);
		m_glHandle = 0;
	}

	if (m_loadedPixels)
	{
		stbi_image_free(m_loadedPixels);
		m_loadedPixels = nullptr;
	}

	if (m_filename)
	{
		delete[] m_filename;
		m_filename = nullptr;
	}

	m_width = 0;
	m_height = 0;
}

void Texture::Create(unsigned int width, unsigned int height, Format format, unsigned char* pixels, Filtering filter)
{
	// If there is already a texture loaded in this object, delete it first.
	Unload();

	m_width = width;
	m_height = height;
	m_format = format;

	//Textures must be a multiple of 2 or they will have artifacts.
	//Procedural textures can have a width/height of 1.
	assert((m_width == 1 || (m_width % 2) == 0) && "Texture width must be a multiple of 2");
	assert((m_height == 1 || (m_height % 2) == 0) && "Texture height must be a multiple of 2");

	glGenTextures(1, &m_glHandle);
	glBindTexture(GL_TEXTURE_2D, m_glHandle);

	switch (filter)
	{
	case FILTER_LINEAR:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		break;

	case FILTER_NEAREST:
	default:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		break;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	switch (m_format) 
	{
	case RED:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_width, m_height, 0, GL_RED, GL_UNSIGNED_BYTE, pixels);
		break;

	case RG:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, m_width, m_height, 0, GL_RG, GL_UNSIGNED_BYTE, pixels);
		break;

	case RGB:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
		break;

	case RGBA:
	default:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
		break;
	};

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Bind(unsigned int slot) const 
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_glHandle);
}

} // namespace aie