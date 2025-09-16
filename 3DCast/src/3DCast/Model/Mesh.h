#pragma once

#include "3DCast/Data/ShaderDataObjects/Vertex.h"
#include "3DCast/Scene/Component/MaterialComponent.h"
#include "3DCast/Memory/Batching/BatchManager.h"
#include "3DCast/Model/IVertexEntity.h"

#include <API/texture/Texture.h>
#include <vector>

namespace Cast
{
	enum class MeshNodeType
	{
		Root,
		Intermediate,
		Leaf
	};

	class Mesh final : public IVertexEntity
	{
	public:
		Mesh();
		~Mesh() override = default;

		void SetTextures(const std::vector<Ref<API::Texture::Texture>>& textures);
		void SetupVertexData();
		void SetMaterial(Component::MaterialComponent* material);

		[[nodiscard]] int GetVertexCount() const { return (int)Vertices.size(); }
		[[nodiscard]] bool HasIndices() const { return !Indices.empty(); }
		[[nodiscard]] bool MaterialAssigned() const { return Material != nullptr; }
		[[nodiscard]] bool LoadedSuccessfully() const { return BatchId != UID::None(); }

		void RemoveFromBatchStorage();
		void RetransferToBatchMemory() override;

		inline std::vector<Memory::BatchVertexShaderObject>& GetVertices() { return Vertices; }
		inline std::vector<unsigned int>& GetIndices() { return Indices; }

	private:
		uid BatchId;

		std::vector<Memory::BatchVertexShaderObject> Vertices;
		std::vector<unsigned int> Indices;
		std::vector<Ref<API::Texture::Texture>> Textures;

		Component::MaterialComponent* Material = nullptr;
	};
}
