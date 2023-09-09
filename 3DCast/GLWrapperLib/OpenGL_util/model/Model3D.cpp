#include "Model3D.h"

void GL::model::Model3D::Load(const std::string& path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs); // Some models work with flipped UVs

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		// import.GetErrorString()
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}

	m_Directory = path.substr(0, path.find_last_of('/'));
	ProcessNode(scene->mRootNode, scene);
}

void GL::model::Model3D::ProcessNode(aiNode* node, const aiScene* scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		m_Meshes.push_back(ProcessMesh(mesh, scene));
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		ProcessNode(node->mChildren[i], scene);
	}
}

GL::model::Mesh GL::model::Model3D::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<primitive::vertex::VertexMesh> vertices;
	std::vector<unsigned int> indices;
	std::vector<texture::Texture*> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		primitive::vertex::VertexMesh v;

		glm::vec3 vector;
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		v.Position = vector;

		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		v.Normal = vector;

		v.TexID = 0.f;

		if (mesh->mTextureCoords[0])
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			v.TexCoords = vec;
		}
		else
			v.TexCoords = glm::vec2(0.0f, 0.0f);

		vertices.push_back(v);
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

		std::vector<texture::Texture*> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		std::vector<texture::Texture*> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		std::vector<texture::Texture*> shineMaps = LoadMaterialTextures(material, aiTextureType_SHININESS);
		textures.insert(textures.end(), shineMaps.begin(), shineMaps.end());

		std::vector<texture::Texture*> normalMaps = LoadMaterialTextures(material, aiTextureType_NORMALS);
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

		std::vector<texture::Texture*> heightMaps = LoadMaterialTextures(material, aiTextureType_HEIGHT);
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
	}

	return Mesh(vertices, indices, textures);
}

std::vector<GL::texture::Texture*> GL::model::Model3D::LoadMaterialTextures(aiMaterial* mat, aiTextureType type)
{
	std::vector<texture::Texture*> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		bool skip = false;

		std::string path = (m_Directory + '/' + str.C_Str());

		for (unsigned int j = 0; j < m_LoadedTextures.size(); j++)
		{
			if (std::strcmp(m_LoadedTextures[j]->GetPath().data(), path.c_str()) == 0) {
				textures.push_back(m_LoadedTextures[j]);
				skip = true;
				break;
			}
		}

		if (!skip) {
			texture::Texture* texture = new texture::Texture(path, false); // m_Directory + str ???

			switch (type) {
			case aiTextureType_DIFFUSE: {
				texture->SetType(texture::TextureType::DIFFUSE);
				break;
			}
			case aiTextureType_SPECULAR: {
				texture->SetType(texture::TextureType::SPECULAR);
				break;
			}
			case aiTextureType_SHININESS: {
				texture->SetType(texture::TextureType::SHINE);
				break;
			}
			case aiTextureType_HEIGHT: {
				texture->SetType(texture::TextureType::HEIGHT);
				break;
			}
			case aiTextureType_NORMALS: {
				texture->SetType(texture::TextureType::NORMAL);
				break;
			}
			default:
				texture->SetType(texture::TextureType::DEFAULT);
			}

			textures.push_back(texture);
			m_LoadedTextures.push_back(texture);			// ????
		}
	}
	return textures;
}

GL::model::Model3D::Model3D(const char* path)
{
	Load(path);
}

GL::model::Model3D::~Model3D()
{
	for (texture::Texture* tex : m_LoadedTextures) {
		delete tex;
	}
}

void GL::model::Model3D::Draw(GL::core::Shader& shader)
{
	for (Mesh& mesh : m_Meshes) {
		mesh.Draw(shader);
	}
}