#include "Camera.h"
#include <glm/ext.hpp>
#include <GLFW/glfw3.h>

namespace aie {

Camera::Camera(float fovY, float aspect, float near, float far)
	: m_transform(1),
	m_view(1) {

	setPerspective(fovY, aspect, near, far);
}

void Camera::setPerspective(float fovY, float aspect, float near, float far) {

	m_projection = glm::perspective(fovY, aspect, near, far);

	m_projectionView = m_projection * m_view;
}

void Camera::setOrthographic(float left, float right, 
							 float bottom, float top, 
							 float near, float far) {

	m_projection = glm::ortho(left, right, bottom, top, near, far);

	m_projectionView = m_projection * m_view;
}

void Camera::setLookAt(const glm::vec3& eye, const glm::vec3& target,
					   const glm::vec3& up) {

	m_view = glm::lookAt(eye, target, up);

	m_transform = glm::inverse( m_view );

	m_projectionView = m_projection * m_view;
}

void Camera::setTransform(const glm::mat4& transform) {
	m_transform = transform;
	m_view = glm::inverse(m_transform);
	m_projectionView = m_projection * m_view;
}

void Camera::setPosition(const glm::vec3& position) {
	m_transform[3] = glm::vec4(position,1);
	m_view = glm::inverse(m_transform);
	m_projectionView = m_projection * m_view;
}

FlyCamera::FlyCamera(float speed)
	: Camera(3.14159f * 0.25f, 16.f/9.f, 0.1f, 1000.0f),
	m_speed(speed) {
}

void FlyCamera::update(float deltaTime) {

	GLFWwindow* window = glfwGetCurrentContext();

	float frameSpeed = m_speed * deltaTime;
	
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		frameSpeed *= 2;

	// movement
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		m_transform[3] -= m_transform[2] * frameSpeed;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		m_transform[3] += m_transform[2] * frameSpeed;

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		m_transform[3] -= m_transform[0] * frameSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		m_transform[3] += m_transform[0] * frameSpeed;

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		m_transform[3] -= m_transform[1] * frameSpeed;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		m_transform[3] += m_transform[1] * frameSpeed;

	// rotation
	static bool sbMouseButtonDown = false;
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS) {

		static double prevMouseX = 0;
		static double prevMouseY = 0;

		if (sbMouseButtonDown == false) {
			sbMouseButtonDown = true;
			glfwGetCursorPos(window, &prevMouseX, &prevMouseY);
		}

		double mouseX = 0, mouseY = 0;
		glfwGetCursorPos(window, &mouseX, &mouseY);

		double deltaX = mouseX - prevMouseX, deltaY = mouseY - prevMouseY;

		prevMouseX = mouseX;
		prevMouseY = mouseY;

		// pitch
		if (deltaY != 0) {
			glm::mat4 mat = glm::axisAngleMatrix(glm::vec3(m_transform[0]), (float)deltaY / -150.f);

			m_transform[0] = mat * m_transform[0];
			m_transform[1] = mat * m_transform[1];
			m_transform[2] = mat * m_transform[2];
		}

		// yaw
		if (deltaX != 0) {
			glm::mat4 mat = glm::axisAngleMatrix(glm::vec3(0,1,0), (float)deltaX / -150.f);

			m_transform[0] = mat * m_transform[0];
			m_transform[1] = mat * m_transform[1];
			m_transform[2] = mat * m_transform[2];
		}
	}
	else
		sbMouseButtonDown = false;

	// update transforms
	m_view = glm::inverse( m_transform );
	m_projectionView = m_projection * m_view;
}

} // namespace aie