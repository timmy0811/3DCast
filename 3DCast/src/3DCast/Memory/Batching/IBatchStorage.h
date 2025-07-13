#pragma once

#include "3DCast/Misc/UID.h"

#include <vector>
#include <unordered_set>

#include <API/core/Buffer.h>
#include <API/core/Shader.h>
#include <API/core/VertexBufferLayout.h>
#include <API/core/VertexArray.h>

#include "3DCast/Core.h"

namespace Cast::Memory
{
    class IBatchStorage
    {
    public:
        explicit IBatchStorage(size_t capacity);
        virtual ~IBatchStorage() = default;

        virtual std::vector<uid> RemoveObject(uid object, bool flushAll) = 0;

        virtual void InitBulk();
        virtual void AddToBulk(uid object);
        virtual std::vector<uid> RemoveBulk() = 0;

        virtual void Clear() = 0;
        virtual void Render(Ref<API::Core::Shader> shader) const = 0;

        void SetLayout(const Ref<API::Core::VertexBufferLayout>& layout);

        virtual inline bool IsObjectInStorage(uid object) const = 0;

        [[nodiscard]] virtual inline size_t GetObjectCount() const = 0;

        [[nodiscard]] virtual inline size_t GetCapacity() const { return Capacity; }
        [[nodiscard]] virtual inline size_t GetSize() const { return BatchMemory->GetSize(); }
        [[nodiscard]] virtual inline size_t GetAvailableMemory() const { return Capacity - BatchMemory->GetSize(); }

    protected:
        size_t Capacity;
        std::unordered_set<uid> Bulk;

        Ref<API::Core::Buffer> BatchMemory;
        Ref<API::Core::VertexBufferLayout> Layout;
        Ref<API::Core::VertexArray> VertexArray;
    };

    inline IBatchStorage::IBatchStorage(const size_t capacity)
    {
        VertexArray.reset(API::Core::VertexArray::Create());
        BatchMemory.reset(API::Core::Buffer::Create(API::Core::Buffer::BufferType::ARRAY_BUFFER,
                                                    API::Core::Buffer::MemoryLayout::DYNAMIC, capacity));

        Capacity = capacity;
    }

    inline void IBatchStorage::InitBulk()
    {
        Bulk.clear();
    }

    inline void IBatchStorage::AddToBulk(const uid object)
    {
        if (IsObjectInStorage(object)) {
            Bulk.insert(object);
        }
        else
        {
            LOG_CORE_WARN("Object must be in storage to be added to the bulk.");
        }
    }

    inline void IBatchStorage::SetLayout(const Cast::Ref<API::Core::VertexBufferLayout>& layout)
    {
        Layout = layout;
        VertexArray->AddBuffer(*BatchMemory, *Layout);
    }
}
