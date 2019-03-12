//----------------------------------------------------------------------------
// A singleton class that manages Input from the keyboard and mouse.
//----------------------------------------------------------------------------
#include "Input.h"
#include <GLFW/glfw3.h>

namespace aie 
{

Input* Input::m_instance = nullptr;

Input::Input() 
{
	// Track current/previous key and mouse button states.
	m_lastKeys = new int[GLFW_KEY_LAST + 1];
	m_currentKeys = new int[GLFW_KEY_LAST + 1];

	GLFWwindow* window = glfwGetCurrentContext();

	for (int i = GLFW_KEY_SPACE; i <= GLFW_KEY_LAST; ++i)
		m_lastKeys[i] = m_currentKeys[i] = glfwGetKey(window, i);

	for (int i = 0; i < 8; ++i)
		m_lastButtons[i] = m_currentButtons[i] = glfwGetMouseButton(window, i);

	// Set up callbacks.
	auto keyPressCallback = [](GLFWwindow* window, int key, int scancode, int action, int mods) 
	{
		for (KeyCallback& callback : Input::GetInstance()->m_keyCallbacks)
			callback(window, key, scancode, action, mods);
	};

	auto characterInputCallback = [](GLFWwindow* window, unsigned int character) 
	{
		Input::GetInstance()->m_pressedCharacters.push_back(character);

		for (CharCallback& callback : Input::GetInstance()->m_charCallbacks)
			callback(window, character);
	};

	auto mouseMoveCallback = [](GLFWwindow* window, double x, double y)
	{
		int w = 0, h = 0;
		glfwGetWindowSize(window, &w, &h);

		Input::GetInstance()->OnMouseMove((int)x, h - (int)y);

		for (MouseMoveCallback& callback : Input::GetInstance()->m_mouseMoveCallbacks)
			callback(window, x, h - y);
	};

	auto mouseInputCallback = [](GLFWwindow* window, int button, int action, int mods) 
	{	
		for (MouseButtonCallback& callback : Input::GetInstance()->m_mouseButtonCallbacks)
			callback(window, button, action, mods);
	};

	auto mouseScrollCallback = [](GLFWwindow* window, double xoffset, double yoffset) 
	{
		Input::GetInstance()->m_mouseScroll += yoffset;

		for (MouseScrollCallback& callback : Input::GetInstance()->m_mouseScrollCallbacks)
			callback(window, xoffset, yoffset);
	};

	auto mouseEnterCallback = [](GLFWwindow* window, int entered) 
	{
		// Set flag to prevent large mouse delta on entering screen
		Input::GetInstance()->m_firstMouseMove = true;
	};

	glfwSetKeyCallback(window, keyPressCallback);
	glfwSetCharCallback(window, characterInputCallback);
	glfwSetMouseButtonCallback(window, mouseInputCallback);
	glfwSetCursorPosCallback(window, mouseMoveCallback);
	glfwSetScrollCallback(window, mouseScrollCallback);
	glfwSetCursorEnterCallback(window, mouseEnterCallback);
	
	m_mouseX = 0;
	m_mouseY = 0;
	m_mouseScroll = 0;
}

Input::~Input() 
{
	delete[] m_lastKeys;
	delete[] m_currentKeys;
}

void Input::OnMouseMove(int newXPos, int newYPos) 
{
	m_mouseX = newXPos;
	m_mouseY = newYPos;
	if (m_firstMouseMove) 
	{
		// On first move after startup/entering window reset old mouse position.
		m_oldMouseX = newXPos;
		m_oldMouseY = newYPos;
		m_firstMouseMove = false;
	}
}

void Input::ClearStatus() 
{
	m_pressedCharacters.clear();

	auto window = glfwGetCurrentContext();

	m_pressedKeys.clear();

	// Update keys.
	for (int i = GLFW_KEY_SPACE; i <= GLFW_KEY_LAST; ++i) 
	{
		m_lastKeys[i] = m_currentKeys[i];

		if ((m_currentKeys[i] = glfwGetKey(window, i)) == GLFW_PRESS)
			m_pressedKeys.push_back(m_currentKeys[i]);
	}

	// Update mouse.
	for (int i = 0; i < 8 ; ++i) 
	{
		m_lastButtons[i] = m_currentButtons[i];
		m_currentButtons[i] = glfwGetMouseButton(window, i);
	}

	// Update old mouse position.
	m_oldMouseX = m_mouseX;
	m_oldMouseY = m_mouseY;
}

bool Input::IsKeyDown(int inputKeyID) 
{
	return m_currentKeys[inputKeyID] == GLFW_PRESS;
}

bool Input::IsKeyUp(int inputKeyID) 
{
	return m_currentKeys[inputKeyID] == GLFW_RELEASE;
}

bool Input::WasKeyPressed(int inputKeyID) 
{
	return m_currentKeys[inputKeyID] == GLFW_PRESS && m_lastKeys[inputKeyID] == GLFW_RELEASE;
}

bool Input::WasKeyReleased(int inputKeyID) 
{
	return m_currentKeys[inputKeyID] == GLFW_RELEASE && m_lastKeys[inputKeyID] == GLFW_PRESS;
}

const std::vector<int> &Input::GetPressedKeys() const 
{
	return m_pressedKeys;
}

const std::vector<unsigned int> &Input::GetPressedCharacters() const 
{
	return m_pressedCharacters;
}

bool Input::IsMouseButtonDown(int inputMouseID) 
{
	return m_currentButtons[inputMouseID] == GLFW_PRESS;
}

bool Input::IsMouseButtonUp(int inputMouseID) 
{
	return m_currentButtons[inputMouseID] == GLFW_RELEASE;
}

bool Input::WasMouseButtonPressed(int inputMouseID) 
{
	return m_currentButtons[inputMouseID] == GLFW_PRESS && m_lastButtons[inputMouseID] == GLFW_RELEASE;
}

bool Input::WasMouseButtonReleased(int inputMouseID) 
{
	return m_currentButtons[inputMouseID] == GLFW_RELEASE && m_lastButtons[inputMouseID] == GLFW_PRESS;
}

int Input::GetMouseX() 
{
	return m_mouseX;
}

int Input::GetMouseY()
{
	return m_mouseY;
}

double Input::GetMouseScroll() 
{
	return m_mouseScroll;
}

void Input::GetMouseXY(int& x, int& y) 
{
	x = m_mouseX;
	y = m_mouseY;
}

int Input::GetMouseDeltaX()
{
	return m_mouseX - m_oldMouseX;
}

int Input::GetMouseDeltaY()
{
	return m_mouseY - m_oldMouseY;
}

void Input::GetMouseDelta(int& x, int& y)
{
	x = m_mouseX - m_oldMouseX;
	y = m_mouseY - m_oldMouseY;
}

} // namespace aie