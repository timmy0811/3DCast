#pragma once

#include "3DCast/Scene/Component/AbstractComponent.h"

#include "nfd.h"

namespace Cast::Component {
	struct MaterialComponent : Component {
		/* Bitmask:
		0: normal
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
		TextureInformation parallaxInfo;
		TextureInformation normalInfo;

		std::string diffuseFile;
		std::string specularFile;
		std::string parallaxFile;
		std::string normalFile;

		bool diffuseLoaded, specularLoaded, parallaxLoaded, normalLoaded;

		MaterialComponent(const MaterialComponent&) = default;
		MaterialComponent() {
			SetupSamplerMapping();
		}

		void SetupSamplerMapping() {
			samplerIndex = g_TextureManager.CreateSamplerMapping(
				diffuseInfo.bufferIndex,
				specularInfo.bufferIndex,
				parallaxInfo.bufferIndex,
				normalInfo.bufferIndex
			);
		}

		void UpdateSamplerMapping() {
			g_TextureManager.UpdateSamplerMapping(
				samplerIndex,
				diffuseInfo.bufferIndex,
				specularInfo.bufferIndex,
				parallaxInfo.bufferIndex,
				normalInfo.bufferIndex
			);
		}

		std::string ExtractFilename(const std::string& path) {
			size_t found = path.find_last_of("/\\");
			return path.substr(found + 1);
		}

#pragma region LOADERS
		// Load a normal texture from file
		void LoadDiffuseTexture(const std::string& path, bool flipUV = false) {
			diffuseInfo = g_TextureManager.AddDiffuseTexture(path, flipUV);
			diffuseFile = ExtractFilename(path);
			UpdateSamplerMapping();
			diffuseLoaded = true;
		}

		void LoadDiffuseTexture(API::Texture::Texture* texture) {
			diffuseInfo = g_TextureManager.AddDiffuseTexture(texture);
			diffuseFile = ExtractFilename(texture->GetPath());
			UpdateSamplerMapping();
			diffuseLoaded = true;
		}

		// Load a specular texture from file
		void LoadSpecularTexture(const std::string& path, bool flipUV = false) {
			specularInfo = g_TextureManager.AddSpecularTexture(path, flipUV);
			specularFile = ExtractFilename(path);
			UpdateSamplerMapping();
			specularLoaded = true;
		}

		void LoadSpecularTexture(API::Texture::Texture* texture) {
			specularInfo = g_TextureManager.AddSpecularTexture(texture);
			specularFile = ExtractFilename(texture->GetPath());
			UpdateSamplerMapping();
			specularLoaded = true;
		}

		// Load a parallax texture from file
		void LoadParallaxTexture(const std::string& path, bool flipUV = false) {
			parallaxInfo = g_TextureManager.AddParallaxTexture(path, flipUV);
			parallaxFile = ExtractFilename(path);
			UpdateSamplerMapping();
			parallaxLoaded = true;
		}

		void LoadParallaxTexture(API::Texture::Texture* texture) {
			parallaxInfo = g_TextureManager.AddParallaxTexture(texture);
			parallaxFile = ExtractFilename(texture->GetPath());
			UpdateSamplerMapping();
			parallaxLoaded = true;
		}

		// Load a normal map from file
		void LoadNormalTexture(const std::string& path, bool flipUV = false) {
			normalInfo = g_TextureManager.AddNormalTexture(path, flipUV);
			normalFile = ExtractFilename(path);
			UpdateSamplerMapping();
			normalLoaded = true;
		}

		void LoadNormalTexture(API::Texture::Texture* texture) {
			normalInfo = g_TextureManager.AddNormalTexture(texture);
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

		virtual void OnImGuiRender() override {
			if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen)) {
				float windowWidth = ImGui::GetWindowWidth();
				float textHeight = ImGui::GetTextLineHeightWithSpacing();

				// diffuse
				if (ImGui::TreeNodeEx("diffuse", ImGuiTreeNodeFlags_DefaultOpen)) {
					if (diffuseLoaded) {
						ImGui::Columns(2, nullptr, false);

						ImGui::BeginChild("ImageContainer", ImVec2(TEXTURE_THUMBNAIL_SIZE + 40.f, TEXTURE_THUMBNAIL_SIZE + 20.f), false);
						ImGui::Image((unsigned long long)diffuseInfo.textureId, { TEXTURE_THUMBNAIL_SIZE, TEXTURE_THUMBNAIL_SIZE });
						ImGui::EndChild();

						ImGui::NextColumn();
						ImGui::BeginChild("TextContainer", ImVec2(0, TEXTURE_THUMBNAIL_SIZE + 20.f), false);

						ImGui::Text("File: %s", diffuseFile.c_str());
						ImGui::Text("Dimension: %d x %d", (int)diffuseInfo.size.x, (int)diffuseInfo.size.y);

						ImGui::Dummy({ 0.f, textHeight * 2.f });
						if (ImGui::Button("Delete diffuse Texture")) {
							diffuseInfo = { 0, 0 };
							UpdateSamplerMapping();
							diffuseLoaded = false;
						}

						ImGui::EndChild();
						ImGui::Columns(1);
					}
					else {
						ImGui::Columns(2, nullptr, false);
						ImGui::BeginChild("TextContainer", ImVec2(windowWidth / 2.f - 10.f, 20.f), false);
						ImGui::Text("No diffuse Map Loaded");
						ImGui::EndChild();

						ImGui::NextColumn();

						ImGui::BeginChild("ButtonContainer", ImVec2(windowWidth / 2.f - 10.f, 20.f), false);

						if (ImGui::Button("Load diffuse lexture")) {
							std::string path = OpenFileDialoge();
							if (!path.empty()) {
								LoadDiffuseTexture(path);
							}
						}

						ImGui::EndChild();
						ImGui::Columns(1);
					}
					ImGui::TreePop();
				}

				ImGui::Separator();

				// Normal
				if (ImGui::TreeNodeEx("Normal", ImGuiTreeNodeFlags_DefaultOpen)) {
					if (normalLoaded) {
						ImGui::Columns(2, nullptr, false);

						ImGui::BeginChild("ImageContainer", ImVec2(TEXTURE_THUMBNAIL_SIZE + 40.f, TEXTURE_THUMBNAIL_SIZE + 20.f), false);
						ImGui::Image((unsigned long long)normalInfo.textureId, { TEXTURE_THUMBNAIL_SIZE, TEXTURE_THUMBNAIL_SIZE });
						ImGui::EndChild();

						ImGui::NextColumn();
						ImGui::BeginChild("TextContainer", ImVec2(0, TEXTURE_THUMBNAIL_SIZE + 20.f), false);

						ImGui::Text("File: %s", normalFile.c_str());
						ImGui::Text("Dimension: %d x %d", (int)normalInfo.size.x, (int)normalInfo.size.y);

						ImGui::Dummy({ 0.f, textHeight * 2.f });
						if (ImGui::Button("Delete normal Texture")) {
							normalInfo = { 0, 0 };
							UpdateSamplerMapping();
							normalLoaded = false;
						}

						ImGui::EndChild();
						ImGui::Columns(1);
					}
					else {
						ImGui::Columns(2, nullptr, false);
						ImGui::BeginChild("TextContainer", ImVec2(windowWidth / 2.f - 10.f, 20.f), false);
						ImGui::Text("No normal Map loaded");
						ImGui::EndChild();

						ImGui::NextColumn();

						ImGui::BeginChild("ButtonContainer", ImVec2(windowWidth / 2.f - 10.f, 20.f), false);

						if (ImGui::Button("Load normal Texture")) {
							std::string path = OpenFileDialoge();
							if (!path.empty()) {
								LoadNormalTexture(path);
								normalLoaded = true;
							}
						}

						ImGui::EndChild();
						ImGui::Columns(1);
					}
					ImGui::TreePop();
				}

				ImGui::Separator();

				// Specular
				if (ImGui::TreeNodeEx("Specular", ImGuiTreeNodeFlags_DefaultOpen)) {
					if (specularLoaded) {
						ImGui::Columns(2, nullptr, false);

						ImGui::BeginChild("ImageContainer", ImVec2(TEXTURE_THUMBNAIL_SIZE + 40.f, TEXTURE_THUMBNAIL_SIZE + 20.f), false);
						ImGui::Image((unsigned long long)specularInfo.textureId, { TEXTURE_THUMBNAIL_SIZE, TEXTURE_THUMBNAIL_SIZE });
						ImGui::EndChild();

						ImGui::NextColumn();
						ImGui::BeginChild("TextContainer", ImVec2(0, TEXTURE_THUMBNAIL_SIZE + 20.f), false);

						ImGui::Text("File: %s", specularFile.c_str());
						ImGui::Text("Dimension: %d x %d", (int)specularInfo.size.x, (int)specularInfo.size.y);

						ImGui::Dummy({ 0.f, textHeight * 2.f });
						if (ImGui::Button("Delete specular Texture")) {
							specularInfo = { 0, 0 };
							UpdateSamplerMapping();
							specularLoaded = false;
						}

						ImGui::EndChild();
						ImGui::Columns(1);
					}
					else {
						ImGui::Columns(2, nullptr, false);
						ImGui::BeginChild("TextContainer", ImVec2(windowWidth / 2.f - 10.f, 20.f), false);
						ImGui::Text("No specular Map loaded");
						ImGui::EndChild();

						ImGui::NextColumn();

						ImGui::BeginChild("ButtonContainer", ImVec2(windowWidth / 2.f - 10.f, 20.f), false);

						if (ImGui::Button("Load specular Texture")) {
							std::string path = OpenFileDialoge();
							if (!path.empty()) {
								LoadSpecularTexture(path);
								specularLoaded = true;
							}
						}

						ImGui::EndChild();
						ImGui::Columns(1);
					}
					ImGui::TreePop();
				}

				ImGui::Separator();

				// Parallax
				if (ImGui::TreeNodeEx("Parallax", ImGuiTreeNodeFlags_DefaultOpen)) {
					if (parallaxLoaded) {
						ImGui::Columns(2, nullptr, false);

						ImGui::BeginChild("ImageContainer", ImVec2(TEXTURE_THUMBNAIL_SIZE + 40.f, TEXTURE_THUMBNAIL_SIZE + 20.f), false);
						ImGui::Image((unsigned long long)parallaxInfo.textureId, { TEXTURE_THUMBNAIL_SIZE, TEXTURE_THUMBNAIL_SIZE });
						ImGui::EndChild();

						ImGui::NextColumn();
						ImGui::BeginChild("TextContainer", ImVec2(0, TEXTURE_THUMBNAIL_SIZE + 20.f), false);

						ImGui::Text("File: %s", parallaxFile.c_str());
						ImGui::Text("Dimension: %d x %d", (int)parallaxInfo.size.x, (int)parallaxInfo.size.y);

						ImGui::Dummy({ 0.f, textHeight * 2.f });
						if (ImGui::Button("Delete parallax Texture")) {
							parallaxInfo = { 0, 0 };
							UpdateSamplerMapping();
							parallaxLoaded = false;
						}

						ImGui::EndChild();
						ImGui::Columns(1);
					}
					else {
						ImGui::Columns(2, nullptr, false);
						ImGui::BeginChild("TextContainer", ImVec2(windowWidth / 2.f - 10.f, 20.f), false);
						ImGui::Text("No parallax Map Loaded");
						ImGui::EndChild();

						ImGui::NextColumn();

						ImGui::BeginChild("ButtonContainer", ImVec2(windowWidth / 2.f - 10.f, 20.f), false);

						if (ImGui::Button("Load parallax Texture")) {
							std::string path = OpenFileDialoge();
							if (!path.empty()) {
								LoadParallaxTexture(path);
								parallaxLoaded = true;
							}
						}

						ImGui::EndChild();
						ImGui::Columns(1);
					}
					ImGui::TreePop();
				}

				ImGui::Separator();

				// Display the current sampler index
				ImGui::Text("Sampler Index: %d", samplerIndex);
			}
		}
	};
}