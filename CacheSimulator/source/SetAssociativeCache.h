#include <bitset>
#include <vector>

#include "AssociativeCache.h"
#include "BlockMapping.h"
#include "CacheEntry.h"

using namespace std;

namespace CacheEmulator {
	template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize, unsigned cacheBlockIndexSize, unsigned tagSize>
	class SetAssociativeCache : CacheEmulator::AssociativeCache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize> {

	private:
		unsigned int associativity;
		CacheEmulator::BlockMapping<cacheBlockIndexSize, tagSize, memoryOffsetSize> mappings[];

	public:
		SetAssociativeCache(unsigned int associativity);
		~SetAssociativeCache();

		char read(bitset<memoryBlockIndexSize + memoryOffsetSize> address) final;
		void write(bitset<memoryBlockIndexSize + memoryOffsetSize> address, char data) final;

	private:
		vector<CacheEmulator::CacheEntry<tagSize, memoryOffsetSize>*> retrieveSet(bitset<cacheBlockIndexSize> index);
	};
}

template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize, unsigned cacheBlockIndexSize, unsigned tagSize>
CacheEmulator::SetAssociativeCache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize>::SetAssociativeCache(unsigned int associativity)
{
	this->associativity = associativity;

	if (associativity < 2)
		associativity = 2;

	mappings = new BlockMapping<cacheBlockIndexSize, tagSize, memoryOffsetSize>[associativity];
}

template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize, unsigned cacheBlockIndexSize, unsigned tagSize>
CacheEmulator::SetAssociativeCache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize>::~SetAssociativeCache()
{
	delete[] mappings;
}

template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize, unsigned cacheBlockIndexSize, unsigned tagSize>
char CacheEmulator::SetAssociativeCache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize>::read(bitset<memoryBlockIndexSize + memoryOffsetSize> address)
{
	bitset<cacheBlockIndexSize> index = ((address >> memoryOffsetSize) << tagSize) >> tagSize;
	vector<CacheEntry<tagSize, memoryOffsetSize>*> set = retrieveSet(index);
	return readSet(set, address, data);
}

template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize, unsigned cacheBlockIndexSize, unsigned tagSize>
void CacheEmulator::SetAssociativeCache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize>::write(bitset<memoryBlockIndexSize + memoryOffsetSize> address, char data)
{
	bitset<cacheBlockIndexSize> index = ((address >> memoryOffsetSize) << tagSize) >> tagSize;
	vector<CacheEntry<tagSize, memoryOffsetSize>*> set = retrieveSet(index);
	writeSet(set, address, data);
}

template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize, unsigned cacheBlockIndexSize, unsigned tagSize>
vector<CacheEmulator::CacheEntry<tagSize, memoryOffsetSize>*> CacheEmulator::SetAssociativeCache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize>::retrieveSet(bitset<cacheBlockIndexSize> index)
{
	vector<CacheEntry<tagSize, memoryOffsetSize>*> set;
	for (int i = 0; i < associativity; i++)
	{
		set.push_back(mappings[i][index]);
	}
	return set;
}
