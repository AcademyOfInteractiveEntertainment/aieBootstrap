#include "Application.h"
#include "gl_core_4_4.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace aie 
{

Application* Application::m_instance = nullptr;

Application::Application(const char* title, int width, int height, bool fullscreen)
{
	// Create the game window.
	m_window = CreateGameWindow(title, width, height, fullscreen);

	// Initialise variables.
	m_gameOver = false;
	m_fps = 0;
	m_frames = 0;
	m_deltaTime = 0;
	m_fpsInterval = 0;
	m_prevTime = GetTime();
}

Application::~Application() 
{
	if (m_window)
		DestroyGameWindow();
}

void Application::Create(const char* title, int width, int height, bool fullscreen)
{ 
	if(!m_instance)
		m_instance = new Application(title, width, height, fullscreen); 
}

void Application::Destroy()
{ 
	delete m_instance; 
	m_instance = nullptr;
}

void Application::Update()
{
	// Update delta time.
	double currTime = GetTime();
	m_deltaTime = currTime - m_prevTime;
	m_prevTime = currTime;

	// Update fps every second.
	m_frames++;
	m_fpsInterval += m_deltaTime;
	if (m_fpsInterval >= 1.0f)
	{
		m_fps = m_frames;
		m_frames = 0;
		m_fpsInterval -= 1.0f;
	}

	// Update window events (input etc).
	glfwPollEvents();

	// Should the game exit?
	m_gameOver = m_gameOver || HasWindowClosed();
}

// Create the game window using the GLFW library
GLFWwindow* Application::CreateGameWindow(const char* title, int width, int height, bool fullscreen)
{
	// Load GLFW and make sure it loads successfully.
	if (glfwInit() == GL_FALSE)
		return nullptr;

	// If the application is fullscreen then we will display in on the primary screen.
	GLFWmonitor* screen = nullptr;
	if (fullscreen)
		screen = glfwGetPrimaryMonitor();

	// Create the window
	GLFWwindow* window = glfwCreateWindow(width, height, title, screen, nullptr);
	if (!window) 
	{
		glfwTerminate();
		return nullptr;
	}

	// Set the window to be the current "context" (the window to use for our game).
	glfwMakeContextCurrent(window);

	// Load the OpenGL functions and check that they loaded successfully.
	if (ogl_LoadFunctions() == ogl_LOAD_FAILED) 
	{
		glfwDestroyWindow(window);
		glfwTerminate();
		return nullptr;
	}

	// Assign a Lambda function to be called whenever the window is resized.
	glfwSetWindowSizeCallback(window, [](GLFWwindow*, int w, int h){ glViewport(0, 0, w, h); });

	// Setup the basic rendering settings.
	glClearColor(0, 0, 0, 1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	return window;
}

// Destroy the game window
void Application::DestroyGameWindow()
{
	//Shut down GLFW
	glfwDestroyWindow(m_window);
	glfwTerminate();
}

// Returns whether the window has been closed by the user.
bool Application::HasWindowClosed() 
{
	return glfwWindowShouldClose(m_window) == GL_TRUE;
}

// Clear the screen so that it's ready to render.
void Application::ClearScreen() 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void Application::SwapBuffers()
{
	glfwSwapBuffers(m_window);
}

// The background will be cleared to this colour when ClearScreen() is called.
void Application::SetBackgroundColour(float r, float g, float b, float a) 
{
	glClearColor(r, g, b, a);
}

// Enable or disable V-Sync.
void Application::SetVSync(bool enable) 
{
	glfwSwapInterval(enable ? 1 : 0);
}

// Enable or disable backface culling.
// Usually you want this off for 2D and on for 3D.
void Application::SetBackfaceCull(bool enabled)
{
	if(enabled)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);
}

// Hide or show the OS cursor.
void Application::SetShowCursor(bool visible)
{
	ShowCursor(visible);
}

// Get the current width of the window.
unsigned int Application::GetWindowWidth() const 
{
	int w = 0, h = 0;
	glfwGetWindowSize(m_window, &w, &h);
	return w;
}

// Get the current height of the window.
unsigned int Application::GetWindowHeight() const 
{
	int w = 0, h = 0;
	glfwGetWindowSize(m_window, &w, &h);
	return h;
}

// Get the time since the application started.
float Application::GetTime() const 
{
	return (float)glfwGetTime();
}

// Returns whether the application is minimised.
bool Application::GetMinimised() const
{
	return (glfwGetWindowAttrib(m_window, GLFW_ICONIFIED) != 0);
}

} // namespace aie
