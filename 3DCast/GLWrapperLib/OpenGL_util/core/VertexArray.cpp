#include "VertexArray.h"

GL::core::VertexArray::VertexArray()
{
	GLCall(glGenVertexArrays(1, &m_RendererID));
	//GLCall(glBindVertexArray(m_RendererID));
}

GL::core::VertexArray::~VertexArray()
{
	GLCall(glDeleteVertexArrays(1, &m_RendererID));
}

void GL::core::VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
{
	Bind();
	vb.Bind();
	const auto& elements = layout.GetElements();
	unsigned int offset = 0;

	for (int i = 0; i < elements.size(); i++) {
		const auto& element = elements[i];
		GLCall(glEnableVertexAttribArray(i));
		GLCall(glVertexAttribPointer(i, element.count, element.type, element.normalised, layout.GetStride(), (const void*)offset));
		offset += element.count * VertexBufferElement::GetSize(element.type);
	}
}

void GL::core::VertexArray::Bind() const
{
	GLCall(glBindVertexArray(m_RendererID));
}

void GL::core::VertexArray::Unbind() const
{
	GLCall(glBindVertexArray(0));
}