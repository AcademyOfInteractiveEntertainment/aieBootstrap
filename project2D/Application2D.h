#pragma once

#include "Application.h"
#include "Renderer2D.h"

class Application2D : public aie::Application 
{
public:
	Application2D(const char* title, int width, int height, bool fullscreen);
	virtual ~Application2D();

	virtual void Update(float deltaTime);
	virtual void Draw();

protected:
	aie::Renderer2D*	m_2dRenderer;
	aie::Texture*		m_texture;
	aie::Texture*		m_shipTexture;
	aie::Font*			m_font;

	float m_timer;
};