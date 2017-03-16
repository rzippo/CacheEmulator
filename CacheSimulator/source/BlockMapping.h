#pragma once

#include "CacheEntry.h"

namespace CacheSimulator {
	template<unsigned cacheBlockIndexSize, unsigned tagSize, unsigned memoryOffsetSize>
	struct BlockMapping {
		CacheSimulator::CacheEntry<tagSize, memoryOffsetSize> entries[1 << cacheBlockIndexSize];
	};
}
