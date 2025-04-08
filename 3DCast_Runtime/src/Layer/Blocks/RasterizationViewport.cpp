#include "RasterizationViewport.h"

#include "Config.h"
#include "Data/SharedEditorData.h"

#include <3DCast.h>
#include <3DCast/Data/GlobalShared.h>

#include <imgui.h>
#include <imgui_internal.h>

Runtime::RasterizationViewport::RasterizationViewport(Cast::Layer* parent)
	: Viewport(parent) {}

void Runtime::RasterizationViewport::Init()
{
	constexpr int MaxIndices = 1000000;
	constexpr size_t DefaultStorageSize = sizeof(Cast::Memory::BatchVertex) * 1000000;
	Cast::Memory::BatchMemoryHandler.Init(DefaultStorageSize, MaxIndices);

	PipelineData.GBufferScreenGeometry.reset(API::Advanced::GBufferScreenGeometry::Create(Runtime::conf.WIN_WIDTH, Runtime::conf.WIN_HEIGHT));
	PipelineData.Framebuffer.reset(API::Core::Framebuffer::Create(glm::ivec2(Runtime::conf.WIN_WIDTH, Runtime::conf.WIN_HEIGHT)));
	PipelineData.GBuffer.reset(API::Advanced::GBuffer::Create(Runtime::conf.WIN_WIDTH, Runtime::conf.WIN_HEIGHT));

	PipelineData.GBuffer->Bind();
	PipelineData.GBuffer->AddRenderTarget("Position", 3, API::Core::BufferDataType::_FLOAT, API::Core::WrapMethod::CLAMP_TO_EDGE);
	PipelineData.GBuffer->AddRenderTarget("Normal", 3, API::Core::BufferDataType::_FLOAT, API::Core::WrapMethod::CLAMP_TO_EDGE);
	PipelineData.GBuffer->AddRenderTarget("Albedo", 3, API::Core::BufferDataType::_FLOAT, API::Core::WrapMethod::CLAMP_TO_EDGE);
	PipelineData.GBuffer->AddRenderTarget("Specular", 3, API::Core::BufferDataType::_FLOAT, API::Core::WrapMethod::CLAMP_TO_EDGE);
	PipelineData.GBuffer->AddRenderTarget("Shine_Reflectance", 2, API::Core::BufferDataType::_FLOAT16, API::Core::WrapMethod::CLAMP_TO_EDGE);

	PipelineData.GBuffer->AddDepthTarget(API::Core::DepthBufferType::WRITE_ONLY);
	PipelineData.GBuffer->AddStencilTarget();
	PipelineData.GBuffer->Validate();

	PipelineData.GBuffer->BindDepthTexture(0);
	PipelineData.GBuffer->BindTextures(1);

	CompileShaders();

	Cast::Ref<API::Core::Shader> shader = Cast::AssetCache.GetShaderHandle("shader_shading_pass");
	shader->Bind();
	shader->SetUniform1i("gBuf_Position", PipelineData.GBuffer->GetTargetBoundTextureSlot("Position"));
	shader->SetUniform1i("gBuf_Normal", PipelineData.GBuffer->GetTargetBoundTextureSlot("Normal"));
	shader->SetUniform1i("gBuf_Albedo", PipelineData.GBuffer->GetTargetBoundTextureSlot("Albedo"));
	shader->SetUniform1i("gBuf_Specular", PipelineData.GBuffer->GetTargetBoundTextureSlot("Specular"));
	shader->SetUniform1i("gBuf_Shine_Reflectance", PipelineData.GBuffer->GetTargetBoundTextureSlot("Shine_Reflectance"));
	shader->Unbind();
}

void Runtime::RasterizationViewport::Destroy()
{
}

void Runtime::RasterizationViewport::OnUpdate(Cast::Timestep ts)
{
	static bool initCameraRotation = true;
	if (Cast::Input::IsMouseButtonPressed(CAST_MOUSE_BUTTON_LEFT) && IsHovered) {
		if (initCameraRotation) {
			ParentLayer->GetParentWindow()->SetInputModeDisabled();
			IsCameraRotating = true;
			initCameraRotation = false;
			IsInitFrame = true;
		}
	}
	else {
		ParentLayer->GetParentWindow()->SetInputModeNormal();
		IsCameraRotating = false;
		initCameraRotation = true;
	}

	auto iconShader = Cast::AssetCache.GetShaderHandle("icon_billboard");
	iconShader->Bind();
	iconShader->SetUniformMat4f("u_ViewProjection", Runtime::EditorContext.ActiveCamera->GetViewProjectionMat());
	glm::vec3 camPos = Runtime::EditorContext.ActiveCamera->GetPosition();
	iconShader->SetUniform3f("u_CameraPos", camPos.x, camPos.y, camPos.z);

	Cast::Shared.ActiveScene->OnUpdate();
}

void Runtime::RasterizationViewport::OnEvent(Cast::Event& e)
{
	Cast::EventDispatcher dispatcher(e);
	dispatcher.Dispatch<Cast::MouseMovedEvent>(CAST_BIND_EVENT_FUNC(RasterizationViewport::OnMouseMoved));
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

	uint32_t textureID = PipelineData.Framebuffer->GetColorAttachmentTextureID(0);
	ImGui::Image((unsigned long long)textureID, lastViewportSize, ImVec2(0, 1), ImVec2(1, 0)); // Flip vertically

	IsHovered = ImGui::IsWindowHovered() && ImGui::GetCurrentWindow()->Name == std::string("Viewport");
	IsFocused = ImGui::IsWindowFocused() && ImGui::GetCurrentWindow()->Name == std::string("Viewport");

	Size = { viewportSize.x, viewportSize.y };
	glm::vec2 viewportAbsPos = glm::vec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
	glm::vec2 applicationAbsPos = (glm::vec2)ParentLayer->GetParentWindow()->GetPosition();
	Position = viewportAbsPos - applicationAbsPos;
	Cast::Shared.WindowCenter = { applicationAbsPos.x + (float)ParentLayer->GetParentWindow()->GetWidth() * 0.5f, applicationAbsPos.y + (float)ParentLayer->GetParentWindow()->GetHeight() * 0.5f };

	RelativeMousePosition = { ImGui::GetMousePos().x - viewportAbsPos.x, ImGui::GetMousePos().y - viewportAbsPos.y };

	ImGui::End();
}

void Runtime::RasterizationViewport::OnRender()
{
	Cast::Renderer::RendererContext::BeginScene(*Runtime::EditorContext.ActiveCamera);

	RenderGeometryPass();
	API::Core::RenderCommand::CopyStencilBuffer(PipelineData.GBuffer->GetInternalId(), PipelineData.Framebuffer->GetInternalId(), Runtime::conf.WIN_WIDTH, Runtime::conf.WIN_HEIGHT);
	RenderLightingPass();
	API::Core::RenderCommand::CopyDepthBuffer(PipelineData.GBuffer->GetInternalId(), PipelineData.Framebuffer->GetInternalId(), Runtime::conf.WIN_WIDTH, Runtime::conf.WIN_HEIGHT);
	RenderForwardPass();

	Cast::Renderer::RendererContext::EndScene();
}

void Runtime::RasterizationViewport::RenderGeometryPass()
{
	// Color for bleeding areas
	API::Core::RenderCommand::SetClearColor({ 0.1f, 0.9f, 0.1f, 1.0f });
	API::Core::RenderCommand::SetDepthTest(true);
	API::Core::RenderCommand::SetDepthTestFunc(API::Core::DepthFunction::Less);
	API::Core::RenderCommand::CullFace(API::Core::Face::Back);

	PipelineData.GBuffer->BindAndClear();
	API::Core::RenderCommand::ClearStencilBuffer();
	API::Core::RenderCommand::EnableStencilTestWithConstant(0xFF);

	static Cast::Ref<API::Core::Shader> shader = Cast::AssetCache.GetShaderHandle("shader_geometry_pass");
	shader->Bind();
	glm::vec3 camPos = Runtime::EditorContext.ActiveCamera->GetPosition();
	shader->SetUniform3f("u_ViewPos", camPos.x, camPos.y, camPos.z);
	shader->SetUniform1f("u_ParallaxScale", Runtime::EditorContext.ViewSettings.ParallaxScale);

	Cast::Shared.ActiveScene->OnDeferredRender();
	PipelineData.GBuffer->Unbind();
}

void Runtime::RasterizationViewport::RenderLightingPass()
{
	PipelineData.Framebuffer->BindAndClear();
	PipelineData.GBuffer->BindDepthTexture(0);
	PipelineData.GBuffer->BindTextures(1);

	Cast::Shared.ActiveScene->BindSSBOforShadingPass();

	// Lighting Pass Uniforms
	Cast::Ref<API::Core::Shader> shader = Cast::AssetCache.GetShaderHandle("shader_shading_pass");
	shader->Bind();
	shader->SetUniform2f("u_Resolution", (float)Runtime::conf.WIN_WIDTH, (float)Runtime::conf.WIN_HEIGHT);
	const glm::vec3& pos = Runtime::EditorContext.ActiveCamera->GetPosition();
	shader->SetUniform3f("u_ViewPosition", pos.x, pos.y, pos.z);

	API::Core::RenderCommand::SetDepthTestFunc(API::Core::DepthFunction::Less);

	// Viewport Background Color
	API::Core::RenderCommand::SetClearColor({ 0.10f, 0.10f, 0.10f, 1.0f });
	API::Core::RenderCommand::Clear();

	PipelineData.Framebuffer->Bind();
	API::Core::RenderCommand::SetDefaultStencilTest();

	PipelineData.GBufferScreenGeometry->Draw(Cast::AssetCache.GetShaderHandle("shader_shading_pass").get());

	API::Core::RenderCommand::SetBlend(false);
	API::Core::RenderCommand::SetStencilTest(false);
}

void Runtime::RasterizationViewport::RenderForwardPass()
{
	PipelineData.Framebuffer->Bind();
	API::Core::RenderCommand::SetDepthTestFunc(API::Core::DepthFunction::Less);

	Cast::Shared.ActiveScene->OnForwardRender();
	PipelineData.Framebuffer->Unbind();
}

void Runtime::RasterizationViewport::CompileShaders()
{
	Cast::AssetCache.AddShader("shader_geometry_pass", API::Core::Shader::Create("../3DCast/ressources/shader/deferred/geometry_pass.vert", "../3DCast/ressources/shader/deferred/geometry_pass.frag"));
	Cast::AssetCache.AddShader("shader_shading_pass", API::Core::Shader::Create("../3DCast/ressources/shader/deferred/shading_pass.vert", "../3DCast/ressources/shader/deferred/shading_pass.frag"));
	Cast::AssetCache.AddShader("icon_billboard", API::Core::Shader::Create("../3DCast/ressources/shader/sprite/icon.vert", "../3DCast/ressources/shader/sprite/icon.frag"));
}

bool Runtime::RasterizationViewport::OnMouseMoved(Cast::MouseMovedEvent& e)
{
	if (IsCameraRotating) {
		glm::vec2 center = Position + Size * 0.5f;

		if (!IsInitFrame) {
			glm::vec2 offset = {
				e.GetX() - center.x,
				e.GetY() - center.y
			};

			offset *= Runtime::conf.MOUSE_SENSITIVITY;

			float yaw = Runtime::EditorContext.ActiveCamera->GetYaw() + offset.x;
			float pitch = glm::clamp(Runtime::EditorContext.ActiveCamera->GetPitch() - offset.y, -89.99f, 89.99f);

			Runtime::EditorContext.ActiveCamera->SetRotation({ pitch, yaw, Runtime::EditorContext.ActiveCamera->GetRoll() });
		}

		ParentLayer->GetParentWindow()->SetCursorPosition(center.x, center.y); // Relative to application window
		IsInitFrame = false;
	}

	return false;
}