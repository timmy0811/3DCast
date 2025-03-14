#pragma once

#include "3DCast/Core.h"
#include "3DCast/Misc/Helper.h"
#include "3DCast/Memory/Batching/BatchManager.h"
#include "3DCast/Memory/Batching/UID.h"
#include "3DCast/Renderer/Camera/Camera.h"
#include "3DCast/Scene/DataObjects/ShaderDataObjects.h"
#include "3DCast/Scene/Scene.h"
#include "3DCast/Scene/TextureManager.h"

#include <API/core/Buffer.h>
#include <API/core/IndexBuffer.h>
#include <API/core/Shader.h>
#include <API/core/VertexArray.h>
#include <API/core/VertexBufferLayout.h>
#include <API/texture/Texture.h>

#include <Vendor/glm/glm.hpp>
#include <vendor/glm/gtx/euler_angles.hpp>
#include <Vendor/glm/gtx/matrix_decompose.hpp>

#include <string>

#include "imgui.h"

#define SAMELINE_WIDGET_OFFSET ImGui::GetWindowWidth() / 3
#define TEXTURE_THUMBNAIL_SIZE 100.f

namespace Cast::Component {
	struct Component
	{
		virtual ~Component() = default;
		virtual void OnImGuiRender() {};
		virtual void Print() {};
	};
}