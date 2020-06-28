#ifndef GL_MANAGER_H
#define GL_MANAGER_H

#include "GL/glew.h"            // Need to be before glfw
#include "GLFW/glfw3.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"    // After glfw?
#include "imgui_impl_opengl3.h"

namespace gl
{
	class Manager
	{
	public:

        // System

		static bool InitSuccess() noexcept { return instance.success; }
        static void EnableBlend();

        // Window

        static GLFWwindow* Window() noexcept { return instance.window; }
        static bool WindowShouldClose() noexcept;
        static void SetWindowShouldClose(bool shouldClose) noexcept;
        static void WindowSwapBuffer() noexcept;

        // Event

        static void PollEvent() noexcept;

        // Key

        typedef unsigned int KeyType;

        static bool KeyDown() noexcept { return instance.keyState != 0; }
        static bool KeyDown(KeyType key) noexcept { return (instance.keyState & key) != 0; }

        // Implemented keys

        static constexpr KeyType KEY_NONE = 0;

        static constexpr KeyType KEY_UP = 1 << 0;
        static constexpr KeyType KEY_DOWN = 1 << 1;
        static constexpr KeyType KEY_LEFT = 1 << 2;
        static constexpr KeyType KEY_RIGHT = 1 << 3;
        static constexpr KeyType KEY_SHIFT = 1 << 4;

        static constexpr KeyType KEY_ESCAPE = 1 << 31;

	private:

        Manager();
        ~Manager();

        static Manager instance;

        GLFWwindow* window = nullptr;
        KeyType keyState = KEY_NONE;
        bool success = false;

        friend void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	};

    void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

	class ImguiManager
	{
	public:

		static bool InitSuccess() noexcept { return instance.success; }

	private:

        ImguiManager();
        ~ImguiManager();

        static ImguiManager instance;

		bool success = false;
	};
};


#endif // GL_MANAGER_H
