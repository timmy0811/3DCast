#pragma once

#include "3DCast/Misc/UID.h"
#include "3DCast/Memory/Batching/LinearBatchStorage.h"
#include "3DCast/Memory/Batching/LinearBatchStorageIndexed.h"

#include <API/core/Shader.h>

namespace Cast::Memory
{
	struct MemoryPosition
	{
		int batchStorageId;
		int batchStorageIndexedId;
		int offset;
	};

	class BatchManager
	{
	public:
		BatchManager() = default;
		~BatchManager() = default;

		void Init(size_t defaultStorageSize, int maxIndices);

		uid CreateBatchObject(const void* data, size_t size);
		uid CreateBatchObject(const void* data, size_t size, const void* indices, int count);

		void EditObject(uid objectId, const void* data, size_t size);
		void EditObject(uid objectId, const void* data, size_t size, void* indices, int count);

		void OnBatchEmptyRetransfer(uid objectId, const void* data, size_t size);
		void OnBatchEmptyRetransfer(uid objectId, const void* data, size_t size, const void* indices, int count);

		std::vector<uid> RemoveObject(uid objectId);

		void Clear();
		void DeleteUnused();

		void Render(Ref<API::Core::Shader> shader) const;
		void RenderIndexed(Ref<API::Core::Shader> shader) const;

		inline bool IsEntityInBatchStorage(const uid entityId)
		{
			return EntityIdToMemoryPosition.find(entityId) != EntityIdToMemoryPosition.end();
		}

	private:
		static void QueueRetransfers(const std::vector<uid>& ids);

	private:
		bool HasBeenInitialized = false;

		size_t BatchStorageSize{};
		int MaxIndices{};
		std::vector<LinearBatchStorage> BatchStorages;
		std::vector<LinearBatchStorageIndexed> BatchStoragesIndexed;
		std::unordered_map<uid, MemoryPosition> EntityIdToMemoryPosition;

		Ref<API::Core::VertexBufferLayout> Layout;
	};

	extern Cast::Memory::BatchManager BatchMemoryHandler;
}
