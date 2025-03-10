#pragma once

#include "3DCast/Scene/Component/AbstractComponent.h"

#include "nfd.h"

namespace Cast::Component {
	struct MaterialComponent : Component {
		/* Bitmask:
		0: Diffuse
		1: Specular
		2: Normal
		3: Emmissive
		4: Shine
		5: Opacity
		6: Reflectance
		*/
		uint16_t textureEnabled = 0;
		bool isDeffered = true;

		// Sampler index used by vertex attribute for bindless textures
		unsigned short samplerIndex = 0;

		TextureInformation diffuseInfo;
		TextureInformation specularInfo;
		TextureInformation shineInfo;
		TextureInformation normalInfo;

		MaterialComponent(const MaterialComponent&) = default;
		MaterialComponent() {
			SetupSamplerMapping();
		}

		void SetupSamplerMapping() {
			samplerIndex = g_TextureManager.CreateSamplerMapping(
				diffuseInfo.bufferIndex,
				specularInfo.bufferIndex,
				shineInfo.bufferIndex,
				normalInfo.bufferIndex
			);
		}

		void UpdateSamplerMapping() {
			g_TextureManager.UpdateSamplerMapping(
				samplerIndex,
				diffuseInfo.bufferIndex,
				specularInfo.bufferIndex,
				shineInfo.bufferIndex,
				normalInfo.bufferIndex
			);
		}

		// Load a diffuse texture from file
		void LoadDiffuseTexture(const std::string& path, bool flipUV = false) {
			diffuseInfo = g_TextureManager.AddDiffuseTexture(path, flipUV);
			Helper::setBit(textureEnabled, 0, true);
			UpdateSamplerMapping();
		}

		// Load a specular texture from file
		void LoadSpecularTexture(const std::string& path, bool flipUV = false) {
			specularInfo = g_TextureManager.AddSpecularTexture(path, flipUV);
			Helper::setBit(textureEnabled, 1, true);
			UpdateSamplerMapping();
		}

		// Load a shininess texture from file
		void LoadShininessTexture(const std::string& path, bool flipUV = false) {
			shineInfo = g_TextureManager.AddShininessTexture(path, flipUV);
			Helper::setBit(textureEnabled, 4, true);
			UpdateSamplerMapping();
		}

		// Load a normal map from file
		void LoadNormalTexture(const std::string& path, bool flipUV = false) {
			normalInfo = g_TextureManager.AddNormalTexture(path, flipUV);
			Helper::setBit(textureEnabled, 2, true);
			UpdateSamplerMapping();
		}

		std::string OpenFileDialoge() {
			nfdu8char_t* outPath;
			nfdu8filteritem_t filters[1] = { { "Texture", "png,jpg" } };
			nfdopendialogu8args_t args = { 0 };
			args.filterList = filters;
			args.filterCount = 1;
			nfdresult_t result = NFD_OpenDialogU8_With(&outPath, &args);
			if (result == NFD_OKAY)
			{
				std::string outPathStr(outPath);
				LOG_CORE_TRACE("Loading Texture: {0}", outPathStr);
				NFD_FreePathU8(outPath);
				return outPathStr;
			}
			else if (result == NFD_CANCEL)
			{
				LOG_CORE_TRACE("Cancelled file dialoge");
			}
			else
			{
				LOG_CORE_ERROR("Error: {0}", NFD_GetError());
			}

			return "";
		}

		virtual void OnImGuiRender() override {
			if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen)) {
				// Diffuse
				static bool diffuseLoaded = false;
				if (!diffuseLoaded && ImGui::Button("Load Diffuse Texture")) {
					std::string path = OpenFileDialoge();
					if (!path.empty()) {
						LoadDiffuseTexture(path);
						diffuseLoaded = true;
					}
				}

				if (diffuseLoaded) {
					ImGui::Image((unsigned long long)diffuseInfo.textureId, { TEXTURE_THUMBNAIL_SIZE, TEXTURE_THUMBNAIL_SIZE });
					ImGui::SameLine();
					if (ImGui::Button("Delete Diffuse Texture")) {
						diffuseInfo = { 0, 0 };
						UpdateSamplerMapping();
						diffuseLoaded = false;
					}
				}

				ImGui::Separator();

				// Normal
				static bool normalLoaded = false;
				if (!normalLoaded && ImGui::Button("Load Normal Texture")) {
					std::string path = OpenFileDialoge();
					if (!path.empty()) {
						LoadNormalTexture(path);
						normalLoaded = true;
					}
				}

				if (normalLoaded) {
					ImGui::Image((unsigned long long)normalInfo.textureId, { TEXTURE_THUMBNAIL_SIZE, TEXTURE_THUMBNAIL_SIZE });
					ImGui::SameLine();
					if (ImGui::Button("Delete Normal Texture")) {
						normalInfo = { 0, 0 };
						UpdateSamplerMapping();
						normalLoaded = false;
					}
				}

				// Ability to flip each axis of the normal map

				ImGui::Separator();

				// Specular
				static bool specLoaded = false;
				if (!specLoaded && ImGui::Button("Load Specular Texture")) {
					std::string path = OpenFileDialoge();
					if (!path.empty()) {
						LoadSpecularTexture(path);
						specLoaded = true;
					}
				}

				if (specLoaded) {
					ImGui::Image((unsigned long long)specularInfo.textureId, { TEXTURE_THUMBNAIL_SIZE, TEXTURE_THUMBNAIL_SIZE });
					ImGui::SameLine();
					if (ImGui::Button("Delete Specular Texture")) {
						specularInfo = { 0, 0 };
						UpdateSamplerMapping();
						specLoaded = false;
					}
				}

				ImGui::Separator();

				// Display the current sampler index
				ImGui::Text("Sampler Index: %d", samplerIndex);
			}
		}
	};
}