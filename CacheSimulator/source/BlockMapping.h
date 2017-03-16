#pragma once

#include "CacheEntry.h"

namespace CacheEmulator {
	template<unsigned cacheBlockIndexSize, unsigned tagSize, unsigned memoryOffsetSize>
	struct BlockMapping {
		CacheEmulator::CacheEntry<tagSize, memoryOffsetSize> entries[1 << cacheBlockIndexSize];
	};
}
