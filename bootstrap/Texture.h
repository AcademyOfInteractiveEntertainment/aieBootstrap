#pragma once

#include <string>

namespace aie {

class Texture {
public:

	enum Format : unsigned int {
		RED	= 1,
		RG,
		RGB,
		RGBA
	};

	Texture();
	Texture(const char* filename);
	Texture(unsigned int width, unsigned int height, Format format, unsigned char* pixels = nullptr);
	virtual ~Texture();

	bool load(const char* filename);

	const std::string& getFilename() const { return m_filename; }

	unsigned int getWidth() const { return m_width; }
	unsigned int getHeight() const { return m_height; }

	unsigned int getFormat() const { return m_format; }

	unsigned int getHandle() const { return m_glHandle; }

protected:

	std::string		m_filename;
	unsigned int	m_width;
	unsigned int	m_height;
	unsigned int	m_glHandle;
	unsigned int	m_format;
};

} // namespace aie