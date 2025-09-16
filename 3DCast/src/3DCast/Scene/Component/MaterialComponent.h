#pragma once

#include "3DCast/Scene/Component/AbstractComponent.h"
#include "../Registry/DeferredSamplerStore.h"

#include <imgui.h>
#include <optional>

#include "nfd.h"
#include "3DCast/Scene/Registry/TextureCacheRegistry.h"

#define TEXTURE_THUMBNAIL_SIZE 90.f

namespace Cast::Component
{
	struct MaterialComponent final : Component
	{
#pragma region DATA
		// Sampler index used by vertex attribute for bindless textures
		unsigned short samplerIndex = 0;

		DeferredTextureInformation diffuseInfo;
		UID diffuseID = UID::None();

		DeferredTextureInformation specularInfo;
		UID specularID = UID::None();

		DeferredTextureInformation parallaxInfo;
		UID parallaxID = UID::None();

		DeferredTextureInformation normalInfo;
		UID normalID = UID::None();

		bool isCustomMaterial = false;
		bool isPrivateMaterialCreated = false;
		unsigned short currentMaterialInfo = 0;
		Material currentMaterial{};
		Material privateMaterial{};

		std::string diffuseFile;
		std::string specularFile;
		std::string parallaxFile;
		std::string normalFile;

		std::string selectedItem = "Default";

		bool diffuseLoaded = false, specularLoaded = false, parallaxLoaded = false, normalLoaded = false;

		float textHeight = 0.f;
#pragma endregion

#pragma region CONSTRUCTOR
		MaterialComponent(const MaterialComponent&) = default;

		// Todo: Check if move fields are up to date
		MaterialComponent(MaterialComponent&& other) noexcept
			: samplerIndex(other.samplerIndex),
			  diffuseInfo(other.diffuseInfo),
			  diffuseID(other.diffuseID),
			  specularInfo(other.specularInfo),
			  specularID(other.specularID),
			  parallaxInfo(other.parallaxInfo),
			  parallaxID(other.parallaxID),
			  normalInfo(other.normalInfo),
			  normalID(other.normalID),
			  isCustomMaterial(other.isCustomMaterial),
			  isPrivateMaterialCreated(other.isPrivateMaterialCreated),
			  currentMaterialInfo(other.currentMaterialInfo),
			  currentMaterial(other.currentMaterial),
			  privateMaterial(other.privateMaterial),
			  diffuseFile(std::move(other.diffuseFile)),
			  specularFile(std::move(other.specularFile)),
			  parallaxFile(std::move(other.parallaxFile)),
			  normalFile(std::move(other.normalFile)),
			  diffuseLoaded(other.diffuseLoaded),
			  specularLoaded(other.specularLoaded),
			  parallaxLoaded(other.parallaxLoaded),
			  normalLoaded(other.normalLoaded),
			  textHeight(other.textHeight)
		{
			other.samplerIndex = 0;
			other.diffuseID = UID::None();
			other.specularID = UID::None();
			other.parallaxID = UID::None();
			other.normalID = UID::None();
			other.isPrivateMaterialCreated = false;
		}

		MaterialComponent& operator=(MaterialComponent&& other) noexcept
		{
			if (this != &other)
			{
				// Transfer all data members
				samplerIndex = other.samplerIndex;
				diffuseInfo = other.diffuseInfo;
				diffuseID = other.diffuseID;
				specularInfo = other.specularInfo;
				specularID = other.specularID;
				parallaxInfo = other.parallaxInfo;
				parallaxID = other.parallaxID;
				normalInfo = other.normalInfo;
				normalID = other.normalID;
				isCustomMaterial = other.isCustomMaterial;
				isPrivateMaterialCreated = other.isPrivateMaterialCreated;
				currentMaterialInfo = other.currentMaterialInfo;
				currentMaterial = other.currentMaterial;
				privateMaterial = other.privateMaterial;
				diffuseFile = std::move(other.diffuseFile);
				specularFile = std::move(other.specularFile);
				parallaxFile = std::move(other.parallaxFile);
				normalFile = std::move(other.normalFile);
				diffuseLoaded = other.diffuseLoaded;
				specularLoaded = other.specularLoaded;
				parallaxLoaded = other.parallaxLoaded;
				normalLoaded = other.normalLoaded;
				textHeight = other.textHeight;

				other.samplerIndex = 0;
				other.diffuseID = UID::None();
				other.specularID = UID::None();
				other.parallaxID = UID::None();
				other.normalID = UID::None();
				other.isPrivateMaterialCreated = false;
			}

			return *this;
		}

		explicit MaterialComponent(const std::string& defaultMat = "Default")
		{
			currentMaterial = MaterialCacheRegistryInstance.Get(defaultMat);
			currentMaterialInfo = DeferredSamplerStoreInstance.AddCustomMaterial(currentMaterial);
			SetupSamplerMapping();
		}

		~MaterialComponent() override
		{
			DeferredSamplerStoreInstance.RemoveSamplerMapping(samplerIndex);

			TextureCacheRegistryInstance.Remove(diffuseID);
			DeferredSamplerStoreInstance.RemoveDiffuseTexture(diffuseInfo.samplerArrayIndex);

			TextureCacheRegistryInstance.Remove(specularID);
			DeferredSamplerStoreInstance.RemoveSpecularTexture(specularInfo.samplerArrayIndex);

			TextureCacheRegistryInstance.Remove(parallaxID);
			DeferredSamplerStoreInstance.RemoveParallaxTexture(parallaxInfo.samplerArrayIndex);

			TextureCacheRegistryInstance.Remove(normalID);
			DeferredSamplerStoreInstance.RemoveNormalTexture(normalInfo.samplerArrayIndex);

			if (isPrivateMaterialCreated)
				MaterialCacheRegistryInstance.Remove(privateMaterial.id);
		}
#pragma endregion

#pragma region UTILITY
		void SetupSamplerMapping()
		{
			samplerIndex = DeferredSamplerStoreInstance.CreateSamplerMapping(
				diffuseInfo.samplerArrayIndex,
				specularInfo.samplerArrayIndex,
				parallaxInfo.samplerArrayIndex,
				normalInfo.samplerArrayIndex,
				currentMaterialInfo
			);
		}

		void UpdateSamplerMapping() const
		{
			if (isCustomMaterial)
			{
				DeferredSamplerStoreInstance.UpdateSamplerMapping(
				samplerIndex,
				diffuseInfo.samplerArrayIndex,
				specularInfo.samplerArrayIndex,
				parallaxInfo.samplerArrayIndex,
				normalInfo.samplerArrayIndex,
				currentMaterialInfo);
			}
			else
			{
				DeferredSamplerStoreInstance.UpdateSamplerMapping(
				samplerIndex,
				0,
				0,
				parallaxInfo.samplerArrayIndex,
				normalInfo.samplerArrayIndex,
				currentMaterialInfo);
			}
		}

		static std::string ExtractFilename(const std::string& path)
		{
			const size_t found = path.find_last_of("/\\");
			return path.substr(found + 1);
		}

#pragma region LOADERS
		void LoadDiffuseTexture(const std::string& path, const bool flipUV = false)
		{
			diffuseID = TextureCacheRegistryInstance.AddFromFile(path, flipUV);
			diffuseInfo = DeferredSamplerStoreInstance.AddDiffuseTexture(TextureCacheRegistryInstance.GetHandle(diffuseID));
			diffuseFile = path;
			UpdateSamplerMapping();
			diffuseLoaded = true;
		}

		void LoadDiffuseTexture(Ref<API::Texture::Texture> texture)
		{
			diffuseID = TextureCacheRegistryInstance.Add(texture);
			diffuseInfo = DeferredSamplerStoreInstance.AddDiffuseTexture(TextureCacheRegistryInstance.GetHandle(diffuseID));
			diffuseFile = texture->GetPath();
			UpdateSamplerMapping();
			diffuseLoaded = true;
		}

		void LoadSpecularTexture(const std::string& path, const bool flipUV = false)
		{
			specularID = TextureCacheRegistryInstance.AddFromFile(path, flipUV);
			specularInfo = DeferredSamplerStoreInstance.AddSpecularTexture(TextureCacheRegistryInstance.GetHandle(specularID));
			specularFile = path;
			UpdateSamplerMapping();
			specularLoaded = true;
		}

		void LoadSpecularTexture(Ref<API::Texture::Texture> texture)
		{
			specularID = TextureCacheRegistryInstance.Add(texture);
			specularInfo = DeferredSamplerStoreInstance.AddSpecularTexture(TextureCacheRegistryInstance.GetHandle(specularID));
			specularFile = texture->GetPath();
			UpdateSamplerMapping();
			specularLoaded = true;
		}

		void LoadParallaxTexture(const std::string& path, const bool flipUV = false)
		{
			parallaxID = TextureCacheRegistryInstance.AddFromFile(path, flipUV);
			parallaxInfo = DeferredSamplerStoreInstance.AddParallaxTexture(TextureCacheRegistryInstance.GetHandle(parallaxID));
			parallaxFile = path;
			UpdateSamplerMapping();
			parallaxLoaded = true;
		}

		void LoadParallaxTexture(Ref<API::Texture::Texture> texture)
		{
			parallaxID = TextureCacheRegistryInstance.Add(texture);
			parallaxInfo = DeferredSamplerStoreInstance.AddParallaxTexture(TextureCacheRegistryInstance.GetHandle(parallaxID));
			parallaxFile = texture->GetPath();
			UpdateSamplerMapping();
			parallaxLoaded = true;
		}

		void LoadNormalTexture(const std::string& path, const bool flipUV = false)
		{
			normalID = TextureCacheRegistryInstance.AddFromFile(path, flipUV);
			normalInfo = DeferredSamplerStoreInstance.AddNormalTexture(TextureCacheRegistryInstance.GetHandle(normalID));
			normalFile = path;
			UpdateSamplerMapping();
			normalLoaded = true;
		}

		void LoadNormalTexture(Ref<API::Texture::Texture> texture)
		{
			normalID = TextureCacheRegistryInstance.Add(texture);
			normalInfo = DeferredSamplerStoreInstance.AddNormalTexture(TextureCacheRegistryInstance.GetHandle(normalID));
			normalFile = texture->GetPath();
			UpdateSamplerMapping();
			normalLoaded = true;
		}
#pragma endregion

		static std::string OpenFileDialogue()
		{
			nfdu8char_t* outPath;
			constexpr nfdu8filteritem_t filters[1] = {{"Texture", "png,jpg"}};
			nfdopendialogu8args_t args = {nullptr};
			args.filterList = filters;
			args.filterCount = 1;
			const nfdresult_t result = NFD_OpenDialogU8_With(&outPath, &args);
			if (result == NFD_OKAY)
			{
				std::string outPathStr(outPath);
				LOG_CORE_TRACE("Loading Texture: {0}", outPathStr);
				NFD_FreePathU8(outPath);
				return outPathStr;
			}
			else if (result == NFD_CANCEL)
			{
				LOG_CORE_TRACE("Cancelled file dialogue");
			}
			else
			{
				LOG_CORE_ERROR("Error: {0}", NFD_GetError());
			}

			return "";
		}

		void RenderMaterialMapImGui(const std::string& typeStr, bool& isLoaded, DeferredTextureInformation& info,
		                            const std::string& path,
		                            const std::function<void(const std::string& path, bool flipUV)>& loadProc)
		{
			if (isLoaded)
			{
				if (ImGui::BeginTable("MaterialTable", 2))
				{
					ImGui::TableNextRow();

					ImGui::TableSetColumnIndex(0);
					ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x * 0.5f - TEXTURE_THUMBNAIL_SIZE * 0.5f);
					ImGui::BeginChild("ImageContainer",
					                  ImVec2(TEXTURE_THUMBNAIL_SIZE + 40.f, TEXTURE_THUMBNAIL_SIZE), false);
					ImGui::Image((ImTextureID)info.textureId, ImVec2(TEXTURE_THUMBNAIL_SIZE, TEXTURE_THUMBNAIL_SIZE));
					ImGui::EndChild();

					ImGui::TableSetColumnIndex(1);
					ImGui::BeginChild("TextContainer", ImVec2(0, TEXTURE_THUMBNAIL_SIZE + 20.f), false);
					ImGui::Text("File: %s", ExtractFilename(path).c_str());
					ImGui::Text("Dimension: %d x %d", (int)info.size.x, (int)info.size.y);
					ImGui::Dummy(ImVec2(0.f, textHeight));
					if (ImGui::Button("Remove Texture"))
					{
						info = {0, 0};
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
					ImGui::BeginChild("TextContainer", ImVec2(SAMELINE_WIDGET_OFFSET_1, 25.f), false);
					ImGui::BeginDisabled(true);
					ImGui::SetCursorPosX(20.f);
					ImGui::Text("No Texture loaded");
					ImGui::EndDisabled();
					ImGui::EndChild();

					ImGui::TableSetColumnIndex(1);
					ImGui::BeginChild("ButtonContainer", ImVec2(ImGui::GetContentRegionAvail().x, 25.f), false);
					const std::string buttonText = ICON_FA_FOLDER_OPEN " Load " + typeStr + " Texture";
					if (ImGui::Button(buttonText.c_str()))
					{
						const std::string pathToMap = OpenFileDialogue();
						if (!pathToMap.empty())
						{
							loadProc(pathToMap, false);
						}
					}

					if (typeStr == "Diffuse")
					{
						ImGui::SameLine();
						RenderSampleTextureSelector();
					}
					
					ImGui::EndChild();

					ImGui::EndTable();
				}
			}
		}

		void RenderSampleTextureSelector()
		{
			if (ImGui::Button(ICON_FA_WAND_MAGIC_SPARKLES, {30, 0.f}))
			{
				ImGui::OpenPopup("PrototypeTexture");
			}

			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Select a prototype texture");
				ImGui::EndTooltip();
			}

			std::optional<std::string> selectedPrototypeTexture;

			if (ImGui::BeginPopup("PrototypeTexture"))
			{
				ImGui::Text("Prototype Texture");
				ImGui::Separator();

				if (ImGui::BeginMenu("Wall"))
				{
					if (ImGui::MenuItem("Light")) selectedPrototypeTexture = std::string(ASSET_DIR) + "img/prototype_default/light/wall.png";
					if (ImGui::MenuItem("Dark")) selectedPrototypeTexture = std::string(ASSET_DIR) + "img/prototype_default/dark/wall.png";
					if (ImGui::MenuItem("Orange")) selectedPrototypeTexture = std::string(ASSET_DIR) + "img/prototype_default/orange/wall.png";
					if (ImGui::MenuItem("Purple")) selectedPrototypeTexture = std::string(ASSET_DIR) + "img/prototype_default/purple/wall.png";
					if (ImGui::MenuItem("Green")) selectedPrototypeTexture = std::string(ASSET_DIR) + "img/prototype_default/green/wall.png";
					if (ImGui::MenuItem("Red")) selectedPrototypeTexture = std::string(ASSET_DIR) + "img/prototype_default/red/wall.png";

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Grid"))
				{
					if (ImGui::MenuItem("Light")) selectedPrototypeTexture = std::string(ASSET_DIR) + "img/prototype_default/light/grid_light.png";
					if (ImGui::MenuItem("Dark")) selectedPrototypeTexture = std::string(ASSET_DIR) + "img/prototype_default/dark/grid_light.png";
					if (ImGui::MenuItem("Orange")) selectedPrototypeTexture = std::string(ASSET_DIR) + "img/prototype_default/orange/grid_light.png";
					if (ImGui::MenuItem("Purple")) selectedPrototypeTexture = std::string(ASSET_DIR) + "img/prototype_default/purple/grid_light.png";
					if (ImGui::MenuItem("Green")) selectedPrototypeTexture = std::string(ASSET_DIR) + "img/prototype_default/green/grid_light.png";
					if (ImGui::MenuItem("Red")) selectedPrototypeTexture = std::string(ASSET_DIR) + "img/prototype_default/red/grid_light.png";

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Checker"))
				{
					if (ImGui::MenuItem("Light")) selectedPrototypeTexture = std::string(ASSET_DIR) + "img/prototype_default/light/checker.png";
					if (ImGui::MenuItem("Dark")) selectedPrototypeTexture = std::string(ASSET_DIR) + "img/prototype_default/dark/checker.png";
					if (ImGui::MenuItem("Orange")) selectedPrototypeTexture = std::string(ASSET_DIR) + "img/prototype_default/orange/checker.png";
					if (ImGui::MenuItem("Purple")) selectedPrototypeTexture = std::string(ASSET_DIR) + "img/prototype_default/purple/checker.png";
					if (ImGui::MenuItem("Green")) selectedPrototypeTexture = std::string(ASSET_DIR) + "img/prototype_default/green/checker.png";
					if (ImGui::MenuItem("Red")) selectedPrototypeTexture = std::string(ASSET_DIR) + "img/prototype_default/red/checker.png";

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Checker Small"))
				{
					if (ImGui::MenuItem("Light")) selectedPrototypeTexture = std::string(ASSET_DIR) + "img/prototype_default/light/checker_small.png";
					if (ImGui::MenuItem("Dark")) selectedPrototypeTexture = std::string(ASSET_DIR) + "img/prototype_default/dark/checker_small.png";
					if (ImGui::MenuItem("Orange")) selectedPrototypeTexture = std::string(ASSET_DIR) + "img/prototype_default/orange/checker_small.png";
					if (ImGui::MenuItem("Purple")) selectedPrototypeTexture = std::string(ASSET_DIR) + "img/prototype_default/purple/checker_small.png";
					if (ImGui::MenuItem("Green")) selectedPrototypeTexture = std::string(ASSET_DIR) + "img/prototype_default/green/checker_small.png";
					if (ImGui::MenuItem("Red")) selectedPrototypeTexture = std::string(ASSET_DIR) + "img/prototype_default/red/checker_small.png";

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Checker Crossed"))
				{
					if (ImGui::MenuItem("Light")) selectedPrototypeTexture = std::string(ASSET_DIR) + "img/prototype_default/light/checker_cross.png";
					if (ImGui::MenuItem("Dark")) selectedPrototypeTexture = std::string(ASSET_DIR) + "img/prototype_default/dark/checker_cross.png";
					if (ImGui::MenuItem("Orange")) selectedPrototypeTexture = std::string(ASSET_DIR) + "img/prototype_default/orange/checker_cross.png";
					if (ImGui::MenuItem("Purple")) selectedPrototypeTexture = std::string(ASSET_DIR) + "img/prototype_default/purple/checker_cross.png";
					if (ImGui::MenuItem("Green")) selectedPrototypeTexture = std::string(ASSET_DIR) + "img/prototype_default/green/checker_cross.png";
					if (ImGui::MenuItem("Red")) selectedPrototypeTexture = std::string(ASSET_DIR) + "img/prototype_default/red/checker_cross.png";

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Mesh"))
				{
					if (ImGui::MenuItem("Light")) selectedPrototypeTexture = std::string(ASSET_DIR) + "img/prototype_default/light/mesh.png";
					if (ImGui::MenuItem("Dark")) selectedPrototypeTexture = std::string(ASSET_DIR) + "img/prototype_default/dark/mesh.png";
					if (ImGui::MenuItem("Orange")) selectedPrototypeTexture = std::string(ASSET_DIR) + "img/prototype_default/orange/mesh.png";
					if (ImGui::MenuItem("Purple")) selectedPrototypeTexture = std::string(ASSET_DIR) + "img/prototype_default/purple/mesh.png";
					if (ImGui::MenuItem("Green")) selectedPrototypeTexture = std::string(ASSET_DIR) + "img/prototype_default/green/mesh.png";
					if (ImGui::MenuItem("Red")) selectedPrototypeTexture = std::string(ASSET_DIR) + "img/prototype_default/red/mesh.png";

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Mesh Fine"))
				{
					if (ImGui::MenuItem("Light")) selectedPrototypeTexture = std::string(ASSET_DIR) + "img/prototype_default/light/mesh_light.png";
					if (ImGui::MenuItem("Dark")) selectedPrototypeTexture = std::string(ASSET_DIR) + "img/prototype_default/dark/mesh_light.png";
					if (ImGui::MenuItem("Orange")) selectedPrototypeTexture = std::string(ASSET_DIR) + "img/prototype_default/orange/mesh_light.png";
					if (ImGui::MenuItem("Purple")) selectedPrototypeTexture = std::string(ASSET_DIR) + "img/prototype_default/purple/mesh_light.png";
					if (ImGui::MenuItem("Green")) selectedPrototypeTexture = std::string(ASSET_DIR) + "img/prototype_default/green/mesh_light.png";
					if (ImGui::MenuItem("Red")) selectedPrototypeTexture = std::string(ASSET_DIR) + "img/prototype_default/red/mesh_light.png";

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Quarter Cross"))
				{
					if (ImGui::MenuItem("Light")) selectedPrototypeTexture = std::string(ASSET_DIR) + "img/prototype_default/light/center_light.png";
					if (ImGui::MenuItem("Dark")) selectedPrototypeTexture = std::string(ASSET_DIR) + "img/prototype_default/dark/center_light.png";
					if (ImGui::MenuItem("Orange")) selectedPrototypeTexture = std::string(ASSET_DIR) + "img/prototype_default/orange/center_light.png";
					if (ImGui::MenuItem("Purple")) selectedPrototypeTexture = std::string(ASSET_DIR) + "img/prototype_default/purple/center_light.png";
					if (ImGui::MenuItem("Green")) selectedPrototypeTexture = std::string(ASSET_DIR) + "img/prototype_default/green/center_light.png";
					if (ImGui::MenuItem("Red")) selectedPrototypeTexture = std::string(ASSET_DIR) + "img/prototype_default/red/center_light.png";

					ImGui::EndMenu();
				}

				ImGui::EndPopup();
			}

			if (selectedPrototypeTexture)
			{
				LoadDiffuseTexture(selectedPrototypeTexture.value(), true);
			}
		}
#pragma endregion

#pragma region OVERRIDE
		static inline Type GetType() { return Type::Material; }
		static inline std::string GetName() { return "Material"; }

		UIResponse OnImGuiRender() override
		{
			const bool isOpen = ImGui::CollapsingHeader(
				ICON_FA_CIRCLE_HALF_STROKE "  Material", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap);
			ImGui::SameLine();

			const float xOffset = ImGui::GetContentRegionAvail().x - 30.0f;
			if (xOffset > 0.0f)
			{
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + xOffset);
			}

			if (ImGui::SmallButton(ICON_FA_TRASH_CAN "##Material"))
				return {UIResponse::Code::Remove, Type::Material};

			if (isOpen)
			{
				textHeight = ImGui::GetTextLineHeightWithSpacing();

				const unsigned int previousItemInfo = currentMaterialInfo;

				ImGui::BeginDisabled(isCustomMaterial);
				ImGui::Text("Template Material");
				ImGui::SameLine(SAMELINE_WIDGET_OFFSET_1);
				if (ImGui::BeginCombo("##Material", selectedItem.c_str())) {
					for (const auto& [key, _] : MaterialCacheRegistryInstance.GetMaterialNames()) {
						const bool isSelected = (selectedItem == key);
						if (ImGui::Selectable(key.c_str(), isSelected)) {
							selectedItem = key;
							currentMaterial = MaterialCacheRegistryInstance.Get(key);
							currentMaterialInfo = DeferredSamplerStoreInstance.AddCustomMaterial(currentMaterial);
							DeferredSamplerStoreInstance.RemoveCustomMaterial(previousItemInfo);
							UpdateSamplerMapping();
						}

						if (isSelected) {
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
				ImGui::EndDisabled();

				ImGui::Text("Use Custom Material instead");
				ImGui::SameLine(SAMELINE_WIDGET_OFFSET_1 * 2.f);
				if (ImGui::Checkbox("##CustomMaterial", &isCustomMaterial))
				{
					if (isCustomMaterial)
					{
						if (!isPrivateMaterialCreated)
						{
							privateMaterial = MaterialCacheRegistryInstance.Get(MaterialCacheRegistryInstance.Create());
							DeferredSamplerStoreInstance.AddCustomMaterial(privateMaterial);
							isPrivateMaterialCreated = true;
						}

						currentMaterial = privateMaterial;
					}
					else
					{
						currentMaterial = MaterialCacheRegistryInstance.Get(selectedItem);
					}

					currentMaterialInfo = DeferredSamplerStoreInstance.GetCustomMaterialStoreId(currentMaterial.id);
					UpdateSamplerMapping();
				}

				if (!isCustomMaterial)
				{
					ImGui::BeginDisabled(true);
					ImGui::SeparatorText("Color");

					ImGui::Text("Diffuse Color");
					ImGui::SameLine(SAMELINE_WIDGET_OFFSET_1);
					ImGui::ColorEdit3("##Diffuse", &currentMaterial.shaderObject.diffuseColor.x);

					ImGui::Text("Specular Color");
					ImGui::SameLine(SAMELINE_WIDGET_OFFSET_1);
					ImGui::ColorEdit3("##Specular", &currentMaterial.shaderObject.specularColor.x);

					ImGui::Text("Emissive Color");
					ImGui::SameLine(SAMELINE_WIDGET_OFFSET_1);
					ImGui::ColorEdit3("##Emissive", &currentMaterial.shaderObject.emissiveColor.x);

					ImGui::SeparatorText("Surface");
					ImGui::Text("Metallic");
					ImGui::SameLine(SAMELINE_WIDGET_OFFSET_1);
					ImGui::DragFloat("##Metallic", &currentMaterial.shaderObject.metallic, 0.01f, 0.0f, 1.0f);

					ImGui::Text("Roughness");
					ImGui::SameLine(SAMELINE_WIDGET_OFFSET_1);
					ImGui::DragFloat("##Roughness", &currentMaterial.shaderObject.roughness, 0.01f, 0.0f, 1.0f);

					ImGui::Text("Shininess");
					ImGui::SameLine(SAMELINE_WIDGET_OFFSET_1);
					ImGui::DragFloat("##Shininess", &currentMaterial.shaderObject.shininess, 2.f, 0.0f, 64.0f);

					ImGui::Text("Reflectance");
					ImGui::SameLine(SAMELINE_WIDGET_OFFSET_1);
					ImGui::DragFloat("##Reflectance", &currentMaterial.shaderObject.reflectance, 0.01f, 0.0f, 1.0f);
					ImGui::EndDisabled();
				}
				else
				{
					ImGui::SeparatorText("Color");
					bool defaultMaterialAltered = false;

					ImGui::BeginDisabled(diffuseLoaded);
					ImGui::Text("Diffuse Color");
					ImGui::SameLine(SAMELINE_WIDGET_OFFSET_1);
					defaultMaterialAltered |= ImGui::ColorEdit3("##Diffuse", &privateMaterial.shaderObject.diffuseColor.x);
					ImGui::EndDisabled();

					RenderMaterialMapImGui("Diffuse", diffuseLoaded, diffuseInfo, diffuseFile,
									   [this](const std::string& path, const bool flipUV)
									   {
										   LoadDiffuseTexture(path, flipUV);
									   });

					ImGui::BeginDisabled(specularLoaded);
					ImGui::Text("Specular Color");
					ImGui::SameLine(SAMELINE_WIDGET_OFFSET_1);
					defaultMaterialAltered |= ImGui::ColorEdit3("##Specular", &privateMaterial.shaderObject.specularColor.x);
					ImGui::EndDisabled();

					RenderMaterialMapImGui("Specular", specularLoaded, specularInfo, specularFile,
									   [this](const std::string& path, const bool flipUV)
									   {
										   LoadSpecularTexture(path, flipUV);
									   });

					ImGui::SeparatorText("Surface");

					ImGui::BeginDisabled(true);
					ImGui::Text("Metallic");
					ImGui::SameLine(SAMELINE_WIDGET_OFFSET_1);
					defaultMaterialAltered |= ImGui::DragFloat("##Metallic", &privateMaterial.shaderObject.metallic, 0.01f, 0.0f, 1.0f);

					ImGui::Text("Roughness");
					ImGui::SameLine(SAMELINE_WIDGET_OFFSET_1);
					defaultMaterialAltered |= ImGui::DragFloat("##Roughness", &privateMaterial.shaderObject.roughness, 0.01f, 0.0f, 1.0f);
					ImGui::EndDisabled();

					ImGui::Text("Shininess");
					ImGui::SameLine(SAMELINE_WIDGET_OFFSET_1);
					defaultMaterialAltered |= ImGui::DragFloat("##Shininess", &privateMaterial.shaderObject.shininess, 1.f, 1.0f, 512.0f);

					ImGui::Text("Reflectance");
					ImGui::SameLine(SAMELINE_WIDGET_OFFSET_1);
					defaultMaterialAltered |= ImGui::DragFloat("##Reflectance", &privateMaterial.shaderObject.reflectance, 0.01f, 0.0f, 1.0f);

					if (defaultMaterialAltered)
					{
						MaterialCacheRegistryInstance.Edit(privateMaterial.id, privateMaterial);
						DeferredSamplerStoreInstance.EditCustomMaterial(currentMaterialInfo, privateMaterial);
					}
				}

				ImGui::SeparatorText("Normal Map");
				RenderMaterialMapImGui("Normal", normalLoaded, normalInfo, normalFile,
									   [this](const std::string& path, const bool flipUV)
									   {
										   LoadNormalTexture(path, flipUV);
									   });

				ImGui::SeparatorText("Parallax Map");
				RenderMaterialMapImGui("Parallax", parallaxLoaded, parallaxInfo, parallaxFile,
				                       [this](const std::string& path, const bool flipUV)
				                       {
					                       LoadParallaxTexture(path, flipUV);
				                       });

				ImGui::Dummy(ImVec2(0.f, DUMMYSPACE_AFTER_COMPONENT));
			}

			return {};
		}
#pragma endregion
	};
}
