#pragma once

#include "3DCast/Model/Mesh.h"
#include "castpch.h"

#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Cast {
	class Entity;

	class Model
	{
	public:
		Model();
		~Model() = default;

		bool Load(const std::string& path);

		const std::string& GetDirectory() const { return DirPath; }
		int GetMeshCount() const { return (int)Meshes.size(); }
		int GetTextureCount() const { return (int)LoadedTextures.size(); }

		int GetTotalVertexCount() const
		{
			int count = 0;
			for (const auto& mesh : Meshes)
				count += mesh.GetVertexCount();
			return count;
		}

		bool IsIndexed() const
		{
			for (const auto& mesh : Meshes)
				if (mesh.HasIndices())
					return true;
			return false;
		}

		bool MaterialAssigned() const
		{
			for (const auto& mesh : Meshes)
				if (mesh.MaterialAssigned())
					return true;
			return false;
		}

	private:
		void ProcessNode(aiNode* node, const aiScene* scene);
		Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
		std::vector<API::Texture::Texture*> LoadMaterialTextures(aiMaterial* mat, aiTextureType type);

	private:
		std::vector<Mesh> Meshes;
		std::vector<API::Texture::Texture*> LoadedTextures;

		std::string DirPath;
		Assimp::Importer m_Importer;
	};
}