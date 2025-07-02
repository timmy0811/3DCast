#pragma once

namespace Cast {
	class GraphicsContext {
	public:
		virtual ~GraphicsContext() = default;
		virtual void Init() = 0;
		virtual void SwapBuffer() = 0;
	};
}