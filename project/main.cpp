#include "Application2D.h"
#include "Application3D.h"

int main() {
	
//	auto app = new Application2D();
	auto app = new Application3D();
	if (app->startup())
		app->run();
	app->shutdown();

	delete app;

	return 0;
}