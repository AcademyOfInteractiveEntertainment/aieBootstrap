//----------------------------------------------------------------------------
// The Application class is a Singleton that creates the application's window 
// using GLFW and OpenGL and manages the general state of the application.
//----------------------------------------------------------------------------
#pragma once

// Forward declared structure for access to GLFW window.
struct GLFWwindow;

namespace aie 
{

class Application
{
public:
	// Returns access to the singleton instance.
	static Application* GetInstance() { return m_instance; }

	// Wipes the screen clear so that it is ready to begin drawing.
	void ClearScreen();

	// Sets the colour that will be used to clear the sceen.
	void SetBackgroundColour(float r, float g, float b, float a = 1.0f);

	// Show or hide the OS cursor.
	void SetShowCursor(bool visible);

	// Enable or disable V-Sync.
	void SetVSync(bool enabled);

	// Enable or disable backface culling (enabled by default).
	// Usually you want this off for 2D and on for 3D.
	void SetBackfaceCull(bool enabled);

	// Tells the application to begin shutting down.
	void Quit() { m_gameOver = true; }
	bool GetQuitting() const { return m_gameOver; }

	// Access to the GLFW window.
	GLFWwindow* GetWindowPtr() const { return m_window; }

	// Query if the window has been closed by the user.
	bool HasWindowClosed();

	// Query whether the application is minimised.
	bool GetMinimised() const;

	// Query the width / height of the game window.
	unsigned int GetWindowWidth() const;
	unsigned int GetWindowHeight() const;
	
	// Time related functions.
	float GetTime() const;
	float GetDeltaTime() const { return (float)m_deltaTime; }
	unsigned int GetFPS() const { return m_fps; }

private:
	friend class Game;
	
	// Singleton.
	static void Create(const char* title, int width, int height, bool fullscreen);
	static void Destroy();
	static Application* m_instance;

	// Private Constructor and Destructor to prevent creation by any means other than the Singleton pattern.
	Application(const char* title, int width, int height, bool fullscreen);
	virtual ~Application();

	// Creating and destroying the game windows.
	virtual GLFWwindow* CreateGameWindow(const char* title, int width, int height, bool fullscreen);
	virtual void DestroyGameWindow();

	// Swaps the render buffers to present the frame to the user.
	void SwapBuffers();

	// Update the Application.
	void Update();

	// Misc Variables.
	GLFWwindow*	m_window;
	bool m_gameOver;

	// Calculating framerate.
	double m_prevTime;
	double m_deltaTime;
	unsigned int m_fps;
	unsigned int m_frames;
	double m_fpsInterval;
};

} // namespace aie