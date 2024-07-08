#include "aie/bootstrap/Application.h"

#include <aie/bootstrap/Input.h>
#include <glew/glew.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

namespace aie
{
	Application::Application()
		: m_window{ nullptr }, m_gameOver{ false }, m_fps{ 0 }
	{
	}

	Application::~Application() = default;

	bool Application::CreateWindow(const char* title, const int width, const int height, const bool fullscreen)
	{
		if (glfwInit() == GL_FALSE)
			return false;

		m_window = glfwCreateWindow(width, height, title, fullscreen ? glfwGetPrimaryMonitor() : nullptr, nullptr);
		if (m_window == nullptr) 
		{
			glfwTerminate();
			return false;
		}

		glfwMakeContextCurrent(m_window);

		glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK)
		{
			glfwDestroyWindow(m_window);
			glfwTerminate();
			return false;
		}

		glfwSetWindowSizeCallback(m_window, [](GLFWwindow*, int w, int h) { glViewport(0, 0, w, h); });

		glClearColor(0, 0, 0, 1);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// start input manager
		Input::Create();

		InitImGui();

		return true;
	}

	void Application::DestroyWindow()
	{
		ShutdownImGui();
		Input::Destroy();

		glfwDestroyWindow(m_window);
		glfwTerminate();
	}

	void Application::InitImGui()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

		ImGui::StyleColorsDark();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		GLFWwindow* window = glfwGetCurrentContext();
		// Setup Platform/Renderer backends
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 130");
	}

	void Application::ShutdownImGui()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void Application::ImGuiNewFrame()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void Application::ImGuiRender()
	{
		const ImGuiIO& io = ImGui::GetIO();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Update and Render additional Platform Windows
		// (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
		//  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}

	void Application::Run(const char* title, const int width, const int height, const bool fullscreen)
	{
		// start game loop if successfully initialised
		if (CreateWindow(title, width, height, fullscreen) && Startup()) 
		{
			// variables for timing
			double prevTime = glfwGetTime();
			unsigned int frames = 0;
			double fpsInterval = 0;

			// loop while game is running
			while (!m_gameOver) 
			{
				// Update delta time
				const double currTime = glfwGetTime();
				double deltaTime = currTime - prevTime;
				if (deltaTime > 0.1f)
					deltaTime = 0.1f;

				prevTime = currTime;

				// clear input
				Input::GetInstance()->ClearStatus();

				// Update window events (input etc)
				glfwPollEvents();

				// skip if minimised
				if (glfwGetWindowAttrib(m_window, GLFW_ICONIFIED) != 0)
					continue;

				// Update fps every second
				frames++;
				fpsInterval += deltaTime;
				if (fpsInterval >= 1.0f) 
				{
					m_fps = frames;
					frames = 0;
					fpsInterval -= 1.0f;
				}

				// clear ImGui
				ImGuiNewFrame();

				Update(static_cast<float>(deltaTime));

				Draw();

				// Draw ImGui last
				ImGuiRender();

				//present back buffer to the monitor
				glfwSwapBuffers(m_window);

				// should the game exit?
				m_gameOver = m_gameOver || glfwWindowShouldClose(m_window) == GLFW_TRUE;
			}
		}

		// cleanup
		Shutdown();
		DestroyWindow();
	}

	bool Application::HasWindowClosed() const
	{
		return glfwWindowShouldClose(m_window) == GL_TRUE;
	}

	void Application::ClearScreen()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	void Application::SetBackgroundColour(const float r, const float g, const float b, const float a)
	{
		glClearColor(r, g, b, a);
	}

	void Application::SetVSync(const bool enabled)
	{
		glfwSwapInterval(enabled ? 1 : 0);
	}

	void Application::Quit()
	{
		m_gameOver = true;
	}

	GLFWwindow* Application::GetWindowPtr() const
	{
		return m_window;
	}

	void Application::SetShowCursor(const bool visible) const
	{
		glfwSetInputMode(m_window, GLFW_CURSOR, visible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN);
	}

	unsigned int Application::GetWindowWidth() const
	{
		int w = 0, h = 0;
		glfwGetWindowSize(m_window, &w, &h);
		return w;
	}

	unsigned int Application::GetWindowHeight() const
	{
		int w = 0, h = 0;
		glfwGetWindowSize(m_window, &w, &h);
		return h;
	}

	float Application::GetTime() const
	{
		return static_cast<float>(glfwGetTime());
	}

} // namespace aie
