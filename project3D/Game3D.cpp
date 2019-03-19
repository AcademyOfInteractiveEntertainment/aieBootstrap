#include "Game3D.h"
#include "Gizmos.h"
#include "Input.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "Application.h"

using glm::vec3;
using glm::vec4;
using glm::mat4;
using aie::Gizmos;

Game3D::Game3D(const char* title, int width, int height, bool fullscreen) : Game(title, width, height, fullscreen)
{
	aie::Application* application = aie::Application::GetInstance();

	// Enable backface culling for performance.
	application->SetBackfaceCull(true);

	// Set the background colour to grey.
	application->SetBackgroundColour(0.25f, 0.25f, 0.25f);

	// Initialise the Gizmos and set the maximum primitive counts.
	Gizmos::Create(10000, 10000, 10000, 10000);

	// Create simple camera transforms.
	float aspect = (float)application->GetWindowWidth() / (float)application->GetWindowHeight();
	m_viewMatrix = glm::lookAt(vec3(10), vec3(0), vec3(0, 1, 0));
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, aspect, 0.1f, 1000.f);
}

Game3D::~Game3D()
{
	Gizmos::Destroy();
}

void Game3D::Update(float deltaTime)
{
	aie::Application* application = aie::Application::GetInstance();

	// Query time since application started.
	float time = application->GetTime();

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
	unsigned int windowWidth = application->GetWindowWidth();
	Gizmos::Add2DAABB(glm::vec2(windowWidth * 0.5f, 100),
					  glm::vec2(windowWidth * 0.5f * (fmod(time, 3.f) / 3), 20),
					  vec4(0, 1, 1, 1));

	// Quit if we press escape.
	aie::Input* input = aie::Input::GetInstance();
	if (input->IsKeyDown(aie::INPUT_KEY_ESCAPE))
		application->Quit();
}

void Game3D::Draw()
{
	aie::Application* application = aie::Application::GetInstance();

	// Wipe the screen to the background colour
	application->ClearScreen();

	// update perspective in case window resized
	float width = (float)application->GetWindowWidth();
	float height = (float)application->GetWindowHeight();
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, width / height, 0.1f, 1000.f);

	// draw 3D gizmos
	Gizmos::Draw(m_projectionMatrix * m_viewMatrix);

	// draw 2D gizmos using an orthogonal projection matrix (or screen dimensions)
	Gizmos::Draw2D(width, height);
}