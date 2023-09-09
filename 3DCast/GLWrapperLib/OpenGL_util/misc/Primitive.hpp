#pragma once

#include "../vendor/glm/glm.hpp"
#include "../vendor/glm/gtx/normal.hpp"

namespace GL::primitive {
	namespace vertex {
		struct Vertex3D {
			glm::vec3 Position;
		};

		struct VertexTextured2D {
			glm::vec2 Position;
			glm::vec4 Color;
			glm::vec2 TexCoords;
			int TexID;
		};

		struct Sprite2DVertex {
			glm::vec2 Position;
			glm::vec2 TexCoords;
			float TexID;
		};

		struct VertexTextured3D {
			glm::vec3 Position;
			glm::vec4 Color;
			glm::vec2 TexCoords;
			float TexID;
		};

		struct Vertex3DNormal {
			glm::vec3 Position;
			glm::vec4 Color;
			glm::vec2 TexCoords;
			glm::vec3 Normal;
			float TexID;
		};

		struct VertexMesh {
			glm::vec3 Position;
			glm::vec2 TexCoords;
			glm::vec3 Normal;
			float TexID;
		};

		struct Vertex3DLight {
			glm::vec3 Position;
			glm::vec4 LightColor;
		};

		struct Triangle {
			VertexTextured3D v1;
			VertexTextured3D v2;
			VertexTextured3D v3;
		};

		struct TriangleN {
			Vertex3DNormal v1;
			Vertex3DNormal v2;
			Vertex3DNormal v3;
		};

		struct TriangleLight {
			Vertex3DLight v1;
			Vertex3DLight v2;
			Vertex3DLight v3;
		};

		struct SymbolVertex {
			glm::vec2 Position;
			glm::vec2 uv;
			float background;
			float alpha;
		};

		struct Cube {
			Triangle* t[12];

			~Cube() {
				for (Triangle* ts : t) {
					delete ts;
				}
			};
		};
	}

	struct Face {
		vertex::Vertex3D vertices[4];
	};
}