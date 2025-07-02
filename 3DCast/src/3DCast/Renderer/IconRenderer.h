#pragma once

#include "3DCast/Core.h"

#include <API/core/Buffer.h>
#include <API/texture/Texture.h>
#include <API/core/VertexArray.h>
#include <API/core/VertexBufferLayout.h>

#include "vendor/glm/glm.hpp"

#include <yaml-cpp/yaml.h>

namespace Cast
{
	enum Icon
	{
		LightDirectional = 0,
		LightPoint,
		LightSpot,
		Camera
	};

	class IconRenderer
	{
	public:
		IconRenderer(const std::string& palleteConfigPath, const std::string& palleteImgPath);
		~IconRenderer() = default;

		inline void Clear()
		{
			SourceSSBO->Empty();
			IconsToBeRendered = 0;
		};

		void AddIcon(Icon icon, const glm::vec3& position);
		void RenderAll() const;

		inline void BindBufferBaseDefault() const
		{
			SourceSSBO->BindBase(4);
			IconDataSSBO->BindBase(5);
		}

	private:
		void ParsePalleteConfig(const std::string& palletePath);
		void ParseSingleIcon(const YAML::Node& node, const std::string& iconId, Icon icon);

	private:
		struct BillboardSource
		{
			alignas(16) glm::vec3 position;
			float scale;
			int iconId;
		};

		struct IconData
		{
			alignas(16) glm::vec4 uvx;
			alignas(16) glm::vec4 uvy;
			Icon icon;
		};

		int IconsToBeRendered = 0;

		std::vector<IconData> Icons;

		Ref<API::Texture::Texture> Pallete;

		Ref<API::Core::Buffer> SourceSSBO;
		Ref<API::Core::Buffer> IconDataSSBO;

		Ref<API::Core::VertexBuffer> Vb;
		Ref<API::Core::VertexBufferLayout> VbLayout;
		Ref<API::Core::VertexArray> Va;
	};
}
