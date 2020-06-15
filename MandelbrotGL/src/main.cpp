#include "gl_engine.h"

#include "gl_buffers.h"
#include "gl_shader.h"
#include "gl_texture.h"

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"

// Utility ///////////////////////////////////////////////////////

// Rect type

template<typename T>
struct Rect {
	T x; T y; T w; T h;
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

// Constants ///////////////////////////////////////////////////////

constexpr float ASPECT_RATIO = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;
constexpr glm::vec2 TEXTURE_DIM = { 2048.0f, 2048.0f / ASPECT_RATIO };

constexpr int LOCAL_WORKGROUP_SIZE = 32;

// Program ///////////////////////////////////////////////////////

int main()
{
	// Set up OpenGL

	gl::Context context;
	if (context.initSuccess() == false)
	{
		return -1;
	}
	glfwSwapInterval(1);
	glfwSetKeyCallback(gl::window, &gl::keyCallback);

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
		verteciesSrcRect(vertecies, TEXTURE_DIM, {0, 0, TEXTURE_DIM.x, TEXTURE_DIM.y});
		verteciesDstRect(vertecies, {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT});
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
		tx.updatePixelData(TEXTURE_DIM, nullptr);
		tx.bindToImageUnit(imageSlot);

		gl::Texture txDrawColor(gl::TextureTarget::TEX1D, gl::PixelFormat::RGB8, gl::TextureWrap::CHOP);
		txDrawColor.bind(txColorSlot);
		const uint8_t colorData[] = {
			  0,   0,   0,
			120,   0,   0,
			220,   0,   0,
			220, 230,   0,
			180,   0,   0,
			  0,   0,   0
		};
		txDrawColor.updatePixelData((sizeof(colorData) / sizeof(uint8_t)) / 3, colorData);

		// Shaders

		gl::GraphicShader graphicShader("res\\basic_texture_vs.glsl", "res\\basic_texture_fs.glsl");
		graphicShader.bind();
		graphicShader.setUniform1i("uPositionTexture", txSlot);
		graphicShader.setUniform1i("uColorTexture", txColorSlot);
		graphicShader.setUniformMat4f( "uMVP",
			glm::ortho(0.0f, (float)WINDOW_WIDTH, 0.0f, (float)WINDOW_HEIGHT, -1.0f, 1.0f)
		);

		gl::ComputeShader computeShader("res\\mandelbrot_cs.glsl");
		computeShader.bind();
		computeShader.setUniform1i("uImage", imageSlot);
		computeShader.setUniform2f("uImageDim", TEXTURE_DIM.x, TEXTURE_DIM.y );

		// Imgui

		gl::ImguiContext imguiContext("#version 430 core");

		glm::vec2 numberCenter = { -0.25f, 0.0f };
		glm::vec2 c = { 0.0f, 0.0f };
		float rangeX = 4.0f;
		int iteration = 256;
		
		// Run

		bool needDraw = true;
		bool lazyDraw = true;

		computeShader.validate();
		graphicShader.validate();
		
		while (!gl::windowShouldClose())
		{
			// Compute

			if (needDraw || !lazyDraw)
			{
				computeShader.bind();
				computeShader.setUniform4f("uRangeRect", numberCenter.x - rangeX / 2, numberCenter.y - (rangeX / ASPECT_RATIO) / 2, rangeX, (rangeX / ASPECT_RATIO) );
				computeShader.setUniform1i("uIteration", iteration);
				computeShader.compute({TEXTURE_DIM.x / LOCAL_WORKGROUP_SIZE, TEXTURE_DIM.y / LOCAL_WORKGROUP_SIZE, 1});

				needDraw = false;
				glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);
			}

			// Draw

			glClear(GL_COLOR_BUFFER_BIT);
			graphicShader.bind();
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

			// Imgui window

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
				ImGui::Text("(Delta Time %3.2f ms)", ImGui::GetIO().DeltaTime * 1000.0f);

				ImGui::End();
			}
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			// Finish draw

			gl::windowSwapBuffer();

			// Poll key events

			gl::pollEvent();

			if (gl::keyDown())
			{
				if (gl::keyDown(gl::KEY_ESCAPE))
				{
					glfwSetWindowShouldClose(gl::window, 1);
				}

				if (gl::keyDown(gl::KEY_SHIFT))
				{
					if (gl::keyDown(gl::KEY_UP))
					{
						rangeX *= (1.0f / 0.9f);
						if (rangeX > 4.0f)
							rangeX = 4.0f;
					}
					if (gl::keyDown(gl::KEY_DOWN))
					{
						rangeX *= 0.9f;
					}
					if (gl::keyDown(gl::KEY_LEFT))
					{
						if (iteration > 128)
							iteration -= 128;
					}
					if (gl::keyDown(gl::KEY_RIGHT))
					{
						iteration += 128;
					}

					gl::setKeyState(gl::KEY_SHIFT);
				}
				else
				{
					if (gl::keyDown(gl::KEY_UP))
					{
						numberCenter.y += rangeX / 64;
					}
					if (gl::keyDown(gl::KEY_DOWN))
					{
						numberCenter.y -= rangeX / 64;
					}
					if (gl::keyDown(gl::KEY_LEFT))
					{
						numberCenter.x -= rangeX / 64;
					}
					if (gl::keyDown(gl::KEY_RIGHT))
					{
						numberCenter.x += rangeX / 64;
					}

					gl::setKeyState(gl::KEY_NONE);
				}
				needDraw = true;
			}
		}
	}

    return 0;
}
