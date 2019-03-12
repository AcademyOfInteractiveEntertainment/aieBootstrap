//----------------------------------------------------------------------------
// This class is used to load TrueType fonts using the stb_truetype library
// and stores the resulting glyphs in an OpenGL texture.
//----------------------------------------------------------------------------
#include "gl_core_4_4.h"
#include "Font.h"
#include <stdio.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

namespace aie 
{

Font::Font(const char* trueTypeFontFile, unsigned short fontHeight)  
{
	m_glyphData = nullptr;
	m_glHandle = 0;
	m_pixelBufferHandle = 0;
	m_textureWidth = 0;
	m_textureHeight = 0;

	// Open the font file.
	FILE* file = nullptr;
	fopen_s(&file, trueTypeFontFile, "rb");
	if (file != nullptr) 
	{
		unsigned char* ttf_buffer = new unsigned char[4096 * 1024];

		// Read in the font and then close the file.
		fread(ttf_buffer, 1, 4096 * 1024, file);
		fclose(file);

		// Determine size of texture image
		m_textureWidth = fontHeight / 16 * 256;
		m_textureHeight = fontHeight / 16 * 256;

		if (fontHeight <= 16) 
		{
			m_textureWidth = 256;
			m_textureHeight = 256;
		}
		
		if (m_textureWidth > 2048)
			m_textureWidth = 2048;
		if (m_textureHeight > 2048)
			m_textureHeight = 2048;

		// Generate the OpenGL texture for the font.
		glGenBuffers(1, &m_pixelBufferHandle);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pixelBufferHandle);
		glBufferData(GL_PIXEL_UNPACK_BUFFER, m_textureWidth * m_textureHeight, nullptr, GL_STREAM_COPY);
		unsigned char* tempBitmapData = (GLubyte*)glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, 
																   m_textureWidth * m_textureHeight,
																   GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

		m_glyphData = new stbtt_bakedchar[256];
		memset(m_glyphData, 0, sizeof(stbtt_bakedchar) * 256);
		stbtt_BakeFontBitmap(ttf_buffer, 0, fontHeight, tempBitmapData, m_textureWidth, m_textureHeight, 0, 256, m_glyphData);

		glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);

		glGenTextures(1, &m_glHandle);
		glBindTexture(GL_TEXTURE_2D, m_glHandle);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, m_textureWidth, m_textureHeight, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

		delete[] ttf_buffer;
	}
}

Font::~Font() 
{
	delete[] m_glyphData;

	glDeleteTextures(1, &m_glHandle);
	glDeleteBuffers(1, &m_pixelBufferHandle);
}

float Font::GetStringWidth(const char* str) 
{
	float width = 0.0f;
	float height = 0.0f;

	GetStringSize(str, width, height);

	return width;
}

float Font::GetStringHeight(const char* str) 
{
	float width = 0.0f;
	float height = 0.0f;

	GetStringSize(str, width, height);
	
	return height;
}

void Font::GetStringSize(const char* str, float& width, float& height) 
{
	stbtt_aligned_quad Q = {};
	float low = 9999999, high = -9999999;
	float xPos = 0.0f;
	float yPos = 0.0f;

	while (*str != 0) 
	{
		stbtt_GetBakedQuad(m_glyphData,	m_textureWidth,	m_textureHeight, *str, &xPos, &yPos, &Q, 1);

		low = low > Q.y0 ? Q.y0 : low;
		high = high < Q.y1 ? Q.y1 : high;

		str++;
	}

	height = high - low;
	width = Q.x1;
}

void Font::GetStringRectangle(const char* str, float& x0, float& y0, float& x1, float& y1) 
{
	stbtt_aligned_quad Q = {};
	y1 = 9999999, y0 = -9999999;
	x0 = 9999999, x1 = -9999999;
	float xPos = 0.0f;
	float yPos = 0.0f;

	while (*str != 0) 
	{
		stbtt_GetBakedQuad(m_glyphData, m_textureWidth,	m_textureHeight, *str, &xPos, &yPos, &Q, 1);

		y1 = y1 > Q.y0 ? Q.y0 : y1;
		y0 = y0 < Q.y1 ? Q.y1 : y0;

		x1 = x1 < Q.x1 ? Q.x1 : x1;
		x0 = x0 > Q.x0 ? Q.x0 : x0;

		str++;
	}

	y0 *= -1;
	y1 *= -1;
}

} // namepace aie