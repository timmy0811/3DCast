#pragma once

#include <3DCast.h>

#include "vendor/glm/glm.hpp"

namespace Runtime {
	class Viewport
	{
	public:
		Viewport(Cast::Layer* parent = nullptr)
			: ParentLayer(parent) {};
		virtual ~Viewport() = default;

		virtual void Init() = 0;
		virtual void Destroy() = 0;

		virtual void OnUpdate(Cast::Timestep ts) = 0;
		virtual void OnRender() = 0;

		virtual void OnImGuiRender() = 0;

		virtual void OnEvent(Cast::Event& e) = 0;

		inline void SetFocused(bool focused) { IsFocused = focused; }
		inline void SetHovered(bool hovered) { IsHovered = hovered; }

		inline void SetSize(const glm::vec2& size) { Size = size; }
		inline const glm::vec2& GetSize() const { return Size; }

		inline bool IsViewportFocused() const { return IsFocused; }
		inline bool IsViewportHovered() const { return IsHovered; }

	protected:
		Cast::Layer* ParentLayer;
		bool IsFocused = false;
		bool IsHovered = false;
		glm::vec2 Size = { 0.0f, 0.0f };
		// Position relative to the parent window
		glm::vec2 Position = { 0.0f, 0.0f };
	};
}
