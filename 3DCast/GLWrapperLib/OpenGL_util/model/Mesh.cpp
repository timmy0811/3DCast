#include "glpch.h"
#include "Mesh.h"

GL::Model::Mesh::Mesh(std::vector<primitive::vertex::VertexMesh> vertices, std::vector<unsigned int> indices, std::vector<Texture::Texture*> textures)
	:m_Vertices(vertices), m_Indices(indices), textures(textures)
{
	// Load members
	m_IB = std::make_unique<Core::IndexBuffer>(&m_Indices[0], (unsigned int)m_Indices.size());

	// Basic Setup
	m_VB = std::make_unique<Core::VertexBuffer>(&m_Vertices[0], (unsigned int)(sizeof(primitive::vertex::VertexMesh) * m_Vertices.size()));
	m_VBLayout = std::make_unique<Core::VertexBufferLayout>();

	m_VBLayout->Push<float>(3);	// Position
	m_VBLayout->Push<float>(2);	// TexCoord
	m_VBLayout->Push<float>(3); // Normal
	m_VBLayout->Push<float>(1);	// Texture Index

	m_VA = std::make_unique<Core::VertexArray>();
	m_VA->AddBuffer(*m_VB, *m_VBLayout);
}

void GL::Model::Mesh::LoadTextures(Core::Shader& shader)
{
	unsigned int diffuseInd = 0;
	unsigned int specularInd = 0;
	unsigned int shineIndex = 0;
	unsigned int normalIndex = 0;
	unsigned int heightInd = 0;

	unsigned int bindOffset = 1;

	// const size_t amountTex = m_Textures.size();

	constexpr unsigned int MAX_TEXTURE_SLOTS = 8;

	int samplerDiffuse[MAX_TEXTURE_SLOTS];
	int samplerSpecular[MAX_TEXTURE_SLOTS];
	int samplerShine[MAX_TEXTURE_SLOTS];
	int samplerNormal[MAX_TEXTURE_SLOTS];
	int samplerHeight[MAX_TEXTURE_SLOTS];

	// Clear Arrays to 0
	memset(samplerDiffuse, 0, sizeof(samplerDiffuse));
	memset(samplerSpecular, 0, sizeof(samplerSpecular));
	memset(samplerShine, 0, sizeof(samplerShine));
	memset(samplerNormal, 0, sizeof(samplerNormal));
	memset(samplerHeight, 0, sizeof(samplerHeight));

	shader.Bind();

	for (Texture::Texture* tex : textures) {
		tex->Bind(bindOffset++);

		switch (tex->GetType()) {
		case Texture::TextureType::DIFFUSE:
			samplerDiffuse[diffuseInd++] = tex->GetBoundPort();
			break;
		case Texture::TextureType::SPECULAR:
			samplerSpecular[specularInd++] = tex->GetBoundPort();
			break;
		case Texture::TextureType::SHINE:
			samplerShine[shineIndex++] = tex->GetBoundPort();
			break;
		case Texture::TextureType::NORMAL:
			samplerNormal[normalIndex++] = tex->GetBoundPort();
			break;
		case Texture::TextureType::HEIGHT:
			samplerHeight[heightInd++] = tex->GetBoundPort();
			break;
		}
	}

	shader.SetUniform1iv("u_samplerDiffuse", MAX_TEXTURE_SLOTS, samplerDiffuse);
	shader.SetUniform1iv("u_samplerSpecular", MAX_TEXTURE_SLOTS, samplerSpecular);
	shader.SetUniform1iv("u_samplerShine", MAX_TEXTURE_SLOTS, samplerShine);
	shader.SetUniform1iv("u_samplerNormal", MAX_TEXTURE_SLOTS, samplerNormal);
	shader.SetUniform1iv("u_samplerHeight", MAX_TEXTURE_SLOTS, samplerHeight);
}

void GL::Model::Mesh::Draw(Core::Shader& shader)
{
	LoadTextures(shader);

	m_Renderer.Draw(*m_VA, *m_IB, shader);
}