#pragma once

#include "Renderer2D.h"
#include "Texture.h"

class Player
{
public:
	Player();
	~Player();

	void Update(float deltaTime);
	void Draw(aie::Renderer2D* renderer);

private:
	aie::Texture* m_texture;

	float m_posX;
	float m_posY;
};

