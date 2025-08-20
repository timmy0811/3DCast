#include "castpch.h"
#include "Mesh.h"

#include "3DCast/Data/GlobalShared.h"

Cast::Mesh::Mesh(): BatchId(UID::None())
{
}

void Cast::Mesh::SetTextures(const std::vector<Ref<API::Texture::Texture>>& textures)
{
	Textures = textures;
}

void Cast::Mesh::SetupVertexData()
{
	if (Indices.empty())
		BatchId = Memory::BatchMemoryHandler.CreateBatchObject(Vertices.data(), sizeof(Memory::BatchVertexShaderObject) * Vertices.size());
	else
		BatchId = Memory::BatchMemoryHandler.CreateBatchObject(Vertices.data(), sizeof(Memory::BatchVertexShaderObject) * Vertices.size(), Indices.data(), (int)Indices.size());

	if (BatchId != UID::None())
	{
		Shared.VertexEntities[BatchId] = this;
	}
}

void Cast::Mesh::SetMaterial(Component::MaterialComponent* material)
{
	Material = material;

	if (Material != nullptr) {
		for (const Ref<API::Texture::Texture> tex : Textures) {
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
	if (BatchId != UID::None()) {
		Memory::BatchMemoryHandler.AddToBulk(BatchId);
		BatchId = UID::None();
	}
}

void Cast::Mesh::RetransferToBatchMemory()
{
	if (Indices.empty())
		Cast::Memory::BatchMemoryHandler.OnBatchEmptyRetransfer(BatchId, Vertices.data(), sizeof(Memory::BatchVertexShaderObject) * Vertices.size());
	else
		Cast::Memory::BatchMemoryHandler.OnBatchEmptyRetransfer(BatchId, Vertices.data(), sizeof(Memory::BatchVertexShaderObject) * Vertices.size(), Indices.data(), (int)Indices.size());
}