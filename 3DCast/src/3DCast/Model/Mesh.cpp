#include "castpch.h"
#include "Mesh.h"

Cast::Mesh::Mesh(const std::vector<Memory::BatchVertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<API::Texture::Texture*>& textures)
{
	Vertices = vertices;
	Indices = indices;
	Textures = textures;

	BatchId = UID::Create();

	if (Indices.empty())
		MemPos = Cast::Memory::BatchMemoryHandler.AddObject(BatchId, Vertices.data(), sizeof(Memory::BatchVertex) * vertices.size());
	else
		MemPos = Cast::Memory::BatchMemoryHandler.AddIndexedObject(BatchId, Vertices.data(), sizeof(Memory::BatchVertex) * vertices.size(), Indices.data(), (int)Indices.size());
}

void Cast::Mesh::SetMaterial(Component::MaterialComponent* material)
{
	Material = material;

	if (Material != nullptr) {
		for (API::Texture::Texture* tex : Textures) {
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