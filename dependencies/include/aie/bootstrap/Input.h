#pragma once

#include "Bootstrap.h"

#include <vector>
#include <functional>
#include <map>

using std::vector;
using std::function;
using std::map;

struct GLFWwindow;

constexpr int MOUSE_BUTTON_COUNT = 8;

namespace aie
{
	// a giant list of input codes for keyboard and mouse
	enum EInputCodes : int
	{
		KeyUnknown = -1,
		KeySpace = 32,
		KeyApostrophe = 39,
		KeyComma = 44,
		KeyMinus = 45,
		KeyPeriod = 46,
		KeySlash = 47,
		Key0 = 48,
		Key1 = 49,
		Key2 = 50,
		Key3 = 51,
		Key4 = 52,
		Key5 = 53,
		Key6 = 54,
		Key7 = 55,
		Key8 = 56,
		Key9 = 57,
		KeySemicolon = 59,
		KeyEqual = 61,
		KeyA = 65,
		KeyB = 66,
		KeyC = 67,
		KeyD = 68,
		KeyE = 69,
		KeyF = 70,
		KeyG = 71,
		KeyH = 72,
		KeyI = 73,
		KeyJ = 74,
		KeyK = 75,
		KeyL = 76,
		KeyM = 77,
		KeyN = 78,
		KeyO = 79,
		KeyP = 80,
		KeyQ = 81,
		KeyR = 82,
		KeyS = 83,
		KeyT = 84,
		KeyU = 85,
		KeyV = 86,
		KeyW = 87,
		KeyX = 88,
		KeyY = 89,
		KeyZ = 90,
		KeyLeftBracket = 91,
		KeyBackslash = 92,
		KeyRightBracket = 93,
		KeyGraveAccent = 96,
		KeyEscape = 256,
		KeyEnter = 257,
		KeyTab = 258,
		KeyBackspace = 259,
		KeyInsert = 260,
		KeyDelete = 261,
		KeyRight = 262,
		KeyLeft = 263,
		KeyDown = 264,
		KeyUp = 265,
		KeyPageUp = 266,
		KeyPageDown = 267,
		KeyHome = 268,
		KeyEnd = 269,
		KeyCapsLock = 280,
		KeyScrollLock = 281,
		KeyNumLock = 282,
		KeyPrintScreen = 283,
		KeyPause = 284,
		KeyF1 = 290,
		KeyF2 = 291,
		KeyF3 = 292,
		KeyF4 = 293,
		KeyF5 = 294,
		KeyF6 = 295,
		KeyF7 = 296,
		KeyF8 = 297,
		KeyF9 = 298,
		KeyF10 = 299,
		KeyF11 = 300,
		KeyF12 = 301,
		KeyF13 = 302,
		KeyF14 = 303,
		KeyF15 = 304,
		KeyF16 = 305,
		KeyF17 = 306,
		KeyF18 = 307,
		KeyF19 = 308,
		KeyF20 = 309,
		KeyF21 = 310,
		KeyF22 = 311,
		KeyF23 = 312,
		KeyF24 = 313,
		KeyF25 = 314,
		KeyKeypad0 = 320,
		KeyKeypad1 = 321,
		KeyKeypad2 = 322,
		KeyKeypad3 = 323,
		KeyKeypad4 = 324,
		KeyKeypad5 = 325,
		KeyKeypad6 = 326,
		KeyKeypad7 = 327,
		KeyKeypad8 = 328,
		KeyKeypad9 = 329,
		KeyKeypadDecimal = 330,
		KeyKeypadDivide = 331,
		KeyKeypadMultiply = 332,
		KeyKeypadSubtract = 333,
		KeyKeypadAdd = 334,
		KeyKeypadEnter = 335,
		KeyKeypadEqual = 336,
		KeyLeftShift = 340,
		KeyLeftControl = 341,
		KeyLeftAlt = 342,
		KeyRightShift = 344,
		KeyRightControl = 345,
		KeyRightAlt = 346,
		MouseButtonLeft = 0,
		MouseButtonRight = 1,
		MouseButtonMiddle = 2,
		MouseButton4 = 3,
		MouseButton5 = 4,
		MouseButton6 = 5,
		MouseButton7 = 6,
		MouseButton8 = 7,
	};

	// a singleton class that manages Input from the keyboard and mouse
	class DLL Input
	{
		// just giving the Application class access to the Input singleton
		friend class Application;

	public:
		// delegates for attaching input observers to the Input class
		typedef function<void(GLFWwindow*, int, int, int, int)> KeyCallback;
		typedef function<void(GLFWwindow*, unsigned int)> CharCallback;
		typedef function<void(GLFWwindow*, int, int, int)> MouseButtonCallback;
		typedef function<void(GLFWwindow*, double, double)> MouseScrollCallback;
		typedef function<void(GLFWwindow*, double, double)> MouseMoveCallback;

	public:
		// returns access to the singleton instance
		static Input* GetInstance();

	public:
		// query the keyboard state
		bool IsKeyDown(int inputKeyID) const;
		bool IsKeyUp(int inputKeyID) const;

		// returns true if the key was pressed / released this frame
		bool WasKeyPressed(int inputKeyID) const;
		bool WasKeyReleased(int inputKeyID) const;

		// returns access to all keys that are currently pressed
		const vector<int>& GetPressedKeys() const;
		const vector<unsigned int>& GetPressedCharacters() const;

		// query the mouse button state
		bool IsMouseButtonDown(int inputMouseID);
		bool IsMouseButtonUp(int inputMouseID);

		// returns true if the button was pressed / released this frame
		bool WasMouseButtonPressed(int inputMouseID);
		bool WasMouseButtonReleased(int inputMouseID);

		// query the mouse position
		int GetMouseX();
		int GetMouseY();
		void GetMouseXY(int* x, int* y);

		// query mouse movement
		int GetMouseDeltaX();
		int GetMouseDeltaY();
		void GetMouseDelta(int* x, int* y);

		// query how far the mouse wheel has been moved 
		double GetMouseScroll();

		// functions for attaching input observers
		void AttachKeyObserver(const KeyCallback& callback) { m_keyCallbacks.push_back(callback); }
		void AttachCharObserver(const CharCallback& callback) { m_charCallbacks.push_back(callback); }
		void AttachMouseButtonObserver(const MouseButtonCallback& callback) { m_mouseButtonCallbacks.push_back(callback); }
		void AttachMouseMoveObserver(const MouseMoveCallback& callback) { m_mouseMoveCallbacks.push_back(callback); }
		void AttachMouseScrollObserver(const MouseScrollCallback& callback) { m_mouseScrollCallbacks.push_back(callback); }

	protected:
		// singleton pointer
		static Input* m_instance;

	protected:
		// only want the Application class to be able to create / destroy
		static void Create();
		static void Destroy();

		// should be called once by the application each frame after the current update
		// or before glfwPollEvents
		void ClearStatus();

	private:
		// disabling the warnings here as they are a non-issue and clog up the error output
#pragma warning (push)
#pragma warning (disable : 4251)
		vector<int> m_pressedKeys;
		vector<unsigned int> m_pressedCharacters;
#pragma warning (pop)

		int	m_mouseX;
		int	m_mouseY;
		int	m_oldMouseX;
		int	m_oldMouseY;
		double m_mouseScroll;

		bool m_firstMouseMove;	// flag for first mouse input after start or mouse entering window

		void OnMouseMove(int newXPos, int newYPos);

		// disabling the warnings here as they are a non-issue and clog up the error output
#pragma warning (push)
#pragma warning (disable : 4251)
		vector<KeyCallback> m_keyCallbacks;
		vector<CharCallback> m_charCallbacks;
		vector<MouseMoveCallback> m_mouseMoveCallbacks;
		vector<MouseButtonCallback>	m_mouseButtonCallbacks;
		vector<MouseScrollCallback>	m_mouseScrollCallbacks;
#pragma warning (pop)

		// used to track down/up/released/pressed
		int* m_lastKeys;
		int* m_currentKeys;
		int m_lastButtons[MOUSE_BUTTON_COUNT];
		int m_currentButtons[MOUSE_BUTTON_COUNT];

	private:
		// constructor private for singleton
		Input();
		~Input();

	};

} // namespace aie