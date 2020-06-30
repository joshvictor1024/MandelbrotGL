#ifndef GL_BUFFERS_H
#define GL_BUFFERS_H

#include "GL/glew.h"

#include <vector>
#include <stdio.h>

namespace gl
{
	class VertexBuffer
	{
	public:

		VertexBuffer(GLenum purpose = GL_STATIC_DRAW):
			purpose(purpose)
		{
			glGenBuffers(1, &id);
		}

		~VertexBuffer()
		{
			glDeleteBuffers(1, &id);
		}

		void Bind() const
		{
			glBindBuffer(GL_ARRAY_BUFFER, id);
		}

		void Unbind() const
		{
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		void update(unsigned int size, const void* data) const
		{
			glBufferData(GL_ARRAY_BUFFER, size, data, purpose);
		}

    private:

        GLuint id = 0;
        GLenum purpose;
	};

	class IndexBuffer
	{
	public:

		IndexBuffer(GLenum purpose = GL_STATIC_DRAW):
			purpose(purpose)
		{
			glGenBuffers(1, &id);
		}

		~IndexBuffer()
		{
			glDeleteBuffers(1, &id);
		}

		void Bind() const
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
		}

		void Unbind() const
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}

		void update(unsigned int size, const void* data) const
		{
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, purpose);
		}

    private:

        GLuint id = 0;
        GLenum purpose;
	};

	struct VertexBufferElement
	{
        static constexpr unsigned int sizeofGL(GLenum type)
        {
            switch (type)
            {
            case GL_FLOAT:          return sizeof(GLfloat);
            case GL_UNSIGNED_INT:   return sizeof(GLuint);
            case GL_UNSIGNED_BYTE:  return sizeof(GLubyte);
            }

            // Maybe assert that unimplemented type isn't used
            printf("Unimplemented type is used for a vertex buffer element!\n");

            return 0;
        }

		GLenum type;
		unsigned int count;
		bool normalized;
	};

	class VertexBufferLayout
	{
	public:

		void push(GLenum type, unsigned int count)
		{
			elements.push_back({ type, count, GL_FALSE});
			stride += count * VertexBufferElement::sizeofGL(type);
		}

		inline const std::vector<VertexBufferElement>& getElements() const { return elements; }
		inline unsigned int getStride() const { return stride; }

    private:

        std::vector<VertexBufferElement> elements;
        unsigned int stride = 0;
	};

	class VertexArray
	{
	public:

		VertexArray()
		{
			glGenVertexArrays(1, &id);
		}

		~VertexArray()
		{
			glDeleteVertexArrays(1, &id);
		}

		void Bind() const
		{
			glBindVertexArray(id);
		}

		void Unbind() const
		{
			glBindVertexArray(0);
		}

		void addBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
		{
			vb.Bind();

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

    private:

        GLuint id;
	};
};


#endif // GL_BUFFERS_H
