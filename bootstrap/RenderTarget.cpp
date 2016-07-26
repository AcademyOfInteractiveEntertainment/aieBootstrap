#include "RenderTarget.h"
#include "gl_core_4_4.h"
#include <vector>

namespace aie {

RenderTarget::RenderTarget(unsigned int targetCount, unsigned int width, unsigned int height)
	: m_width(width),
	m_height(height),
	m_targetCount(targetCount),
	m_targets(nullptr) {

	// setup and bind a framebuffer object
	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	// create and attach textures
	if (targetCount > 0) {

		m_targets = new Texture[m_targetCount];

		std::vector<GLenum> drawBuffers = {};

		for (unsigned int i = 0; i < m_targetCount; ++i) {

			m_targets[i].create(m_width, m_height, Texture::RGBA);

			drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + i);

			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
								 m_targets[i].getHandle(), 0);
		}

		glDrawBuffers(drawBuffers.size(), drawBuffers.data());
	}

	// setup and bind a 24bit depth buffer as a render buffer
	glGenRenderbuffers(1, &m_rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24,
						  m_width, m_height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
							  GL_RENDERBUFFER, m_rbo);

	// unbind the FBO so that we can render to the back buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

RenderTarget::~RenderTarget() {
	delete[] m_targets;
	glDeleteRenderbuffers(1, &m_rbo);
	glDeleteFramebuffers(1, &m_fbo);
}

void RenderTarget::bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
}

void RenderTarget::unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

} // namespace aie