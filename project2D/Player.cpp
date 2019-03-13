#include "Player.h"
#include "Input.h"

Player::Player()
{
	// Load the player's sprite.
	m_texture = new aie::Texture("./textures/ship.png");

	// Set the player's position.
	m_posX = 600;
	m_posY = 400;
}

Player::~Player()
{
	// Delete the player's sprite.
	delete m_texture;
	m_texture = nullptr;
}

void Player::Update(float deltaTime)
{
	// Update input for the player.
	aie::Input* input = aie::Input::GetInstance();
	if (input->IsKeyDown(aie::INPUT_KEY_LEFT))
	{
		m_posX -= 500.0f * deltaTime;
	}
	if (input->IsKeyDown(aie::INPUT_KEY_RIGHT))
	{
		m_posX += 500.0f * deltaTime;
	}
	if (input->IsKeyDown(aie::INPUT_KEY_UP))
	{
		m_posY += 500.0f * deltaTime;
	}
	if (input->IsKeyDown(aie::INPUT_KEY_DOWN))
	{
		m_posY -= 500.0f * deltaTime;
	}
}

void Player::Draw(aie::Renderer2D* renderer)
{
	// Draw the player's sprite.
	renderer->DrawSprite(m_texture, m_posX, m_posY);
}