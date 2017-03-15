#include <bitset>
#include <vector>

#include "AssociativeCache.h"
#include "BlockMapping.h"
#include "CacheEntry.h"

using namespace std;

namespace CacheSimulator {
	template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize, unsigned cacheBlockIndexSize, unsigned tagSize, unsigned associativity>
	class SetAssociativeCache : CacheSimulator::AssociativeCache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize, associativity> {
		static_assert(cacheBlockIndexSize > 0, "CacheBlockIndexSize must be > 0");

	private:
		CacheSimulator::BlockMapping<cacheBlockIndexSize, tagSize, memoryOffsetSize> mappings[associativity];

		vector<CacheSimulator::CacheEntry<tagSize, memoryOffsetSize>*> retrieveSet(bitset<cacheBlockIndexSize> index);

	public:
		SetAssociativeCache(RAM<memoryBlockIndexSize, memoryOffsetSize>* memory, WriteStrategy writeStrategy)
			:AssociativeCache(memory, writeStrategy) {};

		char read(bitset<memoryBlockIndexSize + memoryOffsetSize> address) final;
		void write(bitset<memoryBlockIndexSize + memoryOffsetSize> address, char data) final;
		void flush() final;
	};
}

template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize, unsigned cacheBlockIndexSize, unsigned tagSize, unsigned associativity>
char CacheSimulator::SetAssociativeCache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize, associativity>::read(bitset<memoryBlockIndexSize + memoryOffsetSize> address)
{
	bitset<cacheBlockIndexSize> index = address.to_ullong() >> memoryOffsetSize;
	vector<CacheEntry<tagSize, memoryOffsetSize>*> set = retrieveSet(index);
	return readSet(set, address);
}

template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize, unsigned cacheBlockIndexSize, unsigned tagSize, unsigned associativity>
void CacheSimulator::SetAssociativeCache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize, associativity>::write(bitset<memoryBlockIndexSize + memoryOffsetSize> address, char data)
{
	bitset<cacheBlockIndexSize> index = address.to_ullong() >> memoryOffsetSize;
	vector<CacheEntry<tagSize, memoryOffsetSize>*> set = retrieveSet(index);
	writeSet(set, address, data);
}

template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize, unsigned cacheBlockIndexSize, unsigned tagSize, unsigned associativity>
inline void CacheSimulator::SetAssociativeCache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize, associativity>::flush()
{
	for (int i = 0; i < associativity; i++)
	{
		for (int j = 0; j < (1 << cacheBlockIndexSize); j++)
		{
			mappings[i].entries[cacheBlockIndexSize].presenceBit.reset();
		}
	}
}

template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize, unsigned cacheBlockIndexSize, unsigned tagSize, unsigned associativity>
vector<CacheSimulator::CacheEntry<tagSize, memoryOffsetSize>*> CacheSimulator::SetAssociativeCache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize, associativity>::retrieveSet(bitset<cacheBlockIndexSize> index)
{
	vector<CacheEntry<tagSize, memoryOffsetSize>*> set;
	for (int i = 0; i < associativity; i++)
	{
		set.push_back(&mappings[i].entries[index.to_ullong()]);
	}
	return set;
}
