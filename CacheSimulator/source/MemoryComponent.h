#pragma once

#include <bitset>

using namespace std;

namespace CacheEmulator
{
	template<unsigned memoryBlockIndexSize, unsigned memoryOffsetSize>
	class MemoryComponent{
	public:
		virtual char read(bitset<memoryBlockIndexSize + memoryOffsetSize> address) = 0;
		virtual void write(bitset<memoryBlockIndexSize + memoryOffsetSize> address, char data) = 0;
		
		virtual char* readBlock(bitset<memoryBlockIndexSize> blockIndex) = 0;
		virtual void writeBlock(bitset<memoryBlockIndexSize> blockIndex, char data[]) = 0;
	};
}
