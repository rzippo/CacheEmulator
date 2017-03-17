#pragma once

#include <bitset>
#include <vector>

#include "Cache.h"
#include "CacheEntry.h"


using namespace std;

namespace CacheEmulator {
	template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize, unsigned cacheBlockIndexSize, unsigned tagSize, unsigned associativity>
	class AssociativeCache : public CacheEmulator::Cache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize> {
		static_assert(associativity > 0, "Associativity must be > 0");

	protected:
		AssociativeCache(MemoryComponent<memoryBlockIndexSize, memoryOffsetSize>* lowerMemory, WriteStrategy writeStrategy)
			:Cache(lowerMemory, writeStrategy) {};

		CacheEntry<tagSize, memoryOffsetSize>* retrieveEntry(bitset<memoryBlockIndexSize> memoryBlockIndex) final;
		
		CacheEmulator::CacheEntry<tagSize, memoryOffsetSize>* scanSet(vector<CacheEmulator::CacheEntry<tagSize, memoryOffsetSize>*> set, bitset<tagSize> tag);
		CacheEmulator::CacheEntry<tagSize, memoryOffsetSize>* selectRandomVictim(vector<CacheEmulator::CacheEntry<tagSize, memoryOffsetSize>*> set);

		virtual vector<CacheEmulator::CacheEntry<tagSize, memoryOffsetSize>*> retrieveSet(bitset<memoryBlockIndexSize> blockIndex) = 0;
	};
}

template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize, unsigned cacheBlockIndexSize, unsigned tagSize, unsigned associativity>
CacheEmulator::CacheEntry<tagSize, memoryOffsetSize>* CacheEmulator::AssociativeCache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize, associativity>::retrieveEntry(bitset<memoryBlockIndexSize> blockIndex)
{
	bitset<tagSize> tag = blockIndex.to_ullong() >> cacheBlockIndexSize;
	bitset<cacheBlockIndexSize> cacheIndex = blockIndex.to_ullong();

	vector<CacheEmulator::CacheEntry<tagSize, memoryOffsetSize>*> set = retrieveSet(blockIndex);
	
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

	return entry;
}

template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize, unsigned cacheBlockIndexSize, unsigned tagSize, unsigned associativity>
CacheEmulator::CacheEntry<tagSize, memoryOffsetSize>* CacheEmulator::AssociativeCache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize, associativity>::scanSet(vector<CacheEmulator::CacheEntry<tagSize, memoryOffsetSize>*> set, bitset<tagSize> tag) {
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
CacheEmulator::CacheEntry<tagSize, memoryOffsetSize>* CacheEmulator::AssociativeCache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize, associativity>::selectRandomVictim(vector<CacheEmulator::CacheEntry<tagSize, memoryOffsetSize>*> set) {
	int victim = rand() % associativity;
	return set[victim];
}
