#include <bitset>
#include <vector>

#include "AssociativeCache.h"
#include "BlockMapping.h"
#include "CacheEntry.h"

using namespace std;

namespace CacheEmulator {
	template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize, unsigned cacheBlockIndexSize, unsigned tagSize, unsigned associativity>
	class SetAssociativeCache : public CacheEmulator::AssociativeCache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize, associativity> {
		static_assert(cacheBlockIndexSize > 0, "CacheBlockIndexSize must be > 0");

	protected:
		CacheEmulator::BlockMapping<cacheBlockIndexSize, tagSize, memoryOffsetSize> mappings[associativity];

		vector<CacheEmulator::CacheEntry<tagSize, memoryOffsetSize>*> retrieveSet(bitset<memoryBlockIndexSize> memoryBlockIndex) final;
	public:
		SetAssociativeCache(MemoryComponent<memoryBlockIndexSize, memoryOffsetSize>* lowerMemory, WriteStrategy writeStrategy)
			:AssociativeCache(lowerMemory, writeStrategy) {};
		
		void flush() final;
	};
}

template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize, unsigned cacheBlockIndexSize, unsigned tagSize, unsigned associativity>
vector<CacheEmulator::CacheEntry<tagSize, memoryOffsetSize>*> CacheEmulator::SetAssociativeCache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize, associativity>::retrieveSet(bitset<memoryBlockIndexSize> memoryBlockIndex)
{
	bitset<cacheBlockIndexSize> cacheBlockIndex = memoryBlockIndex.to_ulong();

	vector<CacheEntry<tagSize, memoryOffsetSize>*> set;
	for (int i = 0; i < associativity; i++)
	{
		set.push_back(&mappings[i].entries[cacheBlockIndex.to_ullong()]);
	}
	return set;
}

template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize, unsigned cacheBlockIndexSize, unsigned tagSize, unsigned associativity>
inline void CacheEmulator::SetAssociativeCache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize, associativity>::flush()
{
	for (int i = 0; i < associativity; i++)
	{
		for (int j = 0; j < (1 << cacheBlockIndexSize); j++)
		{
			mappings[i].entries[cacheBlockIndexSize].presenceBit.reset();
		}
	}
}
