#pragma once

#include "3DCast/Scene/Component/AbstractComponent.h"
#include "3DCast/Memory/Batching/BatchManager.h"
#include "3DCast/Misc/UID.h"
#include "3DCast/Data/GlobalShared.h"

#include <imgui.h>

namespace Cast::Component {
	struct CustomMeshComponent : public Component, public IVertexEntity
	{
#pragma region DATA
		uid BatchId = UID::None();
		Memory::MemoryPosition memPos{};
		bool IsAvailableInMemory = true;

		float* vertexData = nullptr;
		unsigned int* indexData = nullptr;
		bool isIndexed = false;

		size_t vertexDataSize = 0;
		size_t indexDataSize = 0;
		bool isHeapAlloc = false;
#pragma endregion

#pragma region CONSTRUCTOR
		CustomMeshComponent() {	}
		CustomMeshComponent(const CustomMeshComponent&) = default;
		CustomMeshComponent(size_t vertexBufferSize, size_t indexBufferSize) {
			vertexData = (float*)malloc(vertexBufferSize);
			indexData = (unsigned int*)malloc(indexBufferSize);

			isHeapAlloc = true;
		}

		~CustomMeshComponent() {
			if (isHeapAlloc) {
				free(vertexData);
				free(indexData);
			}

			if (BatchId != UID::None()) {
				Cast::Memory::BatchMemoryHandler.RemoveObject(BatchId);
				BatchId = UID::None();
			}
		}
#pragma endregion

#pragma region UTILITY
		void OnAfterEntitySetBehaviour() override {
			Shared.ActiveScene->RegisterTransformComponent(EntityNode);
		}

		void AllocVertexData(size_t size) {
			free(vertexData);
			vertexData = (float*)malloc(size);
			vertexDataSize = size;
		}

		void AllocIndexData(size_t size) {
			free(indexData);
			indexData = (unsigned int*)malloc(size);
			indexDataSize = size;
		}

		void SwapToDisk() {
			IsAvailableInMemory = false;
		}

		void SwapToMemory() {
			IsAvailableInMemory = true;
		}

		void AddAndAllocVertexData(float* data, size_t size) {
			AllocVertexData(size);
			memcpy(vertexData, data, size);
		}

		void AddAndAllocIndexData(unsigned int* data, size_t size) {
			AllocIndexData(size);
			memcpy(indexData, data, size);
		}

		void AddVertexData(float* data, size_t size) {
			memcpy(vertexData, data, size);
		}

		void AddIndexData(unsigned int* data, size_t size) {
			memcpy(indexData, data, size);
		}

		void Clear() {
			free(vertexData);
			free(indexData);
			vertexData = nullptr;
			indexData = nullptr;
			vertexDataSize = 0;
			indexDataSize = 0;
		}

		void AddToBatchMemory() {
			BatchId = Cast::Memory::BatchMemoryHandler.CreateBatchObject(vertexData, vertexDataSize);

			if (BatchId != Cast::UID::None())
			{
				Shared.VertexEntities[BatchId] = this;
			}
		}

		void AddToIndexedBatchMemory() {
			BatchId = Cast::Memory::BatchMemoryHandler.CreateBatchObject(vertexData, vertexDataSize, indexData, (int)indexDataSize);
			isIndexed = true;

			if (BatchId != Cast::UID::None())
			{
				Shared.VertexEntities[BatchId] = this;
			}
		}
#pragma endregion

#pragma region OVERRIDE
		static inline const Type GetType() { return Type::CustomMesh; }
		static inline std::string GetName() { return "Custom Mesh"; }

		void RetransferToBatchMemory() override {
			if (isIndexed)
				Cast::Memory::BatchMemoryHandler.OnBatchEmptyRetransfer(BatchId, vertexData, vertexDataSize, indexData, indexDataSize / sizeof(int));
			else
				Cast::Memory::BatchMemoryHandler.OnBatchEmptyRetransfer(BatchId, vertexData, vertexDataSize);
		}

		virtual UIResponse OnImGuiRender() override {
			bool isOpen = ImGui::CollapsingHeader("Custom Mesh", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap);
			ImGui::SameLine();

			float xOffset = ImGui::GetContentRegionAvail().x - 80.0f;
			if (xOffset > 0.0f) {
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + xOffset);
			}

			if (ImGui::SmallButton("Remove##CustomMesh"))
				return { UIResponse::Code::Remove, Type::CustomMesh };

			if (isOpen) {
				ImGui::Text("No content to show here :)");
			}

			return {};
		}
#pragma endregion
	};
}