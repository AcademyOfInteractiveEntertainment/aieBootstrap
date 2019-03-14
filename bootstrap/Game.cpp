#include "Game.h"
#include "Application.h"
#include "Input.h"
#include "imgui_glfw3.h"

namespace aie
{

Game::Game(const char* title, int width, int height, bool fullscreen)
{
	// Create the Application.
	Application::Create(title, width, height, fullscreen);

	// Create the Input manager.
	Input::Create();

	// Initalise ImGui.
	GLFWwindow* window = Application::GetInstance()->GetWindowPtr();
	ImGui_Init(window, true);
}

Game::~Game()
{
	// Destroy ImGui
	ImGui_Shutdown();

	// Destroy the input system
	Input::Destroy();

	// Destroy the Application
	Application::Destroy();
}

void Game::Run()
{
	Application* application = Application::GetInstance();

	// Start game loop if successfully initialised.
	if (application)
	{
		// Loop while game is running.
		while (!application->GetQuitting())
		{
			// Clear input.
			Input::GetInstance()->ClearStatus();

			// Update the application. Must be after input has been cleared.
			application->Update();

			// Skip if minimised.
			if (application->GetMinimised())
				continue;

			// Clear ImGui
			ImGui_NewFrame();

			// Update the Application
			Update(application->GetDeltaTime());

			// Draw the Application
			Draw();

			// Draw ImGui last.
			ImGui::Render();

			// Present backbuffer to the monitor
			application->SwapBuffers();
		}
	}
}

} // namespace aie