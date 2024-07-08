#include "Application2D.h"
#include "aie\bootstrap\Texture.h"
#include "aie\bootstrap\Font.h"
#include "aie\bootstrap\Input.h"

Application2D::Application2D() {

}

Application2D::~Application2D() {

}

bool Application2D::Startup() {
	
	m_2dRenderer = new aie::Renderer2D();

	m_texture = new aie::Texture("./textures/numbered_grid.tga");
	m_shipTexture = new aie::Texture("./textures/ship.png");

	m_font = new aie::Font("./font/consolas.ttf", 32);
	
	m_timer = 0;

	return true;
}

void Application2D::Shutdown() {
	
	delete m_font;
	delete m_texture;
	delete m_shipTexture;
	delete m_2dRenderer;
}

void Application2D::Update(float deltaTime) {

	m_timer += deltaTime;

	// input example
	aie::Input* input = aie::Input::GetInstance();

	// Update the camera position using the arrow keys
	float camPosX;
	float camPosY;
	m_2dRenderer->GetCameraPos(camPosX, camPosY);

	if (input->IsKeyDown(aie::KeyUp))
		camPosY += 500.0f * deltaTime;

	if (input->IsKeyDown(aie::KeyDown))
		camPosY -= 500.0f * deltaTime;

	if (input->IsKeyDown(aie::KeyLeft))
		camPosX -= 500.0f * deltaTime;

	if (input->IsKeyDown(aie::KeyRight))
		camPosX += 500.0f * deltaTime;

	m_2dRenderer->SetCameraPos(camPosX, camPosY);

	// exit the application
	if (input->IsKeyDown(aie::KeyEscape))
		Quit();
}

void Application2D::Draw() {

	// wipe the screen to the background colour
	ClearScreen();

	// begin drawing sprites
	m_2dRenderer->Begin();

	// demonstrate animation
	m_2dRenderer->SetUVRect(int(m_timer) % 8 / 8.0f, 0, 1.f / 8, 1.f / 8);
	m_2dRenderer->DrawSprite(m_texture, 200, 200, 100, 100);

	// demonstrate spinning sprite
	m_2dRenderer->SetUVRect(0,0,1,1);
	m_2dRenderer->DrawSprite(m_shipTexture, 600, 400, 0, 0, m_timer, 1);

	// Draw a thin line
	m_2dRenderer->DrawLine(300, 300, 600, 400, 2, 1);

	// Draw a moving purple circle
	m_2dRenderer->SetRenderColour(1, 0, 1, 1);
	m_2dRenderer->DrawCircle(sin(m_timer) * 100 + 600, 150, 50);

	// Draw a rotating red box
	m_2dRenderer->SetRenderColour(1, 0, 0, 1);
	m_2dRenderer->DrawBox(600, 500, 60, 20, m_timer);

	// Draw a slightly rotated sprite with no texture, coloured yellow
	m_2dRenderer->SetRenderColour(1, 1, 0, 1);
	m_2dRenderer->DrawSprite(nullptr, 400, 400, 50, 50, 3.14159f * 0.25f, 1);
	
	// output some text, uses the last used colour
	char fps[32];
	sprintf_s(fps, 32, "FPS: %i", GetFPS());
	m_2dRenderer->DrawText(m_font, fps, 0, 720 - 32);
	m_2dRenderer->DrawText(m_font, "Press ESC to Quit!", 0, 720 - 64);

	// done drawing sprites
	m_2dRenderer->End();
}