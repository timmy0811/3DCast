#include "castpch.h"
#include "DeferredSamplerStore.h"

#include "ShaderCacheRegistry.h"
#include "TextureCacheRegistry.h"
#include "vendor/glm/gtc/type_ptr.inl"

namespace Cast
{
	// Global instance
	DeferredSamplerStore DeferredSamplerStoreInstance;

	DeferredSamplerStore::DeferredSamplerStore()
	{
		DiffuseTextures.reserve(32);
		SpecularTextures.reserve(32);
		ParallaxTextures.reserve(32);
		NormalTextures.reserve(32);
		Materials.reserve(32);

		DiffuseTexIdCache.reserve(0x1000);
		SpecularTexIdCache.reserve(0x1000);
		NormalTexIdCache.reserve(0x1000);
		ParallaxTexIdCache.reserve(0x1000);
	}

	DeferredSamplerStore::~DeferredSamplerStore() = default;

	void DeferredSamplerStore::InitAfterDriverSetup()
	{
		constexpr unsigned int MAX_TEXTURES_PER_SLOT = 128;
		constexpr unsigned int MAX_MATERIALS = 512;

		DiffuseSamplersBuffer.reset(API::Core::Buffer::Create(
			API::Core::Buffer::BufferType::SHADER_STORAGE_BUFFER,
			API::Core::Buffer::MemoryLayout::DYNAMIC,
			MAX_TEXTURES_PER_SLOT * sizeof(uint64_t)
		));

		SpecularSamplersBuffer.reset(API::Core::Buffer::Create(
			API::Core::Buffer::BufferType::SHADER_STORAGE_BUFFER,
			API::Core::Buffer::MemoryLayout::DYNAMIC,
			MAX_TEXTURES_PER_SLOT * sizeof(uint64_t)
		));

		ParallaxSamplersBuffer.reset(API::Core::Buffer::Create(
			API::Core::Buffer::BufferType::SHADER_STORAGE_BUFFER,
			API::Core::Buffer::MemoryLayout::DYNAMIC,
			MAX_TEXTURES_PER_SLOT * sizeof(uint64_t)
		));

		NormalSamplersBuffer.reset(API::Core::Buffer::Create(
			API::Core::Buffer::BufferType::SHADER_STORAGE_BUFFER,
			API::Core::Buffer::MemoryLayout::DYNAMIC,
			MAX_TEXTURES_PER_SLOT * sizeof(uint64_t)
		));

		SamplerMappingsBuffer.reset(API::Core::Buffer::Create(
			API::Core::Buffer::BufferType::SHADER_STORAGE_BUFFER,
			API::Core::Buffer::MemoryLayout::DYNAMIC,
			MAX_SAMPLER_MAPPINGS * sizeof(SamplerMapping)
		));

		CustomMatBuffer.reset(API::Core::Buffer::Create(
			API::Core::Buffer::BufferType::SHADER_STORAGE_BUFFER,
			API::Core::Buffer::MemoryLayout::DYNAMIC,
			MAX_MATERIALS * sizeof(CustomMaterialShaderObject)
		));

		// Default samplers
		AddDiffuseTexture(TextureCacheRegistryInstance.GetHandle(TextureCacheRegistryInstance.AddFromFile(std::string(ASSET_DIR) + "img/default_samplers/default_diffuse.png", false, false)), false);
		AddSpecularTexture(TextureCacheRegistryInstance.GetHandle(TextureCacheRegistryInstance.AddFromFile(std::string(ASSET_DIR) + "img/default_samplers/default_specular.png", false, false)), false);
		AddParallaxTexture(TextureCacheRegistryInstance.GetHandle(TextureCacheRegistryInstance.AddFromFile(std::string(ASSET_DIR) + "img/default_samplers/default_parallax.png", false, false)), false);
		AddNormalTexture(TextureCacheRegistryInstance.GetHandle(TextureCacheRegistryInstance.AddFromFile(std::string(ASSET_DIR) + "img/default_samplers/default_normal.png", false, false)), false);

		AddCustomMaterial(MaterialCacheRegistryInstance.Get(MaterialCacheRegistryInstance.GetDefaultID()));

		CreateSamplerMapping(0, 0, 0, 0, 0);
	}

	unsigned short DeferredSamplerStore::AddCustomMaterial(const Material& customMaterial, const bool removable)
	{
		if (std::find(MaterialCache.begin(), MaterialCache.end(), customMaterial.id) != MaterialCache.end())
		{
			for (const auto& [arrayPosition, materialPair] : Materials)
			{
				if (materialPair.first.id == customMaterial.id)
				{
					Materials[arrayPosition].second.useCount++;
					return arrayPosition;
				}
			}

			FLOG_CORE_ERROR("Material found in Id-Cache but not in Material storage. Check the implementation");
			return {};
		}

		Materials[MatCounter] = {customMaterial, {1, removable}}; // Initialize with use count 1
		MaterialCache.push_back(customMaterial.id);
		UpdateMaterialBuffer();

		return MatCounter++;
	}

	unsigned short DeferredSamplerStore::GetCustomMaterialStoreId(const UID id) const
	{
		if (std::find(MaterialCache.begin(), MaterialCache.end(), id) != MaterialCache.end())
		{
			for (const auto& [arrayPosition, materialPair] : Materials)
			{
				if (materialPair.first.id == id)
				{
					return arrayPosition;
				}
			}

			FLOG_CORE_ERROR("Material found in Id-Cache but not in texture storage. Check the implementation");
		}

		return 0;
	}

	void DeferredSamplerStore::EditCustomMaterial(unsigned short position, const Material& customMaterial)
	{
		if (Materials.find(position) != Materials.end())
		{
			Materials[position].first = customMaterial;
			CustomMatBuffer->AddData(&customMaterial.shaderObject, sizeof(CustomMaterialShaderObject), sizeof(CustomMaterialShaderObject) * position);
		}
		else
		{
			LOG_CORE_ERROR("Attempted to edit non-existent material with ID {}", position);
		}
	}

	void DeferredSamplerStore::RemoveCustomMaterial(unsigned short position, const bool force)
	{
		const auto it = Materials.find(position);
		if (it == Materials.end())
			return;

		it->second.second.useCount--;

		if (it->second.second.useCount == 0 || force)
		{
			const auto cacheIt = std::find(MaterialCache.begin(), MaterialCache.end(), it->second.first.id);
			if (cacheIt != MaterialCache.end())
				MaterialCache.erase(cacheIt);

			Materials.erase(it);
			UpdateMaterialBuffer();
			LOG_CORE_INFO("Material with ID {} removed from DeferredSamplerStore", position);
		}
		else
		{
			LOG_CORE_TRACE("Material with ID {} use count decreased to {}", position, it->second.second.useCount);
		}
	}

#pragma region ADDERS_AND_REMOVERS
	DeferredTextureInformation DeferredSamplerStore::AddDiffuseTexture(Ref<API::Texture::Texture> texture, const bool removable)
	{
		if (!texture) return {};

		if (std::find(DiffuseTexIdCache.begin(), DiffuseTexIdCache.end(), texture->GetRendererID()) != DiffuseTexIdCache.end())
		{
			for (const auto& [fst, pair] : DiffuseTextures)
			{
				if (pair.first->GetRendererID() == texture->GetRendererID())
				{
					// Increment usage count
					DiffuseTextures[fst].second.useCount++;
					return {fst, (int)texture->GetRendererID()};
				}
			}

			FLOG_CORE_ERROR("Texture found in Id-Cache but not in texture storage. Check the implementation");
			return {};
		}

		texture->SetType(API::Texture::TextureType::DIFFUSE);
		DiffuseTextures[DiffuseCounter] = {texture, {1, removable}}; // Initialize with use count 1
		texture->MakeResident();
		DiffuseTexIdCache.push_back(texture->GetRendererID());

		UpdateDiffuseTextureBuffer();

		return {DiffuseCounter++, (int)texture->GetRendererID(), {texture->GetWidth(), texture->GetHeight()}};
	}

	void DeferredSamplerStore::RemoveDiffuseTexture(unsigned short position, const bool force)
	{
		const auto it = DiffuseTextures.find(position);
		if (it == DiffuseTextures.end() || !it->second.second.removable)
			return;

		it->second.second.useCount--;

		if (it->second.second.useCount == 0 || force)
		{
			const auto cacheIt = std::find(DiffuseTexIdCache.begin(), DiffuseTexIdCache.end(), it->second.first->GetRendererID());
			if (cacheIt != DiffuseTexIdCache.end())
				DiffuseTexIdCache.erase(cacheIt);

			DiffuseTextures.erase(it);
			UpdateDiffuseTextureBuffer();
			LOG_CORE_INFO("Diffuse texture with ID {} removed from DeferredSamplerStore", position);
		}
		else
		{
			LOG_CORE_TRACE("Diffuse texture with ID {} use count decreased to {}", position, it->second.second.useCount);
		}
	}

	DeferredTextureInformation DeferredSamplerStore::AddSpecularTexture(Ref<API::Texture::Texture> texture, const bool removable)
	{
		if (!texture) return {};

		if (std::find(SpecularTexIdCache.begin(), SpecularTexIdCache.end(), texture->GetRendererID()) != SpecularTexIdCache.end())
		{
			for (const auto& [fst, pair] : SpecularTextures)
			{
				if (pair.first->GetRendererID() == texture->GetRendererID())
				{
					SpecularTextures[fst].second.useCount++;
					return {fst, (int)texture->GetRendererID()};
				}
			}

			FLOG_CORE_ERROR("Texture found in Id-Cache but not in texture storage. Check the implementation");
			return {};
		}

		texture->SetType(API::Texture::TextureType::SPECULAR);
		SpecularTextures[SpecularCounter] = {texture, {1, removable}}; // Initialize with use count 1
		texture->MakeResident();
		SpecularTexIdCache.push_back((int)texture->GetRendererID());

		UpdateSpecularTextureBuffer();

		return {SpecularCounter++, (int)texture->GetRendererID(), {texture->GetWidth(), texture->GetHeight()}};
	}

	void DeferredSamplerStore::RemoveSpecularTexture(unsigned short position, const bool force)
	{
		const auto it = SpecularTextures.find(position);
		if (it == SpecularTextures.end() || !it->second.second.removable)
			return;

		it->second.second.useCount--;

		if (it->second.second.useCount == 0 || force)
		{
			const auto cacheIt = std::find(SpecularTexIdCache.begin(), SpecularTexIdCache.end(), it->second.first->GetRendererID());
			if (cacheIt != SpecularTexIdCache.end())
				SpecularTexIdCache.erase(cacheIt);

			SpecularTextures.erase(it);
			UpdateSpecularTextureBuffer();
			LOG_CORE_INFO("Specular texture with ID {} removed from DeferredSamplerStore", position);
		}
		else
		{
			LOG_CORE_TRACE("Specular texture with ID {} use count decreased to {}", position, it->second.second.useCount);
		}
	}

	DeferredTextureInformation DeferredSamplerStore::AddParallaxTexture(Ref<API::Texture::Texture> texture, const bool removable)
	{
		if (!texture) return {};

		if (std::find(ParallaxTexIdCache.begin(), ParallaxTexIdCache.end(), texture->GetRendererID()) != ParallaxTexIdCache.end())
		{
			for (const auto& [fst, pair] : ParallaxTextures)
			{
				if (pair.first->GetRendererID() == texture->GetRendererID())
				{
					ParallaxTextures[fst].second.useCount++;
					return {fst, (int)texture->GetRendererID()};
				}
			}

			FLOG_CORE_ERROR("Texture found in Id-Cache but not in texture storage. Check the implementation");
			return {};
		}

		texture->SetType(API::Texture::TextureType::HEIGHT);
		ParallaxTextures[ParallaxCounter] = {texture, {1, removable}}; // Initialize with use count 1
		texture->MakeResident();
		ParallaxTexIdCache.push_back(texture->GetRendererID());

		UpdateParallaxTextureBuffer();

		return {ParallaxCounter++, (int)texture->GetRendererID(), {texture->GetWidth(), texture->GetHeight()}};
	}

	void DeferredSamplerStore::RemoveParallaxTexture(unsigned short position, const bool force)
	{
		const auto it = ParallaxTextures.find(position);
		if (it == ParallaxTextures.end() || !it->second.second.removable)
			return;

		it->second.second.useCount--;

		if (it->second.second.useCount == 0 || force)
		{
			const auto cacheIt = std::find(ParallaxTexIdCache.begin(), ParallaxTexIdCache.end(), it->second.first->GetRendererID());
			if (cacheIt != ParallaxTexIdCache.end())
				ParallaxTexIdCache.erase(cacheIt);

			ParallaxTextures.erase(it);
			UpdateParallaxTextureBuffer();
			LOG_CORE_INFO("Parallax texture with ID {} removed from DeferredSamplerStore", position);
		}
		else
		{
			LOG_CORE_TRACE("Parallax texture with ID {} use count decreased to {}", position, it->second.second.useCount);
		}
	}

	DeferredTextureInformation DeferredSamplerStore::AddNormalTexture(Ref<API::Texture::Texture> texture, const bool removable)
	{
		if (!texture) return {};

		if (std::find(NormalTexIdCache.begin(), NormalTexIdCache.end(), texture->GetRendererID()) != NormalTexIdCache.end())
		{
			for (const auto& [fst, pair] : NormalTextures)
			{
				if (pair.first->GetRendererID() == texture->GetRendererID())
				{
					NormalTextures[fst].second.useCount++;
					return {fst, (int)texture->GetRendererID()};
				}
			}

			FLOG_CORE_ERROR("Texture found in Id-Cache but not in texture storage. Check the implementation");
			return {};
		}

		texture->SetType(API::Texture::TextureType::NORMAL);
		NormalTextures[NormalCounter] = {texture, {1, removable}}; // Initialize with use count 1
		texture->MakeResident();
		NormalTexIdCache.push_back(texture->GetRendererID());

		UpdateNormalTextureBuffer();

		return {NormalCounter++, (int)texture->GetRendererID(), {texture->GetWidth(), texture->GetHeight()}};
	}

	void DeferredSamplerStore::RemoveNormalTexture(unsigned short position, const bool force)
	{
		const auto it = NormalTextures.find(position);
		if (it == NormalTextures.end() || !it->second.second.removable)
			return;

		it->second.second.useCount--;

		if (it->second.second.useCount == 0 || force)
		{
			const auto cacheIt = std::find(NormalTexIdCache.begin(), NormalTexIdCache.end(), it->second.first->GetRendererID());
			if (cacheIt != NormalTexIdCache.end())
				NormalTexIdCache.erase(cacheIt);

			NormalTextures.erase(it);
			UpdateNormalTextureBuffer();
			LOG_CORE_INFO("Normal texture with ID {} removed from DeferredSamplerStore", position);
		}
		else
		{
			LOG_CORE_TRACE("Normal texture with ID {} use count decreased to {}", position, it->second.second.useCount);
		}
	}
#pragma endregion

	void DeferredSamplerStore::UpdateSamplerMapping(const unsigned short index, const unsigned short diffuseId,
	                                                   const unsigned short specularId, const unsigned short parallaxId,
	                                                   const unsigned short normalId, const unsigned short customMatId) const
	{
		if (index >= MAX_SAMPLER_MAPPINGS) return;

		if (!IsMappingUsed(index))
		{
			LOG_CORE_WARN("Editing a sampler mapping that is not assigned to an object.");
		}

		SamplerMapping mapping;
		mapping.diffuseIndex = diffuseId;
		mapping.specularIndex = specularId;
		mapping.parallaxIndex = parallaxId;
		mapping.normalIndex = normalId;
		mapping.customMatIndex = customMatId;

		SamplerMappingsBuffer->AddData(&mapping, sizeof(SamplerMapping), (int)(index * sizeof(SamplerMapping)));
	}

	unsigned short DeferredSamplerStore::CreateSamplerMapping(
		const unsigned short diffuseId,
		const unsigned short specularId,
		const unsigned short parallaxId,
		const unsigned short normalId,
		const unsigned short customMatId)
	{
		SamplerMapping mapping;
		mapping.diffuseIndex = diffuseId;
		mapping.specularIndex = specularId;
		mapping.parallaxIndex = parallaxId;
		mapping.normalIndex = normalId;
		mapping.customMatIndex = customMatId;

		const int index = GetUnusedMapping();
		SetMappingUsed(index);
		SamplerMappingsBuffer->AddData(&mapping, sizeof(SamplerMapping), (int)(index * sizeof(SamplerMapping)));

		return index;
	}

	Ref<API::Texture::Texture> DeferredSamplerStore::GetDiffuseTexture(const unsigned short position)
	{
		return DiffuseTextures.count(position) ? DiffuseTextures[position].first : DiffuseTextures[0].first;
	}

	Ref<API::Texture::Texture> DeferredSamplerStore::GetSpecularTexture(const unsigned short position)
	{
		return SpecularTextures.count(position) ? SpecularTextures[position].first : SpecularTextures[0].first;
	}

	Ref<API::Texture::Texture> DeferredSamplerStore::GetParallaxTexture(const unsigned short position)
	{
		return ParallaxTextures.count(position) ? ParallaxTextures[position].first : ParallaxTextures[0].first;
	}

	Ref<API::Texture::Texture> DeferredSamplerStore::GetNormalTexture(const unsigned short position)
	{
		return NormalTextures.count(position) ? NormalTextures[position].first : NormalTextures[0].first;
	}

	void DeferredSamplerStore::MakeTexturesResidentIdempotent() const
	{
		for (const auto& [fst, pair] : DiffuseTextures)
		{
			pair.first->MakeResident();
		}

		for (const auto& [fst, pair] : SpecularTextures)
		{
			pair.first->MakeResident();
		}

		for (const auto& [fst, pair] : ParallaxTextures)
		{
			pair.first->MakeResident();
		}

		for (const auto& [fst, pair] : NormalTextures)
		{
			pair.first->MakeResident();
		}
	}

	void DeferredSamplerStore::UpdateBufferData() const
	{
		UpdateDiffuseTextureBuffer();
		UpdateSpecularTextureBuffer();
		UpdateParallaxTextureBuffer();
		UpdateNormalTextureBuffer();
	}

	void DeferredSamplerStore::BindSamplerBuffersToShaderPoints() const
	{
		DiffuseSamplersBuffer->BindBase(1);
		SpecularSamplersBuffer->BindBase(2);
		ParallaxSamplersBuffer->BindBase(3);
		NormalSamplersBuffer->BindBase(4);
		CustomMatBuffer->BindBase(5);

		SamplerMappingsBuffer->BindBase(6);
	}

	void DeferredSamplerStore::UpdateDiffuseTextureBuffer() const
	{
		std::vector<uint64_t> diffuseSamplerIds(DiffuseCounter + 1);
		for (const auto& [fst, pair] : DiffuseTextures)
		{
			diffuseSamplerIds[fst] = pair.first->GenerateHandle();
		}
		DiffuseSamplersBuffer->SetData(diffuseSamplerIds.data(), diffuseSamplerIds.size() * sizeof(uint64_t));
	}

	void DeferredSamplerStore::UpdateSpecularTextureBuffer() const
	{
		std::vector<uint64_t> specularSamplerIds(SpecularCounter + 1);
		for (const auto& [fst, pair] : SpecularTextures)
		{
			specularSamplerIds[fst] = pair.first->GenerateHandle();
		}
		SpecularSamplersBuffer->SetData(specularSamplerIds.data(), specularSamplerIds.size() * sizeof(uint64_t));
	}

	void DeferredSamplerStore::UpdateParallaxTextureBuffer() const
	{
		std::vector<uint64_t> parallaxSamplerIds(ParallaxCounter + 1);
		for (const auto& [fst, pair] : ParallaxTextures)
		{
			parallaxSamplerIds[fst] = pair.first->GenerateHandle();
		}
		ParallaxSamplersBuffer->SetData(parallaxSamplerIds.data(), parallaxSamplerIds.size() * sizeof(uint64_t));
	}

	void DeferredSamplerStore::UpdateNormalTextureBuffer() const
	{
		std::vector<uint64_t> normalSamplerIds(NormalCounter + 1);
		for (const auto& [fst, pair] : NormalTextures)
		{
			normalSamplerIds[fst] = pair.first->GenerateHandle();
		}
		NormalSamplersBuffer->SetData(normalSamplerIds.data(), normalSamplerIds.size() * sizeof(uint64_t));
	}

	void DeferredSamplerStore::UpdateMaterialBuffer() const
	{
		std::vector<CustomMaterialShaderObject> materialIds(MatCounter + 1);
		for (const auto& [fst, pair] : Materials)
		{
			materialIds[fst] = pair.first.shaderObject;
		}
		CustomMatBuffer->SetData(materialIds.data(), materialIds.size() * sizeof(CustomMaterialShaderObject));
	}

	int DeferredSamplerStore::GetUnusedMapping() const
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
}
