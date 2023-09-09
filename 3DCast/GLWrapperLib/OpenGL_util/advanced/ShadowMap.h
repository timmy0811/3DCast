#pragma once

#include "../dependencies.hpp"

#include <string>

#include "../vendor/glm/glm.hpp"

#include "../debug/Debug.hpp"

namespace GL::advanced {
	class ShadowMap
	{
	public:
		ShadowMap(const glm::ivec2& size);
		~ShadowMap();

		bool Init(const glm::ivec2& size);
		void BindAndClear();

		unsigned int BindDepthTexture(const unsigned int slot);
		void UnbindDepthTexture();

		inline void Resize(const glm::ivec2& size) { m_Width = size.x; m_Height = size.y; }

	private:
		unsigned int m_Width, m_Height;
		unsigned int m_IdFBO;
		unsigned int m_IdDepthBuffer;

		int m_BoundPort;
	};
}