#pragma once

#include <bitset>

using namespace std;

namespace CacheEmulator {
	template <unsigned memoryBlockIndexSize, unsigned memoryOffsetSize>
	class RAM : public MemoryComponent<memoryBlockIndexSize, memoryOffsetSize> {

	private:
		char blocks[1 << memoryBlockIndexSize][1 << memoryOffsetSize];

	public:
		RAM();

		char read(bitset<memoryBlockIndexSize + memoryOffsetSize> address) final;
		void write(bitset<memoryBlockIndexSize + memoryOffsetSize> address, char data) final;

		char* readBlock(bitset<memoryBlockIndexSize> blockIndex) final;
		void writeBlock(bitset<memoryBlockIndexSize> blockIndex, char data[]) final;
	};
}

template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize>
CacheEmulator::RAM<memoryBlockIndexSize, memoryOffsetSize>::RAM()
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
char CacheEmulator::RAM<memoryBlockIndexSize, memoryOffsetSize>::read(bitset<memoryBlockIndexSize + memoryOffsetSize> address)
{
	bitset<memoryBlockIndexSize> blockIndex = address.to_ullong() >> memoryOffsetSize;
	bitset<memoryOffsetSize> offset = address.to_ullong();

	return blocks[blockIndex.to_ullong()][offset.to_ullong()];
}

template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize>
void CacheEmulator::RAM<memoryBlockIndexSize, memoryOffsetSize>::write(bitset<memoryBlockIndexSize + memoryOffsetSize> address, char data)
{
	bitset<memoryBlockIndexSize> blockIndex = address.to_ullong() >> memoryOffsetSize;
	bitset<memoryOffsetSize> offset = address.to_ullong();

	blocks[blockIndex.to_ullong()][offset.to_ullong()] = data;
}

template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize>
char* CacheEmulator::RAM<memoryBlockIndexSize, memoryOffsetSize>::readBlock(bitset<memoryBlockIndexSize> blockIndex) {
	return blocks[blockIndex.to_ullong()];
}

template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize>
void CacheEmulator::RAM<memoryBlockIndexSize, memoryOffsetSize>::writeBlock(bitset<memoryBlockIndexSize> blockIndex, char data[]) {
	char* block = blocks[blockIndex.to_ullong()];
	for (int i = 0; i < (1 << memoryOffsetSize); i++)
	{
		block[i] = data[i];
	}
}
