#include "Application3D.h"

int main() {
	
	auto app = new Application3D();
	app->run("AIE", 1280, 720, false);
	delete app;

	return 0;
}