#include "castpch.h"
#include "Model.h"

#include "3DCast/Data/GlobalShared.h"
#include "3DCast/Scene/Entity.h"
#include "3DCast/Scene/Component/Component.h"
#include "../Scene/Registry/ShaderCacheRegistry.h"
#include "3DCast/Math/Vertex.h"

#include <filesystem>
#include <assimp/postprocess.h>

#include "3DCast/Scene/Registry/TextureCacheRegistry.h"
#include "3DCast/Util/Filesystem.h"

Cast::Model::Model(): ModelSize(), ModelOffset(), BoundsMin(), BoundsMax()
{
	Meshes.reserve(8);
}

bool Cast::Model::Load(const std::string& path, Ref<Entity> entity)
{
	this->EntityContainer = entity;

	Assimp::Importer importer;

	std::string extension = path.substr(path.find_last_of('.') + 1);
	std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

	unsigned int importFlags = aiProcess_Triangulate |
                              aiProcess_FlipUVs |
                              aiProcess_JoinIdenticalVertices |
                              aiProcess_OptimizeMeshes |
                              aiProcess_OptimizeGraph |
                              aiProcess_GenBoundingBoxes;

	if (extension == "fbx") {
		importFlags |= aiProcess_CalcTangentSpace |
		               aiProcess_GenNormals |
		               aiProcess_FixInfacingNormals;
	}
	else {
		importFlags |= aiProcess_GenSmoothNormals |
		               aiProcess_CalcTangentSpace |
		               aiProcess_FixInfacingNormals;

		if (extension == "obj" || extension == "gltf" || extension == "glb") {
			importFlags |= aiProcess_RemoveRedundantMaterials |
			               aiProcess_FindDegenerates |
			               aiProcess_FindInvalidData;
		}
	}

	const aiScene* scene = importer.ReadFile(path, importFlags);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		LOG_CORE_INFO("Error while loading model using assimp: {0}", importer.GetErrorString());
		return false;
	}

	CalcModelBounds(scene->mRootNode, scene);

	DirPath = path.substr(0, path.find_last_of("/\\"));

	// Register the root entity's transform so all submeshes share the same transform
	Shared.ActiveScene->RegisterTransformComponent(entity);
	const auto& rootTransform = entity->GetComponent<Component::TransformComponent>();
	unsigned short rootTransformIndex = static_cast<unsigned short>(rootTransform.transformRegistryIndex);

	ProcessNode(scene->mRootNode, scene, this->EntityContainer, rootTransformIndex);
	IsLoaded = true;

	return true;
}

void Cast::Model::CalcModelBounds(const aiNode* node, const aiScene* scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		auto bBox = mesh->mAABB;

		if (IsFirstBoundCheck)
		{
			BoundsMin = glm::vec3(bBox.mMin.x, bBox.mMin.y, bBox.mMin.z);
			BoundsMax = glm::vec3(bBox.mMax.x, bBox.mMax.y, bBox.mMax.z);
			IsFirstBoundCheck = false;
		}
		else
		{
			BoundsMin.x = std::min(BoundsMin.x, bBox.mMin.x);
			BoundsMin.y = std::min(BoundsMin.y, bBox.mMin.y);
			BoundsMin.z = std::min(BoundsMin.z, bBox.mMin.z);

			BoundsMax.x = std::max(BoundsMax.x, bBox.mMax.x);
			BoundsMax.y = std::max(BoundsMax.y, bBox.mMax.y);
			BoundsMax.z = std::max(BoundsMax.z, bBox.mMax.z);
		}
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		CalcModelBounds(node->mChildren[i], scene);
	}
}

Cast::Ref<Cast::Entity> Cast::Model::ProcessNode(const aiNode* node, const aiScene* scene, const Ref<Entity>& parent, unsigned short rootTransformIndex, const bool isRoot)
{
	const std::string nodeName = (node->mName.length > 0) ? node->mName.C_Str() : "Unnamed Node";
	Ref<Entity> currentEntity = Shared.ActiveScene->CreateEntity(nodeName);

	if (parent)
	{
		currentEntity->SetParent(parent);
		parent->AddChild(currentEntity);
	}

	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		const Ref<Entity> meshEntity = Shared.ActiveScene->CreateEntity(nodeName + "_" + std::to_string(i));
		meshEntity->SetParent(currentEntity);
		currentEntity->AddChild(meshEntity);

		meshEntity->AddComponents<Component::MaterialComponent>();
		auto& meshComp = meshEntity->AddComponents<Component::MeshComponent>(MeshNodeType::Leaf);

		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		// Use the root entity's transform index
		auto sceneMesh = ProcessMesh(mesh, scene, meshEntity, rootTransformIndex);
		if (!sceneMesh->LoadedSuccessfully())
		{
			LOG_CORE_ERROR("Submesh could not be loaded.");
			continue;
		}
		Meshes.push_back(sceneMesh);

		meshComp.SetMeshAsChildNode(sceneMesh);
	}

	if (!isRoot && (node->mNumChildren > 0 || node->mNumMeshes > 0))
		currentEntity->AddComponents<Component::MeshComponent>(MeshNodeType::Intermediate);

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		const Ref<Entity> childEntity = ProcessNode(node->mChildren[i], scene, currentEntity, rootTransformIndex);
		if (!childEntity)
		{
			LOG_CORE_WARN("Detected empty mesh node. Ignoring.");
		}
	}

	return currentEntity;
}

Cast::Mesh* Cast::Model::ProcessMesh(aiMesh* mesh, const aiScene* scene, Ref<Entity> context,
                                     unsigned short transformIndex) const
{
	auto* castMesh = new Mesh();
	std::vector<Ref<API::Texture::Texture>> textures;
	textures.reserve(8);

	if (mesh->mMaterialIndex < scene->mNumMaterials)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		std::vector<Ref<API::Texture::Texture>> diffuseMaps = LoadMaterialTextures(
			material, aiTextureType_DIFFUSE, API::Texture::TextureType::DIFFUSE);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		std::vector<Ref<API::Texture::Texture>> specularMaps = LoadMaterialTextures(
			material, aiTextureType_SPECULAR, API::Texture::TextureType::SPECULAR);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		std::vector<Ref<API::Texture::Texture>> shineMaps = LoadMaterialTextures(
			material, aiTextureType_SHININESS, API::Texture::TextureType::SHINE);
		textures.insert(textures.end(), shineMaps.begin(), shineMaps.end());

		std::vector<Ref<API::Texture::Texture>> normalMaps = LoadMaterialTextures(
			material, aiTextureType_NORMALS, API::Texture::TextureType::NORMAL);
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

		std::vector<Ref<API::Texture::Texture>> heightMaps = LoadMaterialTextures(
			material, aiTextureType_HEIGHT, API::Texture::TextureType::HEIGHT);
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
	}

	castMesh->SetTextures(textures);
	auto& material = context->GetComponent<Component::MaterialComponent>();
	castMesh->SetMaterial(&material);
	material.UpdateSamplerMapping();
	unsigned short samplerSlot = material.samplerIndex;

	auto& vertices = castMesh->GetVertices();
	vertices.reserve(mesh->mNumVertices);

	auto& indices = castMesh->GetIndices();
	indices.reserve(mesh->mNumFaces * 3);

	glm::vec3 center = BoundsMin + (BoundsMax - BoundsMin) * 0.5f;
	glm::vec3 size = BoundsMax - BoundsMin;

	float maxDim = std::max({size.x, size.y, size.z});
	float invDim = (1.0f / maxDim) * 5.f;

	bool hasNormals = mesh->HasNormals();
	bool hasTangents = mesh->HasTangentsAndBitangents();

	if (!hasNormals) {
		LOG_CORE_WARN("Mesh does not have normals. Will use fallback normals.");
	}

	if (!hasTangents) {
		LOG_CORE_WARN("Mesh does not have tangents/bitangents. Will use fallback vectors.");
	}

	// Prepare data for calculations if needed
	std::vector<glm::vec3> positions;
	std::vector<glm::vec2> texcoords;
	std::vector<unsigned int> faceIndices;
	std::vector<glm::vec3> generatedNormals;
	std::vector<Math::TangentSpace> tangentSpaces;

	if (!hasNormals || !hasTangents) {
		positions.reserve(mesh->mNumVertices);

		for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
			positions.emplace_back(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		}

		if (mesh->mTextureCoords[0] && !hasTangents) {
			texcoords.reserve(mesh->mNumVertices);
			for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
				texcoords.emplace_back(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
			}
		}

		// Face indices
		faceIndices.reserve(mesh->mNumFaces * 3);
		for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
			const aiFace& face = mesh->mFaces[i];
			if (face.mNumIndices == 3) {
				faceIndices.push_back(face.mIndices[0]);
				faceIndices.push_back(face.mIndices[1]);
				faceIndices.push_back(face.mIndices[2]);
			}
		}
	}

	if (!hasNormals) {
		generatedNormals = Math::CalculateNormals(positions, faceIndices);
	}

	if (!hasTangents && mesh->mTextureCoords[0]) {
		std::vector<glm::vec3> normals;
		normals.reserve(mesh->mNumVertices);

		if (hasNormals) {
			for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
				normals.emplace_back(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
			}
		} else {
			normals = generatedNormals;
		}

		tangentSpaces = Math::CalculateTangentSpaces(positions, texcoords, normals, faceIndices);
	}

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Memory::BatchVertexShaderObject vertex{};

		vertex.Position = {
			mesh->mVertices[i].x,
			mesh->mVertices[i].y,
			mesh->mVertices[i].z
		};

		vertex.Position -= center;
		vertex.Position *= invDim;

		if (hasNormals) {
			vertex.Normal = {
				mesh->mNormals[i].x,
				mesh->mNormals[i].y,
				mesh->mNormals[i].z
			};
		} else {
			vertex.Normal = generatedNormals[i];
		}

		if (hasTangents) {
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
		} else if (!tangentSpaces.empty()) {
			vertex.Tangent = tangentSpaces[i].tangent;
			vertex.Bitangent = tangentSpaces[i].bitangent;
		} else {
			Math::TangentSpace ts = Math::GenerateDefaultTangentSpace(vertex.Normal);
			vertex.Tangent = ts.tangent;
			vertex.Bitangent = ts.bitangent;
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
			vertex.TexCoords = {0.0f, 0.0f};
		}

		vertex.SamplerIndex = samplerSlot;
		vertex.TransformIndex = transformIndex;

		vertices.push_back(vertex);
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	castMesh->SetupVertexData();

	return castMesh;
}

std::vector<Cast::Ref<API::Texture::Texture>> Cast::Model::LoadMaterialTextures(
	const aiMaterial* mat, const aiTextureType type, const API::Texture::TextureType typeAPI) const
{
	std::vector<Ref<API::Texture::Texture>> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);

		std::string resolvedPath = Util::FindTexturePath(DirPath, str.C_Str());

		const auto textureId = TextureCacheRegistryInstance.AddFromFile(resolvedPath, true); // make adjustable
		Ref<API::Texture::Texture> texture = TextureCacheRegistryInstance.GetHandle(textureId);
		if (!texture) continue;
		texture->SetType(typeAPI);

		textures.push_back(texture);
	}
	return textures;
}



