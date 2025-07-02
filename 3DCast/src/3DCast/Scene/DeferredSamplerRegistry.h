#pragma once

#include "3DCast/Core.h"
#include <API/texture/Texture.h>
#include <API/core/Buffer.h>
#include <unordered_map>
#include <vector>

#include <vendor/glm/glm.hpp>

#define MAX_SAMPLER_MAPPINGS 1024

namespace Cast
{
	// SamplerMapping corresponds to the struct declared in the shader
	struct SamplerMapping
	{
		unsigned short diffuseIndex = 0;
		unsigned short specularIndex = 0;
		unsigned short parallaxIndex = 0;
		unsigned short normalIndex = 0;
	};

	struct TextureInformation
	{
		unsigned short transformRegistryIndex = 0;
		int textureId = 0;
		glm::vec2 size = {0.0f, 0.0f};
	};

	class DeferredSamplerRegistry
	{
	public:
		DeferredSamplerRegistry();
		~DeferredSamplerRegistry();

		void InitAfterDriverSetup();

		TextureInformation AddDiffuseTexture(Ref<API::Texture::Texture> texture);
		TextureInformation AddSpecularTexture(Ref<API::Texture::Texture> texture);
		TextureInformation AddParallaxTexture(Ref<API::Texture::Texture> texture);
		TextureInformation AddNormalTexture(Ref<API::Texture::Texture> texture);

		TextureInformation AddDiffuseTexture(const std::string& path, bool flipUV = false);
		TextureInformation AddSpecularTexture(const std::string& path, bool flipUV = false);
		TextureInformation AddParallaxTexture(const std::string& path, bool flipUV = false);
		TextureInformation AddNormalTexture(const std::string& path, bool flipUV = false);

		void UpdateSamplerMapping(
			unsigned short index,
			unsigned short diffuseId = 0,
			unsigned short specularId = 0,
			unsigned short parallaxId = 0,
			unsigned short normalId = 0) const;

		unsigned short CreateSamplerMapping(
			unsigned short diffuseId = 0,
			unsigned short specularId = 0,
			unsigned short parallaxId = 0,
			unsigned short normalId = 0);

		inline void RemoveSamplerMapping(const int index) { SetMappingUnused(index); }
		inline bool IsSamplerMappingUsed(const int index) const { return IsMappingUsed(index); }

		Ref<API::Texture::Texture> GetDiffuseTexture(unsigned short id);
		Ref<API::Texture::Texture> GetSpecularTexture(unsigned short id);
		Ref<API::Texture::Texture> GetParallaxTexture(unsigned short id);
		Ref<API::Texture::Texture> GetNormalTexture(unsigned short id);

		void MakeTexturesResidentIdempotent() const;
		void UpdateBufferData() const;
		void BindSamplerBuffersToShaderPoints() const;

	private:
		int GetUnusedMapping() const
		{
			for (int i = 0; i < MAX_SAMPLER_MAPPINGS / 64; i++)
			{
				if (isMappingUsedMap[i] != 0xFFFFFFFFFFFFFFFF)
				{
					for (int j = 0; j < 64; j++)
					{
						if ((isMappingUsedMap[i] & (1ULL << j)) == 0)
						{
							return i * 64 + j;
						}
					}
				}
			}

			LOG_CORE_WARN("Mapping buffer is full, cannot register more mappings.");
			return -1;
		}

		inline void SetMappingUsed(const int index)
		{
			isMappingUsedMap[index / sizeof(uint64_t)] |= (1ULL << index % 64);
		}

		inline void SetMappingUnused(const int index)
		{
			isMappingUsedMap[index / sizeof(uint64_t)] &= ~(1ULL << index % 64);
		}

		inline bool IsMappingUsed(const int index) const
		{
			return (isMappingUsedMap[index / sizeof(uint64_t)] & (1ULL << index % 64)) != 0;
		}

	private:
		std::unordered_map<unsigned short, Ref<API::Texture::Texture>> DiffuseTextures;
		std::unordered_map<unsigned short, Ref<API::Texture::Texture>> SpecularTextures;
		std::unordered_map<unsigned short, Ref<API::Texture::Texture>> ParallaxTextures;
		std::unordered_map<unsigned short, Ref<API::Texture::Texture>> NormalTextures;

		unsigned short DiffuseCounter = 0;
		unsigned short SpecularCounter = 0;
		unsigned short ParallaxCounter = 0;
		unsigned short NormalCounter = 0;

		std::vector<std::string> PathCache;
		std::vector<int> DiffuseTexIdCache;
		std::vector<int> SpecularTexIdCache;
		std::vector<int> ParallaxTexIdCache;
		std::vector<int> NormalTexIdCache;

		Ref<API::Core::Buffer> DiffuseSamplersBuffer;
		Ref<API::Core::Buffer> SpecularSamplersBuffer;
		Ref<API::Core::Buffer> ParallaxSamplersBuffer;
		Ref<API::Core::Buffer> NormalSamplersBuffer;

		Ref<API::Core::Buffer> SamplerMappingsBuffer;

		uint64_t isMappingUsedMap[MAX_SAMPLER_MAPPINGS / 64] = {};
	};

	extern DeferredSamplerRegistry SamplerRegistry;
}
