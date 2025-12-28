#include "RasterizationViewport.h"

#include "Config.h"
#include "Data/SharedEditorData.h"
#include "Application/KeymapLayout.h"
#include "GUI/Panels/SceneHierarchyPanel.h"

#include <3DCast.h>
#include <3DCast/Data/GlobalShared.h>
#include <3DCast/Event/MouseEvent.h>

#include <vendor/glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <imgui_internal.h>
#include <ImGuizmo.h>
#include <entt/entt.hpp>

Runtime::RasterizationViewport::RasterizationViewport(Cast::Layer* parent)
	: Viewport(parent)
{
}

void Runtime::RasterizationViewport::Init()
{
	Cast::Memory::BatchMemoryHandler.Init(sizeof(Cast::Memory::BatchVertexShaderObject) * MAX_BATCH_VERTICES, MAX_BATCH_INDICES);

	CompileShaders();
	BuildViewport({conf.WIN_WIDTH, conf.WIN_HEIGHT});

	const Cast::Ref<API::Core::Shader> shader = Cast::ShaderCacheRegistryInstance.GetHandle("shading_pass");
	shader->Bind();

	// Cascaded shadow maps
	constexpr std::array<glm::ivec2, API::Advanced::CascadedShadowMap::CascadeCount> csmSizes = {
		glm::ivec2(2048, 2048),
		glm::ivec2(1024, 1024),
		glm::ivec2(512, 512)
	};
	PipelineData.ShadowProcessor = Cast::CreateRef<Cast::Renderer::CSMProcessor>();
	PipelineData.ShadowProcessor->Init(csmSizes);

	shader->SetUniform1i("u_ShadowMap[0]", 10);
	shader->SetUniform1i("u_ShadowMap[1]", 11);
	shader->SetUniform1i("u_ShadowMap[2]", 12);
	shader->Unbind();

	// SSAO
	const Cast::Ref<API::Core::Shader> ssaoBlurShader = Cast::ShaderCacheRegistryInstance.GetHandle("ssao_blur");
	ssaoBlurShader->Bind();
	ssaoBlurShader->SetUniform1i("ssaoInput", 0);
	ssaoBlurShader->Unbind();

	//EditorContext.Skybox.AddCubemap("cartoon_day", std::string(ASSET_DIR) + "img/cubemap/cartoon_day", ".png");
	//EditorContext.Skybox.AddCubemap("cartoon_redsky", std::string(ASSET_DIR) + "img/cubemap/cartoon_redsky", ".png");
	//EditorContext.Skybox.AddCubemap("test", std::string(ASSET_DIR) + "img/cubemap/test", ".png");
	//EditorContext.Skybox.AddCubemap("cartoon_clouds", std::string(ASSET_DIR) + "img/cubemap/cartoon_clouds", ".png");
	EditorContext.Skybox.AddCubemap("cartoon_clear", std::string(ASSET_DIR) + "img/cubemap/cartoon_clear", ".png");
	EditorContext.Skybox.AddCubemap("cartoon_evening", std::string(ASSET_DIR) + "img/cubemap/cartoon_evening", ".png");
	EditorContext.Skybox.SetActiveCubemap("cartoon_clear");
	EditorContext.Skybox.SetCubemapShaderCache(Cast::ShaderCacheRegistryInstance.GetHandle("cubemap"));
}

void Runtime::RasterizationViewport::Destroy()
{
	API::Core::RenderCommand::UnbindAllFramebuffers();
	API::Core::RenderCommand::UnbindShaderProgram();
	API::Core::RenderCommand::UnbindAllTextures(32);
	API::Core::RenderCommand::UnbindRenderbuffer();

	// Destroy SSAO resources
	PipelineData.SSAOProcessor.reset();
	PipelineData.SSAOBlurFramebuffer.reset();
	PipelineData.SSAOFramebuffer.reset();

	// Destroy GBuffer resources
	PipelineData.GBuffer.reset();
	PipelineData.GBufferScreenGeometry.reset();
	PipelineData.Framebuffer.reset();

	API::Core::RenderCommand::ForceSync();
}

std::array<ImVec2, 2> Runtime::RasterizationViewport::GetViewportBounds() const
{
	const auto topLeft = ImVec2(Position.x, Position.y);
	const auto bottomRight = ImVec2(Position.x + Size.x, Position.y + Size.y);
	return {topLeft, bottomRight};
}

void Runtime::RasterizationViewport::OnUpdate(Cast::Timestep ts, const bool hasCameraChanged)
{
	static bool initCameraRotation = true;

	if (Cast::Input::IsMouseButtonPressed(CAST_MOUSE_BUTTON_LEFT) && IsMainComponentHovered && !IsUsingGizmo())
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

	if (Runtime::EditorContext.ActiveCamera.value()->HasChanged(0))
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

	constexpr char windowName[] = ICON_FA_IMAGE " Raster-Viewport";

	auto aspectRatioConstraint = [](ImGuiSizeCallbackData* data) {
		const float aspect = *static_cast<float*>(data->UserData);
		const float titleBarHeight = ImGui::GetFrameHeight();
		const float paddingY = ImGui::GetStyle().WindowPadding.y * 2.0f;
		const float paddingX = ImGui::GetStyle().WindowPadding.x * 2.0f;

		const float contentHeight = data->DesiredSize.y - titleBarHeight - paddingY;
		const float contentWidth = contentHeight * aspect;
		data->DesiredSize.x = contentWidth + paddingX;
	};

	float aspectCopy = CachedAspectRatio;
	ImGui::SetNextWindowSizeConstraints(
		ImVec2(100, 100),
		ImVec2(FLT_MAX, FLT_MAX),
		aspectRatioConstraint,
		&aspectCopy
	);

	if (IsMainComponentHovered)
		ImGui::Begin(windowName, nullptr, ImGuiWindowFlags_NoMove);
	else
		ImGui::Begin(windowName, nullptr);

	ImVec2 viewportSize = ImGui::GetContentRegionAvail();

	float displayWidth = viewportSize.y * CachedAspectRatio;
	float displayHeight = viewportSize.y;

	if (displayWidth > viewportSize.x)
	{
		displayWidth = viewportSize.x;
		displayHeight = displayWidth / CachedAspectRatio;
	}

	const ImVec2 displaySize(displayWidth, displayHeight);

	const ImVec2 contentMin = ImGui::GetCursorScreenPos();
	ImageDisplayMin = contentMin;
	ImageDisplayMax = ImVec2(contentMin.x + displayWidth, contentMin.y + displayHeight);

	static auto lastViewportSize = ImVec2(0, 0);

	if (viewportSize.x != lastViewportSize.x || viewportSize.y != lastViewportSize.y)
	{
		lastViewportSize = viewportSize;

		switch (EditorContext.ActiveCamera.value()->GetType())
		{
		case Cast::Renderer::Camera::Type::Orthographic:
			dynamic_cast<Cast::Renderer::OrthographicCamera*>(EditorContext.ActiveCamera.value())->SetFrustumOnResized(
				displayWidth, displayHeight);
			break;
		case Cast::Renderer::Camera::Type::Perspective:
			dynamic_cast<Cast::Renderer::PerspectiveCamera*>(EditorContext.ActiveCamera.value())->SetAspectRatio(
				displayWidth / displayHeight);
		}
	}

	const uint32_t textureID = PipelineData.Framebuffer->GetColorAttachmentTextureID(0);
	ImGui::Image(textureID, displaySize, ImVec2(0, 1), ImVec2(1, 0)); // Flip vertically
	const bool isCurrentlyHovered = ImGui::IsItemHovered() &&
						 ImGui::GetCurrentWindow()->Name == std::string(windowName);

	if (!IsCameraRotating)
		IsMainComponentHovered = isCurrentlyHovered;
	else if (isCurrentlyHovered)
		IsMainComponentHovered = true;

	ImGui::SetItemAllowOverlap();

	ImVec2 regionAvail = ImGui::GetWindowContentRegionMin();
	regionAvail.x += 5;
	regionAvail.y += 5;

	ImGui::SetCursorPos(regionAvail);
	ImGui::Checkbox(ICON_FA_DRAW_POLYGON " Wireframe", &Wireframe);

	IsHovered = ImGui::IsWindowHovered() && ImGui::GetCurrentWindow()->Name == std::string(windowName);
	IsFocused = ImGui::IsWindowFocused() && ImGui::GetCurrentWindow()->Name == std::string(windowName);

	Size = {viewportSize.x, viewportSize.y};

#ifdef CAST_DESKTOP_WAYLAND
	const auto viewportAbsPos = glm::vec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
	const glm::vec2 applicationAbsPos = ParentLayer->GetParentWindow()->GetPosition();

	Position = viewportAbsPos; // ImGui positions are already relative to the application window -> misleading signature
#else
	glm::vec2 viewportAbsPos = glm::vec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
	glm::vec2 applicationAbsPos = (glm::vec2)ParentLayer->GetParentWindow()->GetPosition();

	Position = viewportAbsPos - applicationAbsPos;
#endif

	Cast::Shared.WindowCenter = {
		applicationAbsPos.x + static_cast<float>(ParentLayer->GetParentWindow()->GetWidth()) * 0.5f,
		applicationAbsPos.y + static_cast<float>(ParentLayer->GetParentWindow()->GetHeight()) * 0.5f
	};

	RelativeMousePosition = {ImGui::GetMousePos().x - viewportAbsPos.x, ImGui::GetMousePos().y - viewportAbsPos.y};

	RenderGizmos();

	ImGui::End();
}

void Runtime::RasterizationViewport::OnRender()
{
	Cast::Renderer::RendererContext::BeginScene(*EditorContext.ActiveCamera.value());

	RenderShadowPassCSM();

	RenderGeometryPass();

	if (EditorContext.ViewSettings.SSAOEnabled) {
		RenderSSAOPass();
		RenderSSAOBlurPass();
	}

	RenderLightingPass();
	API::Core::RenderCommand::CopyDepthBuffer(PipelineData.GBuffer->GetInternalId(),
	                                          PipelineData.Framebuffer->GetInternalId(), RenderedSize.x,
	                                          RenderedSize.y);
	RenderForwardPass();

	Cast::Renderer::RendererContext::EndScene();
}

void Runtime::RasterizationViewport::Resize(const glm::vec2& size)
{
	if (size.x <= 0 || size.y <= 0) return;

	RenderedSize = size;
	CachedAspectRatio = size.x / size.y;

	// Resize all framebuffers in-place
	PipelineData.Framebuffer->Resize(size);
	PipelineData.GBuffer->Resize(static_cast<unsigned int>(size.x), static_cast<unsigned int>(size.y));
	PipelineData.SSAOFramebuffer->Resize(size);
	PipelineData.SSAOBlurFramebuffer->Resize(size);

	// Apply texture filter mode to the main viewport framebuffer
	const unsigned int glFilter = (EditorContext.ViewSettings.FilterMode == ViewportFilterMode::Nearest)
		? GL_NEAREST : GL_LINEAR;
	PipelineData.Framebuffer->SetColorAttachmentFilter(0, glFilter);

	PipelineData.SSAOProcessor->GenerateSSAONoiseMap();

	// Update SSAO shader uniforms with new screen size
	const Cast::Ref<API::Core::Shader> ssaoShader = Cast::ShaderCacheRegistryInstance.GetHandle("ssao");
	ssaoShader->Bind();
	ssaoShader->SetUniform2f("screenSize", static_cast<float>(size.x), static_cast<float>(size.y));
	ssaoShader->Unbind();

	// Update camera aspect ratio
	if (EditorContext.ActiveCamera.has_value())
	{
		switch (EditorContext.ActiveCamera.value()->GetType())
		{
		case Cast::Renderer::Camera::Type::Orthographic:
			dynamic_cast<Cast::Renderer::OrthographicCamera*>(EditorContext.ActiveCamera.value())->SetFrustumOnResized(
				static_cast<float>(size.x), static_cast<float>(size.y));
			break;
		case Cast::Renderer::Camera::Type::Perspective:
			{
				auto* perspCam = dynamic_cast<Cast::Renderer::PerspectiveCamera*>(EditorContext.ActiveCamera.value());
				if (perspCam) {
					const float newAspect = static_cast<float>(size.x) / static_cast<float>(size.y);
					perspCam->SetAspectRatio(newAspect);
				}
			}
			break;
		}
	}
}

void Runtime::RasterizationViewport::RenderShadowPassCSM() const
{
	if (!PipelineData.ShadowProcessor)
		return;

	PipelineData.ShadowProcessor->OnRender(*Cast::Shared.ActiveScene, *Runtime::EditorContext.ActiveCamera.value());
}

void Runtime::RasterizationViewport::RenderGeometryPass() const
{
	UpdateCameraUniforms();
	API::Core::RenderCommand::SetClearColor({0.0f, 0.0f, 0.0f, 1.0f});
	API::Core::RenderCommand::SetDepthTest(true);
	API::Core::RenderCommand::SetDepthTestFunc(API::Core::DepthFunction::Less);
	API::Core::RenderCommand::CullFace(API::Core::Face::Back);

	PipelineData.GBuffer->BindAndClear();
	API::Core::RenderCommand::ClearStencilBuffer();
	API::Core::RenderCommand::EnableStencilTestWithConstant(0xFF);

	static Cast::Ref<API::Core::Shader> shader = Cast::ShaderCacheRegistryInstance.GetHandle("geometry_pass");
	shader->Bind();
	shader->SetUniform1f("u_ParallaxScale", EditorContext.ViewSettings.ParallaxScale);

	if (Wireframe)
		API::Core::RenderCommand::SetWireframeMode(true);

	Cast::Shared.ActiveScene->OnDeferredRender();

	API::Core::RenderCommand::SetWireframeMode(false);

	PipelineData.GBuffer->Unbind();
}

void Runtime::RasterizationViewport::RenderSSAOPass() const
{
	// Disable depth and stencil for full-screen pass
	API::Core::RenderCommand::SetDepthTest(false);
	API::Core::RenderCommand::SetStencilTest(false);

	PipelineData.SSAOFramebuffer->BindAndClear();

	PipelineData.SSAOProcessor->BindNoiseTex(6);

	const Cast::Ref<API::Core::Shader> ssaoShader = Cast::ShaderCacheRegistryInstance.GetHandle("ssao");
	ssaoShader->Bind();
	ssaoShader->SetUniformMat4f("projection", EditorContext.ActiveCamera.value()->GetProjectionMat());
	ssaoShader->SetUniformMat4f("view", EditorContext.ActiveCamera.value()->GetViewMat());
	ssaoShader->SetUniform2f("screenSize", static_cast<float>(RenderedSize.x), static_cast<float>(RenderedSize.y));
	ssaoShader->SetUniform3fv("ssaoSamples", 64, PipelineData.SSAOProcessor->getKernelAllocator());

	PipelineData.GBufferScreenGeometry->Draw(ssaoShader.get());

	PipelineData.SSAOFramebuffer->Unbind();
}

void Runtime::RasterizationViewport::RenderSSAOBlurPass() const
{
	API::Core::RenderCommand::SetDepthTest(false);
	API::Core::RenderCommand::SetStencilTest(false);

	PipelineData.SSAOBlurFramebuffer->BindAndClear();

	PipelineData.SSAOFramebuffer->BindTexture(0, 0);

	const Cast::Ref<API::Core::Shader> ssaoBlurShader = Cast::ShaderCacheRegistryInstance.GetHandle("ssao_blur");
	ssaoBlurShader->Bind();
	PipelineData.GBufferScreenGeometry->Draw(ssaoBlurShader.get());

	PipelineData.SSAOBlurFramebuffer->Unbind();
}

void Runtime::RasterizationViewport::RenderLightingPass() const
{
	PipelineData.Framebuffer->BindAndClear();
	PipelineData.GBuffer->BindDepthTexture(0);
	PipelineData.GBuffer->BindTextures(1);
	PipelineData.SSAOBlurFramebuffer->BindTexture(0, 7);

	// CSMs
	if (PipelineData.ShadowProcessor)
	{
		for (int c = 0; c < API::Advanced::CascadedShadowMap::CascadeCount; ++c)
		{
			PipelineData.ShadowProcessor->GetShadowMap()->BindDepthTexture(c, 10 + c);
		}
	}

	// Copy stencil from GBuffer so shading only runs where geometry was drawn
	API::Core::RenderCommand::CopyStencilBuffer(PipelineData.GBuffer->GetInternalId(),
												   PipelineData.Framebuffer->GetInternalId(), RenderedSize.x, RenderedSize.y);

	Cast::Shared.ActiveScene->BindSSBOForShadingPass();

	// Lighting Pass Uniforms
	const Cast::Ref<API::Core::Shader> shader = Cast::ShaderCacheRegistryInstance.GetHandle("shading_pass");
	shader->Bind();
	shader->SetUniform2f("u_Resolution", static_cast<float>(RenderedSize.x), static_cast<float>(RenderedSize.y));
	const float aoAffect = EditorContext.ViewSettings.SSAOEnabled ? EditorContext.ViewSettings.SSAOAffectness : 0.0f;
	shader->SetUniform1f("u_SSAOAffectness", aoAffect);

	const bool hasShadows = PipelineData.ShadowProcessor && PipelineData.ShadowProcessor->HasShadowMapData();
	shader->SetUniform1i("u_HasShadowMap", (int)hasShadows);
	if (hasShadows)
	{
		const auto& lightDir = PipelineData.ShadowProcessor->GetShadowLightDir();
		shader->SetUniform3f("u_ShadowLightDir", lightDir.x, lightDir.y, lightDir.z);
		for (int c = 0; c < API::Advanced::CascadedShadowMap::CascadeCount; ++c)
		{
			shader->SetUniformMat4f("u_LightViewProj[" + std::to_string(c) + "]", PipelineData.ShadowProcessor->GetCascadeLightVP()[c]);
		}
		shader->SetUniform1fv("u_CascadeSplits", API::Advanced::CascadedShadowMap::CascadeCount, PipelineData.ShadowProcessor->GetCascadeSplits());
		shader->SetUniform1fv("u_ShadowBiasFactors", API::Advanced::CascadedShadowMap::CascadeCount, EditorContext.ViewSettings.ShadowBiasFactors);
	}
	shader->SetUniformMat4f("u_View", EditorContext.ActiveCamera.value()->GetViewMat());

	API::Core::RenderCommand::SetStencilTest(true);
	API::Core::RenderCommand::SetDefaultStencilTest();
	API::Core::RenderCommand::SetDepthTestFunc(API::Core::DepthFunction::Less);

	API::Core::RenderCommand::SetClearColor(EditorContext.Skybox.GetClearColor());
	API::Core::RenderCommand::Clear();

	PipelineData.Framebuffer->Bind();

	PipelineData.GBufferScreenGeometry->Draw(shader.get());

	API::Core::RenderCommand::SetBlend(false);
	API::Core::RenderCommand::SetStencilTest(false);
}

void Runtime::RasterizationViewport::RenderForwardPass() const
{
	PipelineData.Framebuffer->Bind();

	// EnaLE depth testing for forward/overlay rendering
	API::Core::RenderCommand::SetDepthTest(true);
	API::Core::RenderCommand::SetDepthTestFunc(API::Core::DepthFunction::Less);

	// Render any forward-rendered scene content
	Cast::Shared.ActiveScene->OnForwardRender();

	// Render skybox behind geometry using LEQUAL
	EditorContext.Skybox.BindCurrentCubemap(6);
	EditorContext.Skybox.Render();

	// Depth test grid
	if (!Cast::Shared.ActiveScene->GetInRenderView())
	{
		API::Core::RenderCommand::SetBlend(true);
		API::Core::RenderCommand::SetBlendFunc(API::Core::BlendFunction::SrcAlpha, API::Core::BlendFunction::OneMinusSrcAlpha);
		API::Core::RenderCommand::CullFace(API::Core::Face::None);

		const Cast::Ref<API::Core::Shader> shader = Cast::ShaderCacheRegistryInstance.GetHandle("tile_grid");
		shader->Bind();
		API::Core::RenderCommand::IssueEmptyDrawCall(6);

		API::Core::RenderCommand::SetBlend(false);
	}

	PipelineData.Framebuffer->Unbind();
}

void Runtime::RasterizationViewport::RenderGizmos()
{
	ImGuizmo::Enable(true);
	ImGuizmo::SetDrawlist();

	// Use the actual displayed image bounds for gizmo rendering
	const float gizmoX = ImageDisplayMin.x;
	const float gizmoY = ImageDisplayMin.y;
	const float gizmoWidth = ImageDisplayMax.x - ImageDisplayMin.x;
	const float gizmoHeight = ImageDisplayMax.y - ImageDisplayMin.y;

	ImGuizmo::SetRect(gizmoX, gizmoY, gizmoWidth, gizmoHeight);

	float* cameraView = Runtime::EditorContext.ActiveCamera.value()->GetViewMatValuePtr();

	IsGizmoScaleURendered = false;
	Cast::Ref<Cast::Entity> selectedEntity = nullptr;
	if (Cast::Shared.ActiveScene->IsEntitySelected())
	{
		selectedEntity = Cast::Shared.ActiveScene->GetEditorSelectionContext();
		if (selectedEntity->HasComponent<Cast::Component::TransformComponent>())
		{
			auto& comp = selectedEntity->GetComponent<Cast::Component::TransformComponent>();
			float* transform = comp.GetTransformValuePtr();

			ImGuizmo::OPERATION operation = ImGuizmo::OPERATION::TRANSLATE | ImGuizmo::OPERATION::SCALE;

			float snapValue = 0.5f;

			if (Application::Keymap::IsActionActive(Application::KEY_ACTION::OBJ_SCALE))
			{
				operation = ImGuizmo::OPERATION::SCALEU;
				IsGizmoScaleURendered = true;
			}
			else if (Application::Keymap::IsActionActive(Application::KEY_ACTION::OBJ_ROT))
			{
				operation = ImGuizmo::OPERATION::ROTATE;
				snapValue = 15.0f;
			}

			const float snap[3] = {snapValue, snapValue, snapValue};

			// Does not really snap but adds offset
			const bool changed = ImGuizmo::Manipulate(
				glm::value_ptr(Runtime::EditorContext.ActiveCamera.value()->GetViewMat()),
				glm::value_ptr(Runtime::EditorContext.ActiveCamera.value()->GetProjectionMat()),
				operation, ImGuizmo::LOCAL, transform, nullptr,
				Application::Keymap::IsActionActive(Application::KEY_ACTION::OBJ_SNAP) ? snap : nullptr);
			
			if (changed)
			{
				comp.DecomposeTransformOnComponents();
				comp.UpdateOnGPUMem();
			}
		}
	}

	const glm::vec3 cameraPosition = Runtime::EditorContext.ActiveCamera.value()->GetPosition();
	const float cameraDistance = selectedEntity ? glm::length(cameraPosition - selectedEntity->GetComponent<Cast::Component::TransformComponent>().GetTranslation()) : 3.0f;

	constexpr float widgetWidth = 100.0f;
	ImGuizmo::ViewManipulate(
		cameraView, cameraDistance,
		ImVec2(ImageDisplayMax.x - widgetWidth - 15, ImageDisplayMin.y + 5),
		ImVec2(widgetWidth, widgetWidth),
		0x10101010
	);

	if (ImGuizmo::IsUsingViewManipulate())
	{
		Runtime::EditorContext.ActiveCamera.value()->MakeConsistentViewMatBase();
	}
}

void Runtime::RasterizationViewport::BuildViewport(const glm::ivec2& viewportSize)
{
	RenderedSize = viewportSize;
	CachedAspectRatio = static_cast<float>(viewportSize.x) / static_cast<float>(viewportSize.y);

	// GBuffer
	PipelineData.GBufferScreenGeometry.reset(
		API::Advanced::GBufferScreenGeometry::Create(viewportSize.x, viewportSize.y));
	PipelineData.Framebuffer.reset(
		API::Core::Framebuffer::Create(glm::ivec2(viewportSize.x, viewportSize.y)));
	PipelineData.GBuffer.reset(API::Advanced::GBuffer::Create(viewportSize.x, viewportSize.y));

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

	// Bind GBuffer textures
	PipelineData.GBuffer->BindDepthTexture(0);
	PipelineData.GBuffer->BindTextures(1);

	// GBuffer uniforms
	const Cast::Ref<API::Core::Shader> shader_shading = Cast::ShaderCacheRegistryInstance.GetHandle("shading_pass");
	shader_shading->Bind();
	shader_shading->SetUniform1i("gBuf_Position", static_cast<int>(PipelineData.GBuffer->GetTargetBoundTextureSlot("Position")));
	shader_shading->SetUniform1i("gBuf_Normal", static_cast<int>(PipelineData.GBuffer->GetTargetBoundTextureSlot("Normal")));
	shader_shading->SetUniform1i("gBuf_Albedo", static_cast<int>(PipelineData.GBuffer->GetTargetBoundTextureSlot("Albedo")));
	shader_shading->SetUniform1i("gBuf_Specular", static_cast<int>(PipelineData.GBuffer->GetTargetBoundTextureSlot("Specular")));
	shader_shading->SetUniform1i("gBuf_Shine_Reflectance",
						 static_cast<int>(PipelineData.GBuffer->GetTargetBoundTextureSlot("Shine_Reflectance")));

	// SSAO
	PipelineData.SSAOFramebuffer.reset(API::Core::Framebuffer::Create(glm::ivec2(viewportSize.x, viewportSize.y), false));
	PipelineData.SSAOFramebuffer->Bind();
	PipelineData.SSAOFramebuffer->PushColorAttribute(1, API::Core::BufferDataType::_FLOAT, nullptr);
	std::ignore = PipelineData.SSAOFramebuffer->Validate();
	PipelineData.SSAOFramebuffer->Unbind();

	PipelineData.SSAOBlurFramebuffer.reset(API::Core::Framebuffer::Create(glm::ivec2(viewportSize.x, viewportSize.y), false));
	PipelineData.SSAOBlurFramebuffer->Bind();
	PipelineData.SSAOBlurFramebuffer->PushColorAttribute(1, API::Core::BufferDataType::_FLOAT, nullptr);
	std::ignore = PipelineData.SSAOBlurFramebuffer->Validate();
	PipelineData.SSAOBlurFramebuffer->Unbind();

	// Initialize SSAO processor
	PipelineData.SSAOProcessor.reset(API::Advanced::SSAO::Create());
	PipelineData.SSAOProcessor->GenerateSampleKernel(64);
	PipelineData.SSAOProcessor->GenerateSSAONoiseMap();

	// SSAO uniforms
	shader_shading->SetUniform1i("u_SSAO", 7);
	shader_shading->Unbind();

	const Cast::Ref<API::Core::Shader> ssaoShader = Cast::ShaderCacheRegistryInstance.GetHandle("ssao");
	ssaoShader->Bind();
	ssaoShader->SetUniform1i("gBuf_Position", static_cast<int>(PipelineData.GBuffer->GetTargetBoundTextureSlot("Position")));
	ssaoShader->SetUniform1i("gBuf_Normal", static_cast<int>(PipelineData.GBuffer->GetTargetBoundTextureSlot("Normal")));
	ssaoShader->SetUniform1i("texNoise", 6);
	ssaoShader->SetUniform1i("kernelSize", 64);
	ssaoShader->SetUniform2f("screenSize", static_cast<float>(viewportSize.x), static_cast<float>(viewportSize.y));
	ssaoShader->Unbind();
}

void Runtime::RasterizationViewport::OnResizeCallback()
{
	glm::ivec2 newSize;
	if (EditorContext.ViewSettings.AdjustToWindowSize)
	{
		newSize = {static_cast<int>(Size.x), static_cast<int>(Size.y)};
	}
	else
	{
		newSize = {EditorContext.ViewSettings.ViewportWidth, EditorContext.ViewSettings.ViewportHeight};
	}

	Resize(newSize);
}

void Runtime::RasterizationViewport::CompileShaders()
{
	Cast::ShaderCacheRegistryInstance.AddProxy(Cast::ShaderCacheRegistryInstance.Add(API::Core::Shader::Create(std::string(ASSET_DIR) + "shader/deferred/geometry_pass.vert",
	                                                     std::string(ASSET_DIR) + "shader/deferred/geometry_pass.frag")), "geometry_pass");

	Cast::ShaderCacheRegistryInstance.AddProxy(Cast::ShaderCacheRegistryInstance.Add(API::Core::Shader::Create(std::string(ASSET_DIR) + "shader/deferred/shading_pass.vert",
	                                                     std::string(ASSET_DIR) + "shader/deferred/shading_pass.frag")), "shading_pass");

	// SSAO passes
	Cast::ShaderCacheRegistryInstance.AddProxy(Cast::ShaderCacheRegistryInstance.Add(API::Core::Shader::Create(std::string(ASSET_DIR) + "shader/view/ssao.vert",
	                                                     std::string(ASSET_DIR) + "shader/view/ssao.frag")), "ssao");
	Cast::ShaderCacheRegistryInstance.AddProxy(Cast::ShaderCacheRegistryInstance.Add(API::Core::Shader::Create(std::string(ASSET_DIR) + "shader/view/ssao_blur.vert",
	                                                     std::string(ASSET_DIR) + "shader/view/ssao_blur.frag")), "ssao_blur");

	Cast::ShaderCacheRegistryInstance.AddProxy(Cast::ShaderCacheRegistryInstance.Add(API::Core::Shader::Create(std::string(ASSET_DIR) + "shader/sprite/icon.vert",
	                                                     std::string(ASSET_DIR) + "shader/sprite/icon.frag")), "icon_billboard");

	Cast::ShaderCacheRegistryInstance.AddProxy(Cast::ShaderCacheRegistryInstance.Add(API::Core::Shader::Create(std::string(ASSET_DIR) + "shader/effect/tile_grid.vert",
													 std::string(ASSET_DIR) + "shader/effect/tile_grid.frag")), "tile_grid");

	Cast::ShaderCacheRegistryInstance.AddProxy(Cast::ShaderCacheRegistryInstance.Add(API::Core::Shader::Create(std::string(ASSET_DIR) + "shader/world/cubemap.vert",
													 std::string(ASSET_DIR) + "shader/world/cubemap.frag")), "cubemap");
}

bool Runtime::RasterizationViewport::IsUsingGizmo()
{
	return ImGuizmo::IsUsingAny() || ImGuizmo::IsUsingViewManipulate();
}

bool Runtime::RasterizationViewport::IsHoveringGizmo()
{
	return Cast::Shared.ActiveScene->IsEntitySelected() && ImGuizmo::IsOver();
}

void Runtime::RasterizationViewport::UpdateCameraUniforms()
{
	const auto camPos = EditorContext.ActiveCamera.value()->GetPosition();

	const auto iconShader = Cast::ShaderCacheRegistryInstance.GetHandle("icon_billboard");
	iconShader->Bind();
	iconShader->SetUniformMat4f("u_ViewProjection", EditorContext.ActiveCamera.value()->GetViewProjectionMat());
	iconShader->SetUniform3f("u_CameraPos", camPos.x, camPos.y, camPos.z);

	const auto geometryShader = Cast::ShaderCacheRegistryInstance.GetHandle("geometry_pass");
	geometryShader->Bind();
	geometryShader->SetUniformMat4f("u_View", EditorContext.ActiveCamera.value()->GetViewMat());
	geometryShader->SetUniformMat4f("u_Projection", EditorContext.ActiveCamera.value()->GetProjectionMat());
	geometryShader->SetUniform3f("u_ViewPos", camPos.x, camPos.y, camPos.z);

	const Cast::Ref<API::Core::Shader> lightingShader = Cast::ShaderCacheRegistryInstance.GetHandle("shading_pass");
	lightingShader->Bind();
	lightingShader->SetUniform3f("u_ViewPosition", camPos.x, camPos.y, camPos.z);
	lightingShader->SetUniformMat4f("u_View", EditorContext.ActiveCamera.value()->GetViewMat());

	const Cast::Ref<API::Core::Shader> gridShader = Cast::ShaderCacheRegistryInstance.GetHandle("tile_grid");
	gridShader->Bind();
	gridShader->SetUniformMat4f("u_ViewProjection", EditorContext.ActiveCamera.value()->GetViewProjectionMat());
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
			float yaw = EditorContext.ActiveCamera.value()->GetYaw() + offset.x;
			float pitch = glm::clamp(EditorContext.ActiveCamera.value()->GetPitch() - offset.y, -89.99f, 89.99f);
			EditorContext.ActiveCamera.value()->SetRotation({
				pitch, yaw, EditorContext.ActiveCamera.value()->GetRoll()
			});
		}
		lastMousePos = {e.GetX(), e.GetY()};

		IsCameraInitFrame = false;
	}

	return false;
}
