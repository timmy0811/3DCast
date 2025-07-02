#pragma once

#include "3DCast/Misc/UID.h"

#include <unordered_map>
#include <API/core/Buffer.h>
#include <API/core/Shader.h>
#include <API/core/VertexBufferLayout.h>
#include <API/core/VertexArray.h>

namespace Cast::Memory
{
	class LinearBatchStorage
	{
	public:
		explicit LinearBatchStorage(size_t capacity);
		~LinearBatchStorage() = default;

		int CreateBatchObject(uid object, const void* data, size_t size);
		std::vector<uid> RemoveObject(uid object);

		bool EditObject(uid object, const void* data, size_t size);
		void EditObject(size_t offset, const void* data, size_t size) const;

		int RetransferVertexEntity(uid object, const void* data, size_t size);

		inline size_t GetCapacity() const { return Capacity; }
		inline size_t GetSize() const { return BatchMemory->GetSize(); }
		inline size_t GetObjectCount() const { return Objects.size(); }
		inline size_t GetAvailableMemory() const { return Capacity - BatchMemory->GetSize(); }

		void Render(Ref<API::Core::Shader> shader) const;

		void Clear();

		void SetLayout(Ref<API::Core::VertexBufferLayout> layout);

	private:
		size_t Capacity;
		std::unordered_map<uid, int> Objects;

		Ref<API::Core::Buffer> BatchMemory;
		Ref<API::Core::VertexBufferLayout> Layout;
		Ref<API::Core::VertexArray> VertexArray;
	};
}
