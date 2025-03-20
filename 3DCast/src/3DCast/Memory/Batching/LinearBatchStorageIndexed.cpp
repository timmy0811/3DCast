#include "castpch.h"

#include "LinearBatchStorageIndexed.h"
#include "3DCast/Renderer/Renderer.h"

Cast::Memory::LinearBatchStorageIndexed::LinearBatchStorageIndexed(size_t capacity, size_t indexCapacity)
{
	VertexArray.reset(API::Core::VertexArray::Create());

	BatchMemory.reset(API::Core::Buffer::Create(API::Core::Buffer::BufferType::ARRAY_BUFFER, API::Core::Buffer::MemoryLayout::DYNAMIC, capacity));
	BatchIndices.reset(API::Core::Buffer::Create(API::Core::Buffer::BufferType::ELEMENT_ARRAY_BUFFER, API::Core::Buffer::MemoryLayout::DYNAMIC, indexCapacity));

	IndexCapacity = indexCapacity;
	Capacity = capacity;
}

int Cast::Memory::LinearBatchStorageIndexed::AddObject(uid object, void* data, size_t size, void* indices, int count)
{
	size_t availableMemory = Capacity - BatchMemory->GetSize();
	size_t availableIndexMemory = IndexCapacity - BatchIndices->GetSize();

	if (availableMemory < size)
		return -1;
	else if (availableIndexMemory < count * sizeof(unsigned int))
		return -2;

	int offset = BatchMemory->AddData(data, (int)size);

	unsigned int* shiftedIndex = new unsigned int[count];
	int indexOffset = offset / Layout->GetStride();
	memcpy(shiftedIndex, indices, count * sizeof(unsigned int));

	for (int i = 0; i < count; i++)
	{
		shiftedIndex[i] = shiftedIndex[i] + indexOffset;
	}

	indexOffset = BatchIndices->AddData(shiftedIndex, sizeof(unsigned int) * count);
	delete[] shiftedIndex;

	if (offset == -1 || indexOffset == -1)
	{
		LOG_CORE_ERROR("Internal Error: Batch memory or indices reporting overflow even though pre-check has been performed. Check implementation!");
		return -3;
	}

	Objects[object] = offset;

	return offset;
}

std::vector<Cast::uid> Cast::Memory::LinearBatchStorageIndexed::RemoveObject(uid object)
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

bool Cast::Memory::LinearBatchStorageIndexed::EditObject(uid object, void* data, size_t size)
{
	if (Objects.find(object) == Objects.end()) {
		return false;
	}

	BatchMemory->AddData(data, (int)size, Objects[object]);
	return true;
}

void Cast::Memory::LinearBatchStorageIndexed::EditObject(size_t offset, void* data, size_t size)
{
	BatchMemory->AddData(data, (int)size, (int)offset);
}

void Cast::Memory::LinearBatchStorageIndexed::Render(Cast::Ref<API::Core::Shader> shader)
{
	BatchMemory->Bind();
	BatchIndices->Bind();
	VertexArray->SetVBCount(BatchMemory->GetSize() / Layout->GetStride());

	Renderer::RendererContext::Submit(VertexArray, BatchIndices, shader);
}

void Cast::Memory::LinearBatchStorageIndexed::Clear()
{
	BatchMemory->Empty();
	BatchIndices->Empty();
	Objects.clear();
}

void Cast::Memory::LinearBatchStorageIndexed::SetLayout(Cast::Ref<API::Core::VertexBufferLayout> layout)
{
	Layout = layout;
	VertexArray->AddBuffer(*BatchMemory, *layout);
}