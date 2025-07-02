#pragma once

#include "Viewport.h"

namespace Runtime {
	class PBRViewport final : public Viewport
	{
	public:
		PBRViewport() = default;
		explicit PBRViewport(Cast::Layer* parent);
		~PBRViewport() override = default;

		void Init() override;
		void Destroy() override;

		void OnUpdate(Cast::Timestep ts) override;
		void OnRender() override;

		void OnImGuiRender() override;

		void OnEvent(Cast::Event& e) override;
	};
}
