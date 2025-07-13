#pragma once

#include "3DCast/Scene/Component/AbstractComponent.h"
#include "3DCast/Memory/Batching/BatchManager.h"
#include "3DCast/Misc/UID.h"
#include "3DCast/Data/GlobalShared.h"

#include <imgui.h>

namespace Cast::Component
{
	struct CustomMeshComponent final : public Component, public IVertexEntity
	{
#pragma region DATA

	private:
		uid BatchId = UID::None();

	public:
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
		explicit CustomMeshComponent() = default;
		CustomMeshComponent(const CustomMeshComponent&) = default;

		explicit CustomMeshComponent(const size_t vertexBufferSize, const size_t indexBufferSize)
		{
			vertexData = static_cast<float*>(malloc(vertexBufferSize));
			indexData = static_cast<unsigned int*>(malloc(indexBufferSize));

			isHeapAlloc = true;
		}

		CustomMeshComponent(CustomMeshComponent&& other) noexcept
			: BatchId(other.BatchId), memPos(other.memPos),
			  IsAvailableInMemory(other.IsAvailableInMemory),
			  vertexData(other.vertexData), indexData(other.indexData),
			  isIndexed(other.isIndexed), vertexDataSize(other.vertexDataSize),
			  indexDataSize(other.indexDataSize), isHeapAlloc(other.isHeapAlloc)
		{
			if (BatchId != UID::None())
			{
				Shared.VertexEntities[BatchId] = this;
			}

			other.vertexData = nullptr;
			other.indexData = nullptr;
			other.BatchId = UID::None();
			other.isHeapAlloc = false;
		}

		CustomMeshComponent& operator=(CustomMeshComponent&& other) noexcept
		{
			if (this != &other)
			{
				if (isHeapAlloc)
				{
					free(vertexData);
					free(indexData);
				}

				if (BatchId != UID::None())
				{
					Shared.VertexEntities.erase(BatchId);
				}

				BatchId = other.BatchId;
				memPos = other.memPos;
				IsAvailableInMemory = other.IsAvailableInMemory;
				vertexData = other.vertexData;
				indexData = other.indexData;
				isIndexed = other.isIndexed;
				vertexDataSize = other.vertexDataSize;
				indexDataSize = other.indexDataSize;
				isHeapAlloc = other.isHeapAlloc;

				// entt transfers ownership of the last added component to the deleted componen (pointer) to reduce memory size
				// references to it need to be updated
				if (BatchId != UID::None())
				{
					Shared.VertexEntities[BatchId] = this;
				}

				// Invalidate the source component
				other.vertexData = nullptr;
				other.indexData = nullptr;
				other.BatchId = UID::None();
				other.isHeapAlloc = false;
			}
			return *this;
		}

		~CustomMeshComponent() override
		{
			if (isHeapAlloc)
			{
				free(vertexData);
				free(indexData);
			}

			if (BatchId != UID::None())
			{
				Memory::BatchMemoryHandler.AddToBulk(BatchId);
				Shared.VertexEntities.erase(BatchId);
				BatchId = UID::None();
			}
		}
#pragma endregion

#pragma region UTILITY
		void OnAfterEntitySetBehaviour() override
		{
			Shared.ActiveScene->RegisterTransformComponent(EntityNode);
		}

		void AllocVertexData(const size_t size)
		{
			free(vertexData);
			vertexData = static_cast<float*>(malloc(size));
			vertexDataSize = size;
		}

		void AllocIndexData(const size_t size)
		{
			free(indexData);
			indexData = static_cast<unsigned int*>(malloc(size));
			indexDataSize = size;
		}

		void SwapToDisk()
		{
			IsAvailableInMemory = false;
		}

		void SwapToMemory()
		{
			IsAvailableInMemory = true;
		}

		void AddAndAllocVertexData(const float* data, const size_t size)
		{
			AllocVertexData(size);
			memcpy(vertexData, data, size);
		}

		void AddAndAllocIndexData(const unsigned int* data, const size_t size)
		{
			AllocIndexData(size);
			memcpy(indexData, data, size);
		}

		void AddVertexData(const float* data, const size_t size) const
		{
			memcpy(vertexData, data, size);
		}

		void AddIndexData(const unsigned int* data, const size_t size) const
		{
			memcpy(indexData, data, size);
		}

		void Clear()
		{
			free(vertexData);
			free(indexData);
			vertexData = nullptr;
			indexData = nullptr;
			vertexDataSize = 0;
			indexDataSize = 0;
		}

		void AddToBatchMemory()
		{
			BatchId = Memory::BatchMemoryHandler.CreateBatchObject(vertexData, vertexDataSize);

			if (BatchId != UID::None())
			{
				Shared.VertexEntities[BatchId] = this;
			}
		}

		void AddToIndexedBatchMemory()
		{
			BatchId = Memory::BatchMemoryHandler.CreateBatchObject(vertexData, vertexDataSize, indexData, static_cast<int>(indexDataSize));
			isIndexed = true;

			if (BatchId != UID::None())
			{
				Shared.VertexEntities[BatchId] = this;
			}
		}
#pragma endregion

#pragma region OVERRIDE
		static inline Type GetType() { return Type::CustomMesh; }
		static inline std::string GetName() { return "Custom Mesh"; }

		void RetransferToBatchMemory() override
		{
			if (isIndexed)
				Memory::BatchMemoryHandler.OnBatchEmptyRetransfer(BatchId, vertexData, vertexDataSize, indexData, indexDataSize / sizeof(int));
			else
				Memory::BatchMemoryHandler.OnBatchEmptyRetransfer(BatchId, vertexData, vertexDataSize);
		}

		UIResponse OnImGuiRender() override
		{
			const bool isOpen = ImGui::CollapsingHeader("Custom Mesh", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap);
			ImGui::SameLine();

			const float xOffset = ImGui::GetContentRegionAvail().x - 80.0f;
			if (xOffset > 0.0f)
			{
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + xOffset);
			}

			if (ImGui::SmallButton("Remove##CustomMesh"))
				return {UIResponse::Code::Remove, Type::CustomMesh};

			if (isOpen)
			{
				ImGui::Text("No content to show here :)");
			}

			return {};
		}
#pragma endregion
	};
}
