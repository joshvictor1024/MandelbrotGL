#ifndef GL_SHADER_H
#define GL_SHADER_H

#include "GL/glew.h"
#include "glm.hpp"

#include <unordered_map>

namespace gl
{
	class Shader
	{
	public:
		virtual ~Shader();
		void Validate();    // Validation should happen after uniforms are set, right before using

		void Bind() const
		{
			glUseProgram(id);
		}

		void Unbind() const
		{
			glUseProgram(0);
		}

		void SetUniform1i(const char* name, int v0)
		{
			glUniform1i(getUniformLocation(name), v0);
		}

		void SetUniform2i(const char* name, int v0, int v1)
		{
			glUniform2i(getUniformLocation(name), v0, v1);
		}

		void SetUniform2f(const char* name, float v0, float v1)
		{
			glUniform2f(getUniformLocation(name), v0, v1);
		}

		void SetUniform4f(const char* name, float v0, float v1, float v2, float v3)
		{
			glUniform4f(getUniformLocation(name), v0, v1, v2, v3);
		}

		void SetUniformMat4f(const char* name, const glm::mat4& a_matrix)
		{
			glUniformMatrix4fv(getUniformLocation(name), 1/*num of mat*/, GL_FALSE, &a_matrix[0][0]);
		}

	protected:
		GLuint compile(GLenum shaderType, const char* path) const;
		void link();

	private:
		GLint getUniformLocation(const char* name);

    protected:
        GLuint id = 0;
        std::unordered_map<const char*, GLint> mUniformLocations;
	};

	class GraphicShader : public Shader
	{
	public:
		GraphicShader(const char* vertexShaderpath, const char* fragmentShaderPath);
	};

	class ComputeShader : public Shader
	{
	public:
		ComputeShader(const char* computeShaderpath);
		void compute(glm::ivec3 workgroupCount) const;
	};
};

#endif // GL_SHADER_H
