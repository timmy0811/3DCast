#include "castpch.h"
#include "Mesh.h"

#include "3DCast/Data/GlobalShared.h"

Cast::Mesh::Mesh()
{
}

void Cast::Mesh::SetTextures(const std::vector<Cast::Ref<API::Texture::Texture>>& textures)
{
	Textures = textures;
}

void Cast::Mesh::SetupVertexData()
{
	if (Indices.empty())
		BatchId = Cast::Memory::BatchMemoryHandler.CreateBatchObject(Vertices.data(), sizeof(Memory::BatchVertex) * Vertices.size());
	else
		BatchId = Cast::Memory::BatchMemoryHandler.CreateBatchObject(Vertices.data(), sizeof(Memory::BatchVertex) * Vertices.size(), Indices.data(), (int)Indices.size());

	if (BatchId != Cast::UID::None())
	{
		Shared.VertexEntities[BatchId] = this;
	}
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

void Cast::Mesh::RemoveFromBatchStorage()
{
	if (BatchId != Cast::UID::None()) {
		Cast::Memory::BatchMemoryHandler.RemoveObject(BatchId);
		BatchId = Cast::UID::None();
	}
}

void Cast::Mesh::RetransferToBatchMemory()
{
	if (Indices.empty())
		Cast::Memory::BatchMemoryHandler.OnBatchEmptyRetransfer(BatchId, Vertices.data(), sizeof(Memory::BatchVertex) * Vertices.size());
	else
		Cast::Memory::BatchMemoryHandler.OnBatchEmptyRetransfer(BatchId, Vertices.data(), sizeof(Memory::BatchVertex) * Vertices.size(), Indices.data(), (int)Indices.size());
}