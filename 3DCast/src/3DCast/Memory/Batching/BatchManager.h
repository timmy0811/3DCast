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

		MemoryPosition AddObject(uid objectId, void* data, size_t size);
		MemoryPosition AddIndexedObject(uid objectId, void* data, size_t size, void* indices, int count);

		void EditObject(uid objectId, void* data, size_t size);
		void EditObject(MemoryPosition pos, void* data, size_t size);

		std::vector<uid> RemoveIndexedObject(uid objectId);
		std::vector<uid> RemoveObject(uid objectId);

		void Clear();
		void DeleteUnused();

		void Render(Cast::Ref<API::Core::Shader> shader);
		void RenderIndexed(Cast::Ref<API::Core::Shader> shader);

	private:
		bool HasBeenInitialized = false;

		size_t BatchStorageSize;
		int MaxIndices;
		std::vector<LinearBatchStorage> BatchStorages;
		std::vector<LinearBatchStorageIndexed> BatchStoragesIndexed;

		Cast::Ref<API::Core::VertexBufferLayout> Layout;
	};

	extern Cast::Memory::BatchManager BatchMemoryHandler;
}
