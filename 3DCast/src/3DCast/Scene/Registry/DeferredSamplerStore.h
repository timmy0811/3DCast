#pragma once

#include "3DCast/Core.h"
#include <API/texture/Texture.h>
#include <API/core/Buffer.h>
#include <unordered_map>
#include <vector>

#include <vendor/glm/glm.hpp>

#include "MaterialCacheRegistry.h"
#include "3DCast/Data/ShaderDataObjects/Material.h"

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
		unsigned short customMatIndex = 0;
	};

	struct DeferredTextureInformation
	{
		unsigned short samplerArrayIndex = 0;
		int textureId = 0;
		glm::vec2 size = {0.0f, 0.0f};
	};

	class DeferredSamplerStore
	{
	public:
		DeferredSamplerStore();
		~DeferredSamplerStore();

		void InitAfterDriverSetup();

		unsigned short AddCustomMaterial(const Material& customMaterial);
		unsigned short GetCustomMaterialStoreId(UID id) const;

		void EditCustomMaterial(unsigned short position, const Material& customMaterial);

		void RemoveCustomMaterial(unsigned short position, bool force = false);

		DeferredTextureInformation AddDiffuseTexture(Ref<API::Texture::Texture> texture);
		DeferredTextureInformation AddSpecularTexture(Ref<API::Texture::Texture> texture);
		DeferredTextureInformation AddParallaxTexture(Ref<API::Texture::Texture> texture);
		DeferredTextureInformation AddNormalTexture(Ref<API::Texture::Texture> texture);

		void RemoveDiffuseTexture(unsigned short position, bool force = false);
		void RemoveSpecularTexture(unsigned short position, bool force = false);
		void RemoveParallaxTexture(unsigned short position, bool force = false);
		void RemoveNormalTexture(unsigned short position, bool force = false);

		void UpdateSamplerMapping(
			unsigned short index,
			unsigned short diffuseId = 0,
			unsigned short specularId = 0,
			unsigned short parallaxId = 0,
			unsigned short normalId = 0,
			unsigned short customMatId = 0) const;

		unsigned short CreateSamplerMapping(
			unsigned short diffuseId = 0,
			unsigned short specularId = 0,
			unsigned short parallaxId = 0,
			unsigned short normalId = 0,
			unsigned short customMatId = 0);

		inline void RemoveSamplerMapping(const int index) { SetMappingUnused(index); }
		inline bool IsSamplerMappingUsed(const int index) const { return IsMappingUsed(index); }

		Ref<API::Texture::Texture> GetDiffuseTexture(unsigned short position);
		Ref<API::Texture::Texture> GetSpecularTexture(unsigned short position);
		Ref<API::Texture::Texture> GetParallaxTexture(unsigned short position);
		Ref<API::Texture::Texture> GetNormalTexture(unsigned short position);

		void MakeTexturesResidentIdempotent() const;
		void UpdateBufferData() const;
		void BindSamplerBuffersToShaderPoints() const;

#pragma region ANALYTICS
		inline unsigned int GetDiffuseTextureCount() const { return DiffuseTextures.size(); }
		inline unsigned int GetSpecularTextureCount() const { return SpecularTextures.size(); }
		inline unsigned int GetParallaxTextureCount() const { return ParallaxTextures.size(); }
		inline unsigned int GetNormalTextureCount() const { return NormalTextures.size(); }
		inline unsigned int GetMaterialCount() const { return Materials.size(); }

		inline size_t GetDiffuseTextureSSAOSize() const { return DiffuseSamplersBuffer->GetSize(); }
		inline size_t GetSpecularTextureSSAOSize() const { return SpecularSamplersBuffer->GetSize(); }
		inline size_t GetParallaxTextureSSAOSize() const { return ParallaxSamplersBuffer->GetSize(); }
		inline size_t GetNormalTextureSSAOSize() const { return NormalSamplersBuffer->GetSize(); }
		inline size_t GetCustomMaterialSSAOSize() const { return CustomMatBuffer->GetSize(); }
		inline size_t GetSamplerMappingsSSAOSize() const { return SamplerMappingsBuffer->GetSize(); }

		inline size_t GetDiffuseTextureSSAOCapacity() const { return DiffuseSamplersBuffer->GetCapacity(); }
		inline size_t GetSpecularTextureSSAOCapacity() const { return SpecularSamplersBuffer->GetCapacity(); }
		inline size_t GetParallaxTextureSSAOCapacity() const { return ParallaxSamplersBuffer->GetCapacity(); }
		inline size_t GetNormalTextureSSAOCapacity() const { return NormalSamplersBuffer->GetCapacity(); }
		inline size_t GetCustomMaterialSSAOCapacity() const { return CustomMatBuffer->GetCapacity(); }
		inline size_t GetSamplerMappingsSSAOCapacity() const { return SamplerMappingsBuffer->GetCapacity(); }
#pragma endregion

	private:
		void UpdateDiffuseTextureBuffer() const;
		void UpdateSpecularTextureBuffer() const;
		void UpdateParallaxTextureBuffer() const;
		void UpdateNormalTextureBuffer() const;
		void UpdateMaterialBuffer() const;

		int GetUnusedMapping() const;

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
		std::unordered_map<unsigned short, std::pair<Ref<API::Texture::Texture>, unsigned int>> DiffuseTextures;
		std::unordered_map<unsigned short, std::pair<Ref<API::Texture::Texture>, unsigned int>> SpecularTextures;
		std::unordered_map<unsigned short, std::pair<Ref<API::Texture::Texture>, unsigned int>> ParallaxTextures;
		std::unordered_map<unsigned short, std::pair<Ref<API::Texture::Texture>, unsigned int>> NormalTextures;

		// Todo: Add map<UID, Material> to improve performance of material lookups
		std::unordered_map<unsigned short, std::pair<Material, unsigned int>> Materials;

		unsigned short DiffuseCounter = 0;
		unsigned short SpecularCounter = 0;
		unsigned short ParallaxCounter = 0;
		unsigned short NormalCounter = 0;
		unsigned short MatCounter = 0;

		std::vector<int> DiffuseTexIdCache;
		std::vector<int> SpecularTexIdCache;
		std::vector<int> ParallaxTexIdCache;
		std::vector<int> NormalTexIdCache;
		std::vector<UID> MaterialCache;

		Ref<API::Core::Buffer> DiffuseSamplersBuffer;
		Ref<API::Core::Buffer> SpecularSamplersBuffer;
		Ref<API::Core::Buffer> ParallaxSamplersBuffer;
		Ref<API::Core::Buffer> NormalSamplersBuffer;
		Ref<API::Core::Buffer> CustomMatBuffer;

		Ref<API::Core::Buffer> SamplerMappingsBuffer;

		uint64_t isMappingUsedMap[MAX_SAMPLER_MAPPINGS / 64] = {};
	};

	extern DeferredSamplerStore DeferredSamplerStoreInstance;
}
