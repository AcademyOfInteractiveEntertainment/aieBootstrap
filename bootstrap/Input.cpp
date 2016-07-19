#include "Input.h"
#include <GLFW/glfw3.h>

namespace aie {

Input* Input::m_instance = nullptr;

Input::Input() {
	auto KeyPressCallback = [](GLFWwindow *pWindow, int key, int scancode, int action, int mods) {
		if( action == GLFW_PRESS )
			Input::getInstance()->onKeyPressed( key );
		if( action == GLFW_RELEASE )
			Input::getInstance()->onKeyReleased( key );
		if( action == GLFW_REPEAT )
			Input::getInstance()->onKeyRepeate(key);
	};

	auto CharacterInputCallback = [](GLFWwindow *pWindow, unsigned int character) {
		Input::getInstance()->onCharInput(character);
	};

	auto MouseInputCallback = [](GLFWwindow *pWindow, int button, int action, int mods) {
		if (action == GLFW_PRESS)
			Input::getInstance()->onMousePressed( button );
		if (action == GLFW_RELEASE)
			Input::getInstance()->onMouseReleased( button );
	};

	auto MouseMoveCallback = [](GLFWwindow *pWindow, double mx, double my) {
		Input::getInstance()->onMouseMove( (int)mx, (int)my );
	};

	GLFWwindow *pWindow = glfwGetCurrentContext();
	glfwSetKeyCallback(pWindow, KeyPressCallback);
	glfwSetCharCallback(pWindow, CharacterInputCallback);
	glfwSetMouseButtonCallback(pWindow, MouseInputCallback);
	glfwSetCursorPosCallback(pWindow, MouseMoveCallback );

	m_mouseX = 0;
	m_mouseY = 0;
}

Input::~Input() {

}

void Input::onKeyPressed(int keyID) {
	m_keyStatus[keyID] = JUST_PRESSED;
	m_keysToUpdate.push_back( keyID );
	m_pressedKeys.push_back( keyID );
}

void Input::onKeyReleased(int keyID) {
	m_keyStatus[keyID] = JUST_RELEASED;
	m_keysToUpdate.push_back( keyID );
}

void Input::onKeyRepeate(int key) {
	m_pressedKeys.push_back(key);
}

void Input::onCharInput(unsigned int character) {
	m_pressedCharacters.push_back( character );
}

void Input::onMouseMove(int newXPos, int newYPos) {
	m_mouseX = newXPos;
	m_mouseY = newYPos;
}

void Input::onMousePressed(int mouseButtonID) {
	m_mouseState[mouseButtonID] = JUST_PRESSED;
	m_mouseToUpdate.push_back(mouseButtonID);
}

void Input::onMouseReleased(int mouseButtonID) {
	m_mouseState[mouseButtonID] = JUST_RELEASED;
	m_mouseToUpdate.push_back(mouseButtonID);
}

void Input::update() {
	for (auto& key : m_keysToUpdate)
		key += 1;

	for (auto& state : m_mouseToUpdate)
		state += 1;

	m_mouseToUpdate.clear();
	m_keysToUpdate.clear();

	m_pressedKeys.clear();
	m_pressedCharacters.clear();
}

bool Input::isKeyDown(int inputKeyID) {
	return m_keyStatus[inputKeyID] == JUST_PRESSED || m_keyStatus[inputKeyID] == DOWN;
}

bool Input::isKeyUp(int inputKeyID) {
	return m_keyStatus[inputKeyID] == JUST_RELEASED || m_keyStatus[inputKeyID] == UP;
}

bool Input::wasKeyPressed(int inputKeyID) {
	return m_keyStatus[inputKeyID] == JUST_PRESSED;
}

bool Input::wasKeyReleased(int inputKeyID) {
	return m_keyStatus[inputKeyID] == JUST_RELEASED;
}

const std::vector<unsigned int> &Input::getPressedKeys() const {
	return m_pressedKeys;
}

const std::vector<unsigned int> &Input::getPressedCharacters() const {
	return m_pressedCharacters;
}

bool Input::isMouseButtonDown(int inputMouseID) {
	return m_mouseState[inputMouseID] == DOWN;
}

bool Input::isMouseButtonUp(int inputMouseID) {
	return m_mouseState[inputMouseID] == UP || m_mouseState[inputMouseID] == JUST_RELEASED;
}

bool Input::wasMouseButtonPressed(int inputMouseID) {
	return m_mouseState[inputMouseID] == JUST_PRESSED;
}

bool Input::wasMouseButtonReleased(int inputMouseID) {
	return m_mouseState[inputMouseID] == JUST_RELEASED;
}

int Input::getMouseX() {
	return m_mouseX;
}

int Input::getMouseY() {
	return m_mouseY;
}

void Input::getMouseXY(int* x, int* y) {
	if ( x != nullptr ) *x = m_mouseX;
	if ( y != nullptr) *y = m_mouseY;
}

}