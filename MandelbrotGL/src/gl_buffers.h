#ifndef GL_BUFFERS_H
#define GL_BUFFERS_H

#include "GL/glew.h"

#include <vector>
#include <stdio.h>

namespace gl
{
	class VertexBuffer
	{
		GLuint mId = 0;
		GLenum mPurpose;

	public:
		VertexBuffer(GLenum purpose = GL_STATIC_DRAW):
			mPurpose(purpose)
		{
			glGenBuffers(1, &mId);
		}
		~VertexBuffer()
		{
			glDeleteBuffers(1, &mId);
		}

		void bind() const
		{
			glBindBuffer(GL_ARRAY_BUFFER, mId);
		}
		void unbind() const
		{
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
		void update(unsigned int size, const void* data)
		{
			glBufferData(GL_ARRAY_BUFFER, size, data, mPurpose);
		}
	};


	class IndexBuffer
	{
		GLuint mId = 0;
		GLenum mPurpose;

	public:
		IndexBuffer(GLenum purpose = GL_STATIC_DRAW):
			mPurpose(purpose)
		{
			glGenBuffers(1, &mId);
		}
		~IndexBuffer()
		{
			glDeleteBuffers(1, &mId);
		}

		void bind() const
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mId);
		}
		void unbind() const
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
		void update(unsigned int size, const void* data)
		{
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, mPurpose);
		}
	};


	struct VertexBufferElement
	{
		GLenum type;
		unsigned int count;
		bool normalized;

		static unsigned int sizeofGL(GLenum type)
		{
			switch (type)
			{
			case GL_FLOAT:			return sizeof(GLfloat);
			case GL_UNSIGNED_INT:	return sizeof(GLuint);
			case GL_UNSIGNED_BYTE:	return sizeof(GLubyte);
			}
			//assert
			return 0;
		}
	};
	class VertexBufferLayout
	{
	private:
		std::vector<VertexBufferElement> mElements;
		unsigned int mStride = 0;

	public:
		void push(GLenum type, unsigned int count)
		{
			mElements.push_back({ type, count, GL_FALSE});
			mStride += count * VertexBufferElement::sizeofGL(type);
		}

		inline const std::vector<VertexBufferElement>& getElements() const { return mElements; }
		inline unsigned int getStride() const { return mStride; }
	};


	class VertexArray
	{
	private:
		GLuint mId;

	public:
		VertexArray()
		{
			glGenVertexArrays(1, &mId);
		}
		~VertexArray()
		{
			glDeleteVertexArrays(1, &mId);
		}

		void bind() const
		{
			glBindVertexArray(mId);
		}
		void unbind() const
		{
			glBindVertexArray(0);
		}

		void addBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
		{
			vb.bind();

			const auto& elements = layout.getElements();
			unsigned int offset = 0;
			for (unsigned int i = 0; i < elements.size(); i++)
			{
				const auto& element = elements[i];
				glEnableVertexAttribArray(i);
				glVertexAttribPointer(
					i, element.count, element.type, element.normalized, layout.getStride(), (const void*)offset
				);

				offset += element.count * VertexBufferElement::sizeofGL(element.type);
			}
		}
	};
};


#endif // GL_BUFFERS_H
