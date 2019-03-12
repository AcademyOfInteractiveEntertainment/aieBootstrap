//----------------------------------------------------------------------------
// The Application class is an abstract base class that initialises most of the
// essential systems. It also creates the application's window using GLFW and OpenGL.
// We derive it into Application2D for 2D games and Application3D for 3D games.
//----------------------------------------------------------------------------
#pragma once

// Forward declared structure for access to GLFW window.
struct GLFWwindow;

namespace aie 
{

class Application 
{
public:
	Application(const char* title, int width, int height, bool fullscreen);
	virtual ~Application();

	// Begins the game loop which calls Update() and Draw() repeatedly.
	void Run();

	// These functions must be implemented by the derived class.
	virtual void Update(float deltaTime) = 0;
	virtual void Draw() = 0;

	// Wipes the screen clear so that it is ready to begin drawing.
	void ClearScreen();

	// Sets the colour that will be used to clear the sceen.
	void SetBackgroundColour(float r, float g, float b, float a = 1.0f);

	// Show or hide the OS cursor.
	void SetShowCursor(bool visible);

	// Enable or disable V-Sync.
	void SetVSync(bool enabled);

	// Sets m_gameOver to true which will close the application safely when the frame ends.
	void Quit() { m_gameOver = true; }

	// Access to the GLFW window.
	GLFWwindow* GetWindowPtr() const { return m_window; }

	// Query if the window has been closed by the user.
	bool HasWindowClosed();

	// Returns the frames-per-second that the loop is running at.
	unsigned int GetFPS() const { return m_fps; }

	// Returns the width / height of the game window.
	unsigned int GetWindowWidth() const;
	unsigned int GetWindowHeight() const;
	
	// Returns time since application started.
	float GetTime() const;

private:
	virtual GLFWwindow* CreateGameWindow(const char* title, int width, int height, bool fullscreen);
	virtual void DestroyGameWindow();

	GLFWwindow*	m_window;
	bool m_gameOver;
	unsigned int m_fps;
};

} // namespace aie