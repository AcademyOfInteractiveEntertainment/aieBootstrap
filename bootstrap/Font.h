#pragma once

namespace aie {

class Font {

	friend class Renderer2D;

public:

	Font(const char* trueTypeFontFile, unsigned short fontHeight);
	~Font();

	unsigned int	getHandle() const { return m_glHandle; }

private:

	void*			m_glyphData;
	unsigned int	m_glHandle, m_pixelBufferHandle;
	unsigned short	m_sizeOfBytesX, m_sizeOfBytesY;
};

} // namespace aie