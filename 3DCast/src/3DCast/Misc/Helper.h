#pragma once

#include <cstdint>
#include <thread>
#include <algorithm>
#include <vector>

#ifdef TBB
#include <execution>
#endif

#include "3DCast/Data/ShaderDataObjects/Vertex.h"

namespace Helper
{
#define NAN_UNSIGNED_SHORT = 0xFFFF;

	inline bool isBitSet(const uint16_t value, const int bit) {
		return (value & (1 << bit)) != 0;
	}

	inline void setBit(uint16_t& value, const int bit, const bool state) {
		if (state) {
			value |= (1 << bit);
		}
		else {
			value &= ~(1 << bit);
		}
	}

	inline void patchRegistryDataOnVertexBlob(Cast::Memory::BatchVertexShaderObject* vertexData, const size_t vertexCount, const int samplerIndex, const int transformIndex)
	{
		if (vertexCount > 10000) {
			const int numThreads = (int)std::thread::hardware_concurrency();
			std::vector<std::thread> threads(numThreads);

			const size_t chunkSize = vertexCount / numThreads;
			for (int t = 0; t < numThreads; t++) {
				const size_t start = t * chunkSize;
				const size_t end = (t == numThreads - 1) ? vertexCount : start + chunkSize;

				threads[t] = std::thread([=]() {
					for (size_t i = start; i < end; i++) {
						vertexData[i].SamplerIndex = static_cast<float>(samplerIndex);
						vertexData[i].TransformIndex = static_cast<float>(transformIndex);
					}
				});
			}

			for (auto& t : threads) {
				t.join();
			}
		} else {
#ifdef TBB
			std::for_each(std::execution::par_unseq, vertexData, vertexData + vertexCount,
				[samplerIndex, transformIndex](Cast::Memory::BatchVertexShaderObject& vertex) {
					vertex.SamplerIndex = static_cast<float>(samplerIndex);
					vertex.TransformIndex = static_cast<float>(transformIndex);
				});
#else
			for (size_t i = 0; i < vertexCount; i++) {
				vertexData[i].SamplerIndex = static_cast<float>(samplerIndex);
				vertexData[i].TransformIndex = static_cast<float>(transformIndex);
			}
#endif
		}
	}
}
