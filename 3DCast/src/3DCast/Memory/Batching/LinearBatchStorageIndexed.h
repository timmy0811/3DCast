#pragma once

#include "3DCast/Misc/UID.h"

#include <unordered_map>

#include <API/core/Buffer.h>
#include <API/core/Shader.h>
#include <API/core/VertexBufferLayout.h>

#include "IBatchStorage.h"

namespace Cast::Memory
{
	class LinearBatchStorageIndexed final : public IBatchStorage
	{
	public:
		explicit LinearBatchStorageIndexed(size_t capacity, size_t indexCapacity);
		~LinearBatchStorageIndexed() override = default;

		int CreateBatchObject(uid object, const void* data, size_t size, const void* indices, int count);
		std::vector<uid> RemoveObject(uid object, bool flushAll = false) override;

		std::vector<uid> RemoveBulk() override;

		bool EditObject(uid object, const void* data, size_t size, const void* indices,
		                int count);
		bool EditObject(uid object, const void* data, size_t size);
		bool EditObject(size_t offset_vert, const void* data, size_t size, size_t offset_ind, const void* indices,
		                int count) const;

		int RetransferVertexEntity(uid object, const void* data, size_t size, const void* indices, int count);

		inline size_t GetObjectCount() const override { return Objects.size(); }
		inline size_t GetAvailableIndexMemory() const { return IndexCapacity - BatchIndices->GetSize(); }

		inline bool IsObjectInStorage(const uid object) const override
		{
			return Objects.find(object) != Objects.end();
		}

		void Clear() override;
		void Render(Ref<API::Core::Shader> shader) const override;

	private:
		struct Offset
		{
			int vertexOffset;
			int indexOffset;
		};

		size_t IndexCapacity;
		std::unordered_map<uid, Offset> Objects;

		Ref<API::Core::Buffer> BatchIndices;
	};
}
