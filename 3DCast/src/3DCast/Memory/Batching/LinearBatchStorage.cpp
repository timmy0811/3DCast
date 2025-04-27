#include "castpch.h"

#include "LinearBatchStorage.h"
#include "3DCast/Renderer/Renderer.h"
#include "3DCast/Data/ShaderDataObjects/Vertex.h"

Cast::Memory::LinearBatchStorage::LinearBatchStorage(size_t capacity)
{
	VertexArray.reset(API::Core::VertexArray::Create());

	BatchMemory.reset(API::Core::Buffer::Create(API::Core::Buffer::BufferType::ARRAY_BUFFER, API::Core::Buffer::MemoryLayout::DYNAMIC, capacity));
	Capacity = capacity;
}

int Cast::Memory::LinearBatchStorage::CreateBatchObject(uid object, void* data, size_t size)
{
	int offset = BatchMemory->AddData(data, (int)size);
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

	BatchMemory->AddData(data, (int)size, Objects[object]);
	return true;
}

void Cast::Memory::LinearBatchStorage::EditObject(size_t offset, void* data, size_t size)
{
	BatchMemory->AddData(data, (int)size, (int)offset);
}

int Cast::Memory::LinearBatchStorage::RetransferVertexEntity(uid object, void* data, size_t size)
{
	if (Objects.find(object) == Objects.end()) {
		LOG_CORE_ERROR("Object queued for retransfer that does not exists in the current batch storage");
		return -1;
	}

	return CreateBatchObject(object, data, size);
}

void Cast::Memory::LinearBatchStorage::Render(Cast::Ref<API::Core::Shader> shader)
{
	constexpr double stride_rez = 1.0 / sizeof(Memory::BatchVertex);

	BatchMemory->Bind();
	VertexArray->SetVBCount(std::ceil(BatchMemory->GetSize() * stride_rez));

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