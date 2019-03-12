//----------------------------------------------------------------------------
// This class is used to load textures for drawing as sprites.
// 
// Example Usage:
//		aie::Texture texture = new aie::Texture("./textures/ship.png");
//----------------------------------------------------------------------------
#pragma once

#include <string>

namespace aie 
{

class Texture 
{
public:
	enum Format : unsigned int 
	{
		NONE = 0,
		RED,
		RG,
		RGB,
		RGBA
	};

	Texture();
	Texture(const char* filename);
	Texture(unsigned int width, unsigned int height, Format format, unsigned char* pixels = nullptr);
	virtual ~Texture();

	// Load a jpg, bmp, png, or tga.
	bool Load(const char* filename);
	void Unload();

	// Creates a texture that can be filled with pixels.
	void Create(unsigned int width, unsigned int height, Format format, unsigned char* pixels = nullptr);

	// Returns the filename or nullptr if not loaded from a file.
	const char* GetFilename() const { return m_filename; }

	// Binds the texture to the specified slot.
	void Bind(unsigned int slot) const;

	// Returns the OpenGL texture handle.
	unsigned int GetHandle() const { return m_glHandle; }

	// Get the texture's properties.
	unsigned int GetWidth() const { return m_width; }
	unsigned int GetHeight() const { return m_height; }
	unsigned int GetFormat() const { return m_format; }

	// Get the pixel data from the texture.
	const unsigned char* GetPixels() const { return m_loadedPixels; }

protected:
	char*			m_filename;
	unsigned int	m_width;
	unsigned int	m_height;
	unsigned int	m_glHandle;
	unsigned int	m_format;
	unsigned char*	m_loadedPixels;
};

} // namespace aie