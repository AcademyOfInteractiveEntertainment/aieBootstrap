#include "aie/bootstrap/Input.h"
#include <GLFW/glfw3.h>

namespace aie
{
	Input* Input::m_instance = nullptr;

	Input::Input()
		: m_oldMouseX{ 0 }, m_oldMouseY{ 0 }, m_firstMouseMove{ false },
		m_lastKeys{ new int[GLFW_KEY_LAST + 1] }, m_currentKeys{ new int[GLFW_KEY_LAST + 1] }
	{
		auto window = glfwGetCurrentContext();

		for (int i = GLFW_KEY_SPACE; i <= GLFW_KEY_LAST; ++i)
			m_lastKeys[i] = m_currentKeys[i] = glfwGetKey(window, i);

		for (int i = 0; i < 8; ++i)
			m_lastButtons[i] = m_currentButtons[i] = glfwGetMouseButton(window, i);

		// set up callbacks
		auto KeyPressCallback = [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				for (auto& f : Input::GetInstance()->m_keyCallbacks)
					f(window, key, scancode, action, mods);
			};

		auto CharacterInputCallback = [](GLFWwindow* window, unsigned int character)
			{
				GetInstance()->m_pressedCharacters.push_back(character);

				for (auto& f : GetInstance()->m_charCallbacks)
					f(window, character);
			};

		auto MouseMoveCallback = [](GLFWwindow* window, double x, double y)
			{
				int w = 0, h = 0;
				glfwGetWindowSize(window, &w, &h);

				GetInstance()->OnMouseMove(static_cast<int>(x), h - static_cast<int>(y));

				for (auto& f : Input::GetInstance()->m_mouseMoveCallbacks)
					f(window, x, h - y);
			};

		auto MouseInputCallback = [](GLFWwindow* window, int button, int action, int mods)
			{
				for (auto& f : GetInstance()->m_mouseButtonCallbacks)
					f(window, button, action, mods);
			};

		auto MouseScrollCallback = [](GLFWwindow* window, double xOffset, double yOffset)
			{
				GetInstance()->m_mouseScroll += yOffset;

				for (auto& f : GetInstance()->m_mouseScrollCallbacks)
					f(window, xOffset, yOffset);
			};

		auto MouseEnterCallback = [](GLFWwindow* window, int entered)
			{
				// Set flag to prevent large mouse delta on entering screen
				GetInstance()->m_firstMouseMove = true;
			};

		glfwSetKeyCallback(window, KeyPressCallback);
		glfwSetCharCallback(window, CharacterInputCallback);
		glfwSetMouseButtonCallback(window, MouseInputCallback);
		glfwSetCursorPosCallback(window, MouseMoveCallback);
		glfwSetScrollCallback(window, MouseScrollCallback);
		glfwSetCursorEnterCallback(window, MouseEnterCallback);

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
			// On first move after Startup/entering window reset old mouse position
			m_oldMouseX = newXPos;
			m_oldMouseY = newYPos;
			m_firstMouseMove = false;
		}
	}

	void Input::Create()
	{
		m_instance = new Input();
	}

	void Input::Destroy()
	{
		delete m_instance;
	}

	void Input::ClearStatus()
	{
		m_pressedCharacters.clear();

		auto window = glfwGetCurrentContext();

		m_pressedKeys.clear();

		// Update keys
		for (int i = GLFW_KEY_SPACE; i <= GLFW_KEY_LAST; ++i) 
		{
			m_lastKeys[i] = m_currentKeys[i];

			if ((m_currentKeys[i] = glfwGetKey(window, i)) == GLFW_PRESS)
				m_pressedKeys.push_back(m_currentKeys[i]);
		}

		// Update mouse
		for (int i = 0; i < 8; ++i) 
		{
			m_lastButtons[i] = m_currentButtons[i];
			m_currentButtons[i] = glfwGetMouseButton(window, i);
		}

		// Update old mouse position
		m_oldMouseX = m_mouseX;
		m_oldMouseY = m_mouseY;
	}

	Input* Input::GetInstance()
	{
		return m_instance;
	}

	bool Input::IsKeyDown(int inputKeyID) const
	{
		return m_currentKeys[inputKeyID] == GLFW_PRESS;
	}

	bool Input::IsKeyUp(int inputKeyID) const
	{
		return m_currentKeys[inputKeyID] == GLFW_RELEASE;
	}

	bool Input::WasKeyPressed(int inputKeyID) const
	{
		return m_currentKeys[inputKeyID] == GLFW_PRESS && m_lastKeys[inputKeyID] == GLFW_RELEASE;
	}

	bool Input::WasKeyReleased(int inputKeyID) const
	{
		return m_currentKeys[inputKeyID] == GLFW_RELEASE && m_lastKeys[inputKeyID] == GLFW_PRESS;
	}

	const std::vector<int>& Input::GetPressedKeys() const
	{
		return m_pressedKeys;
	}

	const std::vector<unsigned int>& Input::GetPressedCharacters() const {
		return m_pressedCharacters;
	}

	bool Input::IsMouseButtonDown(int inputMouseID) {
		return m_currentButtons[inputMouseID] == GLFW_PRESS;
	}

	bool Input::IsMouseButtonUp(int inputMouseID) {
		return m_currentButtons[inputMouseID] == GLFW_RELEASE;
	}

	bool Input::WasMouseButtonPressed(int inputMouseID) {
		return m_currentButtons[inputMouseID] == GLFW_PRESS &&
			m_lastButtons[inputMouseID] == GLFW_RELEASE;
	}

	bool Input::WasMouseButtonReleased(int inputMouseID) {
		return m_currentButtons[inputMouseID] == GLFW_RELEASE &&
			m_lastButtons[inputMouseID] == GLFW_PRESS;
	}

	int Input::GetMouseX() {
		return m_mouseX;
	}

	int Input::GetMouseY() {
		return m_mouseY;
	}

	double Input::GetMouseScroll() {
		return m_mouseScroll;
	}

	void Input::GetMouseXY(int* x, int* y) {
		if (x != nullptr) *x = m_mouseX;
		if (y != nullptr) *y = m_mouseY;
	}

	int Input::GetMouseDeltaX()
	{
		return m_mouseX - m_oldMouseX;
	}

	int Input::GetMouseDeltaY()
	{
		return m_mouseY - m_oldMouseY;
	}

	void Input::GetMouseDelta(int* x, int* y)
	{
		if (x != nullptr) *x = m_mouseX - m_oldMouseX;
		if (y != nullptr) *y = m_mouseY - m_oldMouseY;
	}

} // namespace aie