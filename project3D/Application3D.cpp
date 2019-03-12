#include "Application3D.h"
#include "Gizmos.h"
#include "Input.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>

using glm::vec3;
using glm::vec4;
using glm::mat4;
using aie::Gizmos;

Application3D::Application3D(const char* title, int width, int height, bool fullscreen) 
	: Application(title, width, height, fullscreen)
{
	// Set the background colour to grey.
	SetBackgroundColour(0.25f, 0.25f, 0.25f);

	// Initialise the Gizmos and set the maximum primitive counts.
	Gizmos::Create(10000, 10000, 10000, 10000);

	// Create simple camera transforms.
	m_viewMatrix = glm::lookAt(vec3(10), vec3(0), vec3(0, 1, 0));
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f,
		GetWindowWidth() / (float)GetWindowHeight(),
		0.1f, 1000.f);
}

Application3D::~Application3D() 
{
	Gizmos::Destroy();
}

void Application3D::Update(float deltaTime) 
{
	// Query time since application started.
	float time = GetTime();

	// Rotate camera based on time.
	m_viewMatrix = glm::lookAt(vec3(glm::sin(time) * 10, 10, glm::cos(time) * 10), vec3(0), vec3(0, 1, 0));

	// Wipe the gizmos clean for this frame.
	Gizmos::Clear();

	// Draw a simple grid with gizmos.
	vec4 white(1);
	vec4 black(0, 0, 0, 1);
	for (int i = 0; i < 21; ++i) 
	{
		Gizmos::AddLine(vec3(-10 + i, 0, 10), vec3(-10 + i, 0, -10), i == 10 ? white : black);
		Gizmos::AddLine(vec3(10, 0, -10 + i), vec3(-10, 0, -10 + i), i == 10 ? white : black);
	}

	// Add a transform so that we can see the axis.
	Gizmos::AddTransform(mat4(1));

	// Demonstrate a few shapes.
	Gizmos::AddAABBFilled(vec3(0), vec3(1), vec4(0, 0.5f, 1, 0.25f));
	Gizmos::AddSphere(vec3(5, 0, 5), 1, 8, 8, vec4(1, 0, 0, 0.5f));
	Gizmos::AddRing(vec3(5, 0, -5), 1, 1.5f, 8, vec4(0, 1, 0, 1));
	Gizmos::AddDisk(vec3(-5, 0, 5), 1, 16, vec4(1, 1, 0, 1));
	Gizmos::AddArc(vec3(-5, 0, -5), 0, 2, 1, 8, vec4(1, 0, 1, 1));

	mat4 t = glm::rotate(mat4(1), time, glm::normalize(vec3(1, 1, 1)));
	t[3] = vec4(-2, 0, 0, 1);
	Gizmos::AddCylinderFilled(vec3(0), 0.5f, 1, 5, vec4(0, 1, 1, 1), &t);

	// Demonstrate 2D gizmos.
	Gizmos::Add2DAABB(glm::vec2(GetWindowWidth() / 2, 100),
					  glm::vec2(GetWindowWidth() / 2 * (fmod(GetTime(), 3.f) / 3), 20),
					  vec4(0, 1, 1, 1));

	// Quit if we press escape.
	aie::Input* input = aie::Input::GetInstance();
	if (input->IsKeyDown(aie::INPUT_KEY_ESCAPE))
		Quit();
}

void Application3D::Draw() 
{
	// Wipe the screen to the background colour
	ClearScreen();

	// update perspective in case window resized
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f,
										  GetWindowWidth() / (float)GetWindowHeight(),
										  0.1f, 1000.f);

	// draw 3D gizmos
	Gizmos::Draw(m_projectionMatrix * m_viewMatrix);

	// draw 2D gizmos using an orthogonal projection matrix (or screen dimensions)
	Gizmos::Draw2D((float)GetWindowWidth(), (float)GetWindowHeight());
}