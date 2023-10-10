#pragma once

#include "debug/Debug.h"

#include "core/VertexBuffer.h"
#include "core/VertexArray.h"
#include "core/VertexBufferLayout.h"
#include "core/IndexBuffer.h"
#include "core/Renderer.h"

#include "texture/Texture.h"
#include "misc/Primitive.hpp"

#include <vector>

namespace GL::Model {
	class Mesh
	{
	public:
		Mesh(std::vector<GL::primitive::vertex::VertexMesh> vertices, std::vector<unsigned int> indices, std::vector<GL::Texture::Texture*> textures);

		void Draw(Core::Shader& shader);

	private:
		std::vector<primitive::vertex::VertexMesh> m_Vertices;
		std::vector<unsigned int> m_Indices;
		std::vector<Texture::Texture*> textures;

		std::unique_ptr<Core::VertexArray> m_VA;
		std::unique_ptr<Core::VertexBuffer> m_VB;
		std::unique_ptr<Core::VertexBufferLayout> m_VBLayout;
		std::unique_ptr<Core::IndexBuffer> m_IB;

		Core::GLContext m_Renderer;

		void LoadTextures(Core::Shader& shader);
	};
}
