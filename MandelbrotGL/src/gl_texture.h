#ifndef GL_TEXTURE_H
#define GL_TEXTURE_H

#include "glm.hpp"

namespace gl
{
    // Parameter enums

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

    // Class def

	class Texture
	{
	public:
		Texture(TextureTarget textureTarget, PixelFormat pixelFormat, TextureWrap textureWrap);
		~Texture();

		void bind(unsigned int slot = 0);
		void unbind();

		void updatePixelData(glm::ivec2 dataDimension, const void* pixelData);  // 2D overload
		void updatePixelData(int dataDimension, const void* pixelData);         // 1D overload
		void bindToImageUnit(unsigned int slot = 0);

		//constexpr int getPixelDataStride();

    private:
        unsigned int id = 0;
        GLenum target;
        GLenum pixelFormat;
        GLenum internalPixelFormat;
        GLenum pixelType;
        GLenum wrap;
	};

    // Method def

	Texture::Texture(TextureTarget textureTarget, PixelFormat pixelFormat, TextureWrap textureWrap = TextureWrap::CHOP)
	{	
        // Parameter setting

		this->target = [textureTarget]()
		{
			switch (textureTarget)
			{
			case TextureTarget::TEX1D: return GL_TEXTURE_1D;
			case TextureTarget::TEX2D: return GL_TEXTURE_2D;
			}
		}();

		this->pixelFormat = [pixelFormat]()
		{
			switch (pixelFormat)
			{
			case PixelFormat::R8:
			case PixelFormat::R32F: return GL_RED;
            case PixelFormat::R8UI: return GL_RED_INTEGER;
			case PixelFormat::RGB8: return GL_RGB;
			case PixelFormat::RGBA8:
			case PixelFormat::RGBA32F: return GL_RGBA;
			}
		}();

		this->internalPixelFormat = [pixelFormat]()
		{
			switch (pixelFormat)
			{
			case PixelFormat::R8: return GL_R8;
			case PixelFormat::R32F: return GL_R32F;
            case PixelFormat::R8UI: return GL_R8UI;
			case PixelFormat::RGB8: return GL_RGB8;
			case PixelFormat::RGBA8: return GL_RGBA8;
			case PixelFormat::RGBA32F: return GL_RGBA32F;
			}
		}();

		this->pixelType = [pixelFormat]()
		{
			switch (pixelFormat)
			{
			case PixelFormat::R8:
            case PixelFormat::R8UI:
			case PixelFormat::RGB8:
			case PixelFormat::RGBA8: return GL_UNSIGNED_BYTE;
			case PixelFormat::R32F:
			case PixelFormat::RGBA32F: return GL_FLOAT;
			}
		}();

		wrap = [textureWrap]()
		{
			switch (textureWrap)
			{
			case TextureWrap::CHOP: return GL_CLAMP_TO_BORDER;
			case TextureWrap::SMEAR: return GL_CLAMP_TO_EDGE;
			case TextureWrap::WRAP: return GL_REPEAT;
			}
		}();

        // GL code

		glGenTextures(1, &id);
		glBindTexture(target, id);

		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(target, GL_TEXTURE_WRAP_S, wrap);   // (s, t) means (x, y)
		switch (textureTarget)
		{
		case TextureTarget::TEX1D: break;                   // no t if 1D
		case TextureTarget::TEX2D: glTexParameteri(target, GL_TEXTURE_WRAP_T, wrap); break;		
		}

		glBindTexture(target, 0);
	}

	Texture::~Texture()
	{
		glDeleteTextures(1, &id);
	}

	void Texture::bind(unsigned int slot)
	{
		glActiveTexture(GL_TEXTURE0 + slot);   // Slot is used as a uniform
		glBindTexture(target, id);
	}
	void Texture::unbind()
	{
		glBindTexture(target, 0);
	}

	void Texture::updatePixelData(glm::ivec2 dataDimension, const void* pixelData)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, internalPixelFormat, dataDimension.x, dataDimension.y, 0, pixelFormat, pixelType, pixelData);
	}

	void Texture::updatePixelData(int dataWidth, const void* pixelData)
	{
		glTexImage1D(GL_TEXTURE_1D, 0, internalPixelFormat, dataWidth, 0, pixelFormat, pixelType, pixelData);
	}

	void Texture::bindToImageUnit(unsigned int slot)
	{
		switch (internalPixelFormat)
		{
		case GL_RGB8:
		case GL_RGBA8:
			printf("Current pixel format not supported by image unit!\n");
			break;
		
		// might be dangerous
		default:
			glBindImageTexture(slot, id, 0, GL_FALSE, 0, GL_WRITE_ONLY, internalPixelFormat);
			break;
		}
	}
};

#endif // GL_TEXTURE_H
