#include "Application.h"
#include "gl_core_4_4.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include "RenderTexture.h"
#include "Input.h"
#include "SoundManager.h"
#include "imgui_glfw3.h"

namespace aie {

Application::Application()
	: m_window(nullptr),
	m_gameOver(false),
	m_fps(0),
	m_currentRenderTarget(nullptr) {
}

Application::~Application() {
}

bool Application::createWindow(const char* title, int width, int height, bool fullscreen) {

	if (glfwInit() == GL_FALSE)
		return false;

	m_window = glfwCreateWindow(width, height, title, (fullscreen ? glfwGetPrimaryMonitor() : nullptr), nullptr);
	if (m_window == nullptr) {
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(m_window);

	if (ogl_LoadFunctions() == ogl_LOAD_FAILED) {
		glfwDestroyWindow(m_window);
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(m_window, [](GLFWwindow*, int w, int h){ glViewport(0, 0, w, h); });

	glClearColor(0, 0, 0, 1);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// start input manager
	Input::create();

	// start sound manager
	SoundManager::create();

	// imgui
	ImGui_Init(m_window, true);
	
	return true;
}

void Application::destroyWindow() {

	ImGui_Shutdown();
	SoundManager::destroy();
	Input::destroy();

	glfwDestroyWindow(m_window);
	glfwTerminate();
}

void Application::run(const char* title, int width, int height, bool fullscreen) {

	// start game loop if successfully initialised
	if (createWindow(title,width,height, fullscreen) &&
		startup()) {

		double prevTime = glfwGetTime();
		double currTime = 0;
		unsigned int fpsCount = 0;
		double fpsInterval = 0;

		while (!m_gameOver) {

			currTime = glfwGetTime();
			fpsInterval += currTime - prevTime;
			fpsCount++;

			// update the fps every second
			if (fpsInterval >= 1.0f) {
				m_fps = fpsCount;
				fpsCount = 0;
				fpsInterval -= 1.0f;
			}

			glfwPollEvents();

			ImGui_NewFrame();

			update((float)(currTime - prevTime));

			draw();

			// draw IMGUI after all 3D stuff
			ImGui::Render();

			glfwSwapBuffers(m_window);

			m_gameOver = m_gameOver || glfwWindowShouldClose(m_window) == GLFW_TRUE;
			prevTime = currTime;
		}
	}

	// cleanup
	shutdown();
	destroyWindow();
}

bool Application::hasWindowClosed() {
	return glfwWindowShouldClose(m_window) == GL_TRUE;
}

void Application::clearScreen() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void Application::setBackgroundColour(float r, float g, float b, float a) {
	glClearColor(r, g, b, a);
}

void Application::setVSync(bool enable) {
	if (enable)
		glfwSwapInterval(1);
	else
		glfwSwapInterval(0);
}

void Application::setShowCursor(bool visible) {
	ShowCursor(visible);
}

unsigned int Application::getWindowWidth() const {
	int w = 0, h = 0;
	glfwGetWindowSize(m_window, &w, &h);
	return w;
}

unsigned int Application::getWindowHeight() const {
	int w = 0, h = 0;
	glfwGetWindowSize(m_window, &w, &h);
	return h;
}

unsigned int Application::getViewWidth() const {
	if (m_currentRenderTarget != nullptr)
		return m_currentRenderTarget->getWidth();
	else
		return getWindowWidth();
}

unsigned int Application::getViewHeight() const {
	if (m_currentRenderTarget != nullptr)
		return m_currentRenderTarget->getHeight();
	else
		return getWindowHeight();
}

void Application::setRenderTarget(RenderTexture* renderTarget) {
	m_currentRenderTarget = renderTarget;
	if (m_currentRenderTarget != nullptr) {
		glBindFramebuffer(GL_FRAMEBUFFER, m_currentRenderTarget->getFrameBufferHandle());
	}
	else {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

} // namespace aie