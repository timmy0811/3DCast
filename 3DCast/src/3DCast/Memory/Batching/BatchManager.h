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

		void InitBulk();
		void AddToBulk(uid object);
		std::vector<uid> RemoveBulk(bool postClear = false);

		void Clear();
		void DeleteUnused();

		void Render(Ref<API::Core::Shader> shader) const;
		void RenderIndexed(Ref<API::Core::Shader> shader) const;

		inline bool IsEntityInBatchStorage(const uid entityId)
		{
			return EntityIdToMemoryPosition.find(entityId) != EntityIdToMemoryPosition.end();
		}

#pragma region ANALYTICS
		inline size_t GetBatchStorageCount() const { return BatchStorages.size(); }
		inline size_t GetBatchStorageIndexedCount() const { return BatchStoragesIndexed.size(); }
		inline size_t GetTotalObjectCount() const { return EntityIdToMemoryPosition.size(); }
		inline size_t GetBulkObjectCount() const { return BulkDisplay.size(); }

		inline size_t GetBatchStorageSize() const { return BatchStorageSize; }

		inline size_t GetBatchStorageMemoryUsed(const int index) const
		{
			if (index >= 0 && index < BatchStorages.size())
				return BatchStorages[index].GetSize();
			return 0;
		}

		inline size_t GetBatchStorageMemoryCapacity(const int index) const
		{
			if (index >= 0 && index < BatchStorages.size())
				return BatchStorages[index].GetCapacity();
			return 0;
		}

		inline size_t GetBatchStorageObjectCount(const int index) const
		{
			if (index >= 0 && index < BatchStorages.size())
				return BatchStorages[index].GetObjectCount();
			return 0;
		}

		inline int GetMaxIndices() const { return MaxIndices; }

		inline size_t GetBatchStorageIndexedMemoryUsed(const int index) const
		{
			if (index >= 0 && index < BatchStoragesIndexed.size())
				return BatchStoragesIndexed[index].GetSize();
			return 0;
		}

		inline size_t GetBatchStorageIndexedMemoryCapacity(const int index) const
		{
			if (index >= 0 && index < BatchStoragesIndexed.size())
				return BatchStoragesIndexed[index].GetCapacity();
			return 0;
		}

		inline size_t GetBatchStorageIndexedObjectCount(const int index) const
		{
			if (index >= 0 && index < BatchStoragesIndexed.size())
				return BatchStoragesIndexed[index].GetObjectCount();
			return 0;
		}

		inline size_t GetTotalBatchStorageMemoryUsed() const
		{
			size_t total = 0;
			for (const auto& storage : BatchStorages)
				total += storage.GetSize();
			return total;
		}

		inline size_t GetTotalBatchStorageMemoryCapacity() const
		{
			size_t total = 0;
			for (const auto& storage : BatchStorages)
				total += storage.GetCapacity();
			return total;
		}

		inline size_t GetTotalBatchStorageIndexedMemoryUsed() const
		{
			size_t total = 0;
			for (const auto& storage : BatchStoragesIndexed)
				total += storage.GetSize();
			return total;
		}

		inline size_t GetTotalBatchStorageIndexedMemoryCapacity() const
		{
			size_t total = 0;
			for (const auto& storage : BatchStoragesIndexed)
				total += storage.GetCapacity();
			return total;
		}
#pragma endregion

	private:
		static void QueueRetransfers(const std::vector<uid>& ids);

	private:
		bool HasBeenInitialized = false;

		size_t BatchStorageSize{};
		int MaxIndices{};
		std::vector<LinearBatchStorage> BatchStorages;
		std::vector<LinearBatchStorageIndexed> BatchStoragesIndexed;
		std::unordered_map<uid, MemoryPosition> EntityIdToMemoryPosition;
		std::vector<uid> BulkDisplay;

		std::unordered_set<int> ActiveBulks;
		std::unordered_set<int> ActiveIndexedBulks;

		Ref<API::Core::VertexBufferLayout> Layout;
	};

	extern Cast::Memory::BatchManager BatchMemoryHandler;
}
