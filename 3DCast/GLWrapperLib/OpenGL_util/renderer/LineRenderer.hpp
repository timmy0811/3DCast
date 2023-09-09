#pragma once

#include <memory>
#include <string>

#include "../core/Renderer.h"
#include "../misc/Primitive.hpp"

#include "../core/VertexBuffer.h"
#include "../core/VertexArray.h"
#include "../core/IndexBuffer.h"
#include "../core/VertexBufferLayout.h"
#include "../core/Shader.h"

namespace GL::renderer {
	class LineRenderer {
	public:
		std::unique_ptr<core::VertexBuffer> vb;
		std::unique_ptr<core::IndexBuffer> ib;
		std::unique_ptr<core::VertexBufferLayout> vbLayout;
		std::unique_ptr<core::VertexArray> va;

		core::Shader* shader;

		LineRenderer(int count, glm::vec4 color, const std::string& shaderVert, const std::string& shaderFrag)
			: shader(new core::Shader(shaderVert, shaderFrag)) {
			unsigned int* indices = new unsigned int[count];

			for (int i = 0; i < count; i++) {
				indices[i] = i;
			}

			ib = std::make_unique<core::IndexBuffer>(indices, count);
			vb = std::make_unique<core::VertexBuffer>(count, sizeof(primitive::vertex::Vertex3D));

			delete[] indices;

			vbLayout = std::make_unique<core::VertexBufferLayout>();
			vbLayout->Push<float>(3);	// Position

			va = std::make_unique<core::VertexArray>();
			va->AddBuffer(*vb, *vbLayout);

			shader->Bind();
			shader->SetUniform4f("u_Color", color.r, color.g, color.b, color.a);
		}

		~LineRenderer() {
			delete shader;
		}

		inline void Draw() {
			GLCall(glLineWidth(3));
			shader->Bind();
			va->Bind();
			ib->Bind();
			core::GLContext::Draw(*va, *ib, *shader, GL_LINES);
		}

		inline void DrawInstanced(const unsigned int count, const unsigned int instances) {
			GLCall(glLineWidth(3));
			core::GLContext::DrawInstancedLines(*va, *ib, *shader, count, instances);
		}
	};
}