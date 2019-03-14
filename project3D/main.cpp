//----------------------------------------------------------------------------
// This is the entry-point for your game.
// Creates and runs the Application3D class which contains the game loop.
//----------------------------------------------------------------------------
#include <crtdbg.h>
#include "Game3D.h"

int main() 
{
	// Check for memeory leaks.
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// Create the Application.
	Game3D* game = new Game3D("AIE", 1280, 720, false);

	// Run the game loop.
	game->Run();

	// Clean up.
	delete game;

	return 0;
}