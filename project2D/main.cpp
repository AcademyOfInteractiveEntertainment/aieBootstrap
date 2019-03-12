//----------------------------------------------------------------------------
// This is the entry-point for your game.
// Creates and runs the Application2D class which contains the game loop.
//----------------------------------------------------------------------------
#include <crtdbg.h>
#include "Application2D.h"

int main() 
{
	// Check for memeory leaks.
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// Create the Application.
	Application2D* application = new Application2D("AIE", 1280, 720, false);

	// Run the game loop.
	application->Run();

	// Clean up.
	delete application;

	return 0;
}