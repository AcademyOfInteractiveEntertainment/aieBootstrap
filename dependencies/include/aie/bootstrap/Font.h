#pragma once

#include "Bootstrap.h"

namespace aie {

	// a class that wraps up a True Type Font within an OpenGL texture
	class Font {

		friend class Renderer2D;

	public:

		DLL Font(const char* trueTypeFontFile, unsigned short fontHeight);
		DLL ~Font();

		// returns the OpenGL texture handle
		unsigned int	GetTextureHandle() const { return m_glHandle; }

		// returns size of string using this font
		DLL float GetStringWidth(const char* str);

		// height includes characters that go below starting height
		DLL float GetStringHeight(const char* str);
		DLL void GetStringSize(const char* str, float& width, float& height);

		// returns a rectangle that fits the string, with x0y0 being bottom left, x1y1 top right
		DLL void GetStringRectangle(const char* str, float& x0, float& y0, float& x1, float& y1);

	private:

		void* m_glyphData;
		unsigned int	m_glHandle, m_pixelBufferHandle;
		unsigned short	m_textureWidth, m_textureHeight;
	};

} // namespace aie