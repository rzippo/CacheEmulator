#pragma once

#include <bitset>
#include <vector>

#include "AssociativeCache.h"
#include "BlockMapping.h"
#include "CacheEntry.h"

using namespace std;

namespace CacheEmulator {
	template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize, unsigned cacheBlockIndexSize, unsigned tagSize>
	class FullAssociativeCache : CacheEmulator::AssociativeCache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize> {

	private:
		CacheEntry<tagSize, memoryOffsetSize>* set[];

	public:
		FullAssociativeCache(RAM<memoryBlockIndexSize, memoryOffsetSize>* memory, WriteStrategy writeStrategy, unsigned int associativity)
			:AssociativeCache(memory, writeStrategy, associativity)
		{
			set = new CacheEntry<tagSize, memoryOffsetSize>*[associativity];
			for (int i = 0; i < associativity; i++)
			{
				set[i] = new CacheEntry<tagSize, memoryOffsetSize>;
			}
		};		
		
		~FullAssociativeCache();

		char read(bitset<memoryBlockIndexSize + memoryOffsetSize> address) final;
		void write(bitset<memoryBlockIndexSize + memoryOffsetSize> address, char data) final;
		void flush() final;
	};
}

template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize, unsigned cacheBlockIndexSize, unsigned tagSize>
CacheEmulator::FullAssociativeCache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize>::~FullAssociativeCache()
{
	for (int i = 0; i < associativity; i++)
	{
		delete set[i];
	}
	delete[] set;
}

template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize, unsigned cacheBlockIndexSize, unsigned tagSize>
char CacheEmulator::FullAssociativeCache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize>::read(bitset<memoryBlockIndexSize + memoryOffsetSize> address)
{
	vector<CacheEntry<tagSize, memoryOffsetSize>*> vecSet;
	for (int i = 0; i < associativity; i++)
	{
		retSet.push_back(set[i]);
	}
	return readSet(vecSet, address, data);
}

template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize, unsigned cacheBlockIndexSize, unsigned tagSize>
void CacheEmulator::FullAssociativeCache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize>::write(bitset<memoryBlockIndexSize + memoryOffsetSize> address, char data)
{
	vector<CacheEntry<tagSize, memoryOffsetSize>*> vecSet;
	for (int i = 0; i < associativity; i++)
	{
		retSet.push_back(set[i]);
	}
	writeSet(vecSet, address, data);
}

template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize, unsigned cacheBlockIndexSize, unsigned tagSize>
inline void CacheEmulator::FullAssociativeCache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize>::flush()
{
	for (int i = 0; i < associativity; i++)
	{
		set
	}
}
