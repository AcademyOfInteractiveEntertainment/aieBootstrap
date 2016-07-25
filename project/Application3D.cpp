#include "Application3D.h"
#include "Camera.h"
#include "Gizmos.h"
#include "Input.h"
#include <glm/glm.hpp>

using glm::vec3;
using glm::vec4;
using glm::mat4;
using aie::Gizmos;

Application3D::Application3D() {

}

Application3D::~Application3D() {

}

bool Application3D::startup() {
	
	setBackgroundColour(0.25f, 0.25f, 0.25f);

	// initialise gizmo primitive counts
	Gizmos::create(10000, 10000, 10000, 10000);

	// create a simple camera
	m_camera = new aie::FlyCamera();
	m_camera->setLookAt(vec3(10), vec3(0), vec3(0, 1, 0));

	return true;
}

void Application3D::shutdown() {

	delete m_camera;
	Gizmos::destroy();
}

void Application3D::update(float deltaTime) {

	// update camera
	m_camera->update(deltaTime);

	// wipe the gizmos clean for this frame
	Gizmos::clear();

	// draw a simple grid with gizmos
	vec4 white(1);
	vec4 black(0, 0, 0, 1);
	for (int i = 0; i < 21; ++i) {
		Gizmos::addLine(vec3(-10 + i, 0, 10),
						vec3(-10 + i, 0, -10),
						i == 10 ? white : black);
		Gizmos::addLine(vec3(10, 0, -10 + i),
						vec3(-10, 0, -10 + i),
						i == 10 ? white : black);
	}

	// add a transform so that we can see the axis
	Gizmos::addTransform(mat4(1));
}

void Application3D::draw() {

	// wipe the screen to the background colour
	clearScreen();

	Gizmos::draw(m_camera->getProjectionView());
}