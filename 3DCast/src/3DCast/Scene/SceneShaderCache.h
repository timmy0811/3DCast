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

		template <TextureType T>
		unsigned short AddTexture(API::Texture::Texture* texture) {
			switch (T) {
			case TextureType::Diffuse:
				DiffuseTextures[IdCounterDiffuse] = Ref<API::Texture::Texture>(texture);
				return IdCounterDiffuse++;
			case TextureType::Specular:
				SpecularTextures[IdCounterSpecular] = Ref<API::Texture::Texture>(texture);
				return IdCounterSpecular++;
			case TextureType::Ambient:
				AmbientTextures[IdCounterAmbient] = Ref<API::Texture::Texture>(texture);
				return IdCounterAmbient++;
			case TextureType::Normal:
				NormalTextures[IdCounterNormal] = Ref<API::Texture::Texture>(texture);
				return IdCounterNormal++;
			case TextureType::Height:
				HeightTextures[IdCounterHeight] = Ref<API::Texture::Texture>(texture);
				return IdCounterHeight++;
			case TextureType::Emissive:
				EmissiveTextures[IdCounterEmissive] = Ref<API::Texture::Texture>(texture);
				return IdCounterEmissive++;
			case TextureType::Shininess:
				ShininessTextures[IdCounterShinieness] = Ref<API::Texture::Texture>(texture);
				return IdCounterShinieness++;
			case TextureType::Opacity:
				OpacityTextures[IdCounterOpacity] = Ref<API::Texture::Texture>(texture);
				return IdCounterOpacity++;
			case TextureType::Reflectance:
				ReflectanceTextures[IdCounterReflectance] = Ref<API::Texture::Texture>(texture);
				return IdCounterReflectance++;
			default:
				return NAN_UNSIGNED_SHORT;
			}
		}

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

	private:
		unsigned short IdCounterDiffuse = 0;
		unsigned short IdCounterSpecular = 0;
		unsigned short IdCounterAmbient = 0;
		unsigned short IdCounterNormal = 0;
		unsigned short IdCounterHeight = 0;
		unsigned short IdCounterEmissive = 0;
		unsigned short IdCounterShinieness = 0;
		unsigned short IdCounterOpacity = 0;
		unsigned short IdCounterReflectance = 0;

		std::unordered_map<unsigned short, Ref<API::Texture::Texture>> DiffuseTextures;
		std::unordered_map<unsigned short, Ref<API::Texture::Texture>> SpecularTextures;
		std::unordered_map<unsigned short, Ref<API::Texture::Texture>> AmbientTextures;
		std::unordered_map<unsigned short, Ref<API::Texture::Texture>> NormalTextures;
		std::unordered_map<unsigned short, Ref<API::Texture::Texture>> HeightTextures;
		std::unordered_map<unsigned short, Ref<API::Texture::Texture>> EmissiveTextures;
		std::unordered_map<unsigned short, Ref<API::Texture::Texture>> ShininessTextures;
		std::unordered_map<unsigned short, Ref<API::Texture::Texture>> OpacityTextures;
		std::unordered_map<unsigned short, Ref<API::Texture::Texture>> ReflectanceTextures;

		unsigned short IdCounterShaders = 0;

		std::unordered_map<unsigned short, Ref<API::Core::Shader>> Shaders;
		std::unordered_map<std::string, unsigned short> ShaderIdentifierMap;

		Ref<API::Core::Buffer> PointLightBuffer;
		Ref<API::Core::Buffer> DirectionalLightBuffer;
		Ref<API::Core::Buffer> SpotLightBuffer;
	};

	extern SceneShaderCache AssetCache;
}