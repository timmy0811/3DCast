#include "EditorLayer.h"

#include <3DCast.h>
#include <3DCast/Scene/ObjectCreator.h>
#include <3DCast/Scene/Components.h>
#include <3DCast/Renderer/Camera/PerspectiveCamera.h>
#include <3DCast/Log.h>
#include <3DCast/Scene/SceneShaderCache.h>

#include "Config.h"
#include "GUI/ImGuiStyle.h"
#include <imgui_internal.h>

EditorLayer::EditorLayer()
	: Layer("EditorLayer")
{
}

void EditorLayer::OnAttach()
{
	Runtime::SetupImGuiStyle(true, 0.3f);

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

	ActiveScene = Cast::CreateRef<Cast::Scene>();

	Cast::Entity cameraEntity = ActiveScene->CreateEntity("Camera");

	ActiveCamera.reset(new Cast::Renderer::PerspectiveCamera(70.f, 1.5f, 0.1f, 100.f));
	ActiveCamera->SetPosition(glm::vec3(0.0f, 0.0f, 3.0f));
	cameraEntity.AddComponents<Cast::Component::CameraComponent>(*ActiveCamera);

	// Sample Content
	SampleContent();
}

void EditorLayer::OnDetach()
{
}

void EditorLayer::OnUpdate(Cast::Timestep ts)
{
	glm::vec3 cameraPosition = ActiveCamera->GetPosition();
	if (Cast::Input::IsKeyPressed(CAST_KEY_A)) {
		cameraPosition -= ActiveCamera->GetRight() * CameraSpeed;
	}
	if (Cast::Input::IsKeyPressed(CAST_KEY_D)) {
		cameraPosition += ActiveCamera->GetRight() * CameraSpeed;
	}
	if (Cast::Input::IsKeyPressed(CAST_KEY_UP)) {
		cameraPosition.y += CameraSpeed;
	}
	if (Cast::Input::IsKeyPressed(CAST_KEY_DOWN)) {
		cameraPosition.y -= CameraSpeed;
	}
	if (Cast::Input::IsKeyPressed(CAST_KEY_W)) {
		cameraPosition += ActiveCamera->GetForward() * CameraSpeed;
	}
	if (Cast::Input::IsKeyPressed(CAST_KEY_S)) {
		cameraPosition -= ActiveCamera->GetForward() * CameraSpeed;
	}

	if (Cast::Input::IsMouseButtonPressed(CAST_MOUSE_BUTTON_LEFT) && ViewportHovered) {
		ParentWindow->SetInputModeDisabled();
		CurrentKeyState.isLMBPressed = true;
	}
	else {
		ParentWindow->SetInputModeNormal();
		CurrentKeyState.isLMBPressed = false;
	}

	ActiveCamera->SetPosition(cameraPosition);

	Render();

	SceneHierarchyPanel.SetContext(ActiveScene);
}

void EditorLayer::OnImGuiRender()
{
	static bool dockspaceOpen = true;
	static bool opt_fullscreen_persistant = true;
	bool opt_fullscreen = opt_fullscreen_persistant;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	if (opt_fullscreen)
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}

	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
	ImGui::PopStyleVar();

	if (opt_fullscreen)
		ImGui::PopStyleVar(2);

	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

			if (ImGui::MenuItem("Exit")) Cast::Application::Get().Close();
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	ImGui::End();

	ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoTitleBar);
	ImVec2 viewportSize = ImGui::GetContentRegionAvail();
	static ImVec2 lastViewportSize = ImVec2(0, 0);
	if (viewportSize.x != lastViewportSize.x || viewportSize.y != lastViewportSize.y)
	{
		lastViewportSize = viewportSize;

		//Framebuffer->Resize({ static_cast<uint32_t>(viewportSize.x), static_cast<uint32_t>(viewportSize.y) });

		switch (ActiveCamera->GetType()) {
		case Cast::Renderer::Camera::Type::Orthographic:
			((Cast::Renderer::OrthographicCamera*)ActiveCamera.get())->SetFrustumOnResized(lastViewportSize.x, lastViewportSize.y);
			break;
		case Cast::Renderer::Camera::Type::Perspective:
			((Cast::Renderer::PerspectiveCamera*)ActiveCamera.get())->SetAspectRatio(lastViewportSize.x / lastViewportSize.y);
		}
	}

	uint32_t textureID = RenderPipelineData.Framebuffer->GetColorAttachmentTextureID(0);
	ImGui::Image((unsigned long long)textureID, lastViewportSize);

	ViewportHovered = ImGui::IsWindowHovered() && ImGui::GetCurrentWindow()->Name == std::string("Viewport");
	GuiHovered != ViewportHovered;

	ImGui::End();

	SceneHierarchyPanel.OnImGuiRender();
}

void EditorLayer::OnEvent(Cast::Event& e)
{
	Cast::EventDispatcher dispatcher(e);
	dispatcher.Dispatch<Cast::MouseMovedEvent>(CAST_BIND_EVENT_FUNC(EditorLayer::OnMouseMoved));
	dispatcher.Dispatch<Cast::MouseScrolledEvent>(CAST_BIND_EVENT_FUNC(EditorLayer::OnMouseScrolled));
}

bool EditorLayer::OnMouseMoved(Cast::MouseMovedEvent& e)
{
	if (CurrentKeyState.isLMBPressed && !IsInitFrame) {
		glm::vec3 cameraRotation = (ActiveCamera->GetRotation());

		glm::vec2 offset = {
			e.GetX() - LastMousePosition.x,
			e.GetY() - LastMousePosition.y
		};

		offset *= Runtime::conf.MOUSE_SENSITIVITY;

		float yaw = ActiveCamera->GetYaw() + offset.x;
		float pitch = ActiveCamera->GetPitch() + offset.y;

		ActiveCamera->SetRotation({ pitch, yaw, cameraRotation.z });
	}

	LastMousePosition = glm::vec2(e.GetX(), e.GetY());
	IsInitFrame = false;

	return false;
}

bool EditorLayer::OnMouseScrolled(Cast::MouseScrolledEvent& e)
{
	if (e.GetYOffset() < 0.f) CameraSpeed *= 0.9f;
	else if (e.GetYOffset() > 0.f) CameraSpeed *= 1.1f;

	return false;
}

void EditorLayer::Render()
{
	RenderGeometryPass();
	RenderLightingPass();
}

void EditorLayer::RenderGeometryPass()
{
	API::Core::RenderCommand::SetDepthTest(true);
	API::Core::RenderCommand::CullFace(API::Core::Face::Back);

	RenderPipelineData.GBuffer->BindAndClear();
	Cast::Renderer::RendererContext::BeginScene(*ActiveCamera);

	ActiveScene->OnUpdate();

	Cast::Renderer::RendererContext::EndScene();
	RenderPipelineData.GBuffer->Unbind();
}

void EditorLayer::RenderLightingPass()
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

void EditorLayer::CompileShaders()
{
	Cast::AssetCache.AddShader("shader_geometry_pass", API::Core::Shader::Create("../3DCast/ressources/shader/deferred/geometry_pass.vert", "../3DCast/ressources/shader/deferred/geometry_pass.frag"));
	Cast::AssetCache.AddShader("shader_shading_pass", API::Core::Shader::Create("../3DCast/ressources/shader/deferred/shading_pass.vert", "../3DCast/ressources/shader/deferred/shading_pass.frag"));
}

void EditorLayer::SampleContent()
{
	// Light
	Cast::Entity lightEntity = ActiveScene->CreateEntity("Light");
	lightEntity.AddComponents<Cast::Component::LightComponent>(Cast::Component::LightComponent::Type::Directional, glm::vec3(0.3f, -.3f, 0.3f), 1.f);

	CubeEntity = Cast::Create::Cube("Cube_1", ActiveScene.get());
}