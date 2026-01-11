#pragma once

#include "3DCast/Scene/Component/AbstractComponent.h"
#include "3DCast/Memory/Batching/BatchManager.h"
#include "3DCast/Misc/UID.h"
#include "3DCast/Data/GlobalShared.h"

#include <imgui.h>

#include "../../Util/Helper.h"

namespace Cast::Component
{
	struct CustomMeshComponent final : public Component, public IVertexEntity
	{
#pragma region DATA

	private:
		uid BatchId = UID::None();

	public:
		Memory::MemoryPosition memPos{};
		bool isAvailableInMemory = true;

		float* vertexData = nullptr;
		unsigned int* indexData = nullptr;
		bool isIndexed = false;

		size_t vertexDataSize = 0;
		size_t indexDataSize = 0;
		bool isHeapAlloc = false;
#pragma endregion

#pragma region CONSTRUCTOR
		explicit CustomMeshComponent() = default;

		// Copy constructor - creates a deep copy without sharing BatchId
		CustomMeshComponent(const CustomMeshComponent& other)
			: BatchId(UID::None()),  // New copy needs its own BatchId
			  memPos{},  // Will be set when added to batch
			  isAvailableInMemory(other.isAvailableInMemory),
			  vertexData(nullptr),
			  indexData(nullptr),
			  isIndexed(other.isIndexed),
			  vertexDataSize(other.vertexDataSize),
			  indexDataSize(other.indexDataSize),
			  isHeapAlloc(false)
		{
			// Deep copy vertex data if it exists
			if (other.vertexData && other.vertexDataSize > 0)
			{
				vertexData = static_cast<float*>(malloc(other.vertexDataSize));
				memcpy(vertexData, other.vertexData, other.vertexDataSize);
				isHeapAlloc = true;
			}

			// Deep copy index data if it exists
			if (other.indexData && other.indexDataSize > 0)
			{
				indexData = static_cast<unsigned int*>(malloc(other.indexDataSize));
				memcpy(indexData, other.indexData, other.indexDataSize);
			}
		}

		// Copy assignment operator
		CustomMeshComponent& operator=(const CustomMeshComponent& other)
		{
			if (this != &other)
			{
				// Clean up existing resources
				if (isHeapAlloc)
				{
					free(vertexData);
					free(indexData);
				}

				if (BatchId != UID::None())
				{
					Memory::BatchMemoryHandler.AddToBulk(BatchId);
					Shared.VertexEntities.erase(BatchId);
				}

				// Copy data without sharing BatchId
				BatchId = UID::None();
				memPos = {};
				isAvailableInMemory = other.isAvailableInMemory;
				isIndexed = other.isIndexed;
				vertexDataSize = other.vertexDataSize;
				indexDataSize = other.indexDataSize;
				isHeapAlloc = false;

				vertexData = nullptr;
				indexData = nullptr;

				// Deep copy vertex data if it exists
				if (other.vertexData && other.vertexDataSize > 0)
				{
					vertexData = static_cast<float*>(malloc(other.vertexDataSize));
					memcpy(vertexData, other.vertexData, other.vertexDataSize);
					isHeapAlloc = true;
				}

				// Deep copy index data if it exists
				if (other.indexData && other.indexDataSize > 0)
				{
					indexData = static_cast<unsigned int*>(malloc(other.indexDataSize));
					memcpy(indexData, other.indexData, other.indexDataSize);
				}
			}
			return *this;
		}

		explicit CustomMeshComponent(const size_t vertexBufferSize, const size_t indexBufferSize)
		{
			vertexData = static_cast<float*>(malloc(vertexBufferSize));
			indexData = static_cast<unsigned int*>(malloc(indexBufferSize));

			isHeapAlloc = true;
		}

		CustomMeshComponent(CustomMeshComponent&& other) noexcept
			: BatchId(other.BatchId), memPos(other.memPos),
			  isAvailableInMemory(other.isAvailableInMemory),
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
				isAvailableInMemory = other.isAvailableInMemory;
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
			isAvailableInMemory = false;
		}

		void SwapToMemory()
		{
			isAvailableInMemory = true;
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

		void CopyIntoVertexBuffer(const float* data, const size_t size) const
		{
			memcpy(vertexData, data, size);
		}

		void CopyIntoIndexBuffer(const unsigned int* data, const size_t size) const
		{
			memcpy(indexData, data, size);
		}

		void SetVertexBuffer(float* data, const size_t size, const bool needsCleanup = false)
		{
			vertexData = data;
			vertexDataSize = size;

			isHeapAlloc = needsCleanup;
		}

		void SetIndexBuffer(unsigned int* data, const size_t size, const bool needsCleanup = false)
		{
			indexData = data;
			indexDataSize = size;

			isHeapAlloc = needsCleanup;
		}

		void PatchRegistryData(const int samplerIndex, const int transformIndex) const
		{
			if (vertexData && vertexDataSize > 0)
				Helper::patchRegistryDataOnVertexBlob((Memory::BatchVertexShaderObject*)vertexData, vertexDataSize / sizeof(Memory::BatchVertexShaderObject), samplerIndex, transformIndex);
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
			const bool isOpen = ImGui::CollapsingHeader(ICON_FA_DRAW_POLYGON "  Custom Mesh", ImGuiTreeNodeFlags_AllowItemOverlap);
			ImGui::SameLine();

			const float xOffset = ImGui::GetContentRegionAvail().x - 30.0f;
			if (xOffset > 0.0f)
			{
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + xOffset);
			}

			if (ImGui::SmallButton(ICON_FA_TRASH_CAN "##CustomMesh"))
				return {UIResponse::Code::Remove, Type::CustomMesh};

			if (isOpen)
			{
				ImGui::Text("No content to show here :)");
				ImGui::Dummy(ImVec2(0.f, DUMMYSPACE_AFTER_COMPONENT));
			}

			return {};
		}
#pragma endregion
	};
}
