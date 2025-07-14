#pragma once

#include <3DCast.h>

#include "vendor/glm/glm.hpp"

namespace Runtime
{
	class Viewport
	{
	public:
		explicit Viewport(Cast::Layer* parent = nullptr)
			: ParentLayer(parent)
		{
		};
		virtual ~Viewport() = default;

		virtual void Init() = 0;
		virtual void Destroy() = 0;

		virtual void OnUpdate(Cast::Timestep ts, bool hasCameraChanged) = 0;
		virtual void OnRender() = 0;

		virtual void OnImGuiRender() = 0;

		virtual void OnEvent(Cast::Event& e) = 0;

		inline void SetFocused(const bool focused) { IsFocused = focused; }
		inline void SetHovered(const bool hovered) { IsHovered = hovered; }
		inline void SetMainComponentHovered(const bool hovered) { IsMainComponentHovered = hovered; }

		inline void SetSize(const glm::vec2& size) { Size = size; }
		[[nodiscard]] inline const glm::vec2& GetSize() const { return Size; }

		[[nodiscard]] inline bool IsViewportFocused() const { return IsFocused; }
		[[nodiscard]] inline bool IsViewportHovered() const { return IsHovered; }
		[[nodiscard]] inline bool IsViewportMainComponentHovered() const { return IsMainComponentHovered; }

	protected:
		Cast::Layer* ParentLayer;
		bool IsFocused = false;
		bool IsHovered = false;
		bool IsMainComponentHovered = false;
		glm::vec2 Size = {0.0f, 0.0f};
		glm::vec2 Position = {0.0f, 0.0f}; // Position relative to the parent window
	};
}
