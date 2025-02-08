#pragma once

#include "Viewport.h"

namespace Runtime {
	class PBRViewport : public Viewport
	{
	public:
		PBRViewport() = default;
		PBRViewport(Cast::Layer* parent);
		virtual ~PBRViewport() = default;

		void Init() override;
		void Destroy() override;

		void OnUpdate(Cast::Timestep ts) override;
		void OnRender() override;

		void OnImGuiRender() override;

		void OnEvent(Cast::Event& e) override;
	};
}
