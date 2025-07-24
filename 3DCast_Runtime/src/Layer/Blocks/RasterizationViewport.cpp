#include "RasterizationViewport.h"

#include "Config.h"
#include "Data/SharedEditorData.h"

#include <3DCast.h>
#include <3DCast/Data/GlobalShared.h>

#include <imgui.h>
#include <imgui_internal.h>

#include "3DCast/Event/MouseEvent.h"

Runtime::RasterizationViewport::RasterizationViewport(Cast::Layer* parent)
	: Viewport(parent)
{
}

void Runtime::RasterizationViewport::Init()
{
	constexpr int MaxIndices = 1000000;
	constexpr size_t DefaultStorageSize = sizeof(Cast::Memory::BatchVertex) * 1000000;
	Cast::Memory::BatchMemoryHandler.Init(DefaultStorageSize, MaxIndices);

	PipelineData.GBufferScreenGeometry.reset(
		API::Advanced::GBufferScreenGeometry::Create(conf.WIN_WIDTH, conf.WIN_HEIGHT));
	PipelineData.Framebuffer.reset(
		API::Core::Framebuffer::Create(glm::ivec2(conf.WIN_WIDTH, conf.WIN_HEIGHT)));
	PipelineData.GBuffer.reset(API::Advanced::GBuffer::Create(conf.WIN_WIDTH, conf.WIN_HEIGHT));

	PipelineData.GBuffer->Bind();
	PipelineData.GBuffer->AddRenderTarget("Position", 3, API::Core::BufferDataType::_FLOAT,
	                                      API::Core::WrapMethod::CLAMP_TO_EDGE);
	PipelineData.GBuffer->AddRenderTarget("Normal", 3, API::Core::BufferDataType::_FLOAT,
	                                      API::Core::WrapMethod::CLAMP_TO_EDGE);
	PipelineData.GBuffer->AddRenderTarget("Albedo", 3, API::Core::BufferDataType::_FLOAT,
	                                      API::Core::WrapMethod::CLAMP_TO_EDGE);
	PipelineData.GBuffer->AddRenderTarget("Specular", 3, API::Core::BufferDataType::_FLOAT,
	                                      API::Core::WrapMethod::CLAMP_TO_EDGE);
	PipelineData.GBuffer->AddRenderTarget("Shine_Reflectance", 2, API::Core::BufferDataType::_FLOAT16,
	                                      API::Core::WrapMethod::CLAMP_TO_EDGE);

	PipelineData.GBuffer->AddDepthTarget(API::Core::DepthBufferType::WRITE_ONLY);
	PipelineData.GBuffer->AddStencilTarget();
	PipelineData.GBuffer->Validate();

	PipelineData.GBuffer->BindDepthTexture(0);
	PipelineData.GBuffer->BindTextures(1);

	CompileShaders();

	const Cast::Ref<API::Core::Shader> shader = Cast::AssetCache.GetShaderHandle("shading_pass");
	shader->Bind();
	shader->SetUniform1i("gBuf_Position", (int)PipelineData.GBuffer->GetTargetBoundTextureSlot("Position"));
	shader->SetUniform1i("gBuf_Normal", (int)PipelineData.GBuffer->GetTargetBoundTextureSlot("Normal"));
	shader->SetUniform1i("gBuf_Albedo", (int)PipelineData.GBuffer->GetTargetBoundTextureSlot("Albedo"));
	shader->SetUniform1i("gBuf_Specular", (int)PipelineData.GBuffer->GetTargetBoundTextureSlot("Specular"));
	shader->SetUniform1i("gBuf_Shine_Reflectance",
	                     (int)PipelineData.GBuffer->GetTargetBoundTextureSlot("Shine_Reflectance"));
	shader->Unbind();

	//EditorContext.Skybox.AddCubemap("cartoon_day", std::string(ASSET_DIR) + "img/cubemap/cartoon_day", ".png");
	//EditorContext.Skybox.AddCubemap("cartoon_redsky", std::string(ASSET_DIR) + "img/cubemap/cartoon_redsky", ".png");
	//EditorContext.Skybox.AddCubemap("test", std::string(ASSET_DIR) + "img/cubemap/test", ".png");
	//EditorContext.Skybox.AddCubemap("cartoon_clouds", std::string(ASSET_DIR) + "img/cubemap/cartoon_clouds", ".png");
	EditorContext.Skybox.AddCubemap("cartoon_clear", std::string(ASSET_DIR) + "img/cubemap/cartoon_clear", ".png");
	EditorContext.Skybox.AddCubemap("cartoon_evening", std::string(ASSET_DIR) + "img/cubemap/cartoon_evening", ".png");
	EditorContext.Skybox.SetActiveCubemap("cartoon_clear");
	EditorContext.Skybox.SetCubemapShaderCache(Cast::AssetCache.GetShaderHandle("cubemap"));
}

void Runtime::RasterizationViewport::Destroy()
{
}

void Runtime::RasterizationViewport::OnUpdate(Cast::Timestep ts, const bool hasCameraChanged)
{
	static bool initCameraRotation = true;
	if (Cast::Input::IsMouseButtonPressed(CAST_MOUSE_BUTTON_LEFT) && IsMainComponentHovered)
	{
		if (initCameraRotation)
		{
			ParentLayer->GetParentWindow()->SetInputModeDisabled();
			IsCameraRotating = true;
			initCameraRotation = false;
			IsCameraInitFrame = true;
		}
	}
	else
	{
		ParentLayer->GetParentWindow()->SetInputModeNormal();
		IsCameraRotating = false;
		initCameraRotation = true;
	}

	if (Runtime::EditorContext.ActiveCamera->HasChanged(0))
	{
		UpdateCameraUniforms();
	}
}

void Runtime::RasterizationViewport::OnEvent(Cast::Event& e)
{
	Cast::EventDispatcher dispatcher(e);
	dispatcher.Dispatch<Cast::MouseMovedEvent>(CAST_BIND_EVENT_FUNC(RasterizationViewport::OnMouseMoved));
}

void Runtime::RasterizationViewport::OnImGuiRender()
{
	if (SetPositionOnNextDraw)
	{
		ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_Always);
		SetPositionOnNextDraw = false;
	}

	if (IsMainComponentHovered)
		ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoMove);
	else
		ImGui::Begin("Viewport", nullptr);

	ImVec2 viewportSize = ImGui::GetContentRegionAvail();
	static auto lastViewportSize = ImVec2(0, 0);
	if (viewportSize.x != lastViewportSize.x || viewportSize.y != lastViewportSize.y)
	{
		lastViewportSize = viewportSize;

		//Framebuffer->Resize({ static_cast<uint32_t>(viewportSize.x), static_cast<uint32_t>(viewportSize.y) });

		switch (EditorContext.ActiveCamera->GetType())
		{
		case Cast::Renderer::Camera::Type::Orthographic:
			((Cast::Renderer::OrthographicCamera*)EditorContext.ActiveCamera.get())->SetFrustumOnResized(
				lastViewportSize.x, lastViewportSize.y);
			break;
		case Cast::Renderer::Camera::Type::Perspective:
			((Cast::Renderer::PerspectiveCamera*)EditorContext.ActiveCamera.get())->SetAspectRatio(
				lastViewportSize.x / lastViewportSize.y);
		}
	}

	const uint32_t textureID = PipelineData.Framebuffer->GetColorAttachmentTextureID(0);
	ImGui::Image(textureID, lastViewportSize, ImVec2(0, 1), ImVec2(1, 0)); // Flip vertically
	IsMainComponentHovered = ImGui::IsItemHovered() && ImGui::GetCurrentWindow()->Name == std::string("Viewport");

	ImGui::SetItemAllowOverlap();

	ImVec2 regionAvail = ImGui::GetWindowContentRegionMin();
	regionAvail.x += 5;
	regionAvail.y += 5;

	ImGui::SetCursorPos(regionAvail);
	ImGui::Checkbox("Wireframe", &Wireframe);

	IsHovered = ImGui::IsWindowHovered() && ImGui::GetCurrentWindow()->Name == std::string("Viewport");
	IsFocused = ImGui::IsWindowFocused() && ImGui::GetCurrentWindow()->Name == std::string("Viewport");

	Size = {viewportSize.x, viewportSize.y};

#ifdef CAST_DESKTOP_WAYLAND
	auto viewportAbsPos = glm::vec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
	glm::vec2 applicationAbsPos = ParentLayer->GetParentWindow()->GetPosition();

	Position = viewportAbsPos; // ImGui positions are already relative to the application window -> misleading signature
#else
	glm::vec2 viewportAbsPos = glm::vec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
	glm::vec2 applicationAbsPos = (glm::vec2)ParentLayer->GetParentWindow()->GetPosition();

	Position = viewportAbsPos - applicationAbsPos;
#endif

	Cast::Shared.WindowCenter = {
		applicationAbsPos.x + (float)ParentLayer->GetParentWindow()->GetWidth() * 0.5f,
		applicationAbsPos.y + (float)ParentLayer->GetParentWindow()->GetHeight() * 0.5f
	};

	RelativeMousePosition = {ImGui::GetMousePos().x - viewportAbsPos.x, ImGui::GetMousePos().y - viewportAbsPos.y};

	ImGui::End();
}

void Runtime::RasterizationViewport::OnRender()
{

	Cast::Renderer::RendererContext::BeginScene(*EditorContext.ActiveCamera);

	RenderGeometryPass();
	API::Core::RenderCommand::CopyStencilBuffer(PipelineData.GBuffer->GetInternalId(),
	                                            PipelineData.Framebuffer->GetInternalId(), (int)conf.WIN_WIDTH,
	                                            (int)conf.WIN_HEIGHT);
	RenderLightingPass();
	API::Core::RenderCommand::CopyDepthBuffer(PipelineData.GBuffer->GetInternalId(),
	                                          PipelineData.Framebuffer->GetInternalId(), (int)conf.WIN_WIDTH,
	                                          (int)conf.WIN_HEIGHT);
	RenderForwardPass();

	Cast::Renderer::RendererContext::EndScene();

}

void Runtime::RasterizationViewport::RenderGeometryPass() const
{
	// Color for bleeding areas
	API::Core::RenderCommand::SetClearColor({0.1f, 0.9f, 0.1f, 1.0f});
	API::Core::RenderCommand::SetDepthTest(true);
	API::Core::RenderCommand::SetDepthTestFunc(API::Core::DepthFunction::Less);
	API::Core::RenderCommand::CullFace(API::Core::Face::Back);

	PipelineData.GBuffer->BindAndClear();
	API::Core::RenderCommand::ClearStencilBuffer();
	API::Core::RenderCommand::EnableStencilTestWithConstant(0xFF);

	static Cast::Ref<API::Core::Shader> shader = Cast::AssetCache.GetShaderHandle("geometry_pass");
	shader->Bind();
	shader->SetUniform1f("u_ParallaxScale", EditorContext.ViewSettings.ParallaxScale);

	if (Wireframe)
		API::Core::RenderCommand::SetWireframeMode(true);

	Cast::Shared.ActiveScene->OnDeferredRender();

	API::Core::RenderCommand::SetWireframeMode(false);

	PipelineData.GBuffer->Unbind();
}

void Runtime::RasterizationViewport::RenderLightingPass() const
{
	PipelineData.Framebuffer->BindAndClear();
	PipelineData.GBuffer->BindDepthTexture(0);
	PipelineData.GBuffer->BindTextures(1);

	Cast::Shared.ActiveScene->BindSSBOforShadingPass();

	// Lighting Pass Uniforms
	const Cast::Ref<API::Core::Shader> shader = Cast::AssetCache.GetShaderHandle("shading_pass");
	shader->Bind();
	shader->SetUniform2f("u_Resolution", (float)conf.WIN_WIDTH, (float)conf.WIN_HEIGHT);

	API::Core::RenderCommand::SetDepthTestFunc(API::Core::DepthFunction::Less);

	// Viewport Background Color
	API::Core::RenderCommand::SetClearColor(EditorContext.Skybox.GetClearColor());
	API::Core::RenderCommand::Clear();

	PipelineData.Framebuffer->Bind();
	API::Core::RenderCommand::SetDefaultStencilTest();

	PipelineData.GBufferScreenGeometry->Draw(shader.get());

	API::Core::RenderCommand::SetBlend(false);
	API::Core::RenderCommand::SetStencilTest(false);
}

void Runtime::RasterizationViewport::RenderForwardPass() const
{
	PipelineData.Framebuffer->Bind();

	API::Core::RenderCommand::SetDepthTestFunc(API::Core::DepthFunction::Less);
	Cast::Shared.ActiveScene->OnForwardRender();

	EditorContext.Skybox.BindCurrentCubemap(6);
	EditorContext.Skybox.Render();

	if (!Cast::Shared.ActiveScene->GetInRenderView())
	{
		API::Core::RenderCommand::SetBlend(true);
		API::Core::RenderCommand::SetBlendFunc(API::Core::BlendFunction::SrcAlpha, API::Core::BlendFunction::OneMinusSrcAlpha);
		API::Core::RenderCommand::CullFace(API::Core::Face::None);

		const Cast::Ref<API::Core::Shader> shader = Cast::AssetCache.GetShaderHandle("tile_grid");
		shader->Bind();
		API::Core::RenderCommand::IssueEmptyDrawCall(6);

		API::Core::RenderCommand::SetBlend(false);
	}

	PipelineData.Framebuffer->Unbind();
}

void Runtime::RasterizationViewport::CompileShaders()
{
	Cast::AssetCache.AddShader("geometry_pass",
	                           API::Core::Shader::Create(std::string(ASSET_DIR) + "shader/deferred/geometry_pass.vert",
	                                                     std::string(ASSET_DIR) +
	                                                     "shader/deferred/geometry_pass.frag"));
	Cast::AssetCache.AddShader("shading_pass",
	                           API::Core::Shader::Create(std::string(ASSET_DIR) + "shader/deferred/shading_pass.vert",
	                                                     std::string(ASSET_DIR) + "shader/deferred/shading_pass.frag"));
	Cast::AssetCache.AddShader("icon_billboard",
	                           API::Core::Shader::Create(std::string(ASSET_DIR) + "shader/sprite/icon.vert",
	                                                     std::string(ASSET_DIR) + "shader/sprite/icon.frag"));
	Cast::AssetCache.AddShader("tile_grid",
							   API::Core::Shader::Create(std::string(ASSET_DIR) + "shader/effect/tile_grid.vert",
														 std::string(ASSET_DIR) + "shader/effect/tile_grid.frag"));
	Cast::AssetCache.AddShader("cubemap",
							   API::Core::Shader::Create(std::string(ASSET_DIR) + "shader/world/cubemap.vert",
														 std::string(ASSET_DIR) + "shader/world/cubemap.frag"));
}

void Runtime::RasterizationViewport::UpdateCameraUniforms()
{
	const auto camPos = EditorContext.ActiveCamera->GetPosition();

	const auto iconShader = Cast::AssetCache.GetShaderHandle("icon_billboard");
	iconShader->Bind();
	iconShader->SetUniformMat4f("u_ViewProjection", EditorContext.ActiveCamera->GetViewProjectionMat());
	iconShader->SetUniform3f("u_CameraPos", camPos.x, camPos.y, camPos.z);

	const auto geometryShader = Cast::AssetCache.GetShaderHandle("geometry_pass");
	geometryShader->Bind();
	geometryShader->SetUniformMat4f("u_View", EditorContext.ActiveCamera->GetViewMat());
	geometryShader->SetUniformMat4f("u_Projection", EditorContext.ActiveCamera->GetProjectionMat());
	geometryShader->SetUniform3f("u_ViewPos", camPos.x, camPos.y, camPos.z);

	const Cast::Ref<API::Core::Shader> lightingShader = Cast::AssetCache.GetShaderHandle("shading_pass");
	lightingShader->Bind();
	lightingShader->SetUniform3f("u_ViewPosition", camPos.x, camPos.y, camPos.z);

	const Cast::Ref<API::Core::Shader> gridShader = Cast::AssetCache.GetShaderHandle("tile_grid");
	gridShader->Bind();
	gridShader->SetUniformMat4f("u_ViewProjection", EditorContext.ActiveCamera->GetViewProjectionMat());
	gridShader->SetUniform3f("u_CameraWorldPos", camPos.x, camPos.y, camPos.z);
}

bool Runtime::RasterizationViewport::OnMouseMoved(Cast::MouseMovedEvent& e)
{
	if (IsCameraRotating)
	{
		static glm::vec2 lastMousePos = {0.f, 0.f};
		if (!IsCameraInitFrame)
		{
#ifndef CAST_DESKTOP_WAYLAND
			glm::vec2 center = Position + Size * 0.5f;
			glm::vec2 offset = {
				e.GetX() - center.x,
				e.GetY() - center.y
			};

			ParentLayer->GetParentWindow()->SetCursorPosition(center.x, center.y); // Relative to application window
#else
			glm::vec2 offset = {
				e.GetX() - lastMousePos.x,
				e.GetY() - lastMousePos.y
			};
#endif

			offset *= conf.MOUSE_SENSITIVITY;
			float yaw = EditorContext.ActiveCamera->GetYaw() + offset.x;
			float pitch = glm::clamp(EditorContext.ActiveCamera->GetPitch() - offset.y, -89.99f, 89.99f);
			EditorContext.ActiveCamera->SetRotation({
				pitch, yaw, EditorContext.ActiveCamera->GetRoll()
			});
		}
		lastMousePos = {e.GetX(), e.GetY()};

		IsCameraInitFrame = false;
	}

	return false;
}
