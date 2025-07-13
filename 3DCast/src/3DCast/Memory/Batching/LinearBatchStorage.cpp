#include "castpch.h"

#include <limits>

#include "LinearBatchStorage.h"
#include "3DCast/Renderer/Renderer.h"
#include "3DCast/Data/ShaderDataObjects/Vertex.h"

Cast::Memory::LinearBatchStorage::LinearBatchStorage(const size_t capacity)
	:IBatchStorage(capacity)
{

}

int Cast::Memory::LinearBatchStorage::CreateBatchObject(const uid object, const void* data, const size_t size)
{
	const int offset = BatchMemory->AddData(data, (int)size);
	if (offset == -1) {
		return -1;
	}

	Objects[object] = offset;

	return offset;
}

std::vector<Cast::uid> Cast::Memory::LinearBatchStorage::RemoveObject(const uid object, bool flushAll)
{
	if (Objects.find(object) == Objects.end()) {
		return {};
	}

	const int offset = (int)Objects[object];
	if (Objects.erase(object) == 0) {
		return {};
	}

	std::vector<uid> ids;
	ids.reserve(Objects.size());

	if (flushAll)
	{
		BatchMemory->Empty();

		std::transform(Objects.begin(), Objects.end(), std::back_inserter(ids),
		[](const auto& pair) { return pair.first; });

	}
	else
	{
		BatchMemory->EmptyPastOffset(offset);

		std::for_each(Objects.begin(), Objects.end(), [&](const auto& pair) {
			if (static_cast<int>(pair.second) > offset) ids.push_back(pair.first);
		});
	}

	return ids;
}

std::vector<Cast::uid> Cast::Memory::LinearBatchStorage::RemoveBulk()
{
	int minOffset = INT_MAX;

	for (uid id : Bulk)
	{
		if (const int offset = Objects[id]; offset < minOffset)
		{
			minOffset = offset;
		}

		Objects.erase(id);
	}

	BatchMemory->EmptyPastOffset(minOffset);

	std::vector<uid> ids;
	ids.reserve(Objects.size());
	std::for_each(Objects.begin(), Objects.end(), [&](const auto& pair) {
			if (static_cast<int>(pair.second) > minOffset) ids.push_back(pair.first);
		});

	return ids;
}

bool Cast::Memory::LinearBatchStorage::EditObject(const uid object, const void* data, const size_t size)
{
	if (Objects.find(object) == Objects.end()) {
		return false;
	}

	BatchMemory->AddData(data, (int)size, Objects[object]);
	return true;
}

void Cast::Memory::LinearBatchStorage::EditObject(const size_t offset, const void* data, const size_t size) const
{
	BatchMemory->AddData(data, (int)size, (int)offset);
}

int Cast::Memory::LinearBatchStorage::RetransferVertexEntity(const uid object, const void* data, const size_t size)
{
	if (Objects.find(object) == Objects.end()) {
		LOG_CORE_ERROR("Object queued for retransfer that does not exists in the current batch storage");
		return -1;
	}

	return CreateBatchObject(object, data, size);
}

void Cast::Memory::LinearBatchStorage::Render(Ref<API::Core::Shader> shader) const
{
	constexpr double stride_rez = 1.0 / sizeof(BatchVertex);

	BatchMemory->Bind();
	VertexArray->SetVBCount(std::ceil((double)BatchMemory->GetSize() * stride_rez));

	Renderer::RendererContext::Submit(VertexArray, shader);
}

void Cast::Memory::LinearBatchStorage::Clear()
{
	BatchMemory->Empty();
	Objects.clear();
}

