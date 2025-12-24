#include "castpch.h"
#include "CSMProcessor.h"
#include "3DCast/Scene/Scene.h"
#include "3DCast/Renderer/Camera/Camera.h"
#include "API/core/RendererCommand.h"
#include "3DCast/Scene/Registry/ShaderCacheRegistry.h"
#include "3DCast/Data/ShaderDataObjects/Light.h"
#include "3DCast/Scene/Component/LightComponent.h"
#include <vendor/glm/gtc/matrix_transform.hpp>
#include <cfloat>
#include <cmath>

Cast::Renderer::CSMProcessor::CSMProcessor()
{
}

void Cast::Renderer::CSMProcessor::Init(const std::array<glm::ivec2, API::Advanced::CascadedShadowMap::CascadeCount>& cascadeSizes)
{
    CascadedShadowMap.reset(API::Advanced::CascadedShadowMap::Create(cascadeSizes));
}

void Cast::Renderer::CSMProcessor::OnRender(const Scene& scene, const Camera& camera)
{
	if (!CascadedShadowMap)
		return;

	// Find castable light
	entt::entity caster = scene.GetActiveShadowDirectionalLight();
	if (caster == entt::null)
	{
		// No active caster: just clear cascades and return
		for (int c = 0; c < CascadedShadowMap->GetCascadeCount(); ++c)
			CascadedShadowMap->BindAndClear(c);
		HasShadowMapDataValue = false;
		return;
	}

	auto& lightComp = scene.GetRegistry().get<Cast::Component::LightComponent>(caster);
	auto* dir = reinterpret_cast<Cast::DirectionalLightShaderObject*>(lightComp.Light);
	glm::vec3 lightDir = glm::normalize(dir->direction);
	if (glm::dot(lightDir, lightDir) < 1e-6f) lightDir = glm::vec3(-0.2f, -1.0f, -0.3f);
	ShadowLightDir = lightDir;
	HasShadowMapDataValue = true;

	// Camera data
	const glm::mat4 V = camera.GetViewMat();
	const glm::mat4 Vinv = glm::inverse(V);
	const glm::mat4 P = camera.GetProjectionMat();

	// Extract camera params
	const float m11 = P[1][1];
    const float m00 = P[0][0];
	const float A = P[2][2];       // -(f+n)/(f-n)
	const float B = P[3][2];       // -(2fn)/(f-n)
	const float tanHalfFov = 1.0f / m11;
	const float aspect = m11 / m00;

	// Recover near/far
	const float nearPlane = B / (A - 1.0f);
	const float farPlane  = B / (A + 1.0f);

	// Compute cascade splits (practical split)
	float splits[CascadeCount + 1];
	splits[0] = nearPlane;
	splits[CascadeCount] = farPlane;

	for (int i = 1; i < CascadeCount; ++i)
	{
		constexpr float lambda = 0.8f;
		float p = (float)i / (float)CascadeCount;
		float logi = nearPlane * std::pow(farPlane / nearPlane, p);
		float lini = nearPlane + (farPlane - nearPlane) * p;
		splits[i] = lini * (1.0f - lambda) + logi * lambda;
	}

	// Update shader
	auto shadowShader = Cast::ShaderCacheRegistryInstance.GetHandle("geometry_pass");
	shadowShader->Bind();

	// Depth-only rendering of geometry into each cascade FBO
	API::Core::RenderCommand::SetDepthTest(true);
	API::Core::RenderCommand::SetDepthTestFunc(API::Core::DepthFunction::Less);
	API::Core::RenderCommand::SetStencilTest(false);
	API::Core::RenderCommand::CullFace(API::Core::Face::Back);

	for (int c = 0; c < CascadeCount; ++c)
	{
		const float zn = splits[c];
		const float zf = splits[c + 1];
		CascadeSplits[c] = zf;

		// Frustum corners in view space for this slice
		const float xn_n = tanHalfFov * zn * aspect;
		const float yn_n = tanHalfFov * zn;
		const float xn_f = tanHalfFov * zf * aspect;
		const float yn_f = tanHalfFov * zf;

		glm::vec3 frustumVS[8] = {
			{-xn_n, -yn_n, -zn}, { xn_n, -yn_n, -zn}, { xn_n,  yn_n, -zn}, { -xn_n,  yn_n, -zn},
			{-xn_f, -yn_f, -zf}, { xn_f, -yn_f, -zf}, { xn_f,  yn_f, -zf}, { -xn_f,  yn_f, -zf}
		};

		glm::vec3 frustumWS[8];
		glm::vec3 center(0.0f);
		for (int i = 0; i < 8; ++i)
		{
			glm::vec4 w = Vinv * glm::vec4(frustumVS[i], 1.0f);
			frustumWS[i] = glm::vec3(w);
			center += frustumWS[i];
		}

		center *= 1.0f / 8.0f;

		// Build light view matrix
		const float boundsRadius = glm::length(frustumWS[6] - frustumWS[0]);
		glm::vec3 eye = center - lightDir * boundsRadius;
		glm::mat4 lightView = glm::lookAt(eye, center, glm::vec3(0, 1, 0));

		// Fit ortho bounds in light space
		glm::vec3 minLS(FLT_MAX), maxLS(-FLT_MAX);
		for (auto i : frustumWS)
		{
			glm::vec4 ls = lightView * glm::vec4(i, 1.0f);
			minLS = glm::min(minLS, glm::vec3(ls));
			maxLS = glm::max(maxLS, glm::vec3(ls));
		}

		// Padding to z range
		constexpr float zPad = 10.0f;
		minLS.z -= zPad;
		maxLS.z += zPad;

		glm::mat4 lightProj = glm::ortho(minLS.x, maxLS.x, minLS.y, maxLS.y, -maxLS.z, -minLS.z);

		CascadedShadowMap->BindAndClear(c);
		shadowShader->SetUniformMat4f("u_View", lightView);
		shadowShader->SetUniformMat4f("u_Projection", lightProj);
		CascadeLightVP[c] = lightProj * lightView; // store for lighting pass

		// Render geometry
		scene.OnDeferredRender();
	}
}
