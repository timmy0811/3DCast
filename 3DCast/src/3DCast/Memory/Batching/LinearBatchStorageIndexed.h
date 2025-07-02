#pragma once

#include "3DCast/Misc/UID.h"

#include <unordered_map>

#include <API/core/Buffer.h>
#include <API/core/Shader.h>
#include <API/core/VertexBufferLayout.h>
#include <API/core/VertexArray.h>

namespace Cast::Memory
{
	class LinearBatchStorageIndexed
	{
	public:
		LinearBatchStorageIndexed(size_t capacity, size_t indexCapacity);
		~LinearBatchStorageIndexed() = default;

		int CreateBatchObject(uid object, const void* data, size_t size, const void* indices, int count);
		std::vector<uid> RemoveObject(uid object);

		bool EditObject(uid object, const void* data, size_t size, const void* indices,
		                int count);
		bool EditObject(uid object, const void* data, size_t size);
		bool EditObject(size_t offset_vert, const void* data, size_t size, size_t offset_ind, const void* indices,
		                int count) const;

		int RetransferVertexEntity(uid object, const void* data, size_t size, const void* indices, int count);

		inline size_t GetCapacity() const { return Capacity; }
		inline size_t GetSize() const { return BatchMemory->GetSize(); }
		inline size_t GetObjectCount() const { return Objects.size(); }
		inline size_t GetAvailableMemory() const { return Capacity - BatchMemory->GetSize(); }
		inline size_t GetAvailableIndexMemory() const { return IndexCapacity - BatchIndices->GetSize(); }

		void Render(Ref<API::Core::Shader> shader) const;

		void Clear();

		void SetLayout(Cast::Ref<API::Core::VertexBufferLayout> layout);

	private:
		struct Offset
		{
			int vertexOffset;
			int indexOffset;
		};

		size_t Capacity;
		size_t IndexCapacity;
		std::unordered_map<uid, Offset> Objects;

		Ref<API::Core::Buffer> BatchMemory;
		Ref<API::Core::Buffer> BatchIndices;
		Ref<API::Core::VertexBufferLayout> Layout;
		Ref<API::Core::VertexArray> VertexArray;
	};
}
