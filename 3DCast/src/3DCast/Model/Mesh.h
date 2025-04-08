#pragma once

#include "3DCast/Data/ShaderDataObjects/Vertex.h"
#include "3DCast/Scene/Component/MaterialComponent.h"
#include "3DCast/Memory/Batching/BatchManager.h"

#include <API/texture/Texture.h>
#include <vector>

namespace Cast {
	class Mesh {
	public:
		Mesh();
		~Mesh() = default;

		void SetTextures(const std::vector<Cast::Ref<API::Texture::Texture>>& textures);
		void SetupVertexData(const std::vector<Memory::BatchVertex>& vertices, const std::vector<unsigned int>& indices);
		void SetMaterial(Component::MaterialComponent* material);

		int GetVertexCount() const { return (int)Vertices.size(); }
		bool HasIndices() const { return !Indices.empty(); }
		bool MaterialAssigned() const { return Material != nullptr; }
		bool LoadedSuccessfully() const { return MemPos.batchStorageInstancedId >= 0; }

	private:
		uid BatchId;

		std::vector<Memory::BatchVertex> Vertices;
		std::vector<unsigned int> Indices;
		std::vector<Cast::Ref<API::Texture::Texture>> Textures;

		Cast::Memory::MemoryPosition MemPos;
		Component::MaterialComponent* Material = nullptr;
	};
}