#ifndef GL_ENGINE_H
#define GL_ENGINE_H

#include "GL/glew.h"//need to be before glfw
#include "GLFW/glfw3.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"//after glfw?
#include "imgui_impl_opengl3.h"

constexpr int WINDOW_WIDTH = 1280;
constexpr int WINDOW_HEIGHT = 960;

namespace gl
{
	// System

	extern GLFWwindow* window;

	class Context
	{
	public:
		Context();
		~Context();
		bool initSuccess() const noexcept { return success; }

	private:
		bool success = false;
	};

	class ImguiContext
	{
	public:
		ImguiContext(const char* glslVersionMacro);
		~ImguiContext();
		bool initSuccess() const noexcept { return success; }

	private:
		bool success = false;
	};

	void enableBlend();

	// Window

	bool windowShouldClose();
	void windowSwapBuffer();

	// Event

    void pollEvent();

    // Key

    // Implemented keys
    typedef unsigned int KeyType;
    extern KeyType keyState;

    constexpr KeyType KEY_NONE = 0;

    constexpr KeyType KEY_UP = 1 << 0;
    constexpr KeyType KEY_DOWN = 1 << 1;
    constexpr KeyType KEY_LEFT = 1 << 2;
    constexpr KeyType KEY_RIGHT = 1 << 3;
    constexpr KeyType KEY_SHIFT = 1 << 4;

    constexpr KeyType KEY_ESCAPE = 1 << 31;    

    // Functions
    void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

    inline bool keyDown() { return keyState != 0; }
    inline bool keyDown(KeyType key) { return (keyState & key) != 0; }
    inline KeyType getKeyState() { return keyState; }
    inline void setKeyState(KeyType keys) { keyState = keys; }
};


#endif // GL_ENGINE_H
