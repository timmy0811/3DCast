#include "castpch.h"

#include "LinearBatchStorageInstanced.h"
#include "3DCast/Renderer/Renderer.h"

Cast::Memory::LinearBatchStorageInstanced::LinearBatchStorageInstanced(size_t capacity, size_t indexCapacity)
{
	VertexArray.reset(API::Core::VertexArray::Create());

	BatchMemory.reset(API::Core::Buffer::Create(API::Core::Buffer::BufferType::ARRAY_BUFFER, API::Core::Buffer::MemoryLayout::DYNAMIC, capacity));
	BatchIndices.reset(API::Core::Buffer::Create(API::Core::Buffer::BufferType::ELEMENT_ARRAY_BUFFER, API::Core::Buffer::MemoryLayout::DYNAMIC, capacity));

	indexCapacity = indexCapacity;
	Capacity = capacity;
}

int Cast::Memory::LinearBatchStorageInstanced::AddObject(uid object, void* data, size_t size, void* indices, int count)
{
	size_t availableMemory = Capacity - BatchMemory->GetSize();
	size_t availableIndexMemory = IndexCapacity - BatchIndices->GetSize();
	if (availableMemory < size)
	{
		LOG_CORE_INFO("Not enough memory to store vertex data. Vertex data gets put into new batch storage.");
		return -1;
	}
	else if (availableIndexMemory < count * sizeof(int))
	{
		LOG_CORE_INFO("Not enough memory to store index data. Index data gets put into new index batch storage.");
		return -1;
	}

	int offset = BatchMemory->AddData(data, (int)size);

	unsigned int* shiftedIndex = new unsigned int[count];
	_memccpy(shiftedIndex, indices, count, sizeof(unsigned int));
	for (int i = 0; i < count; i++)
	{
		shiftedIndex[i] = shiftedIndex[i] + offset;
	}

	int indexOffset = BatchIndices->AddData(shiftedIndex, count);
	delete[] shiftedIndex;

	if (offset == -1 || indexOffset == -1)
	{
		LOG_CORE_ERROR("Internal Error: Batch memory or indices reporting overflow even though pre-check has been performed. Check implementation!");
		return -1;
	}

	Objects[object] = offset;

	return offset;
}

std::vector<Cast::uid> Cast::Memory::LinearBatchStorageInstanced::RemoveObject(uid object)
{
	if (Objects.erase(object) == 0) {
		return { Cast::UID::None() };
	}

	BatchMemory->Empty();
	BatchIndices->Empty();

	std::vector<uid> ids;
	ids.reserve(Objects.size());

	std::transform(Objects.begin(), Objects.end(), std::back_inserter(ids),
		[](const auto& pair) { return pair.first; });

	return ids;
}

bool Cast::Memory::LinearBatchStorageInstanced::EditObject(uid object, void* data, size_t size)
{
	if (Objects.find(object) == Objects.end()) {
		return false;
	}

	BatchMemory->AddData(data, (int)size, Objects[object]);
	return true;
}

void Cast::Memory::LinearBatchStorageInstanced::EditObject(size_t offset, void* data, size_t size)
{
	BatchMemory->AddData(data, (int)size, (int)offset);
}

void Cast::Memory::LinearBatchStorageInstanced::Render(Cast::Ref<API::Core::Shader> shader)
{
	BatchMemory->Bind();
	BatchIndices->Bind();
	// VertexArray->SetVBCount(BatchMemory->GetSize() / Layout->GetStride());

	Renderer::RendererContext::Submit(VertexArray, BatchIndices, shader);
}

void Cast::Memory::LinearBatchStorageInstanced::Clear()
{
	BatchMemory->Empty();
	BatchIndices->Empty();
	Objects.clear();
}

void Cast::Memory::LinearBatchStorageInstanced::SetLayout(Cast::Ref<API::Core::VertexBufferLayout> layout)
{
	Layout = layout;
	VertexArray->AddBuffer(*BatchMemory, *layout);
}