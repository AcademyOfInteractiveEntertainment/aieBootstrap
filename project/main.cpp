#include "Application2D.h"

int main() {
	
	auto app = new Application2D();
	if (app->startup())
		app->run();
	app->shutdown();

	delete app;

	return 0;
}