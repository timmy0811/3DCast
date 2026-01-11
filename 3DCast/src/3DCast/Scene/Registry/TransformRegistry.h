#pragma once

#include "3DCast/Core.h"

#include <API/core/Buffer.h>
#include <vendor/glm/glm.hpp>

#define MAX_TRANSFORMS 1024

namespace Cast
{
	class TransformRegistry
	{
	public:
		TransformRegistry()
		{
			SSBO.reset(API::Core::Buffer::Create(API::Core::Buffer::BufferType::SHADER_STORAGE_BUFFER,
			                                     API::Core::Buffer::MemoryLayout::DYNAMIC, MAX_TRANSFORMS,
			                                     sizeof(glm::mat4)));
		}

		~TransformRegistry() = default;

		void InvalidateEntry(const int index)
		{
			SetTransformUnused(index);
		}

		int Register(const glm::mat4* transform)
		{
			const int index = GetUnusedIndex();
			SSBO->AddData(transform, sizeof(glm::mat4), index * sizeof(glm::mat4));
			SetTransformUsed(index);
			return index;
		}

		void Edit(const int index, const glm::mat4* transform) const
		{
			if (!IsTransformUsed(index))
			{
				LOG_CORE_WARN("Editing a transform that is not assigned to an object.");
				return;
			}

			SSBO->AddData(transform, sizeof(glm::mat4), index * sizeof(glm::mat4));
		}

		[[nodiscard]] bool IsValid(const int index) const
		{
			return index >= 0 && index < MAX_TRANSFORMS && IsTransformUsed(index);
		}

		void Clear()
		{
			SSBO->Empty();
			memset(isUsed, 0, sizeof(isUsed));
		}

		void BindBase(const int slot) const
		{
			SSBO->BindBase(slot);
		}

		inline Ref<API::Core::Buffer> GetSSBO() { return SSBO; }

#pragma region ANALYTICS
		[[nodiscard]] inline size_t GetSSAOSize() const { return static_cast<unsigned int>(SSBO->GetSize()); }
		[[nodiscard]] inline unsigned int GetTransformsCount() const { static float fac = 1.0 / sizeof(glm::mat4); return static_cast<float>(SSBO->GetSize()) * fac; }
#pragma endregion

	private:
		[[nodiscard]] int GetUnusedIndex() const
		{
			for (int i = 0; i < MAX_TRANSFORMS / 64; i++)
			{
				if (isUsed[i] != 0xFFFFFFFFFFFFFFFF)
				{
					for (int j = 0; j < 64; j++)
					{
						if ((isUsed[i] & (1ULL << j)) == 0)
						{
							return i * 64 + j;
						}
					}
				}
			}

			LOG_CORE_WARN("TransformRegistry is full, cannot register more transforms.");
			return -1;
		}

		inline void SetTransformUsed(const int index)
		{
			isUsed[index / sizeof(uint64_t)] |= (1ULL << index % 64);
		}

		inline void SetTransformUnused(const int index)
		{
			isUsed[index / sizeof(uint64_t)] &= ~(1ULL << index % 64);
		}

		[[nodiscard]] inline bool IsTransformUsed(const int index) const
		{
			return (isUsed[index / sizeof(uint64_t)] & (1ULL << index % 64)) != 0;
		}

	private:
		Ref<API::Core::Buffer> SSBO;
		uint64_t isUsed[MAX_TRANSFORMS / 64] = {};
	};
}
