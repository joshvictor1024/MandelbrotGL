#include <fstream>
#include <string>
#include <stdio.h>

#include "gl_shader.h"

namespace gl
{
	GraphicShader::GraphicShader(const char* vertexShaderpath, const char* fragmentShaderPath)
	{
		mId = glCreateProgram();
		GLuint vertexShaderId = compile(GL_VERTEX_SHADER, vertexShaderpath);
		GLuint fragmentShaderId = compile(GL_FRAGMENT_SHADER, fragmentShaderPath);

		glAttachShader(mId, vertexShaderId);
		glAttachShader(mId, fragmentShaderId);
		link();

		glDeleteShader(vertexShaderId);//glDetachShader()?
		glDeleteShader(fragmentShaderId);

		GLenum err = glGetError();
		if (err)
		{
			printf("error %d\n", err);
		}
	}
	ComputeShader::ComputeShader(const char* computeShaderpath)
	{
		mId = glCreateProgram();
		GLuint computeShaderId = compile(GL_COMPUTE_SHADER, computeShaderpath);

		glAttachShader(mId, computeShaderId);
		link();

		glDeleteShader(computeShaderId);//glDetachShader()?

		GLenum err = glGetError();
		if (err)
		{
			printf("error %d\n", err);
		}
	}
	Shader::~Shader()
	{
		glDeleteProgram(mId);
	}

	GLuint Shader::compile(GLenum shaderType, const char* path)
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
					printf("not opened\n");
			}
			const char* src = bufferString.c_str();
			//printf("char count %d\n%s", bufferString.size(), src);//FIX: read file wrong behavior
			glShaderSource(shaderId, 1, &src, nullptr);
			glCompileShader(shaderId);

			int compileResult;
			glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileResult);
			if (!compileResult)
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
		glLinkProgram(mId);
		{
			GLint linkResult;
			glGetProgramiv(mId,  GL_LINK_STATUS, &linkResult);
			if (linkResult == GL_FALSE)
			{
				GLint length;
				char message[256] = {};

				glGetProgramiv(mId, GL_INFO_LOG_LENGTH, &length);
				glGetProgramInfoLog(mId, length, &length, message);
				printf("%s", message);
			}
		}
	}
	void Shader::validate()
	{
		glValidateProgram(mId);
		{
			GLint validateResult;
			glGetProgramiv(mId,  GL_VALIDATE_STATUS, &validateResult);
			if (!validateResult)
			{
				GLint length;
				char message[256] = {};

				glGetProgramiv(mId, GL_INFO_LOG_LENGTH, &length);
				glGetProgramInfoLog(mId, length, &length, message);
				printf("%s", message);
			}
		}
	}

	GLint Shader::getUniformLocation(const char* name)
	{
		if (mUniformLocations.find(name) != mUniformLocations.end())
			return mUniformLocations[name];

		GLint location = glGetUniformLocation(mId, name);
		if (location == -1)
			printf("Warning: uniform '%s' doesn't exist\n", name);

		mUniformLocations[name] = location;
		return location;
	}

	void ComputeShader::compute(glm::ivec3 workgroupCount)
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
