#pragma once

struct GLFWwindow;

namespace aie {

class RenderTexture;

class Application {
public:

	Application();
	virtual ~Application();

	void run();

	virtual bool startup() = 0;
	virtual void shutdown() = 0;

	virtual void update(float deltaTime) = 0;
	virtual void draw() = 0;

	void setVSync(bool enabled);

	void setShowCursor(bool visible);

	// sets m_gameOver = true. This will close the application safelly
	void quit() { m_gameOver = true; }

	GLFWwindow* getWindowPtr() const { return m_window; }

	// returns the frames-per-seconds
	unsigned int getFPS() const { return m_fps; }

	// returns the width/height of the game window
	unsigned int getWindowWidth()	const;
	unsigned int getWindowHeight()	const;

	// returns the with/height of the render target
	// if no alternative render target is specified, the width/height 
	// of the backbuffer is returned (aks the window size)
	unsigned int getViewWidth() const;
	unsigned int getViewHeight() const;

	void setRenderTarget(RenderTexture *renderTarget);
	RenderTexture* getRenderTarget() const { return m_currentRenderTarget; }

	void setBackgroundColour(float r, float g, float b, float a = 1.0f);

protected:

	virtual bool createWindow(const char* title, int width, int height, bool fullscreen);
	virtual void destroyWindow();

	bool hasWindowClosed();

	void clearScreen();

	GLFWwindow*	m_window;

	// if set to false, the main game loop will exit
	bool m_gameOver;
	
	// fps calculation
	unsigned int m_fps;

	RenderTexture*	m_currentRenderTarget;
};

} // namespace aie