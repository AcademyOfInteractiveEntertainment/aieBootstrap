#pragma once

#include "Application.h"

namespace aie {
	class Camera;
};

class Application3D : public aie::Application {
public:

	Application3D();
	virtual ~Application3D();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

protected:

	aie::Camera*		m_camera;
};