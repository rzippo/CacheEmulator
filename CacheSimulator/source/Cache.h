#pragma once

#include <bitset>

#include "CacheEntry.h"
#include "RAM.h"
#include "WriteStrategy.h"

using namespace std;

namespace CacheSimulator {
	template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize, unsigned cacheBlockIndexSize, unsigned tagSize>
	class Cache {
		static_assert((cacheBlockIndexSize + tagSize) == memoryBlockIndexSize, "The cache index and tag must be parts of the RAM block index");

	protected:
		CacheSimulator::RAM<memoryBlockIndexSize, memoryOffsetSize>* memory;
		CacheSimulator::WriteStrategy writeStrategy;

		Cache(RAM<memoryBlockIndexSize, memoryOffsetSize>* memory, WriteStrategy writeStrategy);
		void replaceAndFetch(CacheSimulator::CacheEntry<tagSize, memoryOffsetSize>* entry, bitset<memoryBlockIndexSize> entryBlockIndex, bitset<memoryBlockIndexSize> newBlockIndex);

	public:
		virtual char read(bitset<memoryBlockIndexSize + memoryOffsetSize> address) = 0;
		virtual void write(bitset<memoryBlockIndexSize + memoryOffsetSize> address, char data) = 0;
		virtual void flush() = 0;
	};
}

template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize, unsigned cacheBlockIndexSize, unsigned tagSize>
inline CacheSimulator::Cache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize>::Cache(RAM<memoryBlockIndexSize, memoryOffsetSize>* memory, WriteStrategy writeStrategy)
{
	this->memory = memory;
	this->writeStrategy = writeStrategy;
}

template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize, unsigned cacheBlockIndexSize, unsigned tagSize>
void CacheSimulator::Cache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize>::replaceAndFetch(CacheSimulator::CacheEntry<tagSize, memoryOffsetSize>* victimEntry, bitset<memoryBlockIndexSize> victimMemoryBlockIndex, bitset<memoryBlockIndexSize> replacingMemoryBlockIndex)
{
	//Write back the old block's data, if needed
	if (victimEntry->presenceBit == 1 && writeStrategy == WriteStrategy::WriteBack)
	{
		memory->writeBlock(victimMemoryBlockIndex, victimEntry->data);
	}

	//Copy the new data and set tag and presenceBit
	char* newData = memory->readBlock(replacingMemoryBlockIndex);
	for (int i = 0; i < (1 << memoryOffsetSize); i++)
	{
		victimEntry->data[i] = newData[i];
	}
	victimEntry->presenceBit = 1;
	victimEntry->tag = replacingMemoryBlockIndex.to_ullong() >> cacheBlockIndexSize;
}