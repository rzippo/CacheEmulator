#pragma once

#include <bitset>
#include <vector>

#include "Cache.h"
#include "CacheEntry.h"


using namespace std;

namespace CacheSimulator {
	template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize, unsigned cacheBlockIndexSize, unsigned tagSize>
	class AssociativeCache : CacheSimulator::Cache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize> {
	protected:
		unsigned int associativity;

		AssociativeCache(RAM<memoryBlockIndexSize, memoryOffsetSize>* memory, WriteStrategy writeStrategy, unsigned int associativity)
			:Cache(memory, writeStrategy)
		{
			this->associativity = (associativity < 2) ? 2 : associativity;
		};

		char readSet(vector<CacheSimulator::CacheEntry<tagSize, memoryOffsetSize>*> set, bitset<memoryBlockIndexSize + memoryOffsetSize> address);
		void writeSet(vector<CacheSimulator::CacheEntry<tagSize, memoryOffsetSize>*> set, bitset<memoryBlockIndexSize + memoryOffsetSize> address, char data);

		CacheSimulator::CacheEntry<tagSize, memoryOffsetSize>* scanSet(vector<CacheSimulator::CacheEntry<tagSize, memoryOffsetSize>*> set, bitset<tagSize> tag);
		CacheSimulator::CacheEntry<tagSize, memoryOffsetSize>* selectVictim(vector<CacheSimulator::CacheEntry<tagSize, memoryOffsetSize>*> set);
	};
}

template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize, unsigned cacheBlockIndexSize, unsigned tagSize>
char CacheSimulator::AssociativeCache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize>::readSet(vector<CacheSimulator::CacheEntry<tagSize, memoryOffsetSize>*> set, bitset<memoryBlockIndexSize + memoryOffsetSize> address)
{
	bitset<memoryBlockIndexSize> blockIndex = address >> memoryOffsetSize;
	bitset<memoryOffsetSize> offset = (address << memoryBlockIndexSize) >> memoryBlockIndexSize;
	bitset<tagSize> tag = blockIndex >> cacheBlockIndexSize;
	bitset<cacheBlockIndexSize> cacheIndex = (blockIndex << tagSize) >> tagSize;

	CacheEntry<tagSize, memoryOffsetSize>* entry = scanSet(set, tag);
	if (entry == null)
	{
		entry = selectVictim(set);
		replaceAndFetch(entry, (entry->tag << cacheBlockIndexSize) + cacheIndex, blockIndex);
	}
	else if (entry->presenceBit == 0)
	{
		replaceAndFetch(entry, (entry->tag << cacheBlockIndexSize) + cacheIndex, blockIndex);
	}

	return entry->data[offset];
}

template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize, unsigned cacheBlockIndexSize, unsigned tagSize>
void CacheSimulator::AssociativeCache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize>::writeSet(vector<CacheSimulator::CacheEntry<tagSize, memoryOffsetSize>*> set, bitset<memoryBlockIndexSize + memoryOffsetSize> address, char data)
{
	bitset<memoryBlockIndexSize> blockIndex = address >> memoryOffsetSize;
	bitset<memoryOffsetSize> offset = (address << memoryBlockIndexSize) >> memoryBlockIndexSize;
	bitset<tagSize> tag = blockIndex >> cacheBlockIndexSize;
	bitset<cacheBlockIndexSize> cacheIndex = (blockIndex << tagSize) >> tagSize;

	CacheEntry<tagSize, memoryOffsetSize>* entry = scanSet(set, tag);
	if (entry == null)
	{
		entry = selectVictim(set);
		replaceAndFetch(entry, (entry->tag << cacheBlockIndexSize) + cacheIndex, blockIndex);
	}
	else if (entry->presenceBit == 0)
	{
		replaceAndFetch(entry, (entry->tag << cacheBlockIndexSize) + cacheIndex, blockIndex);
	}

	entry->data[offset] = data;
	if (writeStrategy == WriteStrategy::WriteThrough)
		memory->writeBlock(blockIndex, entry->data);
}

template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize, unsigned cacheBlockIndexSize, unsigned tagSize>
CacheSimulator::CacheEntry<tagSize, memoryOffsetSize>* CacheSimulator::AssociativeCache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize>::scanSet(vector<CacheSimulator::CacheEntry<tagSize, memoryOffsetSize>*> set, bitset<tagSize> tag) {
	CacheEntry<tagSize, memoryOffsetSize>* emptySlot = null;
	for (CacheEntry<tagSize, memoryOffsetSize>* entry : set)
	{
		if (entry->tag == tag)
			return entry;
		else
			if (entry->presenceBit == 0)
				emptySlot = entry;
	}

	return emptySlot;
}

template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize, unsigned cacheBlockIndexSize, unsigned tagSize>
CacheSimulator::CacheEntry<tagSize, memoryOffsetSize>* CacheSimulator::AssociativeCache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize>::selectVictim(vector<CacheSimulator::CacheEntry<tagSize, memoryOffsetSize>*> set) {
	int victim = rand() % associativity;
	return set.operator[victim];
}
