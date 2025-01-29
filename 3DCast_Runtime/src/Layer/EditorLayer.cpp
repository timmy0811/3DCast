#include "EditorLayer.h"

#include <3DCast/Scene/Components.h>
#include <3DCast/Renderer/Camera/PerspectiveCamera.h>
#include <3DCast/Log.h>
#include "3DCast/Scene/SceneShaderCache.h"

#include "Config.h"
#include "GUI/ImGuiStyle.h"
#include <3DCast.h>

EditorLayer::EditorLayer()
	: Layer("EditorLayer")
{
}

void EditorLayer::OnAttach()
{
	Runtime::SetupImGuiStyle(true, 0.3f);

	Framebuffer.reset(API::Core::Framebuffer::Create(glm::ivec2(Runtime::conf.WIN_WIDTH, Runtime::conf.WIN_HEIGHT)));

	GBuffer.reset(API::Advanced::GBuffer::Create(Runtime::conf.WIN_WIDTH, Runtime::conf.WIN_HEIGHT));

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

	if (Cast::Input::IsMouseButtonPressed(CAST_MOUSE_BUTTON_LEFT)) {
		ParentWindow->SetInputModeDisabled();
		CurrentKeyState.isLMBPressed = true;
	}
	else {
		ParentWindow->SetInputModeNormal();
		CurrentKeyState.isLMBPressed = false;
	}

	ActiveCamera->SetPosition(cameraPosition);

	Cast::Ref<API::Core::Shader> cubeShader = Cast::AssetCache.GetShaderHandle(CubeEntity.GetComponent<Cast::Component::MaterialComponent>().Shader);
	cubeShader->Bind();
	cubeShader->SetUniform4f("u_Color", 0.4f, 0.2f, 0.4f, 1.f);

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

	uint32_t textureID = Framebuffer->GetColorAttachmentTextureID(0);
	ImGui::Image((unsigned long long)textureID, lastViewportSize);
	ImGui::End();

	SceneHierarchyPanel.OnImGuiRender();
}

void EditorLayer::OnEvent(Cast::Event& e)
{
	Cast::EventDispatcher dispatcher(e);
	dispatcher.Dispatch<Cast::MouseMovedEvent>(CAST_BIND_EVENT_FUNC(EditorLayer::OnMouseMoved));
	dispatcher.Dispatch<Cast::MouseScrolledEvent>(CAST_BIND_EVENT_FUNC(EditorLayer::OnMouseScrolled));
}

bool EditorLayer::OnWindowResize(Cast::WindowResizeEvent& e)
{
	return false;
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
	GBuffer->BindAndClear();

	GBuffer->Unbind();
}

void EditorLayer::RenderLightingPass()
{
	// GUI Background Color
	API::Core::RenderCommand::SetClearColor({ 0.2f, 0.2f, 0.2f, 1.0f });
	API::Core::RenderCommand::Clear();

	Framebuffer->Bind();
	API::Core::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
	API::Core::RenderCommand::Clear();

	Cast::Renderer::RendererContext::BeginScene(*ActiveCamera);

	ActiveScene->OnUpdate();

	Cast::Renderer::RendererContext::EndScene();
	Framebuffer->Unbind();
}

void EditorLayer::SampleContent()
{
	// Example Content
	unsigned short id = Cast::AssetCache.AddShader(API::Core::Shader::Create("../3DCast/ressources/shader/common/shader_single_color.vert", "../3DCast/ressources/shader/common/shader_single_color.frag"));

	// Light
	Cast::Entity lightEntity = ActiveScene->CreateEntity("Light");
	lightEntity.AddComponents<Cast::Component::LightComponent>(Cast::Component::LightComponent::Type::Directional, glm::vec3(0.3f, -.3f, 0.3f), 1.f);

	// Cube
	CubeEntity = ActiveScene->CreateEntity("Cube");
	CubeEntity.AddComponents<Cast::Component::CustomMeshComponent>();
	CubeEntity.AddComponents<Cast::Component::RasterizableComponent>();
	CubeEntity.AddComponents<Cast::Component::MeshComponent>("C:/Git/path/to/file");
	CubeEntity.AddComponents<Cast::Component::MaterialComponent>(id);

	float vertices[3 * 8] = {
				-0.5f, -0.5f, -0.5f,
				 0.5f, -0.5f, -0.5f,
				 0.5f, 0.5f, -0.5f,
				 -0.5f, 0.5f, -0.5f,
				 -0.5f, -0.5f, 0.5f,
				 0.5f, -0.5f, 0.5f,
				 0.5f, 0.5f, 0.5f,
				 -0.5f, 0.5f, 0.5f,
	};

	unsigned int* indices = new unsigned int[36];

	// Front face
	indices[0] = 4; indices[1] = 5; indices[2] = 6;
	indices[3] = 6; indices[4] = 7; indices[5] = 4;

	// Back face
	indices[6] = 0; indices[7] = 1; indices[8] = 2;
	indices[9] = 2; indices[10] = 3; indices[11] = 0;

	// Left face
	indices[12] = 0; indices[13] = 4; indices[14] = 7;
	indices[15] = 7; indices[16] = 3; indices[17] = 0;

	// Right face
	indices[18] = 1; indices[19] = 5; indices[20] = 6;
	indices[21] = 6; indices[22] = 2; indices[23] = 1;

	// Top face
	indices[24] = 3; indices[25] = 2; indices[26] = 6;
	indices[27] = 6; indices[28] = 7; indices[29] = 3;

	// Bottom face
	indices[30] = 0; indices[31] = 1; indices[32] = 5;
	indices[33] = 5; indices[34] = 4; indices[35] = 0;

	Cast::Component::CustomMeshComponent& cubeMesh = CubeEntity.GetComponent<Cast::Component::CustomMeshComponent>();

	cubeMesh.ib.reset(API::Core::IndexBuffer::Create(indices, 36));
	cubeMesh.vb.reset(API::Core::VertexBuffer::Create(8, sizeof(float) * 3));

	cubeMesh.vb->AddVertexData(vertices, sizeof(vertices));

	delete[] indices;

	cubeMesh.vbLayout.reset(API::Core::VertexBufferLayout::Create());
	cubeMesh.vbLayout->Push(API::Core::ShaderDataType::Float3);
	//cubeMesh.vbLayout->Push(API::Core::ShaderDataType::Float3);

	cubeMesh.va.reset(API::Core::VertexArray::Create());
	cubeMesh.va->AddBuffer(*(cubeMesh.vb), *(cubeMesh.vbLayout));
	cubeMesh.va->SetVBCount(4);

	Cast::Ref<API::Core::Shader> cubeShader = Cast::AssetCache.GetShaderHandle(id);
	cubeShader->Bind();
	cubeShader->SetUniformMat4f("u_ViewProjection", ActiveCamera->GetViewProjectionMat());
}