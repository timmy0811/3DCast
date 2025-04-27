#pragma once

#include "3DCast/Data/ShaderDataObjects/Vertex.h"
#include "3DCast/Scene/Component/MaterialComponent.h"
#include "3DCast/Memory/Batching/BatchManager.h"
#include "3DCast/Model/IVertexEntity.h"

#include <API/texture/Texture.h>
#include <vector>

namespace Cast {
	class Mesh : public IVertexEntity {
	public:
		Mesh();
		~Mesh() = default;

		void SetTextures(const std::vector<Cast::Ref<API::Texture::Texture>>& textures);
		void SetupVertexData();
		void SetMaterial(Component::MaterialComponent* material);

		int GetVertexCount() const { return (int)Vertices.size(); }
		bool HasIndices() const { return !Indices.empty(); }
		bool MaterialAssigned() const { return Material != nullptr; }
		bool LoadedSuccessfully() const { return BatchId != UID::None(); }

		void RemoveFromBatchStorage();
		void RetransferToBatchMemory() override;

		inline std::vector<Memory::BatchVertex>& GetVertices() { return Vertices; }
		inline std::vector<unsigned int>& GetIndices() { return Indices; }

	private:
		uid BatchId;

		std::vector<Memory::BatchVertex> Vertices;
		std::vector<unsigned int> Indices;
		std::vector<Cast::Ref<API::Texture::Texture>> Textures;

		Component::MaterialComponent* Material = nullptr;
	};
}