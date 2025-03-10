#pragma once

#include "3DCast/Scene/Component/AbstractComponent.h"
#include <3DCast/Memory/Batching/UID.h>

namespace Cast::Component {
	struct CustomMeshComponent : public Component
	{
		uid BatchId = UID::Create();
		Memory::MemoryPosition memPos;
		bool IsAvailable = true;

		float* vertexData = nullptr;
		unsigned int* indexData = nullptr;

		size_t vertexDataSize = 0;
		size_t indexDataSize = 0;

		CustomMeshComponent() = default;
		CustomMeshComponent(const CustomMeshComponent&) = default;
		CustomMeshComponent(size_t vertexBufferSize, size_t indexBufferSize) {
			vertexData = (float*)malloc(vertexBufferSize);
			indexData = (unsigned int*)malloc(indexBufferSize);
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

		~CustomMeshComponent() {
			free(vertexData);
			free(indexData);
		}

		void SwapToDisk() {
			IsAvailable = false;
		}

		void SwapToMemory() {
			IsAvailable = true;
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
			memPos = Cast::Memory::BatchMemoryHandler.AddObject(BatchId, vertexData, vertexDataSize);
		}

		void AddToIndexedBatchMemory() {
			memPos = Cast::Memory::BatchMemoryHandler.AddIndexedObject(BatchId, vertexData, vertexDataSize, indexData, (int)indexDataSize);
		}
	};
}