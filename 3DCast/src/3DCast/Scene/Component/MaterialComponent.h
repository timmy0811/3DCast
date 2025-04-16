#pragma once

#include "3DCast/Scene/Component/AbstractComponent.h"
#include "3DCast/Scene/DeferredSamplerRegistry.h"

#include <imgui.h>

#include "nfd.h"

#define TEXTURE_THUMBNAIL_SIZE 100.f

namespace Cast::Component {
	struct MaterialComponent : Component {
#pragma region DATA
		// Sampler index used by vertex attribute for bindless textures
		unsigned short samplerIndex = 0;

		TextureInformation diffuseInfo;
		TextureInformation specularInfo;
		TextureInformation parallaxInfo;
		TextureInformation normalInfo;

		std::string diffuseFile;
		std::string specularFile;
		std::string parallaxFile;
		std::string normalFile;

		bool diffuseLoaded = false, specularLoaded = false, parallaxLoaded = false, normalLoaded = false;

		float windowWidth = 0.f;
		float textHeight = 0.f;
#pragma endregion

#pragma region CONSTRUCTOR
		MaterialComponent(const MaterialComponent&) = default;
		MaterialComponent() {
			SetupSamplerMapping();
		}
#pragma endregion

#pragma region UTILITY
		void SetupSamplerMapping() {
			samplerIndex = SamplerRegistry.CreateSamplerMapping(
				diffuseInfo.transformRegistryIndex,
				specularInfo.transformRegistryIndex,
				parallaxInfo.transformRegistryIndex,
				normalInfo.transformRegistryIndex
			);
		}

		void UpdateSamplerMapping() {
			SamplerRegistry.UpdateSamplerMapping(
				samplerIndex,
				diffuseInfo.transformRegistryIndex,
				specularInfo.transformRegistryIndex,
				parallaxInfo.transformRegistryIndex,
				normalInfo.transformRegistryIndex
			);
		}

		std::string ExtractFilename(const std::string& path) {
			size_t found = path.find_last_of("/\\");
			return path.substr(found + 1);
		}

#pragma region LOADERS
		// Load a normal texture from file
		void LoadDiffuseTexture(const std::string& path, bool flipUV = false) {
			diffuseInfo = SamplerRegistry.AddDiffuseTexture(path, flipUV);
			diffuseFile = ExtractFilename(path);
			UpdateSamplerMapping();
			diffuseLoaded = true;
		}

		void LoadDiffuseTexture(Ref<API::Texture::Texture> texture) {
			diffuseInfo = SamplerRegistry.AddDiffuseTexture(texture);
			diffuseFile = ExtractFilename(texture->GetPath());
			UpdateSamplerMapping();
			diffuseLoaded = true;
		}

		// Load a specular texture from file
		void LoadSpecularTexture(const std::string& path, bool flipUV = false) {
			specularInfo = SamplerRegistry.AddSpecularTexture(path, flipUV);
			specularFile = ExtractFilename(path);
			UpdateSamplerMapping();
			specularLoaded = true;
		}

		void LoadSpecularTexture(Ref<API::Texture::Texture> texture) {
			specularInfo = SamplerRegistry.AddSpecularTexture(texture);
			specularFile = ExtractFilename(texture->GetPath());
			UpdateSamplerMapping();
			specularLoaded = true;
		}

		// Load a parallax texture from file
		void LoadParallaxTexture(const std::string& path, bool flipUV = false) {
			parallaxInfo = SamplerRegistry.AddParallaxTexture(path, flipUV);
			parallaxFile = ExtractFilename(path);
			UpdateSamplerMapping();
			parallaxLoaded = true;
		}

		void LoadParallaxTexture(Ref<API::Texture::Texture> texture) {
			parallaxInfo = SamplerRegistry.AddParallaxTexture(texture);
			parallaxFile = ExtractFilename(texture->GetPath());
			UpdateSamplerMapping();
			parallaxLoaded = true;
		}

		// Load a normal map from file
		void LoadNormalTexture(const std::string& path, bool flipUV = false) {
			normalInfo = SamplerRegistry.AddNormalTexture(path, flipUV);
			normalFile = ExtractFilename(path);
			UpdateSamplerMapping();
			normalLoaded = true;
		}

		void LoadNormalTexture(Ref<API::Texture::Texture> texture) {
			normalInfo = SamplerRegistry.AddNormalTexture(texture);
			normalFile = ExtractFilename(texture->GetPath());
			UpdateSamplerMapping();
			normalLoaded = true;
		}
#pragma endregion

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

		void RenderMaterialMapImGui(const std::string& typeStr, bool& isLoaded, TextureInformation& info, const std::string& path, const std::function<void(const std::string& path, bool flipUV)>& loadProc) {
			ImGui::SeparatorText(typeStr.c_str());

			if (isLoaded)
			{
				if (ImGui::BeginTable("MaterialTable", 2))
				{
					ImGui::TableNextRow();

					ImGui::TableSetColumnIndex(0);
					ImGui::BeginChild("ImageContainer", ImVec2(TEXTURE_THUMBNAIL_SIZE + 40.f, TEXTURE_THUMBNAIL_SIZE + 20.f), false);
					ImGui::Image((ImTextureID)info.textureId, ImVec2(TEXTURE_THUMBNAIL_SIZE, TEXTURE_THUMBNAIL_SIZE));
					ImGui::EndChild();

					ImGui::TableSetColumnIndex(1);
					ImGui::BeginChild("TextContainer", ImVec2(0, TEXTURE_THUMBNAIL_SIZE + 20.f), false);
					ImGui::Text("File: %s", path.c_str());
					ImGui::Text("Dimension: %d x %d", (int)info.size.x, (int)info.size.y);
					ImGui::Dummy(ImVec2(0.f, textHeight * 2.f));
					if (ImGui::Button("Remove Texture"))
					{
						info = { 0, 0 };
						UpdateSamplerMapping();
						isLoaded = false;
					}
					ImGui::EndChild();

					ImGui::EndTable();
				}
			}
			else
			{
				if (ImGui::BeginTable("MaterialTable", 2))
				{
					ImGui::TableNextRow();

					ImGui::TableSetColumnIndex(0);
					ImGui::BeginChild("TextContainer", ImVec2(windowWidth / 2.f - 10.f, 25.f), false);
					ImGui::Text("No Texture loaded");
					ImGui::EndChild();

					ImGui::TableSetColumnIndex(1);
					ImGui::BeginChild("ButtonContainer", ImVec2(windowWidth / 2.f - 10.f, 25.f), false);
					std::string buttonText = "Load " + typeStr + " Texture";
					if (ImGui::Button(buttonText.c_str()))
					{
						std::string path = OpenFileDialoge();
						if (!path.empty())
						{
							loadProc(path, false);
						}
					}
					ImGui::EndChild();

					ImGui::EndTable();
				}
			}
		}
#pragma endregion

#pragma region OVERRIDE
		static inline const Cast::Component::Type GetType() { return Type::Material; }
		static inline std::string GetName() { return "Material"; }

		virtual UIResponse OnImGuiRender() override {
			bool isOpen = ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap);
			ImGui::SameLine();

			float xOffset = ImGui::GetContentRegionAvail().x - 80.0f;
			if (xOffset > 0.0f) {
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + xOffset);
			}

			if (ImGui::SmallButton("Remove##Material"))
				return { UIResponse::Code::Remove, Type::Material };

			if (isOpen) {
				windowWidth = ImGui::GetWindowWidth();
				textHeight = ImGui::GetTextLineHeightWithSpacing();

				RenderMaterialMapImGui("Diffuse", diffuseLoaded, diffuseInfo, diffuseFile,
					[this](const std::string& path, bool flipUV) { LoadDiffuseTexture(path, flipUV); });

				RenderMaterialMapImGui("Normal", normalLoaded, normalInfo, normalFile,
					[this](const std::string& path, bool flipUV) { LoadNormalTexture(path, flipUV); });

				RenderMaterialMapImGui("Specular", specularLoaded, specularInfo, specularFile,
					[this](const std::string& path, bool flipUV) { LoadSpecularTexture(path, flipUV); });

				RenderMaterialMapImGui("Parallax", parallaxLoaded, parallaxInfo, parallaxFile,
					[this](const std::string& path, bool flipUV) { LoadParallaxTexture(path, flipUV); });
			}

			return {};
		}
#pragma endregion
	};
}