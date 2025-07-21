#pragma once

#include <GL/glew.h>

#include "3DCast/Application.h"
#include "3DCast/Core/Log.h"
#include "3DCast/Core/Timestep.h"
#include "3DCast/Core.h"
#include "3DCast/Data/GlobalShared.h"

#include "3DCast/Scene/DeferredSamplerRegistry.h"
#include "3DCast/Scene/Scene.h"
#include "3DCast/Scene/Entity.h"
#include "3DCast/Scene/ObjectCreator.h"
#include "3DCast/Scene/Component/Component.h"
#include "3DCast/Layer/Layer.h"
#include "3DCast/ImGui/ImGuiLayer.h"

#include "3DCast/Input/Input.h"
#include "3DCast/Input/MousebuttonCodes.h"
#include "3DCast/Input/KeyCodes.h"

#include "3DCast/Renderer/Renderer.h"
#include "3DCast/Renderer/Camera/Camera.h"
#include "3DCast/Renderer/Camera/OrthographicCamera.h"
#include "3DCast/Renderer/Camera/PerspectiveCamera.h"
#include "3DCast/Renderer/Skybox.h"

#include "3DCast/Renderer/Data/RasterizationPipelineObjects.h"
#include "3DCast/Scene/SceneShaderCache.h"
#include "3DCast/Memory/Batching/BatchManager.h"
#include "3DCast/Data/ShaderDataObjects/Vertex.h"

// --- Entry Point ---
//#include "3DCast/EntryPoint.h"
// -------------------
