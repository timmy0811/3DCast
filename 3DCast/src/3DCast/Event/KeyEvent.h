#pragma once

#include "castpch.h"

#include "Event.h"

namespace Cast {
	class KeyEvent : public Event {
	public:
		[[nodiscard]] inline int GetKeyCode() const { return keyCode; }

		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput);

	protected:
		explicit KeyEvent(const int keycode)
			:keyCode(keycode) {}

		int keyCode;
	};

	class KeyPressedEvent final : public KeyEvent {
	public:
		KeyPressedEvent(const int keyCode, const int repeatCount)
			:KeyEvent(keyCode), m_RepeatCount(repeatCount) {}

		[[nodiscard]] inline int GetRepeatCount() const { return m_RepeatCount; }

		[[nodiscard]] std::string ToString() const override {
			std::stringstream ss;
			ss << "KeyPressedEvent: " << keyCode << " (" << GetRepeatCount() << " repeats)";
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyPressed);

	private:
		int m_RepeatCount;
	};

	class KeyReleasedEvent final : public KeyEvent {
	public:
		explicit KeyReleasedEvent(const int keyCode)
			:KeyEvent(keyCode) {}

		[[nodiscard]] std::string ToString() const override {
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << keyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyReleased);
	};

	class KeyTypedEvent final : public KeyEvent {
	public:
		explicit KeyTypedEvent(const int keyCode)
			:KeyEvent(keyCode) {}

		[[nodiscard]] std::string ToString() const override {
			std::stringstream ss;
			ss << "KeyTypedEvent: " << keyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyTyped);
	};
}