#pragma once

#include "debug/Debug.h"

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"

#include <GLEW/glew.h>

namespace GL::Core {
	class GLContext {
	public:
		static void Init();
		static void BindOrigFramebuffer();

		static void Clear();
		static void ClearDepthBufferOnly();

		static void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader, int mode = GL_TRIANGLES, int count = -1);
		static void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader, size_t count);
		static void DrawArray(const VertexArray& va, const Shader& shader, size_t first, size_t count);
		static void DrawInstancedLines(const VertexArray& va, const IndexBuffer& ib, const Shader& shader, size_t count, size_t instances);

	public:
		static inline int FramebufferOriginId;
	};
}
