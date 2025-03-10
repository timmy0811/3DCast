#pragma once

#include "3DCast/Scene/Component/AbstractComponent.h"

namespace Cast::Component {
	struct ShaderComponent : public Component
	{
		std::string Identifier;
		Cast::Ref<API::Core::Shader> Shader;

		ShaderComponent() = default;
		ShaderComponent(const ShaderComponent&) = default;
		ShaderComponent(const std::string& path_vertex, const std::string& path_frag, const std::string& id)
			: Identifier(id) {
			Shader.reset(API::Core::Shader::Create(path_vertex, path_frag));
		}
	};
}