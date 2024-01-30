#pragma once

#include <3DCast.h>
#include <yaml-cpp/yaml.h>

namespace Runtime {
	class Config {
	private:
		const std::string m_Path;
	public:
		Config(const std::string& path)
			:m_Path(path)
		{
			Parse();
		}

		void Parse() {
			YAML::Node mainNode = YAML::LoadFile(m_Path);

			WIN_WIDTH = mainNode["Config"]["Application"]["WindowWidth"].as<unsigned int>();
			WIN_HEIGHT = mainNode["Config"]["Application"]["WindowHeight"].as<unsigned int>();
		}

		// Window
		unsigned int WIN_WIDTH = 0;
		unsigned int WIN_HEIGHT = 0;
	};

	extern Config conf;
}
