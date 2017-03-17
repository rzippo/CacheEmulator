#pragma once

#include <bitset>

#include "Cache.h"
#include "CacheEntry.h"
#include "BlockMapping.h"

using namespace std;

namespace CacheEmulator {
	template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize, unsigned cacheBlockIndexSize, unsigned tagSize>
	class DirectMappedCache : public CacheEmulator::Cache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize>{

	protected:
		CacheEmulator::BlockMapping<cacheBlockIndexSize, tagSize, memoryOffsetSize> mapping;

		CacheEntry<tagSize, memoryOffsetSize>* retrieveEntry(bitset<memoryBlockIndexSize> blockIndex) final;

	public:
		DirectMappedCache(MemoryComponent<memoryBlockIndexSize, memoryOffsetSize>* lowerMemory, WriteStrategy writeStrategy) : Cache(lowerMemory, writeStrategy) {};

		void flush() final;
	};
}

template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize, unsigned cacheBlockIndexSize, unsigned tagSize>
CacheEmulator::CacheEntry<tagSize, memoryOffsetSize>* CacheEmulator::DirectMappedCache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize>::retrieveEntry(bitset<memoryBlockIndexSize> blockIndex)
{
	bitset<tagSize> tag = blockIndex.to_ullong() >> cacheBlockIndexSize;
	bitset<cacheBlockIndexSize> cacheIndex = blockIndex.to_ullong();

	CacheEntry<tagSize, memoryOffsetSize>* entry = &(mapping.entries[cacheIndex.to_ullong()]);
	if (!entry->presenceBit.test(0) || entry->tag != tag)
	{
		//Miss
		bitset<memoryBlockIndexSize> victimBlockIndex = (entry->tag.to_ullong() << cacheBlockIndexSize) + cacheIndex.to_ullong();
		replaceAndFetch(entry, victimBlockIndex, blockIndex);
	}

	return entry;
}

template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize, unsigned cacheBlockIndexSize, unsigned tagSize>
void CacheEmulator::DirectMappedCache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize>::flush()
{
	for (int i = 0; i < (1 << cacheBlockIndexSize); i++)
	{
		mapping.entries[i].presenceBit.reset();
	}
}
