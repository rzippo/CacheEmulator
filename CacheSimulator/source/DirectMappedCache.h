#pragma once

#include <bitset>

#include "Cache.h"
#include "CacheEntry.h"
#include "BlockMapping.h"

using namespace std;

namespace CacheEmulator {
	template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize, unsigned cacheBlockIndexSize, unsigned tagSize>
	class DirectMappedCache : CacheEmulator::Cache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize>{

	private:
		CacheEmulator::BlockMapping<cacheBlockIndexSize, tagSize, memoryOffsetSize> mapping;

	public:
		DirectMappedCache(RAM<memoryBlockIndexSize, memoryOffsetSize>* memory, WriteStrategy writeStrategy) : Cache(memory, writeStrategy) {};

		char read(bitset<memoryBlockIndexSize + memoryOffsetSize> address) final;
		void write(bitset<memoryBlockIndexSize + memoryOffsetSize> address, char data) final;
		void flush() final;
	};
}

template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize, unsigned cacheBlockIndexSize, unsigned tagSize>
char CacheEmulator::DirectMappedCache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize>::read(bitset<memoryBlockIndexSize + memoryOffsetSize> address) {
	bitset<memoryBlockIndexSize> blockIndex = address.to_ullong() >> memoryOffsetSize;
	bitset<memoryOffsetSize> offset = address.to_ullong();
	bitset<tagSize> tag = blockIndex.to_ullong() >> cacheBlockIndexSize;
	bitset<cacheBlockIndexSize> cacheIndex = blockIndex.to_ullong();

	CacheEntry<tagSize, memoryOffsetSize>* entry = &(mapping.entries[cacheIndex.to_ullong()]);
	if (!entry->presenceBit.test(0) || entry->tag != tag)
	{
		//Miss
		bitset<memoryBlockIndexSize> victimBlockIndex = (entry->tag.to_ullong() << cacheBlockIndexSize) + cacheIndex.to_ullong();
		replaceAndFetch(entry, victimBlockIndex , blockIndex);
	}

	return entry->data[offset.to_ullong()];
}

template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize, unsigned cacheBlockIndexSize, unsigned tagSize>
void CacheEmulator::DirectMappedCache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize>::write(bitset<memoryBlockIndexSize + memoryOffsetSize> address, char data) {
	bitset<memoryBlockIndexSize> blockIndex = address.to_ullong() >> memoryOffsetSize;
	bitset<memoryOffsetSize> offset = address.to_ullong();
	bitset<tagSize> tag = blockIndex.to_ullong() >> cacheBlockIndexSize;
	bitset<cacheBlockIndexSize> cacheIndex = blockIndex.to_ullong();

	CacheEntry<tagSize, memoryOffsetSize>* entry = &(mapping.entries[cacheIndex.to_ullong()]);
	if (!entry->presenceBit.test(0) || entry->tag != tag)
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
void CacheEmulator::DirectMappedCache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize>::flush()
{
	for (int i = 0; i < (1 << cacheBlockIndexSize); i++)
	{
		mapping.entries[i].presenceBit.reset();
	}
}
