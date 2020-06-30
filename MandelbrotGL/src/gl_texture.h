#ifndef GL_TEXTURE_H
#define GL_TEXTURE_H

#include "GL/glew.h"
#include "glm.hpp"

namespace gl
{
	enum class TextureTarget
	{
		TEX1D, TEX2D
	};

	enum class PixelFormat
	{
		RGB8, RGBA8, RGBA32F, R8, R32F, R8UI
	};

	enum class TextureWrap
	{        
		CHOP,   // Stops beyond 0 to 1        
        SMEAR,  // Stretch edge pixels if beyond 0 to 1
        WRAP    // Repeat the texture
	};

	class Texture
	{
	public:

		Texture(TextureTarget textureTarget, PixelFormat pixelFormat, TextureWrap textureWrap);
        Texture(const Texture& rhs) = delete;
        Texture(const Texture&& rhs) = delete;
		~Texture();
        Texture& operator=(const Texture& rhs) = delete;
        Texture& operator=(const Texture&& rhs) = delete;

		void Bind(unsigned int slot = 0);
		void Unbind();

		void UpdatePixelData(glm::ivec2 dataDimension, const void* pixelData);  // 2D overload
		void UpdatePixelData(int dataDimension, const void* pixelData);         // 1D overload
		void BindToImageUnit(unsigned int slot = 0);

		//constexpr int getPixelDataStride();

    private:

        unsigned int id = 0;
        GLenum target;
        GLenum pixelFormat;
        GLenum internalPixelFormat;
        GLenum pixelType;
        GLenum wrap;
	};
};

#endif // GL_TEXTURE_H
