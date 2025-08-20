#include "castpch.h"
#include "ShaderCacheRegistry.h"

namespace Cast
{
	ShaderCacheRegistry ShaderCacheRegistryInstance{};
}

Cast::ShaderCacheRegistry::ShaderCacheRegistry()
{
	Shaders.reserve(0x1000);
}

Cast::UID Cast::ShaderCacheRegistry::Add(API::Core::Shader* shader)
{
	const UID id = UID::Create();
	Shaders[id].reset(shader);
	return id;
}

bool Cast::ShaderCacheRegistry::AddProxy(UID shaderId, const std::string& proxyName)
{
	ProxyIds.insert({proxyName, shaderId});
	if (Shaders.find(shaderId) == Shaders.end())
	{
		LOG_CORE_ERROR("Attempted to add proxy for non-existent shader with ID: {0}", shaderId.GetID());
		return false;
	}

	return true;
}

Cast::Ref<API::Core::Shader> Cast::ShaderCacheRegistry::GetHandle(const UID id)
{
	try
	{
		return Shaders.at(id);
	}
	catch (...)
	{
		LOG_CORE_ERROR(
			"SceneShaderCache::GetShaderHandle(unsigned short id) - No shader with id: " + std::to_string(id));
	}

	return nullptr;
}

Cast::Ref<API::Core::Shader> Cast::ShaderCacheRegistry::GetHandle(const std::string& proxy)
{
	try
	{
		return Shaders.at(ProxyIds.at(proxy));
	}
	catch (...)
	{
		LOG_CORE_ERROR(
			"SceneShaderCache::GetShaderHandle(unsigned short id) - No shader with identifier: " + proxy);
	}

	return nullptr;
}

Cast::UID Cast::ShaderCacheRegistry::GetShaderId(const std::string& proxy) const
{
	try
	{
		return ProxyIds.at(proxy);
	}
	catch (...)
	{
		LOG_CORE_ERROR(
			"SceneShaderCache::GetShaderHandle(unsigned short id) - No shader with identifier: " + proxy);
	}

	return UID::None();
}
