#pragma once

#include "3DCast/Misc/UID.h"
#include "3DCast/Memory/Batching/LinearBatchStorage.h"

#include <unordered_map>
#include <API/core/Buffer.h>

namespace Cast::Memory
{
	class LinearBatchStorageIndexed
	{
	public:
		LinearBatchStorageIndexed(size_t capacity, size_t indexCapacity);
		~LinearBatchStorageIndexed() = default;

		int CreateBatchObject(uid object, void* data, size_t size, void* indices, int count);
		std::vector<uid> RemoveObject(uid object);

		bool EditObject(uid object, void* data, size_t size);
		bool EditObject(uid object, void* data, size_t size, void* indices, int count);

		int RetransferVertexEntity(uid object, void* data, size_t size, void* indices, int count);

		inline size_t GetCapacity() const { return Capacity; }
		inline size_t GetSize() const { return BatchMemory->GetSize(); }
		inline size_t GetObjectCount() const { return Objects.size(); }
		inline size_t GetAvailableMemory() const { return Capacity - BatchMemory->GetSize(); }
		inline size_t GetAvailableIndexMemory() const { return IndexCapacity - BatchIndices->GetSize(); }

		void Render(Cast::Ref<API::Core::Shader> shader);

		void Clear();

		void SetLayout(Cast::Ref<API::Core::VertexBufferLayout> layout);

	private:
		struct Offset {
			int vertexOffset;
			int indexOffset;
		};

		size_t Capacity;
		size_t IndexCapacity;
		std::unordered_map<uid, Offset> Objects;

		Cast::Ref<API::Core::Buffer> BatchMemory;
		Cast::Ref<API::Core::Buffer> BatchIndices;
		Cast::Ref<API::Core::VertexBufferLayout> Layout;
		Cast::Ref<API::Core::VertexArray> VertexArray;
	};
}
