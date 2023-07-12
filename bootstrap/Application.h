#pragma once

#include "Bootstrap.h"

// forward declared structure for access to GLFW window
struct GLFWwindow;

namespace aie {

	// this is the pure-virtual base class that wraps up an application for us.
	// we derive our own applications from this class
	class Application {
	public:

		DLL Application();
		DLL virtual ~Application();

		// creates a window and begins the game loop which calls update() and draw() repeatedly
		// it first calls startup() and if that succeeds it then starts the loop,
		// ending with shutdown() if m_gameOver is true
		DLL void run(const char* title, int width, int height, bool fullscreen);

		// these functions must be implemented by a derived class
		virtual bool startup() = 0;
		virtual void shutdown() = 0;
		virtual void update(float deltaTime) = 0;
		virtual void draw() = 0;

		// wipes the screen clear to begin a frame of drawing
		DLL void clearScreen();

		// sets the colour that the sceen is cleared to
		DLL void setBackgroundColour(float r, float g, float b, float a = 1.0f);

		// show or hide the OS cursor
		DLL void setShowCursor(bool visible);

		// enable or disable v-sync
		DLL void setVSync(bool enabled);

		// sets m_gameOver to true which will close the application safely when the frame ends
		void quit() { m_gameOver = true; }

		// access to the GLFW window
		GLFWwindow* getWindowPtr() const { return m_window; }

		// query if the window has been closed somehow
		DLL bool hasWindowClosed();

		// returns the frames-per-second that the loop is running at
		unsigned int getFPS() const { return m_fps; }

		// returns the width / height of the game window
		DLL unsigned int getWindowWidth() const;
		DLL unsigned int getWindowHeight() const;

		// returns time since application started
		DLL float getTime() const;

	protected:

		DLL virtual bool createWindow(const char* title, int width, int height, bool fullscreen);
		DLL virtual void destroyWindow();

		DLL void InitImGui();
		DLL void ShutdownImGui();
		DLL void ImGuiNewFrame();
		DLL void ImGuiRender();

		GLFWwindow* m_window;

		// if set to false, the main game loop will exit
		bool			m_gameOver;

		unsigned int	m_fps;

	};

} // namespace aie