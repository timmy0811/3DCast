#pragma once

#include "3DCast/Misc/UID.h"
#include "3DCast/Memory/Batching/LinearBatchStorage.h"
#include "3DCast/Memory/Batching/LinearBatchStorageIndexed.h"

#include <API/core/Shader.h>

namespace Cast::Memory
{
	struct MemoryPosition {
		int batchStorageId;
		int batchStorageInstancedId;
		int offset;
	};

	class BatchManager
	{
	public:
		BatchManager() = default;
		~BatchManager() = default;

		void Init(size_t defaultStorageSize, int maxIndices);

		uid CreateBatchObject(void* data, size_t size);
		uid CreateBatchObject(void* data, size_t size, void* indices, int count);

		void EditObject(uid objectId, void* data, size_t size);
		void EditObject(uid objectId, void* data, size_t size, void* indices, int count);

		void OnBatchEmptyRetransfer(uid objectId, void* data, size_t size);
		void OnBatchEmptyRetransfer(uid objectId, void* data, size_t size, void* indices, int count);

		std::vector<uid> RemoveObject(uid objectId);

		void Clear();
		void DeleteUnused();

		void Render(Cast::Ref<API::Core::Shader> shader);
		void RenderIndexed(Cast::Ref<API::Core::Shader> shader);

		inline bool IsEntityInBatchStorage(uid entityId)
		{
			return EntityIdToMemoryPosition.find(entityId) != EntityIdToMemoryPosition.end();
		}

	private:
		void QueueRetransfers(std::vector<uid>& ids);

	private:
		bool HasBeenInitialized = false;

		size_t BatchStorageSize;
		int MaxIndices;
		std::vector<LinearBatchStorage> BatchStorages;
		std::vector<LinearBatchStorageIndexed> BatchStoragesIndexed;
		std::unordered_map<uid, MemoryPosition> EntityIdToMemoryPosition;

		Cast::Ref<API::Core::VertexBufferLayout> Layout;
	};

	extern Cast::Memory::BatchManager BatchMemoryHandler;
}
