#pragma once

#include "dependencies.hpp"
#include "Mesh.h"

namespace GL::Model {
	class Model3D
	{
	public:
		Model3D(const char* path);
		~Model3D();

		void Draw(Core::Shader& shader);

	private:
		std::vector<Mesh> m_Meshes;
		std::string m_Directory;

		std::vector<Texture::Texture*> m_LoadedTextures;

		void Load(const std::string& path);
		void ProcessNode(aiNode* node, const aiScene* scene);
		Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
		std::vector<Texture::Texture*> LoadMaterialTextures(aiMaterial* mat, aiTextureType type);
	};
}
