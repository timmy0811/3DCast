#include "C:/Git/3DCast/3DCast/CMakeFiles/3DCast.dir/Debug/cmake_pch.hxx"
#include "SceneShaderCache.h"

namespace Cast {
	SceneShaderCache AssetCache{};
}

Cast::SceneShaderCache::SceneShaderCache()
{
	DiffuseTextures.reserve(10);
	SpecularTextures.reserve(10);
	AmbientTextures.reserve(10);
	NormalTextures.reserve(10);
	HeightTextures.reserve(10);
	EmissiveTextures.reserve(10);
	ShininessTextures.reserve(10);
	OpacityTextures.reserve(10);
	ReflectanceTextures.reserve(10);

	Shaders.reserve(10);
}

Cast::Ref<API::Core::Shader> Cast::SceneShaderCache::GetShaderHandle(unsigned short id)
{
	try {
		return Shaders.at(id);
	}
	catch (...) {
		LOG_CORE_ERROR("SceneShaderCache::GetShaderHandle(unsigned short id) - No shader with id: " + std::to_string(id));
	}
	return nullptr;
}

Cast::Ref<API::Core::Shader> Cast::SceneShaderCache::GetShaderHandle(const std::string& identifier)
{
	try {
		return Shaders.at(ShaderIdentifierMap.at(identifier));
	}
	catch (...) {
		LOG_CORE_ERROR("SceneShaderCache::GetShaderHandle(unsigned short id) - No shader with identifiert: " + identifier);
	}
	return nullptr;
}

unsigned int Cast::SceneShaderCache::GetShaderId(const std::string& identifier)
{
	try {
		return ShaderIdentifierMap.at(identifier);
	}
	catch (...) {
		LOG_CORE_ERROR("SceneShaderCache::GetShaderHandle(unsigned short id) - No shader with identifiert: " + identifier);
	}
	return 0;
}