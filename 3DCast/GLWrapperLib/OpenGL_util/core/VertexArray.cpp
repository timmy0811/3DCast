#include "glpch.h"
#include "VertexArray.h"

GL::Core::VertexArray::VertexArray()
{
	GLCall(glGenVertexArrays(1, &m_RendererID));
	//GLCall(glBindVertexArray(m_RendererID));
}

GL::Core::VertexArray::~VertexArray()
{
	GLCall(glDeleteVertexArrays(1, &m_RendererID));
}

void GL::Core::VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
{
	Bind();
	vb.Bind();
	const auto& elements = layout.GetElements();
	unsigned int offset = 0;

	for (int i = 0; i < elements.size(); i++) {
		const auto& element = elements[i];
		GLCall(glEnableVertexAttribArray(i));
#pragma warning(push)
#pragma warning(disable:4312)
		GLCall(glVertexAttribPointer(i, element.count, element.type, element.normalised, layout.GetStride(), (const void*)offset));
#pragma warning(pop)
		offset += element.count * VertexBufferElement::GetSize(element.type);
	}
}

void GL::Core::VertexArray::Bind() const
{
	GLCall(glBindVertexArray(m_RendererID));
}

void GL::Core::VertexArray::Unbind() const
{
	GLCall(glBindVertexArray(0));
}