#include "castpch.h"
#include "BatchManager.h"

#include "Vertex.h"

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

Cast::Memory::MemoryPosition Cast::Memory::BatchManager::AddObject(uid objectId, void* data, size_t size)
{
	if (size > BatchStorageSize)
	{
		LOG_CORE_ERROR("Object size is larger than the batch storage size. Object will not be added to the batch storage.");
		return MemoryPosition{ -1, -1, -1 };
	}

	// Add object to the first batch storage that has enough space
	for (int i = 0; i < BatchStorages.size(); i++) {
		int offset = BatchStorages[i].AddObject(objectId, data, size);
		if (offset != -1) {
			return MemoryPosition{ i, offset };
		}
	}

	// If no batch storage has enough space, create a new one
	BatchStorages.push_back(LinearBatchStorage(BatchStorageSize));
	BatchStorages[BatchStorages.size() - 1].SetLayout(Layout);
	int offset = BatchStorages[BatchStorages.size() - 1].AddObject(objectId, data, size);
	return MemoryPosition{ (int)BatchStorages.size() - 1, -1, offset };
}

Cast::Memory::MemoryPosition Cast::Memory::BatchManager::AddIndexedObject(uid objectId, void* data, size_t size, void* indices, int count)
{
	LOG_CORE_INFO("Adding indexed object with ID " + std::to_string(objectId) + " to the batch storage.");
	if (size > BatchStorageSize)
	{
		LOG_CORE_ERROR("Object size is larger than the batch storage size. Object will not be added to the batch storage.");
		return MemoryPosition{ -1, -1, -1 };
	}
	else if (count > MaxIndices)
	{
		LOG_CORE_ERROR("Object indices size is larger than the batch storage index size. Object will not be added to the batch storage.");
		return MemoryPosition{ -1, -1, -1 };
	}

	// Add object to the first batch storage that has enough space
	int offset = -3;
	for (int i = 0; i < BatchStoragesIndexed.size(); i++) {
		offset = BatchStoragesIndexed[i].AddObject(objectId, data, size, indices, count);
		if (offset >= 0) {
			return MemoryPosition{ i, offset };
		}
	}

	// If no batch storage has enough space, create a new one
	BatchStoragesIndexed.push_back(LinearBatchStorageIndexed(BatchStorageSize, MaxIndices));
	std::string cause;
	if (offset == -1)
		cause = "Exceeding vertex buffer capacity.";
	else if (offset == -2)
		cause = "Exceeding index buffer capacity.";
	else
		cause = "Internal error.";

	LOG_CORE_INFO("Creating new batch storage for object with ID " + std::to_string(objectId) + " | Cause: " + cause);
	BatchStoragesIndexed[BatchStoragesIndexed.size() - 1].SetLayout(Layout);
	offset = BatchStoragesIndexed[BatchStoragesIndexed.size() - 1].AddObject(objectId, data, size, indices, count);
	return MemoryPosition{ -1, (int)BatchStoragesIndexed.size() - 1, offset };
}

void Cast::Memory::BatchManager::EditObject(uid objectId, void* data, size_t size)
{
	for (int i = 0; i < BatchStorages.size(); i++) {
		if (BatchStorages[i].EditObject(objectId, data, size))
			return;
	}

	for (int i = 0; i < BatchStoragesIndexed.size(); i++) {
		if (BatchStoragesIndexed[i].EditObject(objectId, data, size))
			return;
	}

	LOG_CORE_WARN("Object with ID " + std::to_string(objectId) + " not found in any batch storage. Could not update the object.");
}

void Cast::Memory::BatchManager::EditObject(MemoryPosition pos, void* data, size_t size)
{
	if (pos.batchStorageId != -1) {
		BatchStorages[pos.batchStorageId].EditObject((size_t)pos.offset, data, size);
	}
	else if (pos.batchStorageInstancedId != -1) {
		BatchStoragesIndexed[pos.batchStorageInstancedId].EditObject((size_t)pos.offset, data, size);
	}
	else {
		LOG_CORE_WARN("Object with ID " + std::to_string(pos.offset) + " not found in any batch storage.");
	}
}

std::vector<Cast::uid> Cast::Memory::BatchManager::RemoveIndexedObject(uid objectId)
{
	for (int i = 0; i < BatchStoragesIndexed.size(); i++) {
		std::vector<uid> ids = BatchStoragesIndexed[i].RemoveObject(objectId);
		if (ids.size() > 0 && ids[0] == UID::None())
			continue;
		return ids;
	}

	LOG_CORE_WARN("Object with ID " + std::to_string(objectId) + " not found in any batch storage.");
	return std::vector<uid>();
}

std::vector<Cast::uid> Cast::Memory::BatchManager::RemoveObject(uid objectId)
{
	for (int i = 0; i < BatchStorages.size(); i++) {
		std::vector<uid> ids = BatchStorages[i].RemoveObject(objectId);
		if (ids.size() > 0 && ids[0] == UID::None())
			continue;
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