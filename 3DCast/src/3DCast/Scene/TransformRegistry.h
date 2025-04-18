#pragma once

#include "3DCast/Core.h"
#include "3DCast/Misc/UID.h"

#include <API/core/Buffer.h>
#include <Vendor/glm/glm.hpp>

#define MAX_TRANSFORMS 1024

namespace Cast {
	class TransformRegistry {
	public:
		TransformRegistry() {
			SSBO.reset(API::Core::Buffer::Create(API::Core::Buffer::BufferType::SHADER_STORAGE_BUFFER, API::Core::Buffer::MemoryLayout::DYNAMIC, MAX_TRANSFORMS, sizeof(glm::mat4)));
		}
		~TransformRegistry() = default;

		void InvalidateEntry(int index) {
			SetTransformUnused(index);
		}

		int RegisterTransform(glm::mat4* transform) {
			int index = GetUnusedIndex();
			SSBO->AddData(transform, sizeof(glm::mat4), index * sizeof(glm::mat4));
			SetTransformUsed(index);
			return index;
		}

		void EditTransform(int index, glm::mat4* transform) {
			if (!IsTransformUsed(index)) {
				LOG_CORE_WARN("Editing a transform that is not assigned to an object.");
			}

			SSBO->AddData(transform, sizeof(glm::mat4), index * sizeof(glm::mat4));
		}

		void Clear() {
			SSBO->Empty();
			memset(isUsed, 0, sizeof(isUsed));
		}

		void BindBase(int slot) const {
			SSBO->BindBase(slot);
		}

		inline Cast::Ref<API::Core::Buffer> GetSSBO() { return SSBO; }

	private:
		int GetUnusedIndex() {
			for (int i = 0; i < MAX_TRANSFORMS / 64; i++) {
				if (isUsed[i] != 0xFFFFFFFFFFFFFFFF) {
					for (int j = 0; j < 64; j++) {
						if ((isUsed[i] & (1ULL << j)) == 0) {
							return i * 64 + j;
						}
					}
				}
			}

			LOG_CORE_WARN("TransformRegistry is full, cannot register more transforms.");
			return -1;
		}

		inline void SetTransformUsed(int index) {
			isUsed[index / sizeof(uint64_t)] |= (1ULL << index % 64);
		}

		inline void SetTransformUnused(int index) {
			isUsed[index / sizeof(uint64_t)] &= ~(1ULL << index % 64);
		}

		inline bool IsTransformUsed(int index) {
			return (isUsed[index / sizeof(uint64_t)] & (1ULL << index % 64)) != 0;
		}

	private:
		Cast::Ref<API::Core::Buffer> SSBO;
		uint64_t isUsed[MAX_TRANSFORMS / 64] = {};
	};
}