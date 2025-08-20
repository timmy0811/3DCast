#pragma once

#include <API/core/Buffer.h>
#include <API/texture/Texture.h>
#include <API/core/Shader.h>

#include "3DCast/Core.h"
#include "3DCast/Misc/UID.h"

namespace Cast
{
	class ShaderCacheRegistry
	{
	public:
		ShaderCacheRegistry();
		~ShaderCacheRegistry() = default;

		UID Add(API::Core::Shader* shader);

		bool AddProxy(UID shaderId, const std::string& proxyName);

		Ref<API::Core::Shader> GetHandle(UID id);
		Ref<API::Core::Shader> GetHandle(const std::string& proxy);

		UID GetShaderId(const std::string& proxy) const;

#pragma region ANALYTICS
		[[nodiscard]] inline unsigned int GetShaderCount() const { return static_cast<unsigned int>(Shaders.size()); }
		[[nodiscard]] inline unsigned int GetProxyCount() const { return static_cast<unsigned int>(ProxyIds.size()); }
#pragma endregion
	private:
		std::unordered_map<UID, Ref<API::Core::Shader>> Shaders;
		std::unordered_map<std::string, UID> ProxyIds;
	};

	extern ShaderCacheRegistry ShaderCacheRegistryInstance;
}
