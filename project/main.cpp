#include "Application2D.h"
#include "Application3D.h"

int main() {
	
	auto app = new Application2D(); // or Application3D or whatever
	app->run("AIE", 1280, 720, false);
	delete app;

	return 0;
}