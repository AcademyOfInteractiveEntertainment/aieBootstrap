//----------------------------------------------------------------------------
// This class is used to load textures for drawing as sprites.
// 
// Example Usage:
//		When loading sprites use:
//		aie::Texture texture = new aie::Texture("./textures/ship.png");
//
//		When loading textures to apply to 3D models instead use:
//		aie::Texture texture = new aie::Texture("./textures/wall.png", GL_LINEAR);
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

	enum Filtering : unsigned int
	{
		FILTER_NEAREST,
		FILTER_LINEAR
	};

	Texture();
	Texture(const char* filename, Filtering filter = FILTER_NEAREST);
	Texture(unsigned int width, unsigned int height, Format format, unsigned char* pixels = nullptr, Filtering filter = FILTER_NEAREST);
	virtual ~Texture();

	// Load a jpg, bmp, png, or tga.
	bool Load(const char* filename, Filtering filter);
	void Unload();

	// Creates a texture that can be filled with pixels.
	void Create(unsigned int width, unsigned int height, Format format, unsigned char* pixels = nullptr, Filtering filter = FILTER_NEAREST);

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