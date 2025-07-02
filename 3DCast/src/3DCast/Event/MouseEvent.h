#pragma once

#include "castpch.h"

#include "Event.h"

namespace Cast {
	class MouseMovedEvent final : public Event {
	public:
		MouseMovedEvent(const float x, const float y)
			:mouseX(x), mouseY(y) {}

		[[nodiscard]] inline float GetX() const { return mouseX; }
		[[nodiscard]] inline float GetY() const { return mouseY; }

		[[nodiscard]] std::string ToString() const override {
			std::stringstream ss;
			ss << "MouseMovedEvent: " << mouseX << ", " << mouseY;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseMoved);
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput);

	private:
		float mouseX, mouseY;
	};

	class MouseScrolledEvent final : public Event {
	public:
		MouseScrolledEvent(const float xOffset, const float yOffset)
			:xOffset(xOffset), yOffset(yOffset) {}

		[[nodiscard]] inline float GetXOffset() const { return xOffset; }
		[[nodiscard]] inline float GetYOffset() const { return yOffset; }

		[[nodiscard]] std::string ToString() const override {
			std::stringstream ss;
			ss << "MouseScrolledEvent: " << GetXOffset() << ", " << GetYOffset();
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseScrolled);
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput);

	private:
		float xOffset, yOffset;
	};

	class MouseButtonEvent : public Event {
	public:
		[[nodiscard]] inline int GetMouseButton() const { return button; }

		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput);

	protected:
		explicit MouseButtonEvent(const int button)
			:button(button) {}

		int button;
	};

	class MouseButtonPressedEvent final : public MouseButtonEvent {
	public:
		explicit MouseButtonPressedEvent(const int button)
			:MouseButtonEvent(button) {}

		[[nodiscard]] std::string ToString() const override {
			std::stringstream ss;
			ss << "MouseButtonPressedEvent: " << GetMouseButton();
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonPressed);
	};

	class MouseButtonReleasedEvent final : public MouseButtonEvent {
	public:
		explicit MouseButtonReleasedEvent(const int button)
			:MouseButtonEvent(button) {}

		[[nodiscard]] std::string ToString() const override {
			std::stringstream ss;
			ss << "MouseButtonReleasedEvent: " << GetMouseButton();
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonReleased);
	};
}