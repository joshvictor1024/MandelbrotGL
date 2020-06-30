#include <fstream>
#include <string>
#include <stdio.h>

#include "gl_shader.h"

namespace gl
{
	GraphicShader::GraphicShader(const char* vertexShaderpath, const char* fragmentShaderPath)
	{
		id = glCreateProgram();
		GLuint vertexShaderId = compile(GL_VERTEX_SHADER, vertexShaderpath);
		GLuint fragmentShaderId = compile(GL_FRAGMENT_SHADER, fragmentShaderPath);

		glAttachShader(id, vertexShaderId);
		glAttachShader(id, fragmentShaderId);
		link();

		glDeleteShader(vertexShaderId); // glDetachShader()?
		glDeleteShader(fragmentShaderId);

		GLenum error = glGetError();
		if (error != 0)
		{
			printf("error %d\n", error);
		}
	}

	ComputeShader::ComputeShader(const char* computeShaderpath)
	{
		id = glCreateProgram();
		GLuint computeShaderId = compile(GL_COMPUTE_SHADER, computeShaderpath);

		glAttachShader(id, computeShaderId);
		link();

		glDeleteShader(computeShaderId); // glDetachShader()?

        GLenum error = glGetError();
        if (error != 0)
        {
            printf("error %d\n", error);
        }
	}

	Shader::~Shader()
	{
		glDeleteProgram(id);
	}

	GLuint Shader::compile(GLenum shaderType, const char* path) const
	{
		GLuint shaderId = glCreateShader(shaderType);

		std::string bufferString;
		{
			{
				std::fstream source(path, std::ios::in);

				if (source)
				{
					source.seekg(0, std::ios::end);
					bufferString.resize(source.tellg());

					source.seekg(0, std::ios::beg);
					source.read(&bufferString[0], bufferString.size());
				}
                else
                {
                    printf("not opened\n");
                }
			}
			const char* src = bufferString.c_str();
			//printf("char count %d\n%s", bufferString.size(), src);    //FIX: read file wrong behavior
			glShaderSource(shaderId, 1, &src, nullptr);
			glCompileShader(shaderId);

			int compileResult;
			glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileResult);

			if (compileResult == 0)
			{
				char message[256] = {};
				GLint length;

				glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &length);
				//char* message = (char*)alloca(length * sizeof(char));//char message[length], the point is that it's on the stack
				glGetShaderInfoLog (shaderId, length, &length, message);

				printf("%s", message);
			}
		}
		return shaderId;
	}
	void Shader::link()
	{
		glLinkProgram(id);
		{
			GLint linkResult;
			glGetProgramiv(id,  GL_LINK_STATUS, &linkResult);

			if (linkResult == GL_FALSE)
			{
				GLint length;
				char message[256] = {};

				glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length);
				glGetProgramInfoLog(id, length, &length, message);
				printf("%s", message);
			}
		}
	}
	void Shader::Validate()
	{
		glValidateProgram(id);
		{
			GLint validateResult;
			glGetProgramiv(id,  GL_VALIDATE_STATUS, &validateResult);

			if (!validateResult)
			{
				GLint length;
				char message[256] = {};

				glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length);
				glGetProgramInfoLog(id, length, &length, message);
				printf("%s", message);
			}
		}
	}

	GLint Shader::getUniformLocation(const char* name)
	{
		if (mUniformLocations.find(name) != mUniformLocations.end())
			return mUniformLocations[name];

		GLint location = glGetUniformLocation(id, name);
        if (location == -1)
        {
            printf("Warning: uniform '%s' doesn't exist\n", name);
        }

		mUniformLocations[name] = location;
		return location;
	}

	void ComputeShader::compute(glm::ivec3 workgroupCount) const
	{
		glDispatchCompute(workgroupCount.x, workgroupCount.y, workgroupCount.z);
	}
};

//void gpuTest()
//{
//	int work_grp_cnt[3];
//
//	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_cnt[0]);
//	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_cnt[1]);
//	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_cnt[2]);
//
//	printf("max global (total) work group size x:%i y:%i z:%i\n",
//	  work_grp_cnt[0], work_grp_cnt[1], work_grp_cnt[2]);
//
//
//	int work_grp_size[3];
//
//	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_grp_size[0]);
//	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_grp_size[1]);
//	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_grp_size[2]);
//
//	printf("max local (in one shader) work group sizes x:%i y:%i z:%i\n",
//	  work_grp_size[0], work_grp_size[1], work_grp_size[2]);
//
//
//	int work_grp_inv;
//	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &work_grp_inv);
//	printf("max local work group invocations %i\n", work_grp_inv);
//}
