#pragma once

struct GLFWwindow;

namespace aie {

class RenderTarget;

class Application {
public:

	Application();
	virtual ~Application();

	void run(const char* title, int width, int height, bool fullscreen);

	virtual bool startup() = 0;
	virtual void shutdown() = 0;

	virtual void update(float deltaTime) = 0;
	virtual void draw() = 0;

	void setVSync(bool enabled);

	void setShowCursor(bool visible);

	// sets m_gameOver to true which will close the application safely
	void quit() { m_gameOver = true; }

	GLFWwindow* getWindowPtr() const { return m_window; }

	// returns the frames-per-seconds
	unsigned int getFPS() const { return m_fps; }

	// returns the width/height of the game window
	unsigned int getWindowWidth()	const;
	unsigned int getWindowHeight()	const;

	// returns the width/height of the render target or
	// width/height of the window if render target is null
	unsigned int getViewWidth() const;
	unsigned int getViewHeight() const;

	void setRenderTarget(RenderTarget *renderTarget);
	RenderTarget* getRenderTarget() const { return m_currentRenderTarget; }

	void setBackgroundColour(float r, float g, float b, float a = 1.0f);

protected:

	virtual bool createWindow(const char* title, int width, int height, bool fullscreen);
	virtual void destroyWindow();

	bool hasWindowClosed();

	void clearScreen();

	GLFWwindow*	m_window;

	// if set to false, the main game loop will exit
	bool m_gameOver;
	
	unsigned int m_fps;

	RenderTarget*	m_currentRenderTarget;
};

} // namespace aie