#include "gl_texture.h"

namespace gl
{
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
        case TextureTarget::TEX1D: break;                   // No "t" if 1D
        case TextureTarget::TEX2D: glTexParameteri(target, GL_TEXTURE_WRAP_T, wrap); break;
        }

        glBindTexture(target, 0);
    }

    Texture::~Texture()
    {
        glDeleteTextures(1, &id);
    }

    void Texture::Bind(unsigned int slot)
    {
        glActiveTexture(GL_TEXTURE0 + slot);   // Slot is used as a uniform
        glBindTexture(target, id);
    }
    void Texture::Unbind()
    {
        glBindTexture(target, 0);
    }

    void Texture::UpdatePixelData(glm::ivec2 dataDimension, const void* pixelData)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, internalPixelFormat, dataDimension.x, dataDimension.y, 0, pixelFormat, pixelType, pixelData);
    }

    void Texture::UpdatePixelData(int dataWidth, const void* pixelData)
    {
        glTexImage1D(GL_TEXTURE_1D, 0, internalPixelFormat, dataWidth, 0, pixelFormat, pixelType, pixelData);
    }

    void Texture::BindToImageUnit(unsigned int slot)
    {
        switch (internalPixelFormat)
        {
        case GL_RGB8:
        case GL_RGBA8:
            printf("Current pixel format not supported by image unit!\n");
            break;

        // Might be dangerous
        default:
            glBindImageTexture(slot, id, 0, GL_FALSE, 0, GL_WRITE_ONLY, internalPixelFormat);
            break;
        }
    }
};