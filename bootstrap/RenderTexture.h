#pragma once

#include "Texture.h"

namespace aie {

class RenderTexture : public Texture {
public:

	RenderTexture(unsigned int width, unsigned int height);
	virtual ~RenderTexture();

	unsigned int getFrameBufferHandle();

protected:

	unsigned int m_fbo;
	unsigned int m_rbo;

};

}