#include "castpch.h"

#include "LinearBatchStorage.h"
#include "3DCast/Renderer/Renderer.h"

Cast::Memory::LinearBatchStorage::LinearBatchStorage(size_t capacity)
{
	VertexArray.reset(API::Core::VertexArray::Create());

	BatchMemory.reset(API::Core::Buffer::Create(API::Core::Buffer::BufferType::ARRAY_BUFFER, API::Core::Buffer::MemoryLayout::DYNAMIC, capacity));
	Capacity = capacity;
}

int Cast::Memory::LinearBatchStorage::AddObject(uid object, void* data, size_t size)
{
	int offset = BatchMemory->AddData(data, size);
	if (offset == -1) {
		return -1;
	}

	Objects[object] = offset;

	return offset;
}

std::vector<Cast::uid> Cast::Memory::LinearBatchStorage::RemoveObject(uid object)
{
	if (Objects.erase(object) == 0) {
		return { Cast::UID::None() };
	}

	BatchMemory->Empty();

	std::vector<uid> ids;
	ids.reserve(Objects.size());

	std::transform(Objects.begin(), Objects.end(), std::back_inserter(ids),
		[](const auto& pair) { return pair.first; });

	return ids;
}

bool Cast::Memory::LinearBatchStorage::EditObject(uid object, void* data, size_t size)
{
	if (Objects.find(object) == Objects.end()) {
		return false;
	}

	BatchMemory->AddData(data, size, Objects[object]);
	return true;
}

void Cast::Memory::LinearBatchStorage::EditObject(size_t offset, void* data, size_t size)
{
	BatchMemory->AddData(data, size, offset);
}

void Cast::Memory::LinearBatchStorage::Render(Cast::Ref<API::Core::Shader> shader)
{
	BatchMemory->Bind();
	VertexArray->SetVBCount(BatchMemory->GetSize() / Layout->GetStride());

	Renderer::RendererContext::Submit(VertexArray, shader);
}

void Cast::Memory::LinearBatchStorage::Clear()
{
	BatchMemory->Empty();
	Objects.clear();
}

void Cast::Memory::LinearBatchStorage::SetLayout(Cast::Ref<API::Core::VertexBufferLayout> layout)
{
	Layout = layout;
	VertexArray->AddBuffer(*BatchMemory, *Layout);
}