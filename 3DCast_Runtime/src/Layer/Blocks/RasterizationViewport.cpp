#include "RasterizationViewport.h"

#include "Config.h"
#include "Data/SharedEditorData.h"

#include <imgui.h>
#include <imgui_internal.h>

Runtime::RasterizationViewport::RasterizationViewport(Cast::Layer* parent)
	: Viewport(parent) {}

void Runtime::RasterizationViewport::Init()
{
	RenderPipelineData.GBufferScreenGeometry.reset(API::Advanced::GBufferScreenGeometry::Create(Runtime::conf.WIN_WIDTH, Runtime::conf.WIN_HEIGHT));
	RenderPipelineData.Framebuffer.reset(API::Core::Framebuffer::Create(glm::ivec2(Runtime::conf.WIN_WIDTH, Runtime::conf.WIN_HEIGHT)));
	RenderPipelineData.GBuffer.reset(API::Advanced::GBuffer::Create(Runtime::conf.WIN_WIDTH, Runtime::conf.WIN_HEIGHT));

	RenderPipelineData.GBuffer->Bind();
	RenderPipelineData.GBuffer->AddRenderTarget("Position", 3, API::Core::BufferDataType::_FLOAT, API::Core::WrapMethod::CLAMP_TO_EDGE);
	RenderPipelineData.GBuffer->AddRenderTarget("Normal", 3, API::Core::BufferDataType::_FLOAT, API::Core::WrapMethod::CLAMP_TO_EDGE);
	RenderPipelineData.GBuffer->AddRenderTarget("Albedo", 3, API::Core::BufferDataType::_FLOAT, API::Core::WrapMethod::CLAMP_TO_EDGE);
	RenderPipelineData.GBuffer->AddRenderTarget("Specular", 3, API::Core::BufferDataType::_FLOAT, API::Core::WrapMethod::CLAMP_TO_EDGE);
	RenderPipelineData.GBuffer->AddRenderTarget("Shine_Reflectance", 2, API::Core::BufferDataType::_FLOAT16, API::Core::WrapMethod::CLAMP_TO_EDGE);

	RenderPipelineData.GBuffer->AddDepthTarget(API::Core::DepthBufferType::WRITE_ONLY);
	RenderPipelineData.GBuffer->Validate();

	CompileShaders();
}

void Runtime::RasterizationViewport::Destroy()
{
}

void Runtime::RasterizationViewport::OnUpdate(Cast::Timestep ts)
{
	if (Cast::Input::IsMouseButtonPressed(CAST_MOUSE_BUTTON_LEFT) && IsHovered) {
		ParentLayer->GetParentWindow()->SetInputModeDisabled();
		CurrentKeyState.isLMBPressed = true;
	}
	else {
		ParentLayer->GetParentWindow()->SetInputModeNormal();
		CurrentKeyState.isLMBPressed = false;
	}
}

void Runtime::RasterizationViewport::OnEvent(Cast::Event& e)
{
	Cast::EventDispatcher dispatcher(e);
	dispatcher.Dispatch<Cast::MouseMovedEvent>(CAST_BIND_EVENT_FUNC(RasterizationViewport::OnMouseMoved));
}

void Runtime::RasterizationViewport::OnRender()
{
	RenderGeometryPass();
	RenderLightingPass();
}

void Runtime::RasterizationViewport::OnImGuiRender()
{
	ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoTitleBar);
	ImVec2 viewportSize = ImGui::GetContentRegionAvail();
	static ImVec2 lastViewportSize = ImVec2(0, 0);
	if (viewportSize.x != lastViewportSize.x || viewportSize.y != lastViewportSize.y)
	{
		lastViewportSize = viewportSize;

		//Framebuffer->Resize({ static_cast<uint32_t>(viewportSize.x), static_cast<uint32_t>(viewportSize.y) });

		switch (Runtime::EditorContext.ActiveCamera->GetType()) {
		case Cast::Renderer::Camera::Type::Orthographic:
			((Cast::Renderer::OrthographicCamera*)Runtime::EditorContext.ActiveCamera.get())->SetFrustumOnResized(lastViewportSize.x, lastViewportSize.y);
			break;
		case Cast::Renderer::Camera::Type::Perspective:
			((Cast::Renderer::PerspectiveCamera*)Runtime::EditorContext.ActiveCamera.get())->SetAspectRatio(lastViewportSize.x / lastViewportSize.y);
		}
	}

	uint32_t textureID = RenderPipelineData.Framebuffer->GetColorAttachmentTextureID(0);
	ImGui::Image((unsigned long long)textureID, lastViewportSize);

	IsHovered = ImGui::IsWindowHovered() && ImGui::GetCurrentWindow()->Name == std::string("Viewport");
	IsFocused = ImGui::IsWindowFocused() && ImGui::GetCurrentWindow()->Name == std::string("Viewport");

	ImGui::End();
}

void Runtime::RasterizationViewport::RenderGeometryPass()
{
	API::Core::RenderCommand::SetDepthTest(true);
	API::Core::RenderCommand::CullFace(API::Core::Face::Back);

	RenderPipelineData.GBuffer->BindAndClear();
	Cast::Renderer::RendererContext::BeginScene(*Runtime::EditorContext.ActiveCamera);

	Runtime::EditorContext.ActiveScene->OnUpdate();

	Cast::Renderer::RendererContext::EndScene();
	RenderPipelineData.GBuffer->Unbind();
}

void Runtime::RasterizationViewport::RenderLightingPass()
{
	API::Core::RenderCommand::SetBlend(true);
	API::Core::RenderCommand::SetBlendFunc(API::Core::BlendFunction::SrcAlpha, API::Core::BlendFunction::OneMinusSrcAlpha);

	RenderPipelineData.Framebuffer->BindAndClear();

	RenderPipelineData.GBuffer->BindDepthTexture(0);
	RenderPipelineData.GBuffer->BindTextures(1);

	// Lighting Pass Uniforms
	Cast::Ref<API::Core::Shader> shader = Cast::AssetCache.GetShaderHandle("shader_shading_pass");
	shader->Bind();
	shader->SetUniform2f("u_Resolution", (float)Runtime::conf.WIN_WIDTH, (float)Runtime::conf.WIN_HEIGHT);

	// TODO: Maybe only set those once on startup
	shader->SetUniform1i("gBuf_Position", RenderPipelineData.GBuffer->GetTargetBoundTextureSlot("Position"));
	shader->SetUniform1i("gBuf_Normal", RenderPipelineData.GBuffer->GetTargetBoundTextureSlot("Normal"));
	shader->SetUniform1i("gBuf_Albedo", RenderPipelineData.GBuffer->GetTargetBoundTextureSlot("Albedo"));
	shader->SetUniform1i("gBuf_Specular", RenderPipelineData.GBuffer->GetTargetBoundTextureSlot("Specular"));
	shader->SetUniform1i("gBuf_Shine_Reflectance", RenderPipelineData.GBuffer->GetTargetBoundTextureSlot("Shine_Reflectance"));

	// GUI Background Color
	API::Core::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
	API::Core::RenderCommand::Clear();

	RenderPipelineData.Framebuffer->Bind();

	RenderPipelineData.GBufferScreenGeometry->Draw(Cast::AssetCache.GetShaderHandle("shader_shading_pass").get());

	RenderPipelineData.Framebuffer->Unbind();

	API::Core::RenderCommand::SetBlend(false);
}

void Runtime::RasterizationViewport::CompileShaders()
{
	Cast::AssetCache.AddShader("shader_geometry_pass", API::Core::Shader::Create("../3DCast/ressources/shader/deferred/geometry_pass.vert", "../3DCast/ressources/shader/deferred/geometry_pass.frag"));
	Cast::AssetCache.AddShader("shader_shading_pass", API::Core::Shader::Create("../3DCast/ressources/shader/deferred/shading_pass.vert", "../3DCast/ressources/shader/deferred/shading_pass.frag"));
}

bool Runtime::RasterizationViewport::OnMouseMoved(Cast::MouseMovedEvent& e)
{
	if (CurrentKeyState.isLMBPressed && IsHovered && !IsInitFrame) {
		glm::vec3 cameraRotation = (Runtime::EditorContext.ActiveCamera->GetRotation());

		glm::vec2 offset = {
			e.GetX() - LastMousePosition.x,
			e.GetY() - LastMousePosition.y
		};

		offset *= Runtime::conf.MOUSE_SENSITIVITY;

		float yaw = Runtime::EditorContext.ActiveCamera->GetYaw() + offset.x;
		float pitch = Runtime::EditorContext.ActiveCamera->GetPitch() + offset.y;

		Runtime::EditorContext.ActiveCamera->SetRotation({ pitch, yaw, cameraRotation.z });
	}

	LastMousePosition = glm::vec2(e.GetX(), e.GetY());
	IsInitFrame = false;

	return false;
}