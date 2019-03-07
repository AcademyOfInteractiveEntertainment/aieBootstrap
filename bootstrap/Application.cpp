//----------------------------------------------------------------------------
// The Application class is an abstract base class that initialises most of the
// essential systems. It also creates the application's window using GLFW and OpenGL.
// We derive it into Application2D for 2D games and Application3D for 3D games.
//----------------------------------------------------------------------------
#include "Application.h"
#include "gl_core_4_4.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include "Input.h"
#include "imgui_glfw3.h"

namespace aie 
{

Application::Application(const char* title, int width, int height, bool fullscreen)
{
	m_gameOver = false;
	m_fps = 0;
	m_window = CreateGameWindow(title, width, height, fullscreen);

	if (m_window)
	{
		// Create the Input manager.
		Input::Create();

		// Initalise ImGui.
		ImGui_Init(m_window, true);
	}
}

Application::~Application() 
{
	if (m_window)
	{
		// Destroy ImGui
		ImGui_Shutdown();

		// Destroy the input system
		Input::Destroy();

		DestroyGameWindow();
	}
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
	glEnable(GL_CULL_FACE);
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

void Application::Run() 
{
	// Start game loop if successfully initialised.
	if (m_window)
	{
		// Variables for timing.
		double prevTime = GetTime();
		double currTime = 0;
		double deltaTime = 0;
		unsigned int frames = 0;
		double fpsInterval = 0;

		// Loop while game is running.
		while (!m_gameOver) 
		{
			// Update delta time.
			currTime = GetTime();
			deltaTime = currTime - prevTime;
			prevTime = currTime;

			// Update fps every second.
			frames++;
			fpsInterval += deltaTime;
			if (fpsInterval >= 1.0f)
			{
				m_fps = frames;
				frames = 0;
				fpsInterval -= 1.0f;
			}

			// Clear input.
			Input::GetInstance()->ClearStatus();

			// Update window events (input etc).
			glfwPollEvents();

			// Skip if minimised.
			if (glfwGetWindowAttrib(m_window, GLFW_ICONIFIED) != 0)
				continue;

			// Clear ImGui
			ImGui_NewFrame();

			// Update the Application
			Update(float(deltaTime));

			// Draw the Application
			Draw();

			// Draw ImGui last.
			ImGui::Render();

			// Present backbuffer to the monitor
			glfwSwapBuffers(m_window);

			// Should the game exit?
			m_gameOver = m_gameOver || HasWindowClosed();
		}
	}
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

} // namespace aie
