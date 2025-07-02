#include "castpch.h"
#include "DeferredSamplerRegistry.h"

#include "SceneShaderCache.h"

namespace Cast
{
	// Global instance
	DeferredSamplerRegistry SamplerRegistry;

	DeferredSamplerRegistry::DeferredSamplerRegistry()
	{
		DiffuseTextures.reserve(32);
		SpecularTextures.reserve(32);
		ParallaxTextures.reserve(32);
		NormalTextures.reserve(32);

		PathCache.reserve(64);
		DiffuseTexIdCache.reserve(0x1000);
		SpecularTexIdCache.reserve(0x1000);
		NormalTexIdCache.reserve(0x1000);
		ParallaxTexIdCache.reserve(0x1000);
	}

	DeferredSamplerRegistry::~DeferredSamplerRegistry() = default;

	void DeferredSamplerRegistry::InitAfterDriverSetup()
	{
		constexpr unsigned int MAX_TEXTURES_PER_SLOT = 128;

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

		// Default samplers
		AddDiffuseTexture(AssetCache.AddTexture(std::string(ASSET_DIR) + "img/default_samplers/default_diffuse.png"));
		AddSpecularTexture(AssetCache.AddTexture(std::string(ASSET_DIR) + "img/default_samplers/default_specular.png"));
		AddParallaxTexture(AssetCache.AddTexture(std::string(ASSET_DIR) + "img/default_samplers/default_parallax.png"));
		AddNormalTexture(AssetCache.AddTexture(std::string(ASSET_DIR) + "img/default_samplers/default_normal.png"));

		CreateSamplerMapping(0, 0, 0, 0);
	}

#pragma region ADDERS
	TextureInformation DeferredSamplerRegistry::AddDiffuseTexture(Ref<API::Texture::Texture> texture)
	{
		if (!texture) return {}; // Return default texture index if null

		if (std::find(DiffuseTexIdCache.begin(), DiffuseTexIdCache.end(), texture->GetRendererID()) != DiffuseTexIdCache
			.end())
		{
			for (const auto& [fst, snd] : DiffuseTextures)
			{
				if (snd->GetRendererID() == texture->GetRendererID())
					return {fst, (int)texture->GetRendererID()};
			}

			LOG_CORE_ERROR("Texture found in Id-Cache but not in texture storage. Check the implementation");
			return {};
		}

		texture->SetType(API::Texture::TextureType::DIFFUSE);
		DiffuseTextures[DiffuseCounter] = texture;
		texture->MakeResident();
		DiffuseTexIdCache.push_back(texture->GetRendererID());

		std::vector<uint64_t> diffuseSamplerIds(DiffuseCounter + 1);
		for (const auto& [fst, snd] : DiffuseTextures)
		{
			diffuseSamplerIds[fst] = snd->GenerateHandle();
		}
		DiffuseSamplersBuffer->SetData(diffuseSamplerIds.data(), diffuseSamplerIds.size() * sizeof(uint64_t));

		return {DiffuseCounter++, (int)texture->GetRendererID(), {texture->GetWidth(), texture->GetHeight()}};
	}

	TextureInformation DeferredSamplerRegistry::AddSpecularTexture(Ref<API::Texture::Texture> texture)
	{
		if (!texture) return {};

		if (std::find(SpecularTexIdCache.begin(), SpecularTexIdCache.end(), texture->GetRendererID()) !=
			SpecularTexIdCache.end())
		{
			for (const auto& [fst, snd] : SpecularTextures)
			{
				if (snd->GetRendererID() == texture->GetRendererID())
					return {fst, (int)texture->GetRendererID()};
			}

			LOG_CORE_ERROR("Texture found in Id-Cache but not in texture storage. Check the implementation");
			return {};
		}

		texture->SetType(API::Texture::TextureType::SPECULAR);
		SpecularTextures[SpecularCounter] = texture;
		texture->MakeResident();
		SpecularTexIdCache.push_back((int)texture->GetRendererID());

		std::vector<uint64_t> specularSamplerIds(SpecularCounter + 1);
		for (const auto& [fst, snd] : SpecularTextures)
		{
			specularSamplerIds[fst] = snd->GenerateHandle();
		}
		SpecularSamplersBuffer->SetData(specularSamplerIds.data(), specularSamplerIds.size() * sizeof(uint64_t));

		return {SpecularCounter++, (int)texture->GetRendererID(), {texture->GetWidth(), texture->GetHeight()}};
	}

	TextureInformation DeferredSamplerRegistry::AddParallaxTexture(Ref<API::Texture::Texture> texture)
	{
		if (!texture) return {};

		if (std::find(ParallaxTexIdCache.begin(), ParallaxTexIdCache.end(), texture->GetRendererID()) !=
			ParallaxTexIdCache.end())
		{
			for (const auto& [fst, snd] : ParallaxTextures)
			{
				if (snd->GetRendererID() == texture->GetRendererID())
					return {fst, (int)texture->GetRendererID()};
			}

			LOG_CORE_ERROR("Texture found in Id-Cache but not in texture storage. Check the implementation");
			return {};
		}

		texture->SetType(API::Texture::TextureType::HEIGHT);
		ParallaxTextures[ParallaxCounter] = texture;
		texture->MakeResident();
		ParallaxTexIdCache.push_back(texture->GetRendererID());

		std::vector<uint64_t> shininessSamplerIds(ParallaxCounter + 1);
		for (const auto& [fst, snd] : ParallaxTextures)
		{
			shininessSamplerIds[fst] = snd->GenerateHandle();
		}
		ParallaxSamplersBuffer->SetData(shininessSamplerIds.data(), shininessSamplerIds.size() * sizeof(uint64_t));

		return {ParallaxCounter++, (int)texture->GetRendererID(), {texture->GetWidth(), texture->GetHeight()}};
	}

	TextureInformation DeferredSamplerRegistry::AddNormalTexture(Ref<API::Texture::Texture> texture)
	{
		if (!texture) return {};

		if (std::find(NormalTexIdCache.begin(), NormalTexIdCache.end(), texture->GetRendererID()) != NormalTexIdCache.
			end())
		{
			for (const auto& [fst, snd] : NormalTextures)
			{
				if (snd->GetRendererID() == texture->GetRendererID())
					return {fst, (int)texture->GetRendererID()};
			}

			LOG_CORE_ERROR("Texture found in Id-Cache but not in texture storage. Check the implementation");
			return {};
		}

		texture->SetType(API::Texture::TextureType::NORMAL);
		NormalTextures[NormalCounter] = texture;
		texture->MakeResident();
		NormalTexIdCache.push_back(texture->GetRendererID());

		std::vector<uint64_t> normalSamplerIds(NormalCounter + 1);
		for (const auto& [fst, snd] : NormalTextures)
		{
			normalSamplerIds[fst] = snd->GenerateHandle();
		}
		NormalSamplersBuffer->SetData(normalSamplerIds.data(), normalSamplerIds.size() * sizeof(uint64_t));

		return {NormalCounter++, (int)texture->GetRendererID(), {texture->GetWidth(), texture->GetHeight()}};
	}

	TextureInformation DeferredSamplerRegistry::AddDiffuseTexture(const std::string& path, const bool flipUV)
	{
		if (path.empty()) return {};

		return AddDiffuseTexture(AssetCache.AddTexture(path, flipUV));
	}

	TextureInformation DeferredSamplerRegistry::AddSpecularTexture(const std::string& path, const bool flipUV)
	{
		if (path.empty()) return {};

		return AddSpecularTexture(AssetCache.AddTexture(path, flipUV));
	}

	TextureInformation DeferredSamplerRegistry::AddParallaxTexture(const std::string& path, const bool flipUV)
	{
		if (path.empty()) return {};

		return AddParallaxTexture(AssetCache.AddTexture(path, flipUV));
	}

	TextureInformation DeferredSamplerRegistry::AddNormalTexture(const std::string& path, const bool flipUV)
	{
		if (path.empty()) return {};

		return AddNormalTexture(AssetCache.AddTexture(path, flipUV));
	}
#pragma endregion

	void DeferredSamplerRegistry::UpdateSamplerMapping(const unsigned short index, const unsigned short diffuseId,
	                                                   const unsigned short specularId, const unsigned short parallaxId,
	                                                   const unsigned short normalId) const
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

		SamplerMappingsBuffer->AddData(&mapping, sizeof(SamplerMapping), (int)(index * sizeof(SamplerMapping)));
	}

	unsigned short DeferredSamplerRegistry::CreateSamplerMapping(
		const unsigned short diffuseId,
		const unsigned short specularId,
		const unsigned short parallaxId,
		const unsigned short normalId)
	{
		SamplerMapping mapping;
		mapping.diffuseIndex = diffuseId;
		mapping.specularIndex = specularId;
		mapping.parallaxIndex = parallaxId;
		mapping.normalIndex = normalId;

		const int index = GetUnusedMapping();
		SetMappingUsed(index);
		SamplerMappingsBuffer->AddData(&mapping, sizeof(SamplerMapping), (int)(index * sizeof(SamplerMapping)));

		return index;
	}

	Ref<API::Texture::Texture> DeferredSamplerRegistry::GetDiffuseTexture(const unsigned short id)
	{
		return DiffuseTextures.count(id) ? DiffuseTextures[id] : DiffuseTextures[0];
	}

	Ref<API::Texture::Texture> DeferredSamplerRegistry::GetSpecularTexture(const unsigned short id)
	{
		return SpecularTextures.count(id) ? SpecularTextures[id] : SpecularTextures[0];
	}

	Ref<API::Texture::Texture> DeferredSamplerRegistry::GetParallaxTexture(const unsigned short id)
	{
		return ParallaxTextures.count(id) ? ParallaxTextures[id] : ParallaxTextures[0];
	}

	Ref<API::Texture::Texture> DeferredSamplerRegistry::GetNormalTexture(const unsigned short id)
	{
		return NormalTextures.count(id) ? NormalTextures[id] : NormalTextures[0];
	}

	void DeferredSamplerRegistry::MakeTexturesResidentIdempotent() const
	{
		for (const auto& [fst, snd] : DiffuseTextures)
		{
			snd->MakeResident();
		}

		for (const auto& [fst, snd] : SpecularTextures)
		{
			snd->MakeResident();
		}

		for (const auto& [fst, snd] : ParallaxTextures)
		{
			snd->MakeResident();
		}

		for (const auto& [fst, snd] : NormalTextures)
		{
			snd->MakeResident();
		}
	}

	void DeferredSamplerRegistry::UpdateBufferData() const
	{
		std::vector<uint64_t> diffuseSamplerIds(DiffuseCounter);
		for (const auto& [fst, snd] : DiffuseTextures)
		{
			diffuseSamplerIds[fst] = snd->GenerateHandle();
		}
		DiffuseSamplersBuffer->SetData(diffuseSamplerIds.data(), diffuseSamplerIds.size() * sizeof(uint64_t));

		std::vector<uint64_t> specularSamplerIds(SpecularCounter);
		for (const auto& [fst, snd] : SpecularTextures)
		{
			specularSamplerIds[fst] = snd->GenerateHandle();
		}
		SpecularSamplersBuffer->SetData(specularSamplerIds.data(), specularSamplerIds.size() * sizeof(uint64_t));

		std::vector<uint64_t> shininessSamplerIds(ParallaxCounter);
		for (const auto& [fst, snd] : ParallaxTextures)
		{
			shininessSamplerIds[fst] = snd->GenerateHandle();
		}
		ParallaxSamplersBuffer->SetData(shininessSamplerIds.data(), shininessSamplerIds.size() * sizeof(uint64_t));

		std::vector<uint64_t> normalSamplerIds(NormalCounter);
		for (const auto& [fst, snd] : NormalTextures)
		{
			normalSamplerIds[fst] = snd->GenerateHandle();
		}
		NormalSamplersBuffer->SetData(normalSamplerIds.data(), normalSamplerIds.size() * sizeof(uint64_t));
	}

	void DeferredSamplerRegistry::BindSamplerBuffersToShaderPoints() const
	{
		DiffuseSamplersBuffer->BindBase(1);
		SpecularSamplersBuffer->BindBase(2);
		ParallaxSamplersBuffer->BindBase(3);
		NormalSamplersBuffer->BindBase(4);

		SamplerMappingsBuffer->BindBase(5);
	}
}
