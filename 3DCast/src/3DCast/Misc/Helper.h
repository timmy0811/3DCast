#pragma once

#include <cstdint>

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
}