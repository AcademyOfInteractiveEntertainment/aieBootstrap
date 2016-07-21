#include "RenderTexture.h"
#include "gl_core_4_4.h"

namespace aie {

RenderTexture::RenderTexture(unsigned int width, unsigned int height)
	: Texture(width, height, Texture::RGBA) {

	glBindTexture(GL_TEXTURE_2D, m_glHandle);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_glHandle, 0);

	glFramebufferParameteri(GL_DRAW_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_WIDTH, width);
	glFramebufferParameteri(GL_DRAW_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_HEIGHT, height);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(status != GL_FRAMEBUFFER_COMPLETE) {
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

RenderTexture::~RenderTexture() {
	glDeleteFramebuffers(1, &m_fbo);
}

unsigned int RenderTexture::getFrameBufferHandle() {
	return m_fbo;
}

} // namespace aie