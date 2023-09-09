#pragma once

#include <memory>
#include <map>
#include <string>

#include "../core/Renderer.h"
#include "../misc/Helper.hpp"
#include "../misc/Primitive.hpp"

#include "../core/VertexBuffer.h"
#include "../core/VertexArray.h"
#include "../core/IndexBuffer.h"
#include "../core/VertexBufferLayout.h"
#include "../core/Shader.h"
#include "../texture/Texture.h"

namespace GL::renderer {
	class FontRenderer {
	private:
		texture::Texture fontSheet;
		static inline std::map<int, GL::Helper::SymbolInformation> symbolsUnicode;
		static inline std::map<int, GL::Helper::SymbolInformation> symbolsGui;
		std::map<int, GL::Helper::SymbolInformation>* symbols;

		static inline bool m_SymbolsParsedGui;
		static inline bool m_SymbolsParsedAscii;

		unsigned int charHeight;
		int sheetHeight;
		int sheetWidth;
		int characterWidth;

		size_t count;
		bool unicode;

		std::unique_ptr<core::VertexBuffer> vb;
		std::unique_ptr<core::IndexBuffer> ib;
		std::unique_ptr<core::VertexBufferLayout> vbLayout;
		std::unique_ptr<core::VertexArray> va;

		glm::mat4 projection;
		glm::mat4 view;
		glm::vec3 translation;

		core::Shader* shader;

		Helper::SymbolInformation GatherSymbolInformation(int posX, int posY, int width) {
			GL::Helper::SymbolInformation information{};

			glm::ivec2 imgPosition;
			imgPosition.x = posX * characterWidth + characterWidth / 2;
			imgPosition.y = posY * charHeight;

			if (unicode) {
				information.uv.u0.x = (imgPosition.x - characterWidth / 2.f) / sheetWidth;
				information.uv.u3.x = (imgPosition.x - characterWidth / 2.f) / sheetWidth;
				information.uv.u1.x = (imgPosition.x - characterWidth / 2.f + width) / sheetWidth;
				information.uv.u2.x = (imgPosition.x - characterWidth / 2.f + width) / sheetWidth;
			}
			else {
				information.uv.u0.x = (imgPosition.x - width / 2.f) / sheetWidth;
				information.uv.u3.x = (imgPosition.x - width / 2.f) / sheetWidth;
				information.uv.u1.x = (imgPosition.x + width / 2.f) / sheetWidth;
				information.uv.u2.x = (imgPosition.x + width / 2.f) / sheetWidth;
			}

			information.uv.u0.y = (float)imgPosition.y / (float)sheetHeight;
			information.uv.u1.y = (float)imgPosition.y / (float)sheetHeight;
			information.uv.u2.y = (imgPosition.y + charHeight) / (float)sheetHeight;
			information.uv.u3.y = (imgPosition.y + charHeight) / (float)sheetHeight;

			information.width = width;

			return information;
		}

	public:
		explicit FontRenderer(const std::string& imgPath, const std::string& fontPath, int capacity, glm::vec2 winSize, const bool unicode = false, unsigned int sheetId = 0)
			:fontSheet(imgPath, true), shader(new core::Shader("res/shaders/font/shader_font_stylized.vert", "res/shaders/font/shader_font_stylized.frag"))
		{
			projection = glm::ortho(0.0f, winSize.x, 0.0f, winSize.y, -1.0f, 1.0f);
			translation = glm::vec3(0.f, 0.f, 0.f);
			view = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, 0.f));
			this->unicode = unicode;

			unsigned int* indices = new unsigned int[capacity * 6];

			unsigned int offset = 0;
			for (size_t i = 0; i < capacity * 6; i += 6) {
				indices[i + 0] = 0 + offset;
				indices[i + 1] = 1 + offset;
				indices[i + 2] = 2 + offset;

				indices[i + 3] = 2 + offset;
				indices[i + 4] = 3 + offset;
				indices[i + 5] = 0 + offset;

				offset += 4;
			}

			ib = std::make_unique<core::IndexBuffer>(indices, capacity * 6);
			vb = std::make_unique<core::VertexBuffer>(capacity * 4, sizeof(primitive::vertex::Sprite2DVertex));

			delete[] indices;

			vbLayout = std::make_unique<core::VertexBufferLayout>();
			vbLayout->Push<float>(2);	// Position
			vbLayout->Push<float>(2);	// UVs
			vbLayout->Push<float>(1);	// background
			vbLayout->Push<float>(1);	// alpha

			va = std::make_unique<core::VertexArray>();
			va->AddBuffer(*vb, *vbLayout);

			shader->Bind();
			shader->SetUniformMat4f("u_MVP", projection * view * glm::translate(glm::mat4(1.f), translation));

			int id = fontSheet.GetBoundPort();
			shader->SetUniform1i("u_FontSheetSampler", id);

			// Sheet
			sheetHeight = fontSheet.GetHeight();
			sheetWidth = fontSheet.GetWidth();

			constexpr int charsPerRow = 16;
			charHeight = sheetHeight / charsPerRow;
			characterWidth = sheetWidth / charsPerRow;

			if (unicode) {
				if (symbolsUnicode.size() == 0) ParseSymbols(fontPath, this->unicode);
				symbols = &symbolsUnicode;
			}
			else {
				if (symbolsGui.size() == 0) ParseSymbols(fontPath, this->unicode);
				symbols = &symbolsGui;
			}
		}

		~FontRenderer() {
			delete shader;
		}

		void BindFontSheet(unsigned int slot) {
			fontSheet.Bind(slot);
		}

		void ParseSymbols(const std::string& fontPath, const bool unicode = false) {
			LOGC("Parsing Fonts", LOG_COLOR::SPECIAL_A);

			YAML::Node mainNode = YAML::LoadFile(fontPath);

			GL::Helper::SymbolInformation informationUnknown = GatherSymbolInformation(
				mainNode["unknown"]["position"][0].as<int>(),
				mainNode["unknown"]["position"][1].as<int>(),
				mainNode["unknown"]["width"].as<unsigned int>()
			);

			for (auto symbol : mainNode["characters"]) {
				const char character = symbol.first.as<char>();

				int newWidth = (int)(symbol.second["width"].as<int>() / (255.f / sheetWidth));

				const GL::Helper::SymbolInformation& information = GatherSymbolInformation(
					symbol.second["position"][0].as<int>(),
					symbol.second["position"][1].as<int>() - (unicode ? 2 : 0),
					newWidth
				);

				if (unicode) {
					symbolsUnicode[255] = informationUnknown;
					symbolsUnicode[character] = information;
				}
				else {
					symbolsGui[255] = informationUnknown;
					symbolsGui[character] = information;
				}
			}
		}

		void Clear() {
			vb->Empty();
		}

		/// <summary>
		///
		/// </summary>
		/// <param name="text"></param>
		/// <param name="position"></param>
		/// <param name="size"></param>
		/// <param name="background"></param>
		/// <returns>Returns the maximum width of the printed text</returns>
		float PrintMultilineText(const char* text, const glm::vec2& position, float size = 1.f, const glm::vec4& background = {}) {
			const char* symPtr = text;
			glm::vec2 symbolPosition = position;
			float widthTotal = 0.f, widthLine = 0.f;

			while (*symPtr != '\0') {
				const char symbol = *symPtr;
				if (symbol == '\n' || *(symPtr + 1) == '\0') {
					widthTotal = std::max(widthTotal, widthLine);
					widthLine = 0.f;

					if (symbol == '\n') {
						symbolPosition.x = position.x;
						symbolPosition.y -= (float)charHeight * size;

						symPtr++;
						continue;
					}
				}

				Helper::SymbolInformation information;
				if (symbols->find(symbol) != symbols->end()) {
					information = (*symbols)[symbol];
				}
				else {
					information = (*symbols)[255];
				}

				widthLine += AddLetter(information, symbolPosition, size, background);
				symbolPosition.x += information.width * size;

				symPtr++;
			}

			return widthTotal;
		}

		float AddLetter(const Helper::SymbolInformation& information, const glm::vec2& position, float size = 1.f, const glm::vec4& background = {}) {
			primitive::vertex::SymbolVertex v[4];

			for (int i = 0; i < 4; i++) {
				v[i].background = (float)Helper::mapRGBToInt(background);
				v[i].alpha = background.a;
			}

			float w = information.width * size;
			float h = charHeight * size;

			v[0].Position = position;
			v[1].Position = position + glm::vec2(w, 0.f);
			v[2].Position = position + glm::vec2(w, h);
			v[3].Position = position + glm::vec2(0.f, h);

			v[0].uv = information.uv.u0;
			v[1].uv = information.uv.u1;
			v[2].uv = information.uv.u2;
			v[3].uv = information.uv.u3;

			vb->AddVertexData(v, sizeof(primitive::vertex::SymbolVertex) * 4);
			count++;

			return w;
		}

		inline void Draw() {
			shader->Bind();
			core::GLContext::Draw(*va, *ib, *shader, GL_TRIANGLES, (int)count * 6);
			shader->Unbind();
		}

		inline const size_t getCount() const { return count; }
	};
}