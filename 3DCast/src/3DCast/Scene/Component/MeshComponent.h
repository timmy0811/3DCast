#pragma once

#include "3DCast/Scene/Component/AbstractComponent.h"
#include "3DCast/Model/Model.h"
#include "3DCast/Data/GlobalShared.h"

#include "3DCast/ImGui/UIComponents.h"

#include <imgui.h>

namespace Cast::Component
{
	struct MeshComponent final : public Component
	{
#pragma region DATA
		std::string Path;
		std::string Filename;
		std::string header;

		Ref<Model> RootModel; // Complex intermediate and leafs do not need a model instance
		bool IsRootNode = false;
		bool IsMeshLeaf = false;

		int _load = false;
		int _loadC = 0;

	private:
		Mesh* MeshInstance = nullptr;

	public:
#pragma endregion

#pragma region CONSTRUCTOR
		MeshComponent(const MeshComponent&)
		{
		};

		explicit MeshComponent(const bool isRootNode = true)
		{
			if (isRootNode)
			{
				RootModel = Ref<Model>();
				IsRootNode = true;
				header = "Model Root Node";
			}
			else
			{
				IsMeshLeaf = true;
				header = "Mesh Leaf Node";
			}
		}

		explicit MeshComponent(Cast::Mesh* mesh)
			: MeshInstance(mesh)
		{
			IsMeshLeaf = true;
			header = "Mesh Leaf Node";
		}

		explicit MeshComponent(const std::string& path)
			: Path(path)
		{
			IsRootNode = true;
			RootModel = CreateRef<Model>();
			RootModel->Load(path, EntityNode);
			header = "Model Root Node";
		}

		MeshComponent(MeshComponent&& other) noexcept
			: Path(std::move(other.Path)), Filename(std::move(other.Filename)),
			  header(std::move(other.header)), RootModel(std::move(other.RootModel)),
			  IsRootNode(other.IsRootNode), IsMeshLeaf(other.IsMeshLeaf),
			  _load(other._load), _loadC(other._loadC),
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
				IsRootNode = other.IsRootNode;
				IsMeshLeaf = other.IsMeshLeaf;
				_load = other._load;
				_loadC = other._loadC;
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
			IsMeshLeaf = true;
		}

		static std::string OpenFileDialogue()
		{
			nfdu8char_t* outPath;
			constexpr nfdu8filteritem_t filters[1] = {{"Model", "3d,3ds,csm,dae,dxf,fbx,md2,mesh,nff,obj,stl"}};
			nfdopendialogu8args_t args = {nullptr};
			args.filterList = filters;
			args.filterCount = 1;
			const nfdresult_t result = NFD_OpenDialogU8_With(&outPath, &args);
			if (result == NFD_OKAY)
			{
				std::string outPathStr(outPath);
				LOG_CORE_TRACE("Loading Model: {0}", outPathStr);
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
#pragma endregion

#pragma region OVERRIDE
		static inline Type GetType() { return Type::Mesh; }
		static inline std::string GetName() { return "Mesh"; }

		void OnAfterEntitySetBehaviour() override
		{
			if (IsMeshLeaf)
				Shared.ActiveScene->RegisterTransformComponent(EntityNode);
		}

		UIResponse OnImGuiRender() override
		{
			if (_load)
			{
				// ImGui needs to swap buffer once to make modal window show up
				UI::ModalImportInProgress(Path);
				_loadC++;
			}

			if (_loadC > 2)
			{
				RootModel = CreateRef<Model>();
				RootModel->Load(Path, EntityNode);
				UI::ModalImportInProgress(Path, true);

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
				if (IsRootNode)
				{
					// Is the model root node
					if (RootModel && RootModel->IsModelLoaded())
					{
						// The model is loaded
						ImGui::Text("Model: %s", Filename.c_str());
						ImGui::Text("Submesh Count: %d", RootModel->GetMeshCount());
						ImGui::Text("Vertex Count: %d", RootModel->GetTotalVertexCount());
						ImGui::Text("Indexed: %s", RootModel->IsIndexed() ? "Yes" : "No");
						ImGui::Text("Material Assigned: %s", RootModel->MaterialAssigned() ? "Yes" : "No");
						ImGui::Text("Texture Count: %d", RootModel->GetTextureCount());
					}
					else
					{
						ImGui::Text("No model loaded");
						ImGui::SameLine(SAMELINE_WIDGET_OFFSET);

						if (ImGui::Button("Load from file"))
						{
							Path = OpenFileDialogue();
							Filename = ExtractFilename(Path);
							_load = true;
						}
					}
				}
				else
				{
					if (IsMeshLeaf)
					{
						// Is a leaf node representing a single mesh without children
						ImGui::Text("Vertex Count: %d", MeshInstance->GetVertexCount());
						ImGui::Text("Indexed: %s", MeshInstance->HasIndices() ? "Yes" : "No");
						ImGui::Text("Material Assigned: %s", MeshInstance->MaterialAssigned() ? "Yes" : "No");
					}
					else
					{
						// Is a node with children
						ImGui::Text("Summed up Vertex Count: %d", 10); // TODO: replace placeholder
					}
				}
			}

			return {};
		}
#pragma endregion
	};
}
