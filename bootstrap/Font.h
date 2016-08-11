#pragma once

namespace aie {

// a class that wraps up a True Type Font within an OpenGL texture
class Font {

	friend class Renderer2D;

public:

	Font(const char* trueTypeFontFile, unsigned short fontHeight);
	~Font();

	// returns the OpenGL texture handle
	unsigned int	getTextureHandle() const { return m_glHandle; }

private:

	void*			m_glyphData;
	unsigned int	m_glHandle, m_pixelBufferHandle;
	unsigned short	m_textureWidth, m_textureHeight;
};

} // namespace aie