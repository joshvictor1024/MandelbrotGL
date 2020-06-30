#ifndef GL_CONSTANTS_H
#define GL_CONSTANTS_H

#include "glm.hpp"

namespace gl
{
    constexpr int WINDOW_WIDTH = 1280;
    constexpr int WINDOW_HEIGHT = 720;
    constexpr float ASPECT_RATIO = (float)gl::WINDOW_WIDTH / (float)gl::WINDOW_HEIGHT;
    constexpr glm::vec2 TEXTURE_DIM = { 2048.0f, 2048.0f / ASPECT_RATIO };

    static const char* GLSL_VERSION_MACRO = "#version 430 core";
    constexpr int LOCAL_WORKGROUP_SIZE = 32;
};

#endif // !GL_CONSTANTS_H

