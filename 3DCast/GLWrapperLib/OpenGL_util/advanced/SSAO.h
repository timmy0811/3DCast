#pragma once

#include <vector>
#include "../vendor/glm/glm.hpp"
#include "../dependencies.hpp"

#include "../debug/Debug.hpp"

namespace GL::advanced {
	class SSAO
	{
	public:
		SSAO();

		void GenerateSampleKernel(int samples = 64);
		void GenerateSSAONoiseMap();
		glm::vec3* getKernelAllocator() { return &(m_Kernel[0]); }
		void BindNoiseTex(const unsigned int slot);

	private:
		std::vector<glm::vec3> m_Kernel;
		unsigned int m_IdNoise;
	};
}