#include "castpch.h"
#include "TextureManager.h"

namespace Cast {
	// Global instance
	TextureManager g_TextureManager;

	TextureManager::TextureManager() {
		DiffuseTextures.reserve(32);
		SpecularTextures.reserve(32);
		ShininessTextures.reserve(32);
		NormalTextures.reserve(32);
		SamplerMappings.reserve(32);

		PathCache.reserve(64);
	}

	TextureManager::~TextureManager() {
		// Clean up resources if needed
	}

	void TextureManager::InitAfterDriverSetup()
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

		ShininessSamplersBuffer.reset(API::Core::Buffer::Create(
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
			MAX_TEXTURES_PER_SLOT * 2 * sizeof(SamplerMapping)
		));

		// Default samplers
		AddDiffuseTexture(API::Texture::Texture::Create("../3DCast/ressources/img/default_samplers/default_diffuse.png"));
		AddSpecularTexture(API::Texture::Texture::Create("../3DCast/ressources/img/default_samplers/default_specular.png"));
		AddShininessTexture(API::Texture::Texture::Create("../3DCast/ressources/img/default_samplers/default_shine.png"));
		AddNormalTexture(API::Texture::Texture::Create("../3DCast/ressources/img/default_samplers/default_normal.png"));

		CreateSamplerMapping(0, 0, 0, 0);
	}

	TextureInformation TextureManager::AddDiffuseTexture(API::Texture::Texture* texture) {
		if (!texture) return {}; // Return default texture index if null

		if (std::find(TexIdCache.begin(), TexIdCache.end(), texture->GetRendererID()) != TexIdCache.end()) {
			for (const auto& pair : DiffuseTextures) {
				if (pair.second->GetRendererID() == texture->GetRendererID())
					return { pair.first, texture->GetRendererID() };
			}

			LOG_CORE_ERROR("Texture found in Id-Cache but not in texture storage. Check the implementation");
			return {};
		}

		texture->SetType(API::Texture::TextureType::DIFFUSE);
		DiffuseTextures[DiffuseCounter] = Ref<API::Texture::Texture>(texture);
		texture->MakeResident();

		std::vector<uint64_t> diffuseSamplerIds(DiffuseCounter + 1);
		for (const auto& pair : DiffuseTextures) {
			diffuseSamplerIds[pair.first] = pair.second->GenerateHandle();
		}
		DiffuseSamplersBuffer->SetData(diffuseSamplerIds.data(), diffuseSamplerIds.size() * sizeof(uint64_t));

		return { DiffuseCounter++, texture->GetRendererID() };
	}

	TextureInformation TextureManager::AddSpecularTexture(API::Texture::Texture* texture) {
		if (!texture) return {};

		if (std::find(TexIdCache.begin(), TexIdCache.end(), texture->GetRendererID()) != TexIdCache.end()) {
			for (const auto& pair : SpecularTextures) {
				if (pair.second->GetRendererID() == texture->GetRendererID())
					return { pair.first, texture->GetRendererID() };
			}

			LOG_CORE_ERROR("Texture found in Id-Cache but not in texture storage. Check the implementation");
			return {};
		}

		texture->SetType(API::Texture::TextureType::SPECULAR);
		SpecularTextures[SpecularCounter] = Ref<API::Texture::Texture>(texture);
		texture->MakeResident();

		std::vector<uint64_t> specularSamplerIds(SpecularCounter + 1);
		for (const auto& pair : SpecularTextures) {
			specularSamplerIds[pair.first] = pair.second->GenerateHandle();
		}
		SpecularSamplersBuffer->SetData(specularSamplerIds.data(), specularSamplerIds.size() * sizeof(uint64_t));

		return { SpecularCounter++, texture->GetRendererID() };
	}

	TextureInformation TextureManager::AddShininessTexture(API::Texture::Texture* texture) {
		if (!texture) return {};

		if (std::find(TexIdCache.begin(), TexIdCache.end(), texture->GetRendererID()) != TexIdCache.end()) {
			for (const auto& pair : ShininessTextures) {
				if (pair.second->GetRendererID() == texture->GetRendererID())
					return { pair.first, texture->GetRendererID() };
			}

			LOG_CORE_ERROR("Texture found in Id-Cache but not in texture storage. Check the implementation");
			return {};
		}

		texture->SetType(API::Texture::TextureType::SHINE);
		ShininessTextures[ShininessCounter] = Ref<API::Texture::Texture>(texture);
		texture->MakeResident();

		std::vector<uint64_t> shininessSamplerIds(ShininessCounter + 1);
		for (const auto& pair : ShininessTextures) {
			shininessSamplerIds[pair.first] = pair.second->GenerateHandle();
		}
		ShininessSamplersBuffer->SetData(shininessSamplerIds.data(), shininessSamplerIds.size() * sizeof(uint64_t));

		return { ShininessCounter++, texture->GetRendererID() };
	}

	TextureInformation TextureManager::AddNormalTexture(API::Texture::Texture* texture) {
		if (!texture) return {};

		if (std::find(TexIdCache.begin(), TexIdCache.end(), texture->GetRendererID()) != TexIdCache.end()) {
			for (const auto& pair : NormalTextures) {
				if (pair.second->GetRendererID() == texture->GetRendererID())
					return { pair.first, texture->GetRendererID() };
			}

			LOG_CORE_ERROR("Texture found in Id-Cache but not in texture storage. Check the implementation");
			return {};
		}

		texture->SetType(API::Texture::TextureType::NORMAL);
		NormalTextures[NormalCounter] = Ref<API::Texture::Texture>(texture);
		texture->MakeResident();

		std::vector<uint64_t> normalSamplerIds(NormalCounter + 1);
		for (const auto& pair : NormalTextures) {
			normalSamplerIds[pair.first] = pair.second->GenerateHandle();
		}
		NormalSamplersBuffer->SetData(normalSamplerIds.data(), normalSamplerIds.size() * sizeof(uint64_t));

		return { NormalCounter++, texture->GetRendererID() };
	}

	TextureInformation TextureManager::AddDiffuseTexture(const std::string& path, bool flipUV) {
		if (path.empty()) return{};

		return AddDiffuseTexture(API::Texture::Texture::Create(path, flipUV));
	}

	TextureInformation TextureManager::AddSpecularTexture(const std::string& path, bool flipUV) {
		if (path.empty()) return {};

		return AddSpecularTexture(API::Texture::Texture::Create(path, flipUV));
	}

	TextureInformation TextureManager::AddShininessTexture(const std::string& path, bool flipUV) {
		if (path.empty()) return {};

		return AddShininessTexture(API::Texture::Texture::Create(path, flipUV));
	}

	TextureInformation TextureManager::AddNormalTexture(const std::string& path, bool flipUV) {
		if (path.empty()) return {};

		return AddNormalTexture(API::Texture::Texture::Create(path, flipUV));
	}

	void TextureManager::UpdateSamplerMapping(unsigned short id, unsigned short diffuseId, unsigned short specularId, unsigned short shininessId, unsigned short normalId)
	{
		if (id >= SamplerMappings.size()) return;

		SamplerMappings[id].diffuseIndex = diffuseId;
		SamplerMappings[id].specularIndex = specularId;
		SamplerMappings[id].shininessIndex = shininessId;
		SamplerMappings[id].normalIndex = normalId;

		SamplerMappingsBuffer->SetData(SamplerMappings.data(), SamplerMappings.size() * sizeof(SamplerMapping));
	}

	unsigned short TextureManager::CreateSamplerMapping(
		unsigned short diffuseId,
		unsigned short specularId,
		unsigned short shininessId,
		unsigned short normalId)
	{
		SamplerMapping mapping;
		mapping.diffuseIndex = diffuseId;
		mapping.specularIndex = specularId;
		mapping.shininessIndex = shininessId;
		mapping.normalIndex = normalId;

		SamplerMappings.push_back(mapping);
		SamplerMappingsBuffer->SetData(SamplerMappings.data(), SamplerMappings.size() * sizeof(SamplerMapping));

		return SamplerMappingCounter++;
	}

	Ref<API::Texture::Texture> TextureManager::GetDiffuseTexture(unsigned short id) {
		return (DiffuseTextures.count(id)) ? DiffuseTextures[id] : DiffuseTextures[0];
	}

	Ref<API::Texture::Texture> TextureManager::GetSpecularTexture(unsigned short id) {
		return (SpecularTextures.count(id)) ? SpecularTextures[id] : SpecularTextures[0];
	}

	Ref<API::Texture::Texture> TextureManager::GetShininessTexture(unsigned short id) {
		return (ShininessTextures.count(id)) ? ShininessTextures[id] : ShininessTextures[0];
	}

	Ref<API::Texture::Texture> TextureManager::GetNormalTexture(unsigned short id) {
		return (NormalTextures.count(id)) ? NormalTextures[id] : NormalTextures[0];
	}

	SamplerMapping& TextureManager::GetSamplerMapping(unsigned short id) {
		return (id < SamplerMappings.size()) ? SamplerMappings[id] : SamplerMappings[0];
	}

	void TextureManager::MakeTexturesResidentIdempotent() {
		for (const auto& pair : DiffuseTextures) {
			pair.second->MakeResident();
		}

		for (const auto& pair : SpecularTextures) {
			pair.second->MakeResident();
		}

		for (const auto& pair : ShininessTextures) {
			pair.second->MakeResident();
		}

		for (const auto& pair : NormalTextures) {
			pair.second->MakeResident();
		}
	}

	void TextureManager::UpdateBufferData()
	{
		std::vector<uint64_t> diffuseSamplerIds(DiffuseCounter);
		for (const auto& pair : DiffuseTextures) {
			diffuseSamplerIds[pair.first] = pair.second->GenerateHandle();
		}
		DiffuseSamplersBuffer->SetData(diffuseSamplerIds.data(), diffuseSamplerIds.size() * sizeof(uint64_t));

		std::vector<uint64_t> specularSamplerIds(SpecularCounter);
		for (const auto& pair : SpecularTextures) {
			specularSamplerIds[pair.first] = pair.second->GenerateHandle();
		}
		SpecularSamplersBuffer->SetData(specularSamplerIds.data(), specularSamplerIds.size() * sizeof(uint64_t));

		std::vector<uint64_t> shininessSamplerIds(ShininessCounter);
		for (const auto& pair : ShininessTextures) {
			shininessSamplerIds[pair.first] = pair.second->GenerateHandle();
		}
		ShininessSamplersBuffer->SetData(shininessSamplerIds.data(), shininessSamplerIds.size() * sizeof(uint64_t));

		std::vector<uint64_t> normalSamplerIds(NormalCounter);
		for (const auto& pair : NormalTextures) {
			normalSamplerIds[pair.first] = pair.second->GenerateHandle();
		}
		NormalSamplersBuffer->SetData(normalSamplerIds.data(), normalSamplerIds.size() * sizeof(uint64_t));
	}

	void TextureManager::BindSamplerBuffersToShaderPoints()
	{
		DiffuseSamplersBuffer->BindBase(1);
		SpecularSamplersBuffer->BindBase(2);
		ShininessSamplersBuffer->BindBase(3);
		NormalSamplersBuffer->BindBase(4);

		SamplerMappingsBuffer->BindBase(5);
	}
}