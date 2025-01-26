#pragma once

#include <cstdint>

namespace Helper
{
#define NAN_UNSIGNED_SHORT = 0xFFFF;

	inline bool isBitSet(uint16_t value, int bit) {
		return (value & (1 << bit)) != 0;
	}

	inline void setBit(uint16_t& value, int bit, bool state) {
		if (state) {
			value |= (1 << bit);
		}
		else {
			value &= ~(1 << bit);
		}
	}
}