#pragma once

#include <bitset>

#include "Cache.h"
#include "CacheEntry.h"
#include "BlockMapping.h"

using namespace std;

namespace CacheSimulator {
	template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize, unsigned cacheBlockIndexSize, unsigned tagSize>
	class DirectMappedCache : CacheSimulator::Cache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize>{

	private:
		CacheSimulator::BlockMapping<cacheBlockIndexSize, tagSize, memoryOffsetSize> mapping;

	public:
		DirectMappedCache(RAM<memoryBlockIndexSize, memoryOffsetSize>* memory, WriteStrategy writeStrategy) : Cache(memory, writeStrategy) {};

		char read(bitset<memoryBlockIndexSize + memoryOffsetSize> address) final;
		void write(bitset<memoryBlockIndexSize + memoryOffsetSize> address, char data) final;
		void flush() final;
	};
}

template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize, unsigned cacheBlockIndexSize, unsigned tagSize>
char CacheSimulator::DirectMappedCache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize>::read(bitset<memoryBlockIndexSize + memoryOffsetSize> address) {
	bitset<memoryBlockIndexSize> blockIndex = address.to_ullong() >> memoryOffsetSize;
	bitset<memoryOffsetSize> offset = (address.to_ullong() << memoryBlockIndexSize) >> memoryBlockIndexSize;
	bitset<tagSize> tag = blockIndex.to_ullong() >> cacheBlockIndexSize;
	bitset<cacheBlockIndexSize> cacheIndex = (blockIndex.to_ullong() << tagSize) >> tagSize;

	CacheEntry<tagSize, memoryOffsetSize>* entry = &(mapping.entries[cacheIndex.to_ullong()]);
	if (entry->presenceBit != 1 || entry->tag != tag)
	{
		//Miss
		bitset<memoryBlockIndexSize> victimBlockIndex = (entry->tag.to_ullong() << cacheBlockIndexSize) + cacheIndex.to_ullong();
		replaceAndFetch(entry, victimBlockIndex , blockIndex);
	}

	return entry->data[offset.to_ullong()];
}

template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize, unsigned cacheBlockIndexSize, unsigned tagSize>
void CacheSimulator::DirectMappedCache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize>::write(bitset<memoryBlockIndexSize + memoryOffsetSize> address, char data) {
	bitset<memoryBlockIndexSize> blockIndex = address.to_ullong() >> memoryOffsetSize;
	bitset<memoryOffsetSize> offset = (address.to_ullong() << memoryBlockIndexSize) >> memoryBlockIndexSize;
	bitset<tagSize> tag = blockIndex.to_ullong() >> cacheBlockIndexSize;
	bitset<cacheBlockIndexSize> cacheIndex = (blockIndex.to_ullong() << tagSize) >> tagSize;

	CacheEntry<tagSize, memoryOffsetSize>* entry = &(mapping.entries[cacheIndex.to_ullong()]);
	if (entry->presenceBit != 1 || entry->tag != tag)
	{
		//Miss
		bitset<memoryBlockIndexSize> victimBlockIndex = (entry->tag.to_ullong() << cacheBlockIndexSize) + cacheIndex.to_ullong();
		replaceAndFetch(entry, victimBlockIndex, blockIndex);
	}

	entry->data[offset.to_ullong()] = data;
	if (writeStrategy == WriteStrategy::WriteThrough)
		memory->writeBlock(blockIndex, entry->data);
}

template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize, unsigned cacheBlockIndexSize, unsigned tagSize>
void CacheSimulator::DirectMappedCache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize>::flush()
{
	for (int i = 0; i < (1 << cacheBlockIndexSize); i++)
	{
		mapping.entries[i].presenceBit = 0;
	}
}
