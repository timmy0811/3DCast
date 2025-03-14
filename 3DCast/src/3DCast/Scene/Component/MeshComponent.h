#pragma once

#include "3DCast/Scene/Component/AbstractComponent.h"
#include "3DCast/Model/Model.h"

namespace Cast::Component {
	struct MeshComponent : public Component
	{
		std::string Path;
		std::string Filename;
		Ref<Cast::Model> Model3D;

		MeshComponent() = default;
		MeshComponent(const MeshComponent&) = default;
		MeshComponent(const std::string& path)
			: Path(path) {
			Model3D = CreateRef<Cast::Model>();
			Model3D->Load(path);
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

		std::string ExtractFilename(const std::string& path) {
			size_t found = path.find_last_of("/\\");
			return path.substr(found + 1);
		}

		virtual void OnImGuiRender() override {
			if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen)) {
				if (Model3D) {
					ImGui::Text("Model: %s", Filename.c_str());
					ImGui::Text("Submesh Count: %d", Model3D->GetMeshCount());
					ImGui::Text("Vertex Count: %d", Model3D->GetTotalVertexCount());
					ImGui::Text("Indexed: %s", Model3D->IsIndexed() ? "Yes" : "No");
					ImGui::Text("Material Assigned: %s", Model3D->MaterialAssigned() ? "Yes" : "No");
					ImGui::Text("Texture Count: %d", Model3D->GetTextureCount());
				}
				else {
					ImGui::Text("No model loaded");
					ImGui::SameLine(SAMELINE_WIDGET_OFFSET);

					if (ImGui::Button("Load from file")) {
						Path = OpenFileDialoge();
						Filename = ExtractFilename(Path);
						Model3D = CreateRef<Cast::Model>();
						Model3D->Load(Path);
					}
				}
			}
		}
	};
}