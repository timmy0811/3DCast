#pragma once

#include "3DCast/Core.h"

#include <API/core/Buffer.h>
#include <API/core/Shader.h>
#include <API/texture/Texture.h>
#include <API/core/VertexArray.h>
#include <API/core/VertexBufferLayout.h>

#include "Vendor/glm/glm.hpp"

#include <yaml-cpp/yaml.h>

namespace Cast {
	enum Icon {
		LightDirectional = 0,
		LightPoint,
		LightSpot,
		Camera
	};

	class IconRenderer {
	public:
		IconRenderer(const std::string& palleteConfigPath, const std::string& palleteImgPath);
		~IconRenderer() = default;

		inline void Clear() { SourceSSBO->Empty(); IconsToBeRendered = 0; };

		void AddIcon(Icon icon, const glm::vec3& position);
		void RenderAll();

		inline void BindBufferBaseDefault() { SourceSSBO->BindBase(4); IconDataSSBO->BindBase(5); }

	private:
		void ParsePalleteConfig(const std::string& palletePath);
		void ParseSingleIcon(const YAML::Node& node, const std::string& iconId, Icon icon);

	private:
		struct BillboardSource {
			alignas(16) glm::vec3 position;
			float scale;
			int iconId;
		};

		struct IconData {
			alignas(16) glm::vec4 uvx;
			alignas(16) glm::vec4 uvy;
			Icon icon;
		};

		int IconsToBeRendered = 0;

		std::vector<IconData> Icons;

		Cast::Ref<API::Texture::Texture> Pallete;

		Cast::Ref<API::Core::Buffer> SourceSSBO;
		Cast::Ref<API::Core::Buffer> IconDataSSBO;

		Cast::Ref<API::Core::VertexBuffer> Vb;
		Cast::Ref<API::Core::VertexBufferLayout> VbLayout;
		Cast::Ref<API::Core::VertexArray> Va;
	};
}