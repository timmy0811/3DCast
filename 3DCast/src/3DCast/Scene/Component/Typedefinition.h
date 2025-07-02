#pragma once

#include <vector>
#include <string>

namespace Cast::Component {
	static std::vector<std::string> AddableComponentIds = {
		"Transform",
		"Camera",
		"Light",
		"Mesh",
		"Custom Mesh",
		"Material",
		"Script",
		"Behaviour",
		"RigidBody",
		"Collider",
		"Audio",
		"Particle",
		"Animation",
		"UI",
		"Shader",
		"PBR Material",
		"Rasterizable",
		"PBR Renderable"
	};

	enum class Type
	{
		Unclassified = 0,
		Transform,
		Camera,
		Light,
		Mesh,
		CustomMesh,
		Material,
		Script,
		Behaviour,
		RigidBody,
		Collider,
		Audio,
		Particle,
		Animation,
		UI,
		Shader,
		Tag,
		PBRMat,
		Raster,
		PBR
	};

	struct UIResponse {
		enum Code {
			None = 0,
			Remove
		} action;

		Type component = Type::Unclassified;
	};
}