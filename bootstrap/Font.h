//----------------------------------------------------------------------------
// This class is used to load TrueType fonts using the stb_truetype library
// and stores the resulting glyphs in an OpenGL texture.
// 
// Example Usage:
//		aie::Font* font = new aie::Font("./font/consolas.ttf", 32);
//----------------------------------------------------------------------------
#pragma once

struct stbtt_bakedchar;

namespace aie 
{

class Font 
{
	friend class Renderer2D;

public:
	Font(const char* trueTypeFontFile, unsigned short fontHeight);
	~Font();

	// Returns the OpenGL texture handle.
	unsigned int GetTextureHandle() const { return m_glHandle; }

	// Returns width that a string would be when using this font.
	float GetStringWidth(const char* str);

	// Returns the height that a string would be when using this font.
	// Includes characters that go below starting height.
	float GetStringHeight(const char* str);

	// Uses pass-by-reference to get width and height at the same time.
	void GetStringSize(const char* str, float& width, float& height);

	// Returns a rectangle that fits the string when loaded with this font.
	// (x0, y0) is bottom left and (x1, y1) is top right.
	void GetStringRectangle(const char* str, float& x0, float& y0, float& x1, float& y1);

private:
	stbtt_bakedchar* m_glyphData;
	unsigned int m_glHandle;
	unsigned int m_pixelBufferHandle;
	unsigned short m_textureWidth;
	unsigned short m_textureHeight;
};

} // namespace aie