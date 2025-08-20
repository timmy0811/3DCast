#pragma once

#include <API/texture/Texture.h>
#include <API/core/Shader.h>

#include "3DCast/Core.h"
#include "3DCast/Misc/UID.h"

namespace Cast
{
	class TextureCacheRegistry
	{
	public:
		TextureCacheRegistry();
		~TextureCacheRegistry() = default;

		enum TextureType
		{
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

		UID Add(Ref<API::Texture::Texture> texture, bool useCached = true);
		UID AddFromFile(const std::string& path, bool flipUV = false);

		void Remove(UID textureId, bool force = false);
		void Remove(std::string proxy, bool force = false);

		bool AddProxy(UID textureId, const std::string& proxyName);

		Ref<API::Texture::Texture> GetHandleByPath(const std::string& path);
		Ref<API::Texture::Texture> GetHandleByProxy(const std::string& proxy);
		Ref<API::Texture::Texture> GetHandle(UID textureId);

		UID GetTextureIdByPath(const std::string& path) const;
		UID GetTextureIdByProxy(const std::string& path) const;

#pragma region ANALYTICS
		[[nodiscard]] inline unsigned int GetTextureCount() const { return static_cast<unsigned int>(Textures.size()); }
		[[nodiscard]] inline unsigned int GetProxyCount() const { return static_cast<unsigned int>(ProxyIds.size()); }
		[[nodiscard]] inline unsigned int GetPathCacheCount() const { return static_cast<unsigned int>(PathCache.size()); }
#pragma endregion

	private:
		struct CacheEntry
		{
			UID id;
			unsigned short useCount;
		};

		std::unordered_map<UID, std::pair<Ref<API::Texture::Texture>, unsigned int>> Textures;
		std::unordered_map<std::string, UID> PathCache;
		std::unordered_map<std::string, UID> ProxyIds;
	};

	extern TextureCacheRegistry TextureCacheRegistryInstance;
}
