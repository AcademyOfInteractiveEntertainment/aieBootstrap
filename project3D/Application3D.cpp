#include "Application3D.h"
#include "aie/bootstrap/Gizmos.h"
#include "aie/bootstrap/Input.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>

using glm::vec3;
using glm::vec4;
using glm::mat4;
using aie::Gizmos;

Application3D::Application3D() {

}

Application3D::~Application3D() {

}

bool Application3D::Startup() {
	
	SetBackgroundColour(0.25f, 0.25f, 0.25f);

	// initialise gizmo primitive counts
	Gizmos::Create(10000, 10000, 10000, 10000);

	// create simple camera transforms
	m_viewMatrix = glm::lookAt(vec3(10), vec3(0), vec3(0, 1, 0));
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f,
										  GetWindowWidth() / (float)GetWindowHeight(),
										  0.1f, 1000.f);

	return true;
}

void Application3D::Shutdown() {

	Gizmos::Destroy();
}

void Application3D::Update(float deltaTime) {

	// query time since application started
	float time = GetTime();

	// rotate camera
	m_viewMatrix = glm::lookAt(vec3(glm::sin(time) * 10, 10, glm::cos(time) * 10),
							   vec3(0), vec3(0, 1, 0));

	// wipe the gizmos clean for this frame
	Gizmos::Clear();

	// Draw a simple grid with gizmos
	vec4 white(1);
	vec4 black(0, 0, 0, 1);
	for (int i = 0; i < 21; ++i) {
		Gizmos::AddLine(vec3(-10 + i, 0, 10),
						vec3(-10 + i, 0, -10),
						i == 10 ? white : black);
		Gizmos::AddLine(vec3(10, 0, -10 + i),
						vec3(-10, 0, -10 + i),
						i == 10 ? white : black);
	}

	// add a transform so that we can see the axis
	Gizmos::AddTransform(mat4(1));

	// demonstrate a few shapes
	Gizmos::AddAABBFilled(vec3(0), vec3(1), vec4(0, 0.5f, 1, 0.25f));
	Gizmos::AddSphere(vec3(5, 0, 5), 1, 8, 8, vec4(1, 0, 0, 0.5f));
	Gizmos::AddRing(vec3(5, 0, -5), 1, 1.5f, 8, vec4(0, 1, 0, 1));
	Gizmos::AddDisk(vec3(-5, 0, 5), 1, 16, vec4(1, 1, 0, 1));
	Gizmos::AddArc(vec3(-5, 0, -5), 0, 2, 1, 8, vec4(1, 0, 1, 1));

	mat4 t = glm::rotate(mat4(1), time, glm::normalize(vec3(1, 1, 1)));
	t[3] = vec4(-2, 0, 0, 1);
	Gizmos::AddCylinderFilled(vec3(0), 0.5f, 1, 5, vec4(0, 1, 1, 1), &t);

	// demonstrate 2D gizmos
	Gizmos::Add2DAABB(glm::vec2(GetWindowWidth() / 2, 100),
					  glm::vec2(GetWindowWidth() / 2 * (fmod(GetTime(), 3.f) / 3), 20),
					  vec4(0, 1, 1, 1));

	// Quit if we press escape
	aie::Input* input = aie::Input::GetInstance();

	if (input->IsKeyDown(aie::KeyEscape))
		Quit();
}

void Application3D::Draw() {

	// wipe the screen to the background colour
	ClearScreen();

	// Update perspective in case window resized
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f,
										  GetWindowWidth() / (float)GetWindowHeight(),
										  0.1f, 1000.f);

	// Draw 3D gizmos
	Gizmos::Draw(m_projectionMatrix * m_viewMatrix);

	// Draw 2D gizmos using an orthogonal projection matrix (or screen dimensions)
	Gizmos::Draw2D((float)GetWindowWidth(), (float)GetWindowHeight());
}