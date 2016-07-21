#pragma once

#include <glm/glm.hpp>

namespace aie {

class Camera {
public:

	Camera(float fovY, float aspect, float near, float far);
	virtual ~Camera() {}

	void setPerspective(float fovY, float aspect, float near, float far);
	void setOrthographic(float left, float right, float bottom, float top, float near, float far);

	void setLookAt(const glm::vec3& eye, const glm::vec3& target,
				   const glm::vec3& up = glm::vec3(0, 1, 0));
	
	void setTransform(const glm::mat4& transform);
	void setPosition(const glm::vec3& position);

	const glm::mat4&	getTransform() const		{ return m_transform; }
	const glm::mat4&	getView() const				{ return m_view; }
	const glm::mat4&	getProjection() const		{ return m_projection; }
	const glm::mat4&	getProjectionView() const	{ return m_projectionView; }

	virtual void update(float deltaTime) {}
		
protected:

	glm::mat4	m_transform;

	glm::mat4	m_view;
	glm::mat4	m_projection;

	glm::mat4	m_projectionView;
};

class FlyCamera : public Camera {
public:

	FlyCamera(float speed = 10);
	virtual ~FlyCamera() {}

	void	setSpeed(float speed) { m_speed = speed; }
	float	getSpeed() const { return m_speed; }

	virtual void update(float deltaTime);

protected:

	float	m_speed;
};

} // namespace aie