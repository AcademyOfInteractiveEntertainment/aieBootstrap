#pragma once

#include "Application.h"
#include <glm/mat4x4.hpp>

class Application3D : public aie::Application 
{
public:
	Application3D(const char* title, int width, int height, bool fullscreen);
	virtual ~Application3D();

	virtual void Update(float deltaTime);
	virtual void Draw();

protected:
	glm::mat4	m_viewMatrix;
	glm::mat4	m_projectionMatrix;
};