#include "castpch.h"

#include "LinearBatchStorageIndexed.h"
#include "3DCast/Renderer/Renderer.h"
#include "3DCast/Data/ShaderDataObjects/Vertex.h"

Cast::Memory::LinearBatchStorageIndexed::LinearBatchStorageIndexed(const size_t capacity, const size_t indexCapacity)
	:IBatchStorage(capacity)
{

	BatchIndices.reset(API::Core::Buffer::Create(API::Core::Buffer::BufferType::ELEMENT_ARRAY_BUFFER,
	                                             API::Core::Buffer::MemoryLayout::DYNAMIC, indexCapacity));

	IndexCapacity = indexCapacity;
}

int Cast::Memory::LinearBatchStorageIndexed::CreateBatchObject(const uid object, const void* data, const size_t size,
                                                               const void* indices, const int count)
{
	const size_t availableMemory = Capacity - BatchMemory->GetSize();
	const size_t availableIndexMemory = IndexCapacity - BatchIndices->GetSize();

	if (availableMemory < size)
		return -1;
	else if (availableIndexMemory < count * sizeof(unsigned int))
		return -2;

	const int offset = BatchMemory->AddData(data, (int)size);

	const auto shiftedIndex = new unsigned int[count];
	int indexOffset = (int)offset / Layout->GetStride();
	memcpy(shiftedIndex, indices, count * sizeof(unsigned int));

	for (int i = 0; i < count; i++)
	{
		shiftedIndex[i] = shiftedIndex[i] + indexOffset;
	}

	indexOffset = BatchIndices->AddData(shiftedIndex, sizeof(unsigned int) * count);
	delete[] shiftedIndex;

	if (offset == -1 || indexOffset == -1)
	{
		LOG_CORE_ERROR(
			"Internal Error: Batch memory or indices reporting overflow even though pre-check has been performed. Check implementation!");
		return -3;
	}

	Objects[object] = {offset, indexOffset};

	return offset;
}

std::vector<Cast::uid> Cast::Memory::LinearBatchStorageIndexed::RemoveObject(const uid object, const bool flushAll)
{
	if (Objects.find(object) == Objects.end()) {
		return {};
	}

	const int vertOffset = (int)Objects[object].vertexOffset;
	const int indOffset = (int)Objects[object].indexOffset;

	if (Objects.erase(object) == 0) {
		return {};
	}

	std::vector<uid> ids;
	ids.reserve(Objects.size());

	if (flushAll)
	{
		BatchMemory->Empty();
		BatchIndices->Empty();

		std::transform(Objects.begin(), Objects.end(), std::back_inserter(ids),
		[](const auto& pair) { return pair.first; });
	}
	else
	{
		BatchMemory->EmptyPastOffset(vertOffset);
		BatchIndices->EmptyPastOffset(indOffset);

		std::for_each(Objects.begin(), Objects.end(), [&](const auto& pair) {
			if (static_cast<int>(pair.second.vertexOffset) > vertOffset) ids.push_back(pair.first);
		});

	}

	return ids;
}

std::vector<Cast::uid> Cast::Memory::LinearBatchStorageIndexed::RemoveBulk()
{
	int minVertOffset = INT_MAX;
	int minIndOffset = INT_MAX;

	for (uid id : Bulk)
	{
		if (const int offset = Objects[id].vertexOffset; offset < minVertOffset)
		{
			minVertOffset = offset;
			minIndOffset = Objects[id].indexOffset;
		}

		Objects.erase(id);
	}

	BatchMemory->EmptyPastOffset(minVertOffset);
	BatchIndices->EmptyPastOffset(minIndOffset);

	std::vector<uid> ids;
	ids.reserve(Objects.size());
	std::for_each(Objects.begin(), Objects.end(), [&](const auto& pair) {
			if (static_cast<int>(pair.second.vertexOffset) > minVertOffset) ids.push_back(pair.first);
		});

	return ids;
}

bool Cast::Memory::LinearBatchStorageIndexed::EditObject(const uid object, const void* data, const size_t size,
                                                         const void* indices,
                                                         int count)
{
	if (Objects.find(object) == Objects.end())
	{
		return false;
	}

	BatchMemory->AddData(data, (int)size, Objects[object].vertexOffset);
	BatchIndices->AddData(indices, (int)count, Objects[object].indexOffset);
	return true;
}

bool Cast::Memory::LinearBatchStorageIndexed::EditObject(const uid object, const void* data, const size_t size)
{
	if (Objects.find(object) == Objects.end())
	{
		return false;
	}

	BatchMemory->AddData(data, (int)size, Objects[object].vertexOffset);
	return true;
}

bool Cast::Memory::LinearBatchStorageIndexed::EditObject(const size_t offset_vert, const void* data, const size_t size,
                                                         const size_t offset_ind, const void* indices,
                                                         const int count) const
{
	BatchMemory->AddData(data, size, (int)offset_vert);
	BatchMemory->AddData(indices, count * sizeof(unsigned int), (int)offset_ind);
	return true;
}

int Cast::Memory::LinearBatchStorageIndexed::RetransferVertexEntity(const uid object, const void* data,
                                                                    const size_t size, const void* indices,
                                                                    const int count)
{
	if (Objects.find(object) == Objects.end())
	{
		LOG_CORE_ERROR("Object queued for retransfer that does not exists in the current batch storage");
		return -1;
	}

	return CreateBatchObject(object, data, size, indices, count);
}

void Cast::Memory::LinearBatchStorageIndexed::Render(Ref<API::Core::Shader> shader) const
{
	constexpr double stride_rez = 1.0 / sizeof(BatchVertex);

	const auto count = std::ceil((double)BatchMemory->GetSize() * stride_rez);

	BatchMemory->Bind();
	BatchIndices->Bind();
	VertexArray->SetVBCount((size_t)count);

	Renderer::RendererContext::Submit(VertexArray, BatchIndices, shader);
}

void Cast::Memory::LinearBatchStorageIndexed::Clear()
{
	BatchMemory->Empty();
	BatchIndices->Empty();
	Objects.clear();
}
