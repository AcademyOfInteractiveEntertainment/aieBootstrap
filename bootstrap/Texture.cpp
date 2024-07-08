#include "aie/bootstrap/Texture.h"

#include <glew/glew.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

namespace aie
{
	Texture::Texture()
		: m_filename{ new char[] {"none"} }, m_width{ 0 }, m_height{ 0 }, m_glHandle{ 0 }, m_format{ 0 },
		m_loadedPixels{ nullptr }
	{
	}

	Texture::Texture(const char* filename)
		: m_filename{ nullptr }, m_width{ 0 }, m_height{ 0 }, m_glHandle{ 0 }, m_format{ 0 },
		m_loadedPixels{ nullptr }
	{
		Load(filename);
	}

	Texture::Texture(unsigned int width, unsigned int height, EFormat format, const unsigned char* pixels)
		: m_filename{ new char[] {"none"} }, m_width{ width }, m_height{ height }, m_format{ format },
		m_loadedPixels{ nullptr }
	{
		Create(width, height, format, pixels);
	}

	Texture::~Texture()
	{
		if (m_glHandle != 0)
			glDeleteTextures(1, &m_glHandle);

		if (m_loadedPixels != nullptr)
		{
			stbi_image_free(m_loadedPixels);
			m_loadedPixels = nullptr;
		}

		delete[] m_filename;
	}

	bool Texture::Load(const char* filename)
	{
		if (m_glHandle != 0)
		{
			glDeleteTextures(1, &m_glHandle);
			m_glHandle = 0;
			m_width = 0;
			m_height = 0;
			m_filename = new char[] {"none"};
		}

		int x = 0, y = 0, comp = 0;
		m_loadedPixels = stbi_load(filename, &x, &y, &comp, STBI_default);

		if (m_loadedPixels != nullptr)
		{
			glGenTextures(1, &m_glHandle);
			glBindTexture(GL_TEXTURE_2D, m_glHandle);
			switch (comp)
			{
			case STBI_grey:
				{
					m_format = R;
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, x, y, 0, GL_RED, GL_UNSIGNED_BYTE, m_loadedPixels);
					break;
				}
			case STBI_grey_alpha:
				{
					m_format = RG;
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, x, y, 0, GL_RG, GL_UNSIGNED_BYTE, m_loadedPixels);
					break;
				}
			case STBI_rgb:
				{
					m_format = RGB;
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, m_loadedPixels);
					break;
				}
			case STBI_rgb_alpha:
				{
					m_format = RGBA;
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_loadedPixels);
					break;
				}
			default:
				{
					break;
				}
			}

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glGenerateMipmap(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);

			m_width = static_cast<unsigned int>(x);
			m_height = static_cast<unsigned int>(y);

			m_filename = new char[strlen(filename)];
			strcpy_s(m_filename, strlen(filename) + 1, filename);

			return true;
		}
		return false;
	}

	void Texture::Create(unsigned int width, unsigned int height, EFormat format, const unsigned char* pixels)
	{
		if (m_glHandle != 0)
		{
			glDeleteTextures(1, &m_glHandle);
			m_glHandle = 0;
			m_filename = new char[]{ "none" };
		}

		m_width = width;
		m_height = height;
		m_format = format;

		glGenTextures(1, &m_glHandle);
		glBindTexture(GL_TEXTURE_2D, m_glHandle);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		switch (m_format)
		{
		case R:
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, static_cast<int>(m_width), static_cast<int>(m_height),
					0, GL_RED, GL_UNSIGNED_BYTE, pixels);
				break;
			}
		case RG:
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, static_cast<int>(m_width), static_cast<int>(m_height),
					0, GL_RG, GL_UNSIGNED_BYTE, pixels);
				break;
			}
		case RGB:
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, static_cast<int>(m_width), static_cast<int>(m_height),
					0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
				break;
			}
		case RGBA:
		default:
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, static_cast<int>(m_width), static_cast<int>(m_height), 
					0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
				break;
			}
		}

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	const char* Texture::GetFilename() const
	{
		return m_filename;
	}

	void Texture::Bind(unsigned int slot) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_glHandle);
	}

	unsigned int Texture::GetHandle() const
	{
		return m_glHandle;
	}

	unsigned int Texture::GetWidth() const
	{
		return m_width;
	}

	unsigned int Texture::GetHeight() const
	{
		return m_height;
	}

	unsigned int Texture::GetFormat() const
	{
		return m_format;
	}

	const unsigned char* Texture::GetPixels() const
	{
		return m_loadedPixels;
	}
} // namespace aie