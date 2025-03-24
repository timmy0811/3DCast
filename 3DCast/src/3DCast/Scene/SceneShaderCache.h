#pragma once

#include <API/core/Buffer.h>
#include <API/texture/Texture.h>
#include <API/core/Shader.h>

#include "3DCast/Core.h"
#include "3DCast/Misc/Helper.h"

namespace Cast {
	class SceneShaderCache {
	public:
		SceneShaderCache();
		~SceneShaderCache() = default;

#pragma region TEXTURE
		enum TextureType {
			Diffuse,
			Specular,
			Ambient,
			Normal,
			Height,
			Emissive,
			Shininess,
			Opacity,
			Reflectance
		};

		Ref<API::Texture::Texture> AddTexture(Ref<API::Texture::Texture> texture, bool useCached = true) {
			if (useCached) {
				auto it = TextureIdentifierMap.find(texture->GetPath());
				if (it != TextureIdentifierMap.end()) {
					auto itTex = Textures.find(it->second);
					if (itTex != Textures.end())
						return itTex->second;
					else {
						LOG_CORE_ERROR("Texture identifier found without matching texture. Consider removing it. Returning NULL handle.");
						return nullptr;
					}
				}
			}

			Textures.insert({ IdCounterTextures, texture });
			TextureIdentifierMap.insert({ texture->GetPath(), IdCounterTextures++ });
			return texture;
		}

		Ref<API::Texture::Texture> AddTexture(const std::string& path, bool flipUV = false) {
			auto itId = TextureIdentifierMap.find(path);
			if (itId != TextureIdentifierMap.end()) {
				auto itTex = Textures.find(itId->second);
				if (itTex != Textures.end()) {
					return itTex->second;
				}
				else {
					LOG_CORE_ERROR("Texture identifier found without matching texture. Consider removing it. Returning NULL handle.");
					return nullptr;
				}
			}

			LOG_CORE_TRACE("Loading uncached texture: {0}", path);
			Ref<API::Texture::Texture> texture = Ref<API::Texture::Texture>(API::Texture::Texture::Create(path, flipUV));
			Textures.insert({ IdCounterTextures, texture });
			TextureIdentifierMap.insert({ path, IdCounterTextures++ });
			return texture;
		}

		Ref<API::Texture::Texture> GetTexture(const std::string& path) {
			auto it = TextureIdentifierMap.find(path);
			if (it != TextureIdentifierMap.end()) {
				auto itTex = Textures.find(it->second);
				if (itTex != Textures.end())
					return itTex->second;
				else {
					LOG_CORE_ERROR("Texture identifier found without matching texture. Consider removing it. Returning NULL handle.");
					return nullptr;
				}
			}
			return nullptr;
		}

		Ref<API::Texture::Texture> GetTexture(unsigned int textureId) {
			auto it = Textures.find(textureId);
			if (it != Textures.end())
				return it->second;
			return nullptr;
		}
#pragma endregion

#pragma region SHADER
		unsigned short AddShader(API::Core::Shader* shader) {
			Shaders[IdCounterShaders].reset(shader);
			return IdCounterShaders++;
		}

		unsigned short AddShader(const std::string identifier, API::Core::Shader* shader) {
			Shaders[IdCounterShaders].reset(shader);
			ShaderIdentifierMap[identifier] = IdCounterShaders;
			return IdCounterShaders++;
		}

		Ref<API::Core::Shader> GetShaderHandle(unsigned short id);
		Ref<API::Core::Shader> GetShaderHandle(const std::string& identifier);

		unsigned int GetShaderId(const std::string& identifier);
#pragma endregion

	private:
#pragma region TEXTURE
		std::unordered_map<unsigned short, Ref<API::Texture::Texture>> Textures;
		std::unordered_map<std::string, unsigned short> TextureIdentifierMap;

		unsigned short IdCounterTextures = 0;
#pragma endregion

#pragma region SHADER
		unsigned short IdCounterShaders = 0;

		std::unordered_map<unsigned short, Ref<API::Core::Shader>> Shaders;
		std::unordered_map<std::string, unsigned short> ShaderIdentifierMap;
#pragma endregion

		Ref<API::Core::Buffer> PointLightBuffer;
		Ref<API::Core::Buffer> DirectionalLightBuffer;
		Ref<API::Core::Buffer> SpotLightBuffer;
	};

	extern SceneShaderCache AssetCache;
}