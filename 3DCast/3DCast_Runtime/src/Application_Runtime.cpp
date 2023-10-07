#include <3DCast.h>
#include <imgui.h>

class ExampleLayer : public Cast::Layer
{
public:
	ExampleLayer()
		: Layer("Example") {}

	void OnUpdate() override {
		// LOG_INFO("ExampleLayer::Update");
		if (Cast::Input::IsKeyPressed(CAST_KEY_G)) {
			LOG_INFO("G got presssed!");
		}
	}

	virtual void OnImGuiRender() override {
		ImGui::Begin("TEst");
		ImGui::End();
	}

	void OnEvent(Cast::Event& e) override {
		// LOG_TRACE("{0}", e.ToString());
	}
};

class Application_Runtime : public Cast::Application {
public:
	Application_Runtime() {
		PushLayer(new ExampleLayer());
	}
	~Application_Runtime() {}
};

Cast::Application* Cast::CreatApplication() {
	return new Application_Runtime();
}