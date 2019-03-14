//----------------------------------------------------------------------------
// The Game class is an abstract base class that initialises most of the
// essential systems and runs the game loop.
// We derive it into Game2D for 2D games and Game3D for 3D games.
//----------------------------------------------------------------------------
#pragma once

namespace aie
{

class Game
{
public:
	Game(const char* title, int width, int height, bool fullscreen);
	virtual ~Game();

	// Begins the game loop which calls Update() and Draw() repeatedly.
	void Run();

	// These functions must be implemented by the derived class.
	virtual void Update(float deltaTime) = 0;
	virtual void Draw() = 0;
};

} // namespace aie