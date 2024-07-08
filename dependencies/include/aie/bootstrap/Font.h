#pragma once

#include "Bootstrap.h"

namespace aie
{
	// a class that wraps up a True Type Font within an OpenGL texture
	class DLL Font
	{
		friend class Renderer2D;

	public:
		Font(const char* trueTypeFontFile, unsigned short fontHeight);
		~Font();

		// returns the OpenGL texture handle
		unsigned int GetTextureHandle() const;

		// returns size of string using this font
		float GetStringWidth(const char* str) const;

		// height includes characters that go below starting height
		float GetStringHeight(const char* str) const;
		void GetStringSize(const char* str, float& width, float& height) const;

		// returns a rectangle that fits the string, with x0y0 being bottom left, x1y1 top right
		void GetStringRectangle(const char* str, float& x0, float& y0, float& x1, float& y1) const;

	private:
		void* m_glyphData;
		unsigned int	m_glHandle, m_pixelBufferHandle;
		unsigned short	m_textureWidth, m_textureHeight;

	};

} // namespace aie