#include "runtimepch.h"

#include <3DCast.h>
#include <3DCast/EntryPoint.h>

#include "Layer/EditorLayer.h"
#include "Config.h"
#include "Data/SeedData.h"

class Application_Runtime : public Cast::Application
{
public:
	Application_Runtime()
		: Application(Cast::WindowProperties("3DCast Rendering Engine", Runtime::conf.WIN_WIDTH,
		                                     Runtime::conf.WIN_HEIGHT))
	{
		PushLayer(new EditorLayer());
		Runtime::SetupSceneGeometrySeed();
		Runtime::SetupSeedData();
	}

	~Application_Runtime() = default;
};

Cast::Application* Cast::CreateApplication()
{
	return new Application_Runtime();
}
