#pragma once

#include <3DCast.h>
#include <yaml-cpp/yaml.h>

#include <iostream>
#include <utility>

namespace Runtime {
	class Config {
	private:
		const std::string m_Path;
	public:
		explicit Config(std::string  path)
			:m_Path(std::move(path))
		{
			try {
				Parse();
			}
			catch (...) {
				std::cout << "Preinit Message: Failed to parse config file\n";
				exit(-1);
			}
		}

		void Parse() {
			YAML::Node mainNode = YAML::LoadFile(m_Path);

			WIN_WIDTH = mainNode["Config"]["Application"]["WindowWidth"].as<unsigned int>();
			WIN_HEIGHT = mainNode["Config"]["Application"]["WindowHeight"].as<unsigned int>();

			MOUSE_SENSITIVITY = mainNode["Config"]["Application"]["MouseSens"].as<float>();
		}

		// Window
		unsigned int WIN_WIDTH = 0;
		unsigned int WIN_HEIGHT = 0;
		float MOUSE_SENSITIVITY = 0.1f;
	};

	extern Config conf;
}
