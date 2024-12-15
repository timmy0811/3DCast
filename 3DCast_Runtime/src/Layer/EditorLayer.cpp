#include "EditorLayer.h"

#include "3DCast/Scene/Components.h"
#include <3DCast/Renderer/Camera/PerspectiveCamera.h>

#include <3DCast.h>
#include <imgui.h>

EditorLayer::EditorLayer()
	: Layer("EditorLayer")
{
}

void EditorLayer::OnAttach()
{
	ActiveScene = Cast::CreateRef<Cast::Scene>();

	Cast::Entity cameraEntity = ActiveScene->CreateEntity("Camera");

	ActiveCamera.reset(new Cast::Renderer::PerspectiveCamera(70.f, 1.5f, 0.1f, 100.f));
	ActiveCamera->SetPosition(glm::vec3(0.0f, 0.0f, 3.0f));
	ActiveCamera->LookAt(glm::vec3(0.0f, 0.0f, 0.0f));
	cameraEntity.AddComponents<Cast::Component::CameraComponent>(*ActiveCamera);

	// Exsample Content
	CubeEntity = ActiveScene->CreateEntity("Cube");
	CubeEntity.AddComponents<Cast::Component::CustomMeshComponent>();
	CubeEntity.AddComponents<Cast::Component::RasterizableComponent>();
	CubeEntity.AddComponents<Cast::Component::ShaderComponent>("../3DCast/ressources/shader/common/shader_single_color.vert", "../3DCast/ressources/shader/common/shader_single_color.frag", "single_color");
	CubeEntity.AddComponents<Cast::Component::LightComponent>(Cast::Component::LightComponent::Type::Point, glm::vec3(1.f), 3.f);
	CubeEntity.AddComponents<Cast::Component::MeshComponent>("C:/Git/path/to/file");

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

	cubeMesh.va.reset(API::Core::VertexArray::Create());
	cubeMesh.va->AddBuffer(*(cubeMesh.vb), *(cubeMesh.vbLayout));
	cubeMesh.va->SetVBCount(4);

	Cast::Ref<API::Core::Shader> cubeShader = CubeEntity.GetComponent<Cast::Component::ShaderComponent>().Shader;
	cubeShader->Bind();
	cubeShader->SetUniformMat4f("u_ViewProjection", ActiveCamera->GetViewProjectionMat());
}

void EditorLayer::OnDetach()
{
}

void EditorLayer::OnUpdate(Cast::Timestep ts)
{
	glm::vec3 cameraPosition = ActiveCamera->GetPosition();
	if (Cast::Input::IsKeyPressed(CAST_KEY_LEFT)) {
		cameraPosition.x -= 0.05f;
	}
	if (Cast::Input::IsKeyPressed(CAST_KEY_RIGHT)) {
		cameraPosition.x += 0.05f;
	}
	if (Cast::Input::IsKeyPressed(CAST_KEY_UP)) {
		cameraPosition.y += 0.05f;
	}
	if (Cast::Input::IsKeyPressed(CAST_KEY_DOWN)) {
		cameraPosition.y -= 0.05f;
	}
	if (Cast::Input::IsKeyPressed(CAST_KEY_W)) {
		cameraPosition.z -= 0.05f;
	}
	if (Cast::Input::IsKeyPressed(CAST_KEY_S)) {
		cameraPosition.z += 0.05f;
	}

	ActiveCamera->SetPosition(cameraPosition);

	Cast::Ref<API::Core::Shader> cubeShader = CubeEntity.GetComponent<Cast::Component::ShaderComponent>().Shader;
	cubeShader->Bind();
	cubeShader->SetUniform4f("u_Color", 0.4f, 0.2f, 0.4f, 1.f);

	API::Core::RenderCommand::Clear();
	API::Core::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });

	Cast::Renderer::RendererContext::BeginScene(*ActiveCamera);

	ActiveScene->OnUpdate();

	Cast::Renderer::RendererContext::EndScene();

	SceneHierarchyPanel.SetContext(ActiveScene);
}

void EditorLayer::OnImGuiRender()
{
	SceneHierarchyPanel.OnImGuiRender();
}

void EditorLayer::OnEvent(Cast::Event& e)
{
	Cast::EventDispatcher dispatcher(e);
	dispatcher.Dispatch<Cast::WindowResizeEvent>(CAST_BIND_EVENT_FUNC(EditorLayer::OnWindowResize));
}

bool EditorLayer::OnWindowResize(Cast::WindowResizeEvent& e)
{
	switch (ActiveCamera->GetType()) {
	case Cast::Renderer::Camera::Type::Orthographic:
		((Cast::Renderer::OrthographicCamera*)ActiveCamera.get())->SetFrustumOnResized((float)e.GetWidth(), (float)e.GetHeight());
		break;
	case Cast::Renderer::Camera::Type::Perspective:
		((Cast::Renderer::PerspectiveCamera*)ActiveCamera.get())->SetAspectRatio(static_cast<float>(e.GetWidth()) / static_cast<float>(e.GetHeight()));
	}

	return false;
}