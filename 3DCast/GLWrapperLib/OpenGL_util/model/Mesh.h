#pragma once

#include <vector>

#include "../debug/Debug.hpp"

#include "../core/VertexBuffer.h"
#include "../core/VertexArray.h"
#include "../core/VertexBufferLayout.h"
#include "../core/IndexBuffer.h"
#include "../core/Renderer.h"

#include "../texture/Texture.h"
#include "../misc/Primitive.hpp"

namespace GL::model {
	class Mesh
	{
	private:
		std::vector<primitive::vertex::VertexMesh> m_Vertices;
		std::vector<unsigned int> m_Indices;
		std::vector<texture::Texture*> textures;

		std::unique_ptr<core::VertexArray> m_VA;
		std::unique_ptr<core::VertexBuffer> m_VB;
		std::unique_ptr<core::VertexBufferLayout> m_VBLayout;
		std::unique_ptr<core::IndexBuffer> m_IB;

		core::GLContext m_Renderer;

		void LoadTextures(core::Shader& shader);

	public:
		Mesh(std::vector<GL::primitive::vertex::VertexMesh> vertices, std::vector<unsigned int> indices, std::vector<GL::texture::Texture*> textures);

		void Draw(core::Shader& shader);
	};
}
