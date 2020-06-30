#include "gl_manager.h"

#include "gl_buffers.h"
#include "gl_shader.h"
#include "gl_texture.h"

#include "gl_constants.h"

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"

// Utility ///////////////////////////////////////////////////////

// Rect type

template<typename T>
struct Rect {
    T x, y, w, h;
};
typedef Rect<int> Recti;
typedef Rect<float> Rectf;

// Function for filling vertex buffers

void verteciesSrcRect(float vertecies[16], glm::ivec2 textureDim, Rectf srcRect)
{
	vertecies[2] = vertecies[14] = srcRect.x / textureDim.x;
	vertecies[6] = vertecies[10] = (srcRect.x + srcRect.w) / textureDim.x;
	vertecies[3] = vertecies[7] = srcRect.y / textureDim.y;
	vertecies[11] = vertecies[15] = (srcRect.y + srcRect.h) / textureDim.y;
}

void verteciesDstRect(float vertecies[16], Rectf dstRect)
{
	vertecies[0] = vertecies[12] = dstRect.x;
	vertecies[4] = vertecies[8] = dstRect.x + dstRect.w;
	vertecies[1] = vertecies[5] = dstRect.y;
	vertecies[9] = vertecies[13] = dstRect.y + dstRect.h;
}

// Program ///////////////////////////////////////////////////////

int main()
{
	// Set up OpenGL

	if (gl::Manager::InitSuccess() == false)
	{
		return -1;
	}

	glfwSwapInterval(1);

	// Graphics

	{
		// Buffers

		gl::VertexBuffer vb;
		float vertecies[16] = { // counter-clockwize
			0, 0, 0.0, 0.0,     // bottom left
			0, 0, 1.0, 0.0,
			0, 0, 1.0, 1.0,
			0, 0, 0.0, 1.0
		};
		verteciesSrcRect(vertecies, gl::TEXTURE_DIM, { 0, 0, gl::TEXTURE_DIM.x, gl::TEXTURE_DIM.y });
		verteciesDstRect(vertecies, { 0, 0, gl::WINDOW_WIDTH, gl::WINDOW_HEIGHT });
		vb.bind();
		vb.update(16 * sizeof(float), vertecies);

		gl::VertexBufferLayout ly;
		ly.push(GL_FLOAT, 2);   // vertex coord
		ly.push(GL_FLOAT, 2);   // texture coord

		gl::VertexArray va;
		va.bind();
		va.addBuffer(vb, ly);

		gl::IndexBuffer ib;
		const GLuint indecies[6] = {
			0, 1, 2, 2, 3, 0
		};
		ib.bind();
		ib.update(6 * sizeof(GLuint), indecies);

		// Texture

        const unsigned int txSlot = 0;
        const unsigned int txColorSlot = 1;
        const unsigned int imageSlot = 2;

		gl::Texture tx(gl::TextureTarget::TEX2D, gl::PixelFormat::R8, gl::TextureWrap::WRAP);
		tx.bind(txSlot);
		tx.updatePixelData(gl::TEXTURE_DIM, nullptr);
		tx.bindToImageUnit(imageSlot);

		gl::Texture txDrawColor(gl::TextureTarget::TEX1D, gl::PixelFormat::RGB8, gl::TextureWrap::CHOP);
		txDrawColor.bind(txColorSlot);
        const uint8_t colorData[] = {
              0,   0,   0,
            230, 200,   0,
            240, 160,   0,
            250, 100,   0,
            255,  50,  10,
            230,   0,  50,
            150,   0, 130,
            100,   0, 190,
             20,   0, 220,
              0,  50, 230,
              0, 100, 200,
              0, 150, 140,
              0, 160, 120,
              0, 170, 100,
              0, 180,  80,
             10, 190,  50,
             20, 200,  15,
             30, 210,   0,
            255, 220,   0,
            255, 220,   0,
            255, 220,   0,
            255, 220,   0,
            255, 220,   0,
            255, 220,   0,
            255, 220,   0,
            255, 220,   0
        };
		txDrawColor.updatePixelData((sizeof(colorData) / sizeof(uint8_t)) / 3, colorData);

		// Shaders

		gl::GraphicShader graphicShader("res\\basic_texture_vs.glsl", "res\\basic_texture_fs.glsl");
		graphicShader.bind();
		graphicShader.setUniform1i("uPositionTexture", txSlot);
		graphicShader.setUniform1i("uColorTexture", txColorSlot);
		graphicShader.setUniformMat4f( "uMVP",
			glm::ortho(0.0f, (float)gl::WINDOW_WIDTH, 0.0f, (float)gl::WINDOW_HEIGHT, -1.0f, 1.0f)
		);

		gl::ComputeShader computeShader("res\\mandelbrot_cs.glsl");
		computeShader.bind();
		computeShader.setUniform1i("uImage", imageSlot);
        computeShader.setUniform2f("uImageDim", gl::TEXTURE_DIM.x, gl::TEXTURE_DIM.y);

		// Variables controlled by Imgui

		glm::vec2 numberCenter = { -0.25f, 0.0f };
		glm::vec2 c = { 0.0f, 0.0f };
		float rangeX = 4.0f;
		int iteration = 256;

        constexpr float rangeAddZoomPerSec = 1.0f;  // relative to rangeX
        constexpr float rangeMovePerSec = 0.25f;    // relative to rangeX

        bool shiftLeftPressed = false;
        bool shiftRightPressed = false;
		
		// Run

		bool needDraw = true;
		bool lazyDraw = true;

		computeShader.validate();
		graphicShader.validate();
		
		while (gl::Manager::WindowShouldClose() == false)
		{
			// Compute

			if (needDraw || lazyDraw == false)
			{
                graphicShader.bind();
                graphicShader.setUniform1i("uIteration", iteration);

				computeShader.bind();
				computeShader.setUniform4f("uRangeRect",numberCenter.x - rangeX / 2, numberCenter.y - (rangeX / gl::ASPECT_RATIO) / 2, rangeX, (rangeX / gl::ASPECT_RATIO) );
				computeShader.setUniform1i("uIteration", iteration);
				computeShader.compute({ gl::TEXTURE_DIM.x / gl::LOCAL_WORKGROUP_SIZE, gl::TEXTURE_DIM.y / gl::LOCAL_WORKGROUP_SIZE, 1});

				needDraw = false;
				glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);
			}

			// Draw

			glClear(GL_COLOR_BUFFER_BIT);
			graphicShader.bind();
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

			// Imgui window

            float deltaTime = ImGui::GetIO().DeltaTime; // Cached, in seconds

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			{
				ImGui::Begin("ImGui Window Title");

				ImGui::Checkbox("Lazy Draw", &lazyDraw);

				ImGui::Text("x = %.5f", numberCenter.x);
				ImGui::SameLine();
				ImGui::Text("y = %.5f", numberCenter.y);

				ImGui::Text("range = %.5f", rangeX);

				ImGui::Text("iteration = %d", iteration);

				ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
				ImGui::SameLine();
				ImGui::Text("(Delta Time %3.2f ms)", deltaTime * 1000.0f);

				ImGui::End();
			}
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			// Finish draw

            gl::Manager::WindowSwapBuffer();

			// Poll key events

            gl::Manager::PollEvent();

			if (gl::Manager::KeyDown())
			{
                using GL = gl::Manager;

				if (GL::KeyDown(GL::KEY_ESCAPE))
				{
                    GL::SetWindowShouldClose(true);
				}

				if (GL::KeyDown(GL::KEY_SHIFT))
				{
					if (GL::KeyDown(GL::KEY_UP))
					{
                        rangeX *= (1.0f + deltaTime * rangeAddZoomPerSec);
                        if (rangeX > 4.0f)
                            rangeX = 4.0f;
					}

					if (GL::KeyDown(GL::KEY_DOWN))
					{
                        rangeX *= (1.0f - deltaTime * rangeAddZoomPerSec);
                        if (rangeX < 0.00005f)
                            rangeX = 0.00005f;
					}

					if (GL::KeyDown(GL::KEY_LEFT))
					{
                        if (shiftLeftPressed == false)
                        {
                            shiftLeftPressed = true;
                            if (iteration > 128)
                                iteration -= 128;
                        }
					}
                    else
                    {
                        shiftLeftPressed = false;
                    }

					if (GL::KeyDown(GL::KEY_RIGHT))
					{
                        if (shiftRightPressed == false)
                        {
                            shiftRightPressed = true;
                            if (iteration < 2048)
                                iteration += 128;
                        }
					}
                    else
                    {
                        shiftRightPressed = false;
                    }
				}
				else
				{
                    float move = rangeX * deltaTime * rangeMovePerSec;
					numberCenter.y += (float)(GL::KeyDown(GL::KEY_UP)) * move;
                    numberCenter.y -= (float)(GL::KeyDown(GL::KEY_DOWN)) * move;
                    numberCenter.x -= (float)(GL::KeyDown(GL::KEY_LEFT)) * move;
                    numberCenter.x += (float)(GL::KeyDown(GL::KEY_RIGHT)) * move;
				}

				needDraw = true;
			}
		}
	}

    return 0;
}
