#pragma once

#include "3DCast/Scene/Component/AbstractComponent.h"
#include "3DCast/Model/Model.h"
#include "3DCast/Scene/DataObjects/GlobalShared.h"

namespace Cast::Component {
	struct MeshComponent : public Component
	{
		std::string Path;
		std::string Filename;
		Ref<Cast::Model> RootModel; // Complex intermediate and leafs do not need a model instance
		Ref<Cast::Mesh> Mesh;

		bool IsMeshLeaf = false;
		bool IsRootNode = false;

		MeshComponent(const MeshComponent&) = default;
		MeshComponent(bool isRootNode = true) {
			if (isRootNode) {
				RootModel = CreateRef<Cast::Model>();
				IsRootNode = true;
			}
		}

		MeshComponent(Ref<Cast::Mesh> mesh)
			: Mesh(mesh) {
			IsMeshLeaf = true;
		}

		MeshComponent(const std::string& path)
			: Path(path) {
			IsRootNode = true;
			RootModel = CreateRef<Cast::Model>();
			RootModel->Load(path, EntityNode);
		}

		std::string OpenFileDialoge() {
			nfdu8char_t* outPath;
			nfdu8filteritem_t filters[1] = { { "Model", "3d,3ds,csm,dae,dxf,fbx,md2,mesh,nff,obj,stl" } };
			nfdopendialogu8args_t args = { 0 };
			args.filterList = filters;
			args.filterCount = 1;
			nfdresult_t result = NFD_OpenDialogU8_With(&outPath, &args);
			if (result == NFD_OKAY)
			{
				std::string outPathStr(outPath);
				LOG_CORE_TRACE("Loading Model: {0}", outPathStr);
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

		std::string ExtractFilename(const std::string& path) {
			size_t found = path.find_last_of("/\\");
			return path.substr(found + 1);
		}

		virtual void OnImGuiRender() override {
			std::string header;
			if (IsRootNode) header = "Model Root Node";
			else if (IsMeshLeaf) header = "Mesh Leaf Node";
			else header = "Complex Mesh Node";

			if (ImGui::CollapsingHeader(header.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
				if (IsRootNode) { // Is the model root node
					if (RootModel && RootModel->IsModelLoaded()) { // The model is loaded
						ImGui::Text("Model: %s", Filename.c_str());
						ImGui::Text("Submesh Count: %d", RootModel->GetMeshCount());
						ImGui::Text("Vertex Count: %d", RootModel->GetTotalVertexCount());
						ImGui::Text("Indexed: %s", RootModel->IsIndexed() ? "Yes" : "No");
						ImGui::Text("Material Assigned: %s", RootModel->MaterialAssigned() ? "Yes" : "No");
						ImGui::Text("Texture Count: %d", RootModel->GetTextureCount());
					}
					else {
						ImGui::Text("No model loaded");
						ImGui::SameLine(SAMELINE_WIDGET_OFFSET);

						if (ImGui::Button("Load from file")) {
							Path = OpenFileDialoge();
							Filename = ExtractFilename(Path);
							RootModel = CreateRef<Cast::Model>();
							RootModel->Load(Path, EntityNode);
						}
					}
				}
				else {
					if (IsMeshLeaf) { // Is a leaf node representing a single mesh without children
						ImGui::Text("Vertex Count: %d", Mesh->GetVertexCount());
						ImGui::Text("Indexed: %s", Mesh->HasIndices() ? "Yes" : "No");
						ImGui::Text("Material Assigned: %s", Mesh->MaterialAssigned() ? "Yes" : "No");
					}
					else { // Is a node with children
						ImGui::Text("Summed up Vertex Count: %d", 10); // TODO: replace placeholder
					}
				}
			}
		}
	};
}