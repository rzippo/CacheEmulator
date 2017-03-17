#pragma once

#include <bitset>
#include <vector>

#include "AssociativeCache.h"
#include "BlockMapping.h"
#include "CacheEntry.h"

using namespace std;

namespace CacheEmulator {
	template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize, unsigned cacheBlockIndexSize, unsigned tagSize, unsigned associativity>
	class FullAssociativeCache : public CacheEmulator::AssociativeCache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize, associativity> {
		static_assert(cacheBlockIndexSize == 0, "CacheBlockIndexSize must be == 0");

	protected:
		CacheEntry<tagSize, memoryOffsetSize> set[associativity];

		vector<CacheEmulator::CacheEntry<tagSize, memoryOffsetSize>*> retrieveSet(bitset<memoryBlockIndexSize> blockIndex) final;

	public:
		FullAssociativeCache(MemoryComponent<memoryBlockIndexSize, memoryOffsetSize>* lowerMemory, WriteStrategy writeStrategy)
			:AssociativeCache(lowerMemory, writeStrategy) {};
		
		void flush() final;
	};
}

template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize, unsigned cacheBlockIndexSize, unsigned tagSize, unsigned associativity>
vector<CacheEmulator::CacheEntry<tagSize, memoryOffsetSize>*> CacheEmulator::FullAssociativeCache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize, associativity>::retrieveSet(bitset<memoryBlockIndexSize> blockIndex)
{
	vector<CacheEntry<tagSize, memoryOffsetSize>*> vecSet;
	for (int i = 0; i < associativity; i++)
	{
		vecSet.push_back(&set[i]);
	}
	return vecSet;
}

template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize, unsigned cacheBlockIndexSize, unsigned tagSize, unsigned associativity>
inline void CacheEmulator::FullAssociativeCache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize, associativity>::flush()
{
	for (int i = 0; i < associativity; i++)
	{
		set[i].presenceBit.reset();
	}
}
