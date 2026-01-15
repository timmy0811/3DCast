#pragma once

#include "3DCast/Model/Mesh.h"

#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

namespace Cast
{
	class Entity;

	class Model
	{
	public:
		Model();
		~Model() = default;

		bool Load(const std::string& path, Ref<Entity> entity, bool flipTextures = true);

		[[nodiscard]] const std::string& GetDirectory() const { return DirPath; }
		[[nodiscard]] int GetMeshCount() const { return (int)Meshes.size(); }
		[[nodiscard]] int GetTextureCount() const { return 999; }
		[[nodiscard]] bool IsModelLoaded() const { return IsLoaded; }

		[[nodiscard]] int GetTotalVertexCount() const
		{
			int count = 0;
			for (const auto& mesh : Meshes)
				count += mesh->GetVertexCount();
			return count;
		}

		[[nodiscard]] bool IsIndexed() const
		{
			return std::any_of(Meshes.begin(), Meshes.end(), [](const Mesh* mesh) { return mesh->HasIndices(); });
		}

		[[nodiscard]] bool MaterialAssigned() const
		{
			return std::any_of(Meshes.begin(), Meshes.end(), [](const Mesh* mesh) { return mesh->MaterialAssigned(); });
		}

	private:
 	void CalcModelBounds(const aiNode* node, const aiScene* scene);

 	Ref<Entity> ProcessNode(const aiNode* node, const aiScene* scene, const Ref<Entity>& parent, unsigned short rootTransformIndex, bool isRoot = false);
 	Mesh* ProcessMesh(aiMesh* mesh, const aiScene* scene, Ref<Entity> context,
 	                  unsigned short transformIndex) const;
		std::vector<Ref<API::Texture::Texture>> LoadMaterialTextures(
			const aiMaterial* mat, aiTextureType type, API::Texture::TextureType typeAPI) const;

	private:
		Ref<Entity> EntityContainer;

		std::vector<Mesh*> Meshes;

		std::string DirPath;
		Assimp::Importer Importer;

		glm::vec3 ModelSize;
		glm::vec3 ModelOffset;

		bool IsLoaded = false;
		bool FlipTextures = true;
		bool IsFirstBoundCheck = true;
		glm::vec3 BoundsMin;
		glm::vec3 BoundsMax;
	};
}
