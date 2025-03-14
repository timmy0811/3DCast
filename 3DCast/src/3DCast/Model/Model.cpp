#include "castpch.h"
#include "Model.h"
#include "3DCast/Scene/Entity.h"

Cast::Model::Model()
{
	Meshes.reserve(8);
}

bool Cast::Model::Load(const std::string& path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path,
		aiProcess_Triangulate |
		aiProcess_FlipUVs |
		aiProcess_CalcTangentSpace |
		aiProcess_GenNormals |
		aiProcess_JoinIdenticalVertices |
		aiProcess_OptimizeMeshes |
		aiProcess_OptimizeGraph
	);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		LOG_CORE_INFO("Error while loading model using assimp: {0}", importer.GetErrorString());
		return false;
	}

	DirPath = path.substr(0, path.find_last_of("/\\"));
	ProcessNode(scene->mRootNode, scene);
	return true;
}

void Cast::Model::ProcessNode(aiNode* node, const aiScene* scene)
{
	static bool error = false;
	if (error) return;
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		auto sceneMesh = ProcessMesh(mesh, scene);
		if (!sceneMesh.LoadedSuccessfully()) {
			LOG_CORE_ERROR("At least one submesh could not be loaded. Aborting.");
			error = true;
			return;
		}
		Meshes.push_back(sceneMesh);
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		ProcessNode(node->mChildren[i], scene);
	}
}

Cast::Mesh Cast::Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Memory::BatchVertex> vertices;
	vertices.reserve(mesh->mNumVertices);

	std::vector<unsigned int> indices;
	indices.reserve(mesh->mNumFaces * 3);

	std::vector<API::Texture::Texture*> textures;
	textures.reserve(8);

	// Process vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Memory::BatchVertex vertex;

		vertex.Position = {
			mesh->mVertices[i].x,
			mesh->mVertices[i].y,
			mesh->mVertices[i].z
		};

		if (mesh->HasNormals())
		{
			vertex.Normal = {
				mesh->mNormals[i].x,
				mesh->mNormals[i].y,
				mesh->mNormals[i].z
			};
		}

		if (mesh->HasTangentsAndBitangents())
		{
			vertex.Tangent = {
				mesh->mTangents[i].x,
				mesh->mTangents[i].y,
				mesh->mTangents[i].z
			};

			vertex.Bitangent = {
				mesh->mBitangents[i].x,
				mesh->mBitangents[i].y,
				mesh->mBitangents[i].z
			};
		}

		if (mesh->mTextureCoords[0])
		{
			vertex.TexCoords = {
				mesh->mTextureCoords[0][i].x,
				mesh->mTextureCoords[0][i].y
			};
		}
		else
		{
			vertex.TexCoords = { 0.0f, 0.0f };
		}

		vertex.SamplerIndex = 0.0f;
		vertex.TransformIndex = 0.0f;

		vertices.push_back(vertex);
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}

	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		std::vector<API::Texture::Texture*> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		std::vector<API::Texture::Texture*> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		std::vector<API::Texture::Texture*> shineMaps = LoadMaterialTextures(material, aiTextureType_SHININESS);
		textures.insert(textures.end(), shineMaps.begin(), shineMaps.end());

		std::vector<API::Texture::Texture*> normalMaps = LoadMaterialTextures(material, aiTextureType_NORMALS);
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

		std::vector<API::Texture::Texture*> heightMaps = LoadMaterialTextures(material, aiTextureType_HEIGHT);
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
	}

	return Mesh(vertices, indices, textures);
}

std::vector<API::Texture::Texture*> Cast::Model::LoadMaterialTextures(aiMaterial* mat, aiTextureType type)
{
	std::vector<API::Texture::Texture*> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		bool skip = false;

		std::string path = (DirPath + '/' + str.C_Str());

		for (unsigned int j = 0; j < LoadedTextures.size(); j++)
		{
			if (std::strcmp(LoadedTextures[j]->GetPath().data(), path.c_str()) == 0) {
				textures.push_back(LoadedTextures[j]);
				skip = true;
				break;
			}
		}

		if (!skip) {
			API::Texture::Texture* texture = API::Texture::Texture::Create(path, false);

			switch (type) {
			case aiTextureType_DIFFUSE: {
				texture->SetType(API::Texture::TextureType::DIFFUSE);
				break;
			}
			case aiTextureType_SPECULAR: {
				texture->SetType(API::Texture::TextureType::SPECULAR);
				break;
			}
			case aiTextureType_SHININESS: {
				texture->SetType(API::Texture::TextureType::SHINE);
				break;
			}
			case aiTextureType_HEIGHT: {
				texture->SetType(API::Texture::TextureType::HEIGHT);
				break;
			}
			case aiTextureType_NORMALS: {
				texture->SetType(API::Texture::TextureType::NORMAL);
				break;
			}
			default:
				texture->SetType(API::Texture::TextureType::DEFAULT);
			}

			textures.push_back(texture);
			LoadedTextures.push_back(texture);
		}
	}
	return textures;
}