#pragma once

#include "Mesh.h"

#include "../dependencies.hpp"

namespace GL::model {
	class Model3D
	{
	private:
		std::vector<Mesh> m_Meshes;
		std::string m_Directory;

		std::vector<texture::Texture*> m_LoadedTextures;

		void Load(const std::string& path);
		void ProcessNode(aiNode* node, const aiScene* scene);
		Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
		std::vector<texture::Texture*> LoadMaterialTextures(aiMaterial* mat, aiTextureType type);

	public:
		Model3D(const char* path);
		~Model3D();

		void Draw(core::Shader& shader);
	};
}
