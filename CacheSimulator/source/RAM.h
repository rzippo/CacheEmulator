#pragma once

#include <bitset>

using namespace std;

namespace CacheSimulator {
	template <unsigned memoryBlockIndexSize, unsigned memoryOffsetSize>
	class RAM {

	private:
		char blocks[1 << memoryBlockIndexSize][1 << memoryOffsetSize];

	public:
		RAM();

		char* readBlock(bitset<memoryBlockIndexSize> blockIndex);
		void writeBlock(bitset<memoryBlockIndexSize> blockIndex, char data[]);
	};
}

template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize>
CacheSimulator::RAM<memoryBlockIndexSize, memoryOffsetSize>::RAM()
{
	for (int i = 0; i < (1 << memoryBlockIndexSize); i++)
	{
		for (int j = 0; j < 1 << memoryOffsetSize; j++)
		{
			blocks[i][j] = rand() % (1 << 8);
		}
	}
}

template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize>
char* CacheSimulator::RAM<memoryBlockIndexSize, memoryOffsetSize>::readBlock(bitset<memoryBlockIndexSize> blockIndex) {
	return blocks[blockIndex.to_ullong()];
}

template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize>
void CacheSimulator::RAM<memoryBlockIndexSize, memoryOffsetSize>::writeBlock(bitset<memoryBlockIndexSize> blockIndex, char data[]) {
	char* block = blocks[blockIndex.to_ullong()];
	for (int i = 0; i < 1 << memoryOffsetSize; i++)
	{
		block[i] = data[i];
	}
}
