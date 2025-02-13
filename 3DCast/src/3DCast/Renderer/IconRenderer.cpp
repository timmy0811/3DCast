#include "castpch.h"
#include "IconRenderer.h"

#include "3DCast/Renderer/Renderer.h"
#include "3DCast/Scene/SceneShaderCache.h"

Cast::IconRenderer::IconRenderer(const std::string& palleteConfigPath, const std::string& palleteImgPath)
{
	Pallete.reset(API::Texture::Texture::Create(palleteImgPath));

	SourceSSBO.reset(API::Core::Buffer::Create(API::Core::Buffer::BufferType::SHADER_STORAGE_BUFFER, API::Core::Buffer::MemoryLayout::DYNAMIC, 128, sizeof(BillboardSource)));
	IconDataSSBO.reset(API::Core::Buffer::Create(API::Core::Buffer::BufferType::SHADER_STORAGE_BUFFER, API::Core::Buffer::MemoryLayout::DYNAMIC, 128, sizeof(IconData)));

	ParsePalleteConfig(palleteConfigPath);

	float vertices[] = {
		-0.5f, -0.5f, 0.0f, 2.f,
		 0.5f, -0.5f, 0.0f, 3.f,
		 0.5f,  0.5f, 0.0f, 1.f,

		-0.5f, -0.5f, 0.0f, 2.f,
		 0.5f,  0.5f, 0.0f, 1.f,
		-0.5f,  0.5f, 0.0f, 0.f
	};

	Vb.reset(API::Core::VertexBuffer::Create(vertices, sizeof(vertices)));

	VbLayout.reset(API::Core::VertexBufferLayout::Create());
	VbLayout->Push(API::Core::ShaderDataType::Float3);
	VbLayout->Push(API::Core::ShaderDataType::Float);

	Va.reset(API::Core::VertexArray::Create());
	Va->AddBuffer(*Vb, *VbLayout);
	Va->SetVBCount(6);
}

void Cast::IconRenderer::AddIcon(Icon icon, const glm::vec3& position)
{
	BillboardSource source = { position, 1.0, (int)icon };
	SourceSSBO->AddData(&source, sizeof(BillboardSource));

	IconsToBeRendered++;
}

void Cast::IconRenderer::RenderAll()
{
	auto shader = AssetCache.GetShaderHandle("icon_billboard");
	shader->Bind();
	Pallete->Bind(0);
	shader->SetUniform1i("u_Pallete", Pallete->GetBoundPort());

	API::Core::RenderCommand::DrawInstanced(Va, 6, IconsToBeRendered);
}

void Cast::IconRenderer::ParsePalleteConfig(const std::string& palletePath)
{
	LOG_CORE_INFO("Loading icons");
	YAML::Node mainNode = YAML::LoadFile(palletePath)["icons"];

	ParseSingleIcon(mainNode, "light_directional", Icon::LightDirectional);
	ParseSingleIcon(mainNode, "light_point", Icon::LightPoint);
	ParseSingleIcon(mainNode, "light_spot", Icon::LightSpot);
	ParseSingleIcon(mainNode, "camera", Icon::Camera);

	IconDataSSBO->AddData(Icons.data(), (int)(Icons.size() * sizeof(IconData)));
}

void Cast::IconRenderer::ParseSingleIcon(const YAML::Node& node, const std::string& iconId, Icon icon)
{
	glm::vec4 uvx = {
		node[iconId]["uv0"][0].as<float>(),
		node[iconId]["uv1"][0].as<float>(),
		node[iconId]["uv0"][0].as<float>(),
		node[iconId]["uv1"][0].as<float>()
	};

	glm::vec4 uvy = {
		node[iconId]["uv0"][1].as<float>(),
		node[iconId]["uv0"][1].as<float>(),
		node[iconId]["uv1"][1].as<float>(),
		node[iconId]["uv1"][1].as<float>()
	};

	Icons.push_back({ uvx, uvy, icon });
}