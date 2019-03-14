#pragma once

#include "Game.h"
#include <glm/mat4x4.hpp>

class Game3D : public aie::Game
{
public:
	Game3D(const char* title, int width, int height, bool fullscreen);
	virtual ~Game3D();

	virtual void Update(float deltaTime);
	virtual void Draw();

protected:
	glm::mat4	m_viewMatrix;
	glm::mat4	m_projectionMatrix;
};