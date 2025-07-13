#include "castpch.h"
#include "BatchManager.h"

#include "3DCast/Data/GlobalShared.h"
#include "3DCast/Scene/Component/CustomMeshComponent.h"

namespace Cast::Memory
{
	BatchManager BatchMemoryHandler = {};
}

void Cast::Memory::BatchManager::Init(const size_t defaultStorageSize, const int maxIndices)
{
	if (HasBeenInitialized)
	{
		LOG_CORE_WARN("Batch manager has already been initialized. Ignoring the initialization request.");
		return;
	}

	LOG_CORE_INFO(
		"Initializing batch manager with default storage size " + std::to_string(defaultStorageSize) +
		" and max indices " + std::to_string(maxIndices));

	BatchStorageSize = defaultStorageSize;
	MaxIndices = maxIndices;

	Layout.reset(API::Core::VertexBufferLayout::Create());
	Layout->Push(API::Core::ShaderDataType::Float3);
	Layout->Push(API::Core::ShaderDataType::Float3);
	Layout->Push(API::Core::ShaderDataType::Float3);
	Layout->Push(API::Core::ShaderDataType::Float3);
	Layout->Push(API::Core::ShaderDataType::Float2);
	Layout->Push(API::Core::ShaderDataType::Float);
	Layout->Push(API::Core::ShaderDataType::Float);

	BatchStorages.emplace_back(defaultStorageSize);
	BatchStorages[BatchStorages.size() - 1].SetLayout(Layout);

	BatchStoragesIndexed.emplace_back(defaultStorageSize, maxIndices * sizeof(unsigned int));
	BatchStoragesIndexed[BatchStoragesIndexed.size() - 1].SetLayout(Layout);

	HasBeenInitialized = true;
}

Cast::uid Cast::Memory::BatchManager::CreateBatchObject(const void* data, const size_t size)
{
	if (size > BatchStorageSize)
	{
		LOG_CORE_ERROR(
			"Object size is larger than the batch storage size. Object will not be added to the batch storage.");
		return UID::None();
	}

	const uid id = UID::Create();
	LOG_CORE_INFO("Adding object with ID " + std::to_string(id) + " to the batch storage.");

	// Add object to the first batch storage that has enough space
	int offset = -3;
	for (int i = 0; i < BatchStorages.size(); i++)
	{
		offset = BatchStorages[i].CreateBatchObject(id, data, size);
		if (offset != -1)
		{
			EntityIdToMemoryPosition[id] = MemoryPosition{i, -1, offset};
			return id;
		}
	}

	// If no batch storage has enough space, create a new one
	std::string cause;
	if (offset == -1)
		cause = "Exceeding vertex buffer capacity.";
	else
		cause = "Internal error.";

	LOG_CORE_INFO("Creating new batch storage for object with ID " + std::to_string(id) + " | Cause: " + cause);

	BatchStorages.emplace_back(BatchStorageSize);
	BatchStorages[BatchStorages.size() - 1].SetLayout(Layout);
	offset = BatchStorages[BatchStorages.size() - 1].CreateBatchObject(id, data, size);
	EntityIdToMemoryPosition[id] = MemoryPosition{static_cast<int>(BatchStorages.size()) - 1, -1, offset};

	return id;
}

Cast::uid Cast::Memory::BatchManager::CreateBatchObject(const void* data, const size_t size, const void* indices,
                                                        const int count)
{
	if (size > BatchStorageSize)
	{
		LOG_CORE_ERROR(
			"Object size is larger than the batch storage size. Object will not be added to the batch storage.");
		return UID::None();
	}
	else if (count > MaxIndices)
	{
		LOG_CORE_ERROR(
			"Object indices size is larger than the batch storage index size. Object will not be added to the batch storage.");
		return UID::None();
	}

	const uid id = UID::Create();
	LOG_CORE_INFO("Adding indexed object with ID " + std::to_string(id) + " to the batch storage.");

	// Add object to the first batch storage that has enough space
	int offset = -3;
	for (int i = 0; i < BatchStoragesIndexed.size(); i++)
	{
		offset = BatchStoragesIndexed[i].CreateBatchObject(id, data, size, indices, count);
		if (offset >= 0)
		{
			EntityIdToMemoryPosition[id] = MemoryPosition{-1, i, offset};
			return id;
		}
	}

	// If no batch storage has enough space, create a new one
	std::string cause;
	if (offset == -1)
		cause = "Exceeding vertex buffer capacity.";
	else if (offset == -2)
		cause = "Exceeding index buffer capacity.";
	else
		cause = "Internal error.";

	LOG_CORE_INFO("Creating new batch storage for object with ID " + std::to_string(id) + " | Cause: " + cause);

	BatchStoragesIndexed.emplace_back(BatchStorageSize, MaxIndices);
	BatchStoragesIndexed[BatchStoragesIndexed.size() - 1].SetLayout(Layout);
	offset = BatchStoragesIndexed[BatchStoragesIndexed.size() - 1].CreateBatchObject(id, data, size, indices, count);
	EntityIdToMemoryPosition[id] = MemoryPosition{-1, static_cast<int>(BatchStoragesIndexed.size()) - 1, offset};

	return id;
}

void Cast::Memory::BatchManager::EditObject(const uid objectId, const void* data, const size_t size)
{
	if (!IsEntityInBatchStorage(objectId))
	{
		LOG_CORE_WARN(
			"Object with ID " + std::to_string(objectId) +
			" not found in any batch storage. Edit an object that is always part of the batch storage.");
		return;
	}

	MemoryPosition pos = EntityIdToMemoryPosition[objectId];

	if (pos.batchStorageId != -1)
	{
		if (BatchStorages[pos.batchStorageId].EditObject(objectId, data, size))
			return;
	}
	else if (pos.batchStorageIndexedId != -1)
	{
		if (BatchStoragesIndexed[pos.batchStorageIndexedId].EditObject(objectId, data, size))
			return;
	}

	LOG_CORE_WARN(
		"Object with ID " + std::to_string(objectId) + " not found in any batch storage. Could not update the object.");
}

void Cast::Memory::BatchManager::EditObject(const uid objectId, const void* data, const size_t size, void* indices,
                                            const int count)
{
	if (!IsEntityInBatchStorage(objectId))
	{
		LOG_CORE_WARN(
			"Object with ID " + std::to_string(objectId) +
			" not found in any batch storage. Edit an object that is always part of the batch storage");
		return;
	}

	const MemoryPosition pos = EntityIdToMemoryPosition[objectId];

	if (pos.batchStorageIndexedId != -1)
	{
		if (BatchStoragesIndexed[pos.batchStorageIndexedId].EditObject(objectId, data, size, indices, count))
			return;
	}

	LOG_CORE_WARN(
		"Object with ID " + std::to_string(objectId) + " not found in any batch storage. Could not update the object.");
}

void Cast::Memory::BatchManager::OnBatchEmptyRetransfer(const uid objectId, const void* data, const size_t size)
{
	if (!IsEntityInBatchStorage(objectId))
	{
		LOG_CORE_WARN(
			"Object with ID " + std::to_string(objectId) +
			" not found in any batch storage. Retransfer an object that is always part of the batch storage");
		return;
	}

	const MemoryPosition pos = EntityIdToMemoryPosition[objectId];

	if (pos.batchStorageId != -1)
	{
		BatchStorages[pos.batchStorageId].RetransferVertexEntity(objectId, data, size);
	}
}

void Cast::Memory::BatchManager::OnBatchEmptyRetransfer(const uid objectId, const void* data, const size_t size,
                                                        const void* indices, const int count)
{
	if (!IsEntityInBatchStorage(objectId))
	{
		LOG_CORE_WARN(
			"Object with ID " + std::to_string(objectId) +
			" not found in any batch storage. Retransfer an object that is always part of the indexed batch storage");
		return;
	}

	const MemoryPosition pos = EntityIdToMemoryPosition[objectId];

	if (pos.batchStorageIndexedId != -1)
	{
		BatchStoragesIndexed[pos.batchStorageIndexedId].RetransferVertexEntity(objectId, data, size, indices, count);
	}
}

std::vector<Cast::uid> Cast::Memory::BatchManager::RemoveObject(const uid objectId)
{
	const MemoryPosition position = EntityIdToMemoryPosition[objectId];

	if (position.batchStorageId != -1)
	{
		auto ids = BatchStorages[position.batchStorageId].RemoveObject(objectId);
		EntityIdToMemoryPosition.erase(objectId);

		if (!ids.empty() && ids[0] != UID::None())
			QueueRetransfers(ids);
		return ids;
	}
	else if (position.batchStorageIndexedId != -1)
	{
		auto ids = BatchStoragesIndexed[position.batchStorageIndexedId].RemoveObject(objectId);
		EntityIdToMemoryPosition.erase(objectId);

		if (!ids.empty() && ids[0] != UID::None())
			QueueRetransfers(ids);
		return ids;
	}

	LOG_CORE_WARN("Object with ID " + std::to_string(objectId) + " not found in any batch storage.");
	return {};
}

void Cast::Memory::BatchManager::InitBulk()
{
	BulkDisplay.clear();

	for (const int id : ActiveBulks)
	{
		BatchStorages[id].InitBulk();
	}
	ActiveBulks.clear();

	for (const int id : ActiveIndexedBulks)
	{
		BatchStoragesIndexed[id].InitBulk();
	}
	ActiveIndexedBulks.clear();
}

void Cast::Memory::BatchManager::AddToBulk(uid object)
{
	if (IsEntityInBatchStorage(object))
	{
		BulkDisplay.push_back(object);
		const MemoryPosition pos = EntityIdToMemoryPosition[object];

		if (pos.batchStorageId != -1)
		{
			BatchStorages[pos.batchStorageId].AddToBulk(object);
			ActiveBulks.insert(pos.batchStorageId);
		}
		else if (pos.batchStorageIndexedId != -1)
		{
			BatchStoragesIndexed[pos.batchStorageIndexedId].AddToBulk(object);
			ActiveIndexedBulks.insert(pos.batchStorageIndexedId);
		}
		else
		{
			LOG_CORE_WARN("Cannot add to bulk. Object with ID " + std::to_string(object) + " not found in any batch storage.");
		}
	}
}

std::vector<Cast::uid> Cast::Memory::BatchManager::RemoveBulk(const bool postClear)
{
	std::vector<uid> mergeList;
	mergeList.reserve(10);

	for (const int id : ActiveBulks)
	{
		std::vector<uid> ids = BatchStorages[id].RemoveBulk();
		mergeList.reserve(mergeList.size() + ids.size());
		mergeList.insert(mergeList.end(), ids.begin(), ids.end());

		if (postClear) BatchStorages[id].InitBulk();
	}

	for (const int id : ActiveIndexedBulks)
	{
		std::vector<uid> ids = BatchStoragesIndexed[id].RemoveBulk();
		mergeList.reserve(mergeList.size() + ids.size());
		mergeList.insert(mergeList.end(), ids.begin(), ids.end());

		if (postClear) BatchStoragesIndexed[id].InitBulk();
	}

	if (postClear)
	{
		ActiveBulks.clear();
		ActiveIndexedBulks.clear();
		BulkDisplay.clear();
	}

	if (!mergeList.empty() && mergeList[0] != UID::None())
		QueueRetransfers(mergeList);

	return mergeList;
}

void Cast::Memory::BatchManager::Clear()
{
	for (auto& BatchStorage : BatchStorages)
	{
		BatchStorage.Clear();
	}
}

void Cast::Memory::BatchManager::DeleteUnused()
{
	for (int i = 0; i < BatchStorages.size(); i++)
	{
		if (BatchStorages[i].GetObjectCount() == 0)
		{
			BatchStorages.erase(BatchStorages.begin() + i);
			i--;
		}
	}

	for (int i = 0; i < BatchStoragesIndexed.size(); i++)
	{
		if (BatchStoragesIndexed[i].GetObjectCount() == 0)
		{
			BatchStoragesIndexed.erase(BatchStoragesIndexed.begin() + i);
			i--;
		}
	}
}

void Cast::Memory::BatchManager::Render(const Ref<API::Core::Shader> shader) const
{
	for (const auto& BatchStorage : BatchStorages)
	{
		BatchStorage.Render(shader);
	}
}

void Cast::Memory::BatchManager::RenderIndexed(const Ref<API::Core::Shader> shader) const
{
	for (const auto& i : BatchStoragesIndexed)
	{
		i.Render(shader);
	}
}

void Cast::Memory::BatchManager::QueueRetransfers(const std::vector<uid>& ids)
{
	for (uid id : ids)
	{
		if (Shared.VertexEntities.find(id) != Shared.VertexEntities.end())
		{
			Shared.VertexEntities[id]->RetransferToBatchMemory();
		}
	}
}
