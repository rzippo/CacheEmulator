#pragma once

#include <bitset>

#include "MemoryComponent.h"
#include "CacheEntry.h"
#include "RAM.h"
#include "WriteStrategy.h"

using namespace std;

namespace CacheEmulator {
	template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize, unsigned cacheBlockIndexSize, unsigned tagSize>
	class Cache : public MemoryComponent<memoryBlockIndexSize, memoryOffsetSize> {
		static_assert((cacheBlockIndexSize + tagSize) == memoryBlockIndexSize, "The cache index and tag must be parts of the RAM block index");

	protected:
		CacheEmulator::MemoryComponent<memoryBlockIndexSize, memoryOffsetSize>* lowerMemory;
		CacheEmulator::WriteStrategy writeStrategy;

		Cache(MemoryComponent<memoryBlockIndexSize, memoryOffsetSize>* lowerMemory, WriteStrategy writeStrategy);
		void replaceAndFetch(CacheEmulator::CacheEntry<tagSize, memoryOffsetSize>* entry, bitset<memoryBlockIndexSize> entryBlockIndex, bitset<memoryBlockIndexSize> newBlockIndex);

		virtual CacheEntry<tagSize, memoryOffsetSize>* retrieveEntry(bitset<memoryBlockIndexSize> blockIndex) = 0;
		
	public:
		virtual void flush() = 0;

		char read(bitset<memoryBlockIndexSize + memoryOffsetSize> address) final;
		void write(bitset<memoryBlockIndexSize + memoryOffsetSize> address, char data) final;

		char* readBlock(bitset<memoryBlockIndexSize> blockIndex) final;
		void writeBlock(bitset<memoryBlockIndexSize> blockIndex, char data[]) final;
	};
}

template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize, unsigned cacheBlockIndexSize, unsigned tagSize>
inline CacheEmulator::Cache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize>::Cache(MemoryComponent<memoryBlockIndexSize, memoryOffsetSize>* lowerMemory, WriteStrategy writeStrategy)
{
	this->lowerMemory = lowerMemory;
	this->writeStrategy = writeStrategy;
}

template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize, unsigned cacheBlockIndexSize, unsigned tagSize>
void CacheEmulator::Cache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize>::replaceAndFetch(CacheEmulator::CacheEntry<tagSize, memoryOffsetSize>* victimEntry, bitset<memoryBlockIndexSize> victimMemoryBlockIndex, bitset<memoryBlockIndexSize> replacingMemoryBlockIndex)
{
	//Write back the old block's data, if needed
	if (victimEntry->presenceBit.test(0) && writeStrategy == WriteStrategy::WriteBack)
	{
		lowerMemory->writeBlock(victimMemoryBlockIndex, victimEntry->data);
	}

	//Copy the new data and set tag and presenceBit
	char* newData = lowerMemory->readBlock(replacingMemoryBlockIndex);
	for (int i = 0; i < (1 << memoryOffsetSize); i++)
	{
		victimEntry->data[i] = newData[i];
	}
	victimEntry->presenceBit.set();
	victimEntry->tag = replacingMemoryBlockIndex.to_ullong() >> cacheBlockIndexSize;
}

template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize, unsigned cacheBlockIndexSize, unsigned tagSize>
char CacheEmulator::Cache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize>::read(bitset<memoryBlockIndexSize + memoryOffsetSize> address)
{
	bitset<memoryBlockIndexSize> blockIndex = address.to_ullong() >> memoryOffsetSize;
	bitset<memoryOffsetSize> offset = address.to_ullong();

	CacheEntry<tagSize, memoryOffsetSize>* entry = retrieveEntry(blockIndex);

	return entry->data[offset.to_ullong()];
}

template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize, unsigned cacheBlockIndexSize, unsigned tagSize>
void CacheEmulator::Cache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize>::write(bitset<memoryBlockIndexSize + memoryOffsetSize> address, char data) {
	bitset<memoryBlockIndexSize> blockIndex = address.to_ullong() >> memoryOffsetSize;
	bitset<memoryOffsetSize> offset = address.to_ullong();

	CacheEntry<tagSize, memoryOffsetSize>* entry = retrieveEntry(blockIndex);

	entry->data[offset.to_ullong()] = data;
	if (writeStrategy == WriteStrategy::WriteThrough)
		lowerMemory->writeBlock(blockIndex, entry->data);
}


template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize, unsigned cacheBlockIndexSize, unsigned tagSize>
char* CacheEmulator::Cache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize>::readBlock(bitset<memoryBlockIndexSize> blockIndex)
{
	CacheEntry<tagSize, memoryOffsetSize>* entry = retrieveEntry(blockIndex);
	return entry->data;
}

template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize, unsigned cacheBlockIndexSize, unsigned tagSize>
void CacheEmulator::Cache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize>::writeBlock(bitset<memoryBlockIndexSize> blockIndex, char data[])
{
	CacheEntry<tagSize, memoryOffsetSize>* entry = retrieveEntry(blockIndex);
	for (int i = 0; i < (1 << memoryOffsetSize); i++)
	{
		entry->data[i] = data[i];
		if (writeStrategy == WriteStrategy::WriteThrough)
			lowerMemory->writeBlock(blockIndex, entry->data);
	}
}