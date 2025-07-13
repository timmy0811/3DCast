#pragma once

#include "3DCast/Misc/UID.h"

#include <unordered_map>
#include <API/core/Shader.h>
#include <API/core/VertexBufferLayout.h>

#include "IBatchStorage.h"

namespace Cast::Memory
{
	class LinearBatchStorage final : public IBatchStorage
	{
	public:
		explicit LinearBatchStorage(size_t capacity);
		~LinearBatchStorage() override = default;

		int CreateBatchObject(uid object, const void* data, size_t size);
		std::vector<uid> RemoveObject(uid object, bool flushAll = false) override;

		std::vector<uid> RemoveBulk() override;

		bool EditObject(uid object, const void* data, size_t size);
		void EditObject(size_t offset, const void* data, size_t size) const;

		int RetransferVertexEntity(uid object, const void* data, size_t size);

		inline size_t GetObjectCount() const override{ return Objects.size(); }

		inline bool IsObjectInStorage(const uid object) const override
		{
			return Objects.find(object) != Objects.end();
		}

		void Clear() override;
		void Render(Ref<API::Core::Shader> shader) const override;

	private:
		std::unordered_map<uid, int> Objects;
	};
}
