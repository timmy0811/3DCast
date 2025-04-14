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

		void InvalidateEntry(uid key) {
			auto it = RegisteredNodeMapping.find(key);
			if (it == RegisteredNodeMapping.end())
				return;

			isValid[it->second / sizeof(uint64_t)] &= ~(1ULL << it->second % 64);
			RegisteredNodeMapping.erase(it);
		}

		void ShrinkToFit() {
			// Implement algorithm to repack buffer content
		}

		uid RegisterTransform(glm::mat4* transform) {
			int bufferPosition = SSBO->GetSize();
			SSBO->AddData(transform, sizeof(glm::mat4));
			int transformRegistryIndex = (unsigned short)(bufferPosition / sizeof(glm::mat4));
			isValid[transformRegistryIndex / sizeof(uint64_t)] |= (1ULL << transformRegistryIndex % 64);
			uid key = UID::Create();
			RegisteredNodeMapping[key] = transformRegistryIndex;
			return key;
		}

		void EditTransform(uid key, glm::mat4* transform) {
			auto it = RegisteredNodeMapping.find(key);

			if (it == RegisteredNodeMapping.end()) {
				LOG_CORE_ERROR("Transform key not found in registry.");
				return;
			}

			int index = it->second;
			SSBO->AddData(transform, sizeof(glm::mat4), index * sizeof(glm::mat4));
			isValid[index / sizeof(uint64_t)] |= (1ULL << index % 64);
		}

		void Clear() {
			SSBO->Empty();
		}

		void BindBase(int slot) const {
			SSBO->BindBase(slot);
		}

		inline Cast::Ref<API::Core::Buffer> GetSSBO() { return SSBO; }
		inline int GetPosition(uid key) {
			auto it = RegisteredNodeMapping.find(key);
			if (it == RegisteredNodeMapping.end())
				return -1;

			return it->second;
		}

	private:
		Cast::Ref<API::Core::Buffer> SSBO;
		uint64_t isValid[MAX_TRANSFORMS / 64] = {};
		std::unordered_map<uid, int> RegisteredNodeMapping;
	};
}