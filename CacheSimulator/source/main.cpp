#include <iostream>

#include "DirectMappedCache.h"
#include "FullAssociativeCache.h"
#include "RAM.h"

using namespace CacheEmulator;

void directMappedTest()
{
	const unsigned memoryBlockIndexSize = 2;
	const unsigned memoryOffsetSize = 4;
	const unsigned cacheBlockIndexSize = 1;
	const unsigned tagSize = 1;

	RAM<memoryBlockIndexSize, memoryOffsetSize> ram;
	DirectMappedCache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize> dmCache(&ram, WriteStrategy::WriteBack);

	int a = (int)'A';
	std::cout << "First char: " << (char) a  << endl;

	std::cout << "Number of bytes: " << (1 << (memoryBlockIndexSize + memoryOffsetSize)) << endl;

	for (int i = 0; i < (1 << (memoryBlockIndexSize + memoryOffsetSize)); i++)
	{
		dmCache.write(i, (char)(a + i));
	}

	std::cout << "Now reading:" << endl;

	for (int i = 0; i < (1 << (memoryBlockIndexSize + memoryOffsetSize)); i++)
	{
		std::cout << dmCache.read(i);
	}
	std::cout << endl;
}

void fullAssociativeTest()
{
	const unsigned memoryBlockIndexSize = 2;
	const unsigned memoryOffsetSize = 4;
	const unsigned cacheBlockIndexSize = 0;
	const unsigned tagSize = 2;

	RAM<memoryBlockIndexSize, memoryOffsetSize> ram;
	FullAssociativeCache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize> dmCache(&ram, WriteStrategy::WriteBack, 2);

	int a = (int)'A';
	std::cout << "First char: " << (char)a << endl;

	std::cout << "Number of bytes: " << (1 << (memoryBlockIndexSize + memoryOffsetSize)) << endl;

	for (int i = 0; i < (1 << (memoryBlockIndexSize + memoryOffsetSize)); i++)
	{
		dmCache.write(i, (char)(a + i));
	}

	std::cout << "Now reading:" << endl;

	for (int i = 0; i < (1 << (memoryBlockIndexSize + memoryOffsetSize)); i++)
	{
		std::cout << dmCache.read(i);
	}
	std::cout << endl;
}

int main()
{
	directMappedTest();
	system("Pause");
}