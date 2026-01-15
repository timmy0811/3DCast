#pragma once

#include "3DCast/Scene/Component/AbstractComponent.h"
#include "3DCast/Scene/Component/TagComponent.h"
#include "3DCast/Model/Model.h"
#include "3DCast/Data/GlobalShared.h"

#include "3DCast/Gui/UIComponents.h"

#include <imgui.h>
#include <filesystem>

#include "3DCast/Gui/TempElements/TempGuiElementCollection.h"
#include "3DCast/Gui/TempElements/Elements/NotificationModal.h"

namespace Cast::Component
{
	struct MeshComponent final : public Component
	{
#pragma region DATA
		static inline std::string lastUsedDirectory;

		std::string Path;
		std::string Filename;
		std::string header;

		UID popupID;

		Ref<Model> RootModel; // Complex intermediate and leafs do not need a model instance
		MeshNodeType TypeNode;

		int _load = false;
		int _loadC = 0;
		bool FlipTextures = false;

	private:
		Mesh* MeshInstance = nullptr;
		bool _deferredLoad = false;

	public:
#pragma endregion

#pragma region CONSTRUCTOR
		// Copy constructor - copies basic fields but does not share MeshInstance
		// MeshInstance is owned by RootModel and should not be shallow copied
		MeshComponent(const MeshComponent& other)
			: Path(other.Path),
			  Filename(other.Filename),
			  header(other.header),
			  popupID(UID::None()),
			  RootModel(other.RootModel),  // Shared pointer - OK to share
			  TypeNode(other.TypeNode),
			  _load(other._load),
			  _loadC(other._loadC),
			  FlipTextures(other.FlipTextures),
			  MeshInstance(nullptr),  // Do not copy MeshInstance - it's owned by RootModel
			  _deferredLoad(other._deferredLoad)
		{
			// Note: MeshInstance is intentionally not copied as it's managed by RootModel
			// For Leaf nodes, the MeshInstance will be set by the parent model
		}

		// Copy assignment operator
		MeshComponent& operator=(const MeshComponent& other)
		{
			if (this != &other)
			{
				// Note: Don't delete MeshInstance here as it may be shared with RootModel
				// Only the destructor or move assignment should handle deletion

				Path = other.Path;
				Filename = other.Filename;
				header = other.header;
				popupID = UID::None();
				RootModel = other.RootModel;
				TypeNode = other.TypeNode;
				_load = other._load;
				_loadC = other._loadC;
				FlipTextures = other.FlipTextures;
				MeshInstance = nullptr;  // Do not copy MeshInstance
				_deferredLoad = other._deferredLoad;
			}
			return *this;
		}

		explicit MeshComponent(const MeshNodeType nodeType = MeshNodeType::Root)
			:TypeNode(nodeType)
		{
			switch (nodeType)
			{
				case MeshNodeType::Root:
				RootModel = Ref<Model>();
				header = "Model Root Node";
					break;
				case MeshNodeType::Intermediate:
					header = "Model Intermediate Node";
					break;
				case MeshNodeType::Leaf:
					header = "Mesh Leaf Node";
					break;
				default:
					break;
			}
		}

		explicit MeshComponent(Cast::Mesh* mesh)
			: MeshInstance(mesh), TypeNode(MeshNodeType::Leaf)
		{
			header = "Mesh Leaf Node";
		}

		explicit MeshComponent(const std::string& path, bool deferLoad = false, bool flipTextures = true)
			: Path(path), TypeNode(MeshNodeType::Root), _deferredLoad(deferLoad), FlipTextures(flipTextures)
		{
			RootModel = CreateRef<Model>();
			header = "Model Root Node";
			// If not deferred, load immediately (for UI-triggered loads where EntityNode is already set)
			// For deserialization, deferLoad=true and loading happens in OnAfterEntitySetBehaviour
			if (!deferLoad && EntityNode)
			{
				RootModel->Load(path, EntityNode, flipTextures);
			}
		}

		MeshComponent(MeshComponent&& other) noexcept
			: Path(std::move(other.Path)), Filename(std::move(other.Filename)),
			  header(std::move(other.header)), RootModel(std::move(other.RootModel)),
			  TypeNode(other.TypeNode), _load(other._load), _loadC(other._loadC),
			  FlipTextures(other.FlipTextures),
			  MeshInstance(other.MeshInstance)
		{
			other.MeshInstance = nullptr;
		}

		MeshComponent& operator=(MeshComponent&& other) noexcept
		{
			if (this != &other)
			{
				if (MeshInstance)
				{
					MeshInstance->RemoveFromBatchStorage();
					delete MeshInstance;
				}

				Path = std::move(other.Path);
				Filename = std::move(other.Filename);
				header = std::move(other.header);
				RootModel = std::move(other.RootModel);
				TypeNode = other.TypeNode;
				_load = other._load;
				_loadC = other._loadC;
				FlipTextures = other.FlipTextures;
				MeshInstance = other.MeshInstance;

				other.MeshInstance = nullptr;
			}
			return *this;
		}

		~MeshComponent() override
		{
			if (MeshInstance)
			{
				MeshInstance->RemoveFromBatchStorage();
				delete MeshInstance;
			}
		}
#pragma endregion

#pragma region UTILITY
		void SetMeshAsChildNode(Cast::Mesh* mesh)
		{
			MeshInstance = mesh;
			TypeNode = MeshNodeType::Leaf;
		}

		[[nodiscard]] inline MeshNodeType GetNodeType() const { return TypeNode; }

		static std::string OpenFileDialogue()
		{
			nfdu8char_t* outPath;
			constexpr nfdu8filteritem_t filters[1] = {{"Model", "3d,3ds,ac,amf,zip,ase,b3d,blend,bvh,cob,csm,dae,dxf,fbx,gltf,glb,ifc,irr,lwo,lws,m3d,md2,md3,md5,mdc,mdl,mesh,ms3d,ndo,nff,obj,off,ogex,ply,pmx,q3d,q3bsp,raw,sib,smd,stl,ter,x,x3d,xgl,zgl,3mf"}};
			nfdopendialogu8args_t args = {nullptr};
			args.filterList = filters;
			args.filterCount = 1;

			if (!lastUsedDirectory.empty() && std::filesystem::exists(lastUsedDirectory))
			{
				args.defaultPath = lastUsedDirectory.c_str();
			}

			const nfdresult_t result = NFD_OpenDialogU8_With(&outPath, &args);
			if (result == NFD_OKAY)
			{
				std::string outPathStr(outPath);
				LOG_CORE_TRACE("Loading Model: {0}", outPathStr);

				const size_t found = outPathStr.find_last_of("/\\");
				if (found != std::string::npos)
				{
					lastUsedDirectory = outPathStr.substr(0, found);
				}

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

		static std::string ExtractFilename(const std::string& path)
		{
			const size_t found = path.find_last_of("/\\");
			return path.substr(found + 1);
		}

		static std::string ExtractFilenameWithoutExtension(const std::string& path)
		{
			std::string filename = ExtractFilename(path);
			if (const size_t dotPos = filename.find_last_of('.'); dotPos != std::string::npos)
			{
				return filename.substr(0, dotPos);
			}
			return filename;
		}
#pragma endregion

#pragma region OVERRIDE
		static inline Type GetType() { return Type::Mesh; }
		static inline std::string GetName() { return "Mesh"; }

		void OnAfterEntitySetBehaviour() override
		{
			if (TypeNode == MeshNodeType::Leaf)
				Shared.ActiveScene->RegisterTransformComponent(EntityNode);
			
			// Handle deferred model loading (used during deserialization)
			if (_deferredLoad && TypeNode == MeshNodeType::Root && EntityNode && !Path.empty())
			{
				RootModel->Load(Path, EntityNode, FlipTextures);
				Filename = ExtractFilename(Path);

				// Update entity name from filename if tag hasn't been manually altered
				if (EntityNode->HasComponent<TagComponent>())
				{
					auto& tagComp = EntityNode->GetComponent<TagComponent>();
					if (!tagComp.TagHasManuallyAltered)
					{
						tagComp.Tag = ExtractFilenameWithoutExtension(Path);
					}
				}

				_deferredLoad = false;
			}
		}

		UIResponse OnImGuiRender() override
		{
			if (_load)
			{
				// ImGui needs to swap buffer once to make modal window show up
				//UI::ModalImportInProgress(Path);
				_loadC++;
			}

			if (_loadC > 2)
			{
				RootModel = CreateRef<Model>();
				RootModel->Load(Path, EntityNode, FlipTextures);
				//UI::ModalImportInProgress(Path, true);

				// Update entity name from filename if tag hasn't been manually altered
				if (EntityNode->HasComponent<TagComponent>())
				{
					auto& tagComp = EntityNode->GetComponent<TagComponent>();
					if (!tagComp.TagHasManuallyAltered)
					{
						tagComp.Tag = ExtractFilenameWithoutExtension(Path);
					}
				}

				GUI::TempGuiElementCollection::CloseElement(popupID);

				_load = false;
				_loadC = 0;
			}

			const bool isOpen = ImGui::CollapsingHeader((std::string(ICON_FA_DRAW_POLYGON) + "  " + header).c_str(),
			                                            ImGuiTreeNodeFlags_DefaultOpen |
			                                            ImGuiTreeNodeFlags_AllowItemOverlap);
			ImGui::SameLine();

			const float xOffset = ImGui::GetContentRegionAvail().x - 30.0f;
			if (xOffset > 0.0f)
			{
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + xOffset);
			}

			if (ImGui::SmallButton(ICON_FA_TRASH_CAN "##Mesh"))
				return {UIResponse::Code::Remove, Type::Mesh};

			if (isOpen)
			{
				if (TypeNode == MeshNodeType::Root)
				{
					// Is the model root node
					if (RootModel && RootModel->IsModelLoaded())
					{
						// The model is loaded
						ImGui::Text("Model");
						ImGui::SameLine(GUIWIN_WTHIRD);
						ImGui::Text("%s", Filename.c_str());

						ImGui::Text("Submesh Count");
						ImGui::SameLine(GUIWIN_WTHIRD);
						ImGui::Text("%d", RootModel->GetMeshCount());

						ImGui::Text("Vertex Count");
						ImGui::SameLine(GUIWIN_WTHIRD);
						ImGui::Text("%d", RootModel->GetTotalVertexCount());

						ImGui::Text("Indexed");
						ImGui::SameLine(GUIWIN_WTHIRD);
						ImGui::Text("%s", RootModel->IsIndexed() ? "Yes" : "No");

						ImGui::Text("Material Assigned");
						ImGui::SameLine(GUIWIN_WTHIRD);
						ImGui::Text("%s", RootModel->MaterialAssigned() ? "Yes" : "No");

						ImGui::Text("Texture Count");
						ImGui::SameLine(GUIWIN_WTHIRD);
						ImGui::Text("%d", RootModel->GetTextureCount());
					}
					else
					{
						ImGui::BeginDisabled(true);
						ImGui::SetCursorPosX(20.f);
						ImGui::Text("No Model loaded");
						ImGui::EndDisabled();

						HALF_INPUT

						if (ImGui::Button("Load from file", ImVec2(GUIWIN_WHalf - GUIWIN_ENDELEMENT_PADDING, 0.f)))
						{
							Path = OpenFileDialogue();
							Filename = ExtractFilename(Path);
							popupID = GUI::TempGuiElementCollection::AddElement(new Cast::GUI::NotificationModal("Loading Model", std::string("Model import running for: ") + Path , ICON_FA_HOURGLASS_HALF));
							_load = true;
						}

						ImGui::TextUnformatted("Flip Textures");
						HALF_INPUT
						ImGui::Checkbox("##Flip", &FlipTextures);
					}
				}
				else
				{
					if (TypeNode == MeshNodeType::Leaf)
					{
						// Is a leaf node representing a single mesh without children
						ImGui::Text("Vertex Count");
						ImGui::SameLine(GUIWIN_WTHIRD);
						ImGui::Text("%d", MeshInstance->GetVertexCount());

						ImGui::Text("Indexed");
						ImGui::SameLine(GUIWIN_WTHIRD);
						ImGui::Text("%s", MeshInstance->HasIndices() ? "Yes" : "No");

						ImGui::Text("Has Material");
						ImGui::SameLine(GUIWIN_WTHIRD);
						ImGui::Text("%s", MeshInstance->MaterialAssigned() ? "Yes" : "No");
					}
					else
					{
						// Is a node with children
						ImGui::Text("Summed up Vertex Count");
						ImGui::SameLine(GUIWIN_WTHIRD);
						ImGui::Text("%d", 10); // TODO: replace placeholder
					}
				}

				ImGui::Dummy(ImVec2(0.f, DUMMYSPACE_AFTER_COMPONENT));
			}

			return {};
		}
#pragma endregion
	};
}
