#include "gl_manager.h"
#include "gl_constants.h"

#include <stdio.h>

namespace gl
{
    Manager Manager::instance;

	Manager::Manager()
	{
		/* Initialize the library */
		if (glfwInit() == false)
		{
			return;
		}

		/* Create a windowed mode window and its OpenGL context */
		window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Mandelbrot", NULL, NULL);
		if (window == nullptr)
		{
			glfwTerminate();
			return;
		}

		/* Make the window's context current */
		glfwMakeContextCurrent(window);

		// Init glew after context is created
		GLenum err = glewInit();
		if (err != GLEW_OK)
		{
			/* Problem: glewInit failed, something is seriously wrong. */
			printf("Error: %s\n", glewGetErrorString(err));
			return;
		}

        glfwSetKeyCallback(window, &keyCallback);

		printf("OpenGL version: %s\n", glGetString(GL_VERSION));
		printf("GLEW version: %s\n", glewGetString(GLEW_VERSION));

		success = true;
	}

	Manager::~Manager()
	{
		glfwDestroyWindow(window);
		glfwTerminate();
	}

    // System

	void Manager::EnableBlend()
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	// Window

	bool Manager::WindowShouldClose() noexcept
	{
		return glfwWindowShouldClose(Manager::Window());
	}

    void Manager::SetWindowShouldClose(bool shouldClose) noexcept
    {
        glfwSetWindowShouldClose(Manager::Window(), (int)shouldClose);
    }

	void Manager::WindowSwapBuffer() noexcept
	{
		/* Swap front and back buffers */
		glfwSwapBuffers(Manager::Window());
	}

	// Event

    void Manager::PollEvent() noexcept
    {
		/* Poll for and process events */
		glfwPollEvents();
    }

    // Key

    void keyCallback(GLFWwindow* window, int glfwKey, int scancode, int action, int mods)
    {
		Manager::KeyType& keyState = Manager::instance.keyState;
        auto pressOrRelease = [&keyState, action](Manager::KeyType key)
        {
            switch (action)
            {
            case GLFW_PRESS:
            case GLFW_REPEAT:
                keyState |= key; break;

            case GLFW_RELEASE:
                keyState &= ~key; break;

            default: break;
            }
        };

        switch (glfwKey)
        {
        case GLFW_KEY_UP: pressOrRelease(Manager::KEY_UP); break;
        case GLFW_KEY_DOWN: pressOrRelease(Manager::KEY_DOWN); break;
        case GLFW_KEY_LEFT: pressOrRelease(Manager::KEY_LEFT); break;
        case GLFW_KEY_RIGHT: pressOrRelease(Manager::KEY_RIGHT); break;
        case GLFW_KEY_LEFT_SHIFT:
        case GLFW_KEY_RIGHT_SHIFT: pressOrRelease(Manager::KEY_SHIFT); break;

        case GLFW_KEY_ESCAPE: pressOrRelease(Manager::KEY_ESCAPE); break;
        default: break;
        }
    }

    ImguiManager ImguiManager::instance;

    ImguiManager::ImguiManager()
    {
        ImGui::CreateContext();
        ImGui_ImplGlfw_InitForOpenGL(Manager::Window(), true);
        ImGui_ImplOpenGL3_Init(GLSL_VERSION_MACRO);

        ImGui::StyleColorsDark();

        success = true;
    }

    ImguiManager::~ImguiManager()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
};
