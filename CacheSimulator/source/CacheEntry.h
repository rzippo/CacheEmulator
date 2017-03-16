#pragma once

#include <bitset>

using namespace std;

namespace CacheSimulator {
	template <unsigned tagSize, unsigned memoryOffsetSize>
	struct CacheEntry {
		bitset<tagSize> tag;
		bitset<1> presenceBit = 0;
		char data[1 << memoryOffsetSize];
	};
}
