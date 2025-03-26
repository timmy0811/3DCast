#include "castpch.h"
#include "Mesh.h"

Cast::Mesh::Mesh()
{
	BatchId = UID::Create();
}

void Cast::Mesh::SetTextures(const std::vector<Cast::Ref<API::Texture::Texture>>& textures)
{
	Textures = textures;
}

void Cast::Mesh::SetupVertexData(const std::vector<Memory::BatchVertex>& vertices, const std::vector<unsigned int>& indices)
{
	Vertices = vertices;
	Indices = indices;

	if (Indices.empty())
		MemPos = Cast::Memory::BatchMemoryHandler.AddObject(BatchId, Vertices.data(), sizeof(Memory::BatchVertex) * vertices.size());
	else
		MemPos = Cast::Memory::BatchMemoryHandler.AddIndexedObject(BatchId, Vertices.data(), sizeof(Memory::BatchVertex) * vertices.size(), Indices.data(), (int)Indices.size());
}

void Cast::Mesh::SetMaterial(Component::MaterialComponent* material)
{
	Material = material;

	if (Material != nullptr) {
		for (Cast::Ref<API::Texture::Texture> tex : Textures) {
			switch (tex->GetType()) {
			case API::Texture::TextureType::DIFFUSE:
				Material->LoadDiffuseTexture(tex);
				break;
			case API::Texture::TextureType::SPECULAR:
				Material->LoadSpecularTexture(tex);
				break;
			case API::Texture::TextureType::NORMAL:
				Material->LoadNormalTexture(tex);
				break;
			case API::Texture::TextureType::HEIGHT:
				Material->LoadParallaxTexture(tex);
				break;
			default:
				LOG_CORE_WARN("Texture %s type not supported", API::Texture::Texture::TextureTypeToString(tex->GetType()));
			}
		}
	}
}