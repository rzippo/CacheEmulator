#pragma once

#include <bitset>
#include <vector>

#include "AssociativeCache.h"
#include "BlockMapping.h"
#include "CacheEntry.h"

using namespace std;

namespace CacheEmulator {
	template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize, unsigned cacheBlockIndexSize, unsigned tagSize, unsigned associativity>
	class FullAssociativeCache : CacheEmulator::AssociativeCache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize, associativity> {
		static_assert(cacheBlockIndexSize == 0, "CacheBlockIndexSize must be == 0");

	private:
		CacheEntry<tagSize, memoryOffsetSize> set[associativity];

	public:
		FullAssociativeCache(RAM<memoryBlockIndexSize, memoryOffsetSize>* memory, WriteStrategy writeStrategy)
			:AssociativeCache(memory, writeStrategy) {};
		
		char read(bitset<memoryBlockIndexSize + memoryOffsetSize> address) final;
		void write(bitset<memoryBlockIndexSize + memoryOffsetSize> address, char data) final;
		void flush() final;
	};
}

template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize, unsigned cacheBlockIndexSize, unsigned tagSize, unsigned associativity>
char CacheEmulator::FullAssociativeCache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize, associativity>::read(bitset<memoryBlockIndexSize + memoryOffsetSize> address)
{
	vector<CacheEntry<tagSize, memoryOffsetSize>*> vecSet;
	for (int i = 0; i < associativity; i++)
	{
		vecSet.push_back(&set[i]);
	}
	return readSet(vecSet, address);
}

template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize, unsigned cacheBlockIndexSize, unsigned tagSize, unsigned associativity>
void CacheEmulator::FullAssociativeCache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize, associativity>::write(bitset<memoryBlockIndexSize + memoryOffsetSize> address, char data)
{
	vector<CacheEntry<tagSize, memoryOffsetSize>*> vecSet;
	for (int i = 0; i < associativity; i++)
	{
		vecSet.push_back(&set[i]);
	}
	writeSet(vecSet, address, data);
}

template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize, unsigned cacheBlockIndexSize, unsigned tagSize, unsigned associativity>
inline void CacheEmulator::FullAssociativeCache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize, associativity>::flush()
{
	for (int i = 0; i < associativity; i++)
	{
		set[i].presenceBit.reset();
	}
}
