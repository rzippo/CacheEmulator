#pragma once

#include <bitset>
#include <vector>

#include "Cache.h"
#include "CacheEntry.h"


using namespace std;

namespace CacheSimulator {
	template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize, unsigned cacheBlockIndexSize, unsigned tagSize, unsigned associativity>
	class AssociativeCache : CacheSimulator::Cache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize> {
		static_assert(associativity > 0, "Associativity must be > 0");

	protected:
		AssociativeCache(RAM<memoryBlockIndexSize, memoryOffsetSize>* memory, WriteStrategy writeStrategy)
			:Cache(memory, writeStrategy) {};

		char readSet(vector<CacheSimulator::CacheEntry<tagSize, memoryOffsetSize>*> set, bitset<memoryBlockIndexSize + memoryOffsetSize> address);
		void writeSet(vector<CacheSimulator::CacheEntry<tagSize, memoryOffsetSize>*> set, bitset<memoryBlockIndexSize + memoryOffsetSize> address, char data);

		CacheSimulator::CacheEntry<tagSize, memoryOffsetSize>* scanSet(vector<CacheSimulator::CacheEntry<tagSize, memoryOffsetSize>*> set, bitset<tagSize> tag);
		CacheSimulator::CacheEntry<tagSize, memoryOffsetSize>* selectRandomVictim(vector<CacheSimulator::CacheEntry<tagSize, memoryOffsetSize>*> set);
	};
}

template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize, unsigned cacheBlockIndexSize, unsigned tagSize, unsigned associativity>
char CacheSimulator::AssociativeCache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize, associativity>::readSet(vector<CacheSimulator::CacheEntry<tagSize, memoryOffsetSize>*> set, bitset<memoryBlockIndexSize + memoryOffsetSize> address)
{
	bitset<memoryBlockIndexSize> blockIndex = address.to_ullong() >> memoryOffsetSize;
	bitset<memoryOffsetSize> offset = address.to_ullong();
	bitset<tagSize> tag = blockIndex.to_ullong() >> cacheBlockIndexSize;
	bitset<cacheBlockIndexSize> cacheIndex = blockIndex.to_ullong();

	CacheEntry<tagSize, memoryOffsetSize>* entry = scanSet(set, tag);
	if (entry == nullptr)
	{
		entry = selectRandomVictim(set);
		bitset<memoryBlockIndexSize> victimBlockIndex = (entry->tag.to_ullong() << cacheBlockIndexSize) + cacheIndex.to_ullong();
		replaceAndFetch(entry, victimBlockIndex, blockIndex);
	}
	else if (!entry->presenceBit.test(0))
	{
		bitset<memoryBlockIndexSize> victimBlockIndex = (entry->tag.to_ullong() << cacheBlockIndexSize) + cacheIndex.to_ullong();
		replaceAndFetch(entry, victimBlockIndex, blockIndex);
	}

	return entry->data[offset.to_ullong()];
}

template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize, unsigned cacheBlockIndexSize, unsigned tagSize, unsigned associativity>
void CacheSimulator::AssociativeCache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize, associativity>::writeSet(vector<CacheSimulator::CacheEntry<tagSize, memoryOffsetSize>*> set, bitset<memoryBlockIndexSize + memoryOffsetSize> address, char data)
{
	bitset<memoryBlockIndexSize> blockIndex = address.to_ullong() >> memoryOffsetSize;
	bitset<memoryOffsetSize> offset = address.to_ullong();
	bitset<tagSize> tag = blockIndex.to_ullong() >> cacheBlockIndexSize;
	bitset<cacheBlockIndexSize> cacheIndex = blockIndex.to_ullong();

	CacheEntry<tagSize, memoryOffsetSize>* entry = scanSet(set, tag);
	if (entry == nullptr)
	{
		entry = selectRandomVictim(set);
		bitset<memoryBlockIndexSize> victimBlockIndex = (entry->tag.to_ullong() << cacheBlockIndexSize) + cacheIndex.to_ullong();
		replaceAndFetch(entry, victimBlockIndex, blockIndex);
	}
	else if (!entry->presenceBit.test(0))
	{
		bitset<memoryBlockIndexSize> victimBlockIndex = (entry->tag.to_ullong() << cacheBlockIndexSize) + cacheIndex.to_ullong();
		replaceAndFetch(entry, victimBlockIndex, blockIndex);
	}

	entry->data[offset.to_ullong()] = data;
	if (writeStrategy == WriteStrategy::WriteThrough)
		memory->writeBlock(blockIndex, entry->data);
}

template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize, unsigned cacheBlockIndexSize, unsigned tagSize, unsigned associativity>
CacheSimulator::CacheEntry<tagSize, memoryOffsetSize>* CacheSimulator::AssociativeCache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize, associativity>::scanSet(vector<CacheSimulator::CacheEntry<tagSize, memoryOffsetSize>*> set, bitset<tagSize> tag) {
	CacheEntry<tagSize, memoryOffsetSize>* emptySlot = nullptr;
	for (CacheEntry<tagSize, memoryOffsetSize>* entry : set)
	{
		if (entry->tag == tag)
			return entry;
		else
			if (!entry->presenceBit.test(0))
				emptySlot = entry;
	}

	return emptySlot;
}

template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize, unsigned cacheBlockIndexSize, unsigned tagSize, unsigned associativity>
CacheSimulator::CacheEntry<tagSize, memoryOffsetSize>* CacheSimulator::AssociativeCache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize, associativity>::selectRandomVictim(vector<CacheSimulator::CacheEntry<tagSize, memoryOffsetSize>*> set) {
	int victim = rand() % associativity;
	return set[victim];
}
