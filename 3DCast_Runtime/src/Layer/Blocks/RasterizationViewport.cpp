#include "RasterizationViewport.h"

#include "Config.h"
#include "Data/SharedEditorData.h"

#include <3DCast.h>

#include <imgui.h>
#include <imgui_internal.h>

Runtime::RasterizationViewport::RasterizationViewport(Cast::Layer* parent)
	: Viewport(parent) {}

void Runtime::RasterizationViewport::Init()
{
	constexpr int MaxIndices = 1000;
	constexpr size_t DefaultStorageSize = sizeof(Cast::Memory::BatchVertex) * 1000;
	Cast::Memory::BatchMemoryHandler.Init(DefaultStorageSize, MaxIndices);

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
	RenderPipelineData.GBuffer->AddStencilTarget();
	RenderPipelineData.GBuffer->Validate();

	RenderPipelineData.GBuffer->BindDepthTexture(0);
	RenderPipelineData.GBuffer->BindTextures(1);

	CompileShaders();

	Cast::Ref<API::Core::Shader> shader = Cast::AssetCache.GetShaderHandle("shader_shading_pass");
	shader->Bind();
	shader->SetUniform1i("gBuf_Position", RenderPipelineData.GBuffer->GetTargetBoundTextureSlot("Position"));
	shader->SetUniform1i("gBuf_Normal", RenderPipelineData.GBuffer->GetTargetBoundTextureSlot("Normal"));
	shader->SetUniform1i("gBuf_Albedo", RenderPipelineData.GBuffer->GetTargetBoundTextureSlot("Albedo"));
	shader->SetUniform1i("gBuf_Specular", RenderPipelineData.GBuffer->GetTargetBoundTextureSlot("Specular"));
	shader->SetUniform1i("gBuf_Shine_Reflectance", RenderPipelineData.GBuffer->GetTargetBoundTextureSlot("Shine_Reflectance"));
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

	Runtime::EditorContext.ActiveScene->OnUpdate();
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

	uint32_t textureID = RenderPipelineData.Framebuffer->GetColorAttachmentTextureID(0);
	ImGui::Image((unsigned long long)textureID, lastViewportSize, ImVec2(0, 1), ImVec2(1, 0)); // Flip vertically

	IsHovered = ImGui::IsWindowHovered() && ImGui::GetCurrentWindow()->Name == std::string("Viewport");
	IsFocused = ImGui::IsWindowFocused() && ImGui::GetCurrentWindow()->Name == std::string("Viewport");

	Size = { viewportSize.x, viewportSize.y };
	glm::vec2 viewportAbsPos = glm::vec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
	glm::vec2 applicationAbsPos = (glm::vec2)ParentLayer->GetParentWindow()->GetPosition();
	Position = viewportAbsPos - applicationAbsPos;

	RelativeMousePosition = { ImGui::GetMousePos().x - viewportAbsPos.x, ImGui::GetMousePos().y - viewportAbsPos.y };

	ImGui::End();
}

void Runtime::RasterizationViewport::OnRender()
{
	Cast::Renderer::RendererContext::BeginScene(*Runtime::EditorContext.ActiveCamera);

	RenderGeometryPass();
	API::Core::RenderCommand::CopyStencilBuffer(RenderPipelineData.GBuffer->GetInternalId(), RenderPipelineData.Framebuffer->GetInternalId(), Runtime::conf.WIN_WIDTH, Runtime::conf.WIN_HEIGHT);
	RenderLightingPass();
	API::Core::RenderCommand::CopyDepthBuffer(RenderPipelineData.GBuffer->GetInternalId(), RenderPipelineData.Framebuffer->GetInternalId(), Runtime::conf.WIN_WIDTH, Runtime::conf.WIN_HEIGHT);
	RenderForwardPass();

	Cast::Renderer::RendererContext::EndScene();
}

void Runtime::RasterizationViewport::RenderGeometryPass()
{
	API::Core::RenderCommand::SetClearColor({ 0.1f, 0.9f, 0.1f, 1.0f });
	API::Core::RenderCommand::SetDepthTest(true);
	API::Core::RenderCommand::SetDepthTestFunc(API::Core::DepthFunction::Less);
	API::Core::RenderCommand::CullFace(API::Core::Face::Back);

	RenderPipelineData.GBuffer->BindAndClear();
	API::Core::RenderCommand::ClearStencilBuffer();
	API::Core::RenderCommand::EnableStencilTestWithConstant(0xFF);

	Runtime::EditorContext.ActiveScene->OnDeferredRender();
	RenderPipelineData.GBuffer->Unbind();
}

void Runtime::RasterizationViewport::RenderLightingPass()
{
	RenderPipelineData.Framebuffer->BindAndClear();
	RenderPipelineData.GBuffer->BindDepthTexture(0);
	RenderPipelineData.GBuffer->BindTextures(1);

	Runtime::EditorContext.ActiveScene->BindSSBOforShadingPass();

	// Lighting Pass Uniforms
	Cast::Ref<API::Core::Shader> shader = Cast::AssetCache.GetShaderHandle("shader_shading_pass");
	shader->Bind();
	shader->SetUniform2f("u_Resolution", (float)Runtime::conf.WIN_WIDTH, (float)Runtime::conf.WIN_HEIGHT);
	const glm::vec3& pos = Runtime::EditorContext.ActiveCamera->GetPosition();
	shader->SetUniform3f("u_ViewPosition", pos.x, pos.y, pos.z);

	API::Core::RenderCommand::SetDepthTestFunc(API::Core::DepthFunction::Less);

	//GUI Background Color
	API::Core::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
	API::Core::RenderCommand::Clear();

	RenderPipelineData.Framebuffer->Bind();
	API::Core::RenderCommand::SetDefaultStencilTest();

	RenderPipelineData.GBufferScreenGeometry->Draw(Cast::AssetCache.GetShaderHandle("shader_shading_pass").get());

	API::Core::RenderCommand::SetBlend(false);
	API::Core::RenderCommand::SetStencilTest(false);
}

void Runtime::RasterizationViewport::RenderForwardPass()
{
	RenderPipelineData.Framebuffer->Bind();
	API::Core::RenderCommand::SetDepthTestFunc(API::Core::DepthFunction::Less);

	Runtime::EditorContext.ActiveScene->OnForwardRender();
	RenderPipelineData.Framebuffer->Unbind();
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