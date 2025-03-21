#include "castpch.h"
#include "Model.h"

#include "3DCast/Scene/DataObjects/GlobalShared.h"
#include "3DCast/Scene/Entity.h"
#include "3DCast/Scene/Component/Component.h"
#include <filesystem>

Cast::Model::Model()
{
	Meshes.reserve(8);
}

bool Cast::Model::Load(const std::string& path, Ref<Cast::Entity> entity)
{
	this->Entity = entity;

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path,
		aiProcess_Triangulate |
		aiProcess_FlipUVs |
		aiProcess_CalcTangentSpace |
		aiProcess_GenNormals |
		/*aiProcess_FixInfacingNormals |
		aiProcess_JoinIdenticalVertices |
		aiProcess_OptimizeMeshes |
		aiProcess_OptimizeGraph |*/
		aiProcess_GenBoundingBoxes
	);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		LOG_CORE_INFO("Error while loading model using assimp: {0}", importer.GetErrorString());
		return false;
	}

	CalcModelBounds(scene->mRootNode, scene);

	DirPath = path.substr(0, path.find_last_of("/\\"));
	ProcessNode(scene->mRootNode, scene, this->Entity);
	IsLoaded = true;
	return true;
}

void Cast::Model::CalcModelBounds(const aiNode* node, const aiScene* scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		auto bBox = mesh->mAABB;

		if (IsFirstBoundCheck) {
			BoundsMin = glm::vec3(bBox.mMin.x, bBox.mMin.y, bBox.mMin.z);
			BoundsMax = glm::vec3(bBox.mMax.x, bBox.mMax.y, bBox.mMax.z);
			IsFirstBoundCheck = false;
		}
		else {
			BoundsMin.x = std::min(BoundsMin.x, bBox.mMin.x);
			BoundsMin.y = std::min(BoundsMin.y, bBox.mMin.y);
			BoundsMin.z = std::min(BoundsMin.z, bBox.mMin.z);

			BoundsMax.x = std::max(BoundsMax.x, bBox.mMax.x);
			BoundsMax.y = std::max(BoundsMax.y, bBox.mMax.y);
			BoundsMax.z = std::max(BoundsMax.z, bBox.mMax.z);
		}
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		CalcModelBounds(node->mChildren[i], scene);
	}
}

Cast::Ref<Cast::Entity> Cast::Model::ProcessNode(aiNode* node, const aiScene* scene, Ref<Cast::Entity> parent)
{
	std::string nodeName = (node->mName.length > 0) ? node->mName.C_Str() : "Unnamed Node";
	Ref<Cast::Entity> currentEntity = Cast::Shared.ActiveScene->CreateEntity(nodeName);

	if (parent)
	{
		currentEntity->SetParent(parent);
		parent->AddChild(currentEntity);
	}

	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		Ref<Cast::Entity> meshEntity = Cast::Shared.ActiveScene->CreateEntity(nodeName + "_" + std::to_string(i));
		meshEntity->SetParent(currentEntity);
		currentEntity->AddChild(meshEntity);
		meshEntity->AddComponents<Cast::Component::MaterialComponent>();

		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		auto sceneMesh = ProcessMesh(mesh, scene, meshEntity);
		if (!sceneMesh->LoadedSuccessfully())
		{
			LOG_CORE_ERROR("Submesh could not be loaded.");
			continue;
		}
		Meshes.push_back(sceneMesh);

		meshEntity->AddComponents<Cast::Component::MeshComponent>(sceneMesh);
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		Ref<Cast::Entity> childEntity = ProcessNode(node->mChildren[i], scene, currentEntity);
		if (!childEntity)
		{
			LOG_CORE_WARN("Detected empty mesh node. Ignoring.");
		}
	}

	return currentEntity;
}

Cast::Ref<Cast::Mesh> Cast::Model::ProcessMesh(aiMesh* mesh, const aiScene* scene, Ref<Cast::Entity> context)
{
	Ref<Cast::Mesh> castMesh = CreateRef<Cast::Mesh>();
	std::vector<Cast::Ref<API::Texture::Texture>> textures;
	textures.reserve(8);

	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		std::vector<Cast::Ref<API::Texture::Texture>> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		std::vector<Cast::Ref<API::Texture::Texture>> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		std::vector<Cast::Ref<API::Texture::Texture>> shineMaps = LoadMaterialTextures(material, aiTextureType_SHININESS);
		textures.insert(textures.end(), shineMaps.begin(), shineMaps.end());

		std::vector<Cast::Ref<API::Texture::Texture>> normalMaps = LoadMaterialTextures(material, aiTextureType_NORMALS);
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

		std::vector<Cast::Ref<API::Texture::Texture>> heightMaps = LoadMaterialTextures(material, aiTextureType_HEIGHT);
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
	}

	unsigned short samplerSlot = 0;
	if (!textures.empty()) {
		castMesh->SetTextures(textures);
		auto& material = context->GetComponent<Cast::Component::MaterialComponent>();
		castMesh->SetMaterial(&material);
		material.UpdateSamplerMapping();

		samplerSlot = material.samplerIndex;
	}

	std::vector<Memory::BatchVertex> vertices;
	vertices.reserve(mesh->mNumVertices);

	std::vector<unsigned int> indices;
	indices.reserve(mesh->mNumFaces * 3);

	glm::vec3 center = BoundsMin + (BoundsMax - BoundsMin) * 0.5f;
	glm::vec3 size = BoundsMax - BoundsMin;

	float maxDim = std::max({ size.x, size.y, size.z });
	float invDim = (1.0f / maxDim) * 5.f;

	if (!mesh->HasNormals()) {
		LOG_CORE_WARN("Mesh does not have normals. Returning without vertex data.");
		return castMesh;
	}

	if (!mesh->HasTangentsAndBitangents()) {
		LOG_CORE_WARN("Mesh does not have tangents and bitangents. Returning without vertex data.");
		return castMesh;
	}

	// Process vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Memory::BatchVertex vertex;

		vertex.Position = {
			mesh->mVertices[i].x,
			mesh->mVertices[i].y,
			mesh->mVertices[i].z
		};

		vertex.Position -= center;
		vertex.Position *= invDim;

		vertex.Normal = {
			mesh->mNormals[i].x,
			mesh->mNormals[i].y,
			mesh->mNormals[i].z
		};

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

		vertex.SamplerIndex = 1.0;
		vertex.TransformIndex = 0.0f;

		vertices.push_back(vertex);
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}

	castMesh->SetupVertexData(vertices, indices);

	return castMesh;
}

std::vector<Cast::Ref<API::Texture::Texture>> Cast::Model::LoadMaterialTextures(aiMaterial* mat, aiTextureType type)
{
	std::vector<Cast::Ref<API::Texture::Texture>> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);

		std::filesystem::path texturePath(str.C_Str());
		texturePath = texturePath.filename();

		std::filesystem::path fullPath = std::filesystem::path(DirPath) / texturePath;
		std::string path = fullPath.string();

		bool chached = false;
		for (unsigned int j = 0; j < LoadedTextures.size(); j++)
		{
			if (std::strcmp(LoadedTextures[j]->GetPath().data(), path.c_str()) == 0) {
				textures.push_back(LoadedTextures[j]);
				chached = true;
				break;
			}
		}

		if (!chached) {
			LOG_CORE_TRACE("Loading model texture: {0}", path);
			auto texture = Cast::Ref<API::Texture::Texture>(API::Texture::Texture::Create(path, false));

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