#include "Application2D.h"
#include "Texture.h"
#include "Font.h"
#include "Input.h"
#include "Player.h"

Application2D::Application2D(const char* title, int width, int height, bool fullscreen) 
	: Application(title, width, height, fullscreen)
{
	// Initalise the 2D renderer.
	m_2dRenderer = new aie::Renderer2D();

	// Create some textures for testing.
	m_texture = new aie::Texture("./textures/hero.png");
	m_texture2 = new aie::Texture("./textures/rock_large.png");
	m_font = new aie::Font("./font/consolas.ttf", 24);

	// Create a player object.
	m_Player = new Player();

	m_timer = 0;
}

Application2D::~Application2D() 
{
	// Delete player.
	delete m_Player;
	m_Player = nullptr;

	// Deleted the textures.
	delete m_font;
	delete m_texture;
	delete m_texture2;

	// Delete the renderer.
	delete m_2dRenderer;
}

void Application2D::Update(float deltaTime)
{
	m_timer += deltaTime;

	// Update the player.
	m_Player->Update(deltaTime);

	// Input example: Update the camera position using the arrow keys.
	aie::Input* input = aie::Input::GetInstance();
	float camPosX;
	float camPosY;

	m_2dRenderer->GetCameraPos(camPosX, camPosY);

	if (input->IsKeyDown(aie::INPUT_KEY_W))
		camPosY += 500.0f * deltaTime;

	if (input->IsKeyDown(aie::INPUT_KEY_S))
		camPosY -= 500.0f * deltaTime;

	if (input->IsKeyDown(aie::INPUT_KEY_A))
		camPosX -= 500.0f * deltaTime;

	if (input->IsKeyDown(aie::INPUT_KEY_D))
		camPosX += 500.0f * deltaTime;

	m_2dRenderer->SetCameraPos(camPosX, camPosY);

	// Exit the application if escape is pressed.
	if (input->IsKeyDown(aie::INPUT_KEY_ESCAPE))
		Quit();
}

void Application2D::Draw() 
{
	// Wipe the screen to the background colour.
	ClearScreen();

	// Begin drawing sprites.
	m_2dRenderer->Begin();

	// Draw the player.
	m_Player->Draw(m_2dRenderer);

	// Draw a thin line.
	m_2dRenderer->DrawLine(150.0f, 400.0f, 250.0f, 500.0f, 2.0f);

	// Draw a sprite
	m_2dRenderer->DrawSprite(m_texture2, 200.0f, 200.0f);

	// Draw a moving purple circle.
	m_2dRenderer->SetRenderColour(1.0f, 0.0f, 1.0f, 1.0f);
	m_2dRenderer->DrawCircle(sin(m_timer) * 100.0f + 450.0f, 200.0f, 50.0f);

	// Draw a rotating sprite with no texture, coloured yellow.
	m_2dRenderer->SetRenderColour(1.0f, 1.0f, 0.0f, 1.0f);
	m_2dRenderer->DrawSprite(nullptr, 700.0f, 200.0f, 50.0f, 50.0f, m_timer);
	m_2dRenderer->SetRenderColour(1.0f, 1.0f, 1.0f, 1.0f);

	// Demonstrate animation.
	float animSpeed = 10.0f;
	int frame = ((int)(m_timer * animSpeed) % 6);
	float size = 1.0f / 6.0f;
	m_2dRenderer->SetUVRect(frame * size, 0.0f, size, 1.0f);
	m_2dRenderer->DrawSprite(m_texture, 900.0f, 200.0f, 100.0f, 100.0f);
	m_2dRenderer->SetUVRect(0.0f, 0.0f, 1.0f, 1.0f);
	
	// Draw some text.
	float windowHeight = (float)GetWindowHeight();
	char fps[32];
	sprintf_s(fps, 32, "FPS: %i", GetFPS());
	m_2dRenderer->DrawText(m_font, fps, 15.0f, windowHeight - 32.0f);
	m_2dRenderer->DrawText(m_font, "Arrow keys to move.", 15.0f, windowHeight - 64.0f);
	m_2dRenderer->DrawText(m_font, "WASD to move camera.", 15.0f, windowHeight - 96.0f);
	m_2dRenderer->DrawText(m_font, "Press ESC to quit!", 15.0f, windowHeight - 128.0f);

	// Done drawing sprites.
	m_2dRenderer->End();
}