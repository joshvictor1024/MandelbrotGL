#include "gl_engine.h"

#include <stdio.h>

namespace gl
{
	// System

	GLFWwindow* window = nullptr;

	Context::Context()
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

		printf("OpenGL version: %s\n", glGetString(GL_VERSION));
		printf("GLEW version: %s\n", glewGetString(GLEW_VERSION));

		success = true;
	}

	Context::~Context()
	{
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	ImguiContext::ImguiContext(const char* glslVersionMacro)
	{
		ImGui::CreateContext();
		ImGui_ImplGlfw_InitForOpenGL(gl::window, true);
		ImGui_ImplOpenGL3_Init(glslVersionMacro);

		ImGui::StyleColorsDark();

		success = true;
	}

	ImguiContext::~ImguiContext()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void enableBlend()
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	// Window

	bool windowShouldClose()
	{
		return glfwWindowShouldClose(window);
	}

	void windowSwapBuffer()
	{
		/* Swap front and back buffers */
		glfwSwapBuffers(window);
	}

	// Event

    void pollEvent()
    {
		/* Poll for and process events */
		glfwPollEvents();
    }

    // Key

    KeyType keyState = KEY_NONE;

    void keyCallback(GLFWwindow* window, int glfwKey, int scancode, int action, int mods)
    {
		KeyType& keyState1 = keyState;
        auto pressOrRelease = [&keyState1, action](KeyType key)
        {
            switch (action)
            {
            case GLFW_PRESS:
            case GLFW_REPEAT:
                keyState1 |= key; break;

            case GLFW_RELEASE:
                keyState1 &= ~key; break;

            default: break;
            }
        };

        switch (glfwKey)
        {
        case GLFW_KEY_UP: pressOrRelease(KEY_UP); break;
        case GLFW_KEY_DOWN: pressOrRelease(KEY_DOWN); break;
        case GLFW_KEY_LEFT: pressOrRelease(KEY_LEFT); break;
        case GLFW_KEY_RIGHT: pressOrRelease(KEY_RIGHT); break;
        case GLFW_KEY_LEFT_SHIFT:
        case GLFW_KEY_RIGHT_SHIFT: pressOrRelease(KEY_SHIFT); break;

        case GLFW_KEY_ESCAPE: pressOrRelease(KEY_ESCAPE); break;
        default: break;
        }
    }
};
