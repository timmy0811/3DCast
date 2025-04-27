#include "castpch.h"
#include "BatchManager.h"

#include "3DCast/Data/ShaderDataObjects/Vertex.h"
#include "3DCast/Data/GlobalShared.h"

namespace Cast::Memory
{
	BatchManager BatchMemoryHandler = {};
}

void Cast::Memory::BatchManager::Init(size_t defaultStorageSize, int maxIndices)
{
	if (HasBeenInitialized) {
		LOG_CORE_WARN("Batch manager has already been initialized. Ignoring the initialization request.");
		return;
	}

	LOG_CORE_INFO("Initializing batch manager with default storage size " + std::to_string(defaultStorageSize) + " and max indices " + std::to_string(maxIndices));

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

	BatchStorages.push_back(LinearBatchStorage(defaultStorageSize));
	BatchStorages[BatchStorages.size() - 1].SetLayout(Layout);

	BatchStoragesIndexed.push_back(LinearBatchStorageIndexed(defaultStorageSize, maxIndices * sizeof(unsigned int)));
	BatchStoragesIndexed[BatchStoragesIndexed.size() - 1].SetLayout(Layout);
}

Cast::uid Cast::Memory::BatchManager::CreateBatchObject(void* data, size_t size)
{
	if (size > BatchStorageSize)
	{
		LOG_CORE_ERROR("Object size is larger than the batch storage size. Object will not be added to the batch storage.");
		return UID::None();
	}

	uid id = UID::Create();
	LOG_CORE_INFO("Adding object with ID " + std::to_string(id) + " to the batch storage.");

	// Add object to the first batch storage that has enough space
	int offset = -3;
	for (int i = 0; i < BatchStorages.size(); i++) {
		int offset = BatchStorages[i].CreateBatchObject(id, data, size);
		if (offset != -1) {
			EntityIdToMemoryPosition[id] = MemoryPosition{ i, -1, offset };
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

	BatchStorages.push_back(LinearBatchStorage(BatchStorageSize));
	BatchStorages[BatchStorages.size() - 1].SetLayout(Layout);
	offset = BatchStorages[BatchStorages.size() - 1].CreateBatchObject(id, data, size);
	EntityIdToMemoryPosition[id] = MemoryPosition{ (int)BatchStorages.size() - 1, -1, offset };

	return id;
}

Cast::uid Cast::Memory::BatchManager::CreateBatchObject(void* data, size_t size, void* indices, int count)
{
	if (size > BatchStorageSize)
	{
		LOG_CORE_ERROR("Object size is larger than the batch storage size. Object will not be added to the batch storage.");
		return UID::None();
	}
	else if (count > MaxIndices)
	{
		LOG_CORE_ERROR("Object indices size is larger than the batch storage index size. Object will not be added to the batch storage.");
		return UID::None();
	}

	uid id = UID::Create();
	LOG_CORE_INFO("Adding indexed object with ID " + std::to_string(id) + " to the batch storage.");

	// Add object to the first batch storage that has enough space
	int offset = -3;
	for (int i = 0; i < BatchStoragesIndexed.size(); i++) {
		offset = BatchStoragesIndexed[i].CreateBatchObject(id, data, size, indices, count);
		if (offset >= 0) {
			EntityIdToMemoryPosition[id] = MemoryPosition{ -1, i, offset };
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

	BatchStoragesIndexed.push_back(LinearBatchStorageIndexed(BatchStorageSize, MaxIndices));
	BatchStoragesIndexed[BatchStoragesIndexed.size() - 1].SetLayout(Layout);
	offset = BatchStoragesIndexed[BatchStoragesIndexed.size() - 1].CreateBatchObject(id, data, size, indices, count);
	EntityIdToMemoryPosition[id] = MemoryPosition{ -1, (int)BatchStoragesIndexed.size() - 1, offset };

	return id;
}

void Cast::Memory::BatchManager::EditObject(uid objectId, void* data, size_t size)
{
	if (!IsEntityInBatchStorage(objectId)) {
		LOG_CORE_WARN("Object with ID " + std::to_string(objectId) + " not found in any batch storage. Edit an object that is always part of the batch storage");
		return;
	}

	MemoryPosition pos = EntityIdToMemoryPosition[objectId];

	if (pos.batchStorageId != -1) {
		if (BatchStorages[pos.batchStorageId].EditObject(objectId, data, size))
			return;
	}
	else if (pos.batchStorageInstancedId != -1) {
		if (BatchStoragesIndexed[pos.batchStorageInstancedId].EditObject(objectId, data, size))
			return;
	}

	LOG_CORE_WARN("Object with ID " + std::to_string(objectId) + " not found in any batch storage. Could not update the object.");
}

void Cast::Memory::BatchManager::EditObject(uid objectId, void* data, size_t size, void* indices, int count)
{
	if (!IsEntityInBatchStorage(objectId)) {
		LOG_CORE_WARN("Object with ID " + std::to_string(objectId) + " not found in any batch storage. Edit an object that is always part of the batch storage");
		return;
	}

	MemoryPosition pos = EntityIdToMemoryPosition[objectId];

	if (pos.batchStorageInstancedId != -1) {
		if (BatchStoragesIndexed[pos.batchStorageInstancedId].EditObject(objectId, data, size, indices, count))
			return;
	}

	LOG_CORE_WARN("Object with ID " + std::to_string(objectId) + " not found in any batch storage. Could not update the object.");
}

void Cast::Memory::BatchManager::OnBatchEmptyRetransfer(uid objectId, void* data, size_t size)
{
	if (!IsEntityInBatchStorage(objectId)) {
		LOG_CORE_WARN("Object with ID " + std::to_string(objectId) + " not found in any batch storage. Retransfer an object that is always part of the batch storage");
		return;
	}

	MemoryPosition pos = EntityIdToMemoryPosition[objectId];

	if (pos.batchStorageId != -1) {
		BatchStorages[pos.batchStorageId].RetransferVertexEntity(objectId, data, size);
	}
}

void Cast::Memory::BatchManager::OnBatchEmptyRetransfer(uid objectId, void* data, size_t size, void* indices, int count)
{
	if (!IsEntityInBatchStorage(objectId)) {
		LOG_CORE_WARN("Object with ID " + std::to_string(objectId) + " not found in any batch storage. Retransfer an object that is always part of the indexed batch storage");
		return;
	}

	MemoryPosition pos = EntityIdToMemoryPosition[objectId];

	if (pos.batchStorageInstancedId != -1) {
		BatchStoragesIndexed[pos.batchStorageInstancedId].RetransferVertexEntity(objectId, data, size, indices, count);
	}
}

std::vector<Cast::uid> Cast::Memory::BatchManager::RemoveObject(uid objectId)
{
	Memory::MemoryPosition position = EntityIdToMemoryPosition[objectId];

	if (position.batchStorageId != -1)
	{
		auto ids = BatchStorages[position.batchStorageId].RemoveObject(objectId);
		EntityIdToMemoryPosition.erase(objectId);

		if (!ids.empty() && ids[0] != UID::None())
			QueueRetransfers(ids);
		return ids;
	}
	else if (position.batchStorageInstancedId != -1)
	{
		auto ids = BatchStoragesIndexed[position.batchStorageInstancedId].RemoveObject(objectId);
		EntityIdToMemoryPosition.erase(objectId);

		if (!ids.empty() && ids[0] != UID::None())
			QueueRetransfers(ids);
		return ids;
	}

	LOG_CORE_WARN("Object with ID " + std::to_string(objectId) + " not found in any batch storage.");
	return std::vector<uid>();
}

void Cast::Memory::BatchManager::Clear()
{
	for (int i = 0; i < BatchStorages.size(); i++) {
		BatchStorages[i].Clear();
	}
}

void Cast::Memory::BatchManager::DeleteUnused()
{
	for (int i = 0; i < BatchStorages.size(); i++) {
		if (BatchStorages[i].GetObjectCount() == 0) {
			BatchStorages.erase(BatchStorages.begin() + i);
			i--;
		}
	}

	for (int i = 0; i < BatchStoragesIndexed.size(); i++) {
		if (BatchStoragesIndexed[i].GetObjectCount() == 0) {
			BatchStoragesIndexed.erase(BatchStoragesIndexed.begin() + i);
			i--;
		}
	}
}

void Cast::Memory::BatchManager::Render(Cast::Ref<API::Core::Shader> shader)
{
	for (int i = 0; i < BatchStorages.size(); i++) {
		BatchStorages[i].Render(shader);
	}
}

void Cast::Memory::BatchManager::RenderIndexed(Cast::Ref<API::Core::Shader> shader)
{
	for (int i = 0; i < BatchStoragesIndexed.size(); i++) {
		BatchStoragesIndexed[i].Render(shader);
	}
}

void Cast::Memory::BatchManager::QueueRetransfers(std::vector<uid>& ids)
{
	for (uid id : ids)
	{
		if (Shared.VertexEntities.find(id) != Shared.VertexEntities.end())
		{
			Shared.VertexEntities[id]->RetransferToBatchMemory();
		}
	}
}