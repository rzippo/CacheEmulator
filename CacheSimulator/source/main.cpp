#include <iostream>

#include "DirectMappedCache.h"
#include "FullAssociativeCache.h"
#include "SetAssociativeCache.h"
#include "RAM.h"

using namespace CacheEmulator;

void directMappedTest()
{
	std::cout << "Testing direct mapped cache" << endl;

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
	std::cout << "Testing full associative cache" << endl;

	const unsigned memoryBlockIndexSize = 2;
	const unsigned memoryOffsetSize = 4;
	const unsigned cacheBlockIndexSize = 1;
	const unsigned tagSize = 1;
	const unsigned associativity = 2;

	RAM<memoryBlockIndexSize, memoryOffsetSize> ram;
	SetAssociativeCache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize, associativity> saCache(&ram, WriteStrategy::WriteBack);

	int a = (int)'A';
	std::cout << "First char: " << (char)a << endl;

	std::cout << "Number of bytes: " << (1 << (memoryBlockIndexSize + memoryOffsetSize)) << endl;

	for (int i = 0; i < (1 << (memoryBlockIndexSize + memoryOffsetSize)); i++)
	{
		saCache.write(i, (char)(a + i));
	}

	std::cout << "Now reading:" << endl;

	for (int i = 0; i < (1 << (memoryBlockIndexSize + memoryOffsetSize)); i++)
	{
		std::cout << saCache.read(i);
	}
	std::cout << endl;
}

void setAssociativeTest()
{
	std::cout << "Testing set associative cache" << endl;

	const unsigned memoryBlockIndexSize = 2;
	const unsigned memoryOffsetSize = 4;
	const unsigned cacheBlockIndexSize = 0;
	const unsigned tagSize = 2;
	const unsigned associativity = 2;

	RAM<memoryBlockIndexSize, memoryOffsetSize> ram;
	FullAssociativeCache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize, associativity> faCache(&ram, WriteStrategy::WriteBack);

	int a = (int)'A';
	std::cout << "First char: " << (char)a << endl;

	std::cout << "Number of bytes: " << (1 << (memoryBlockIndexSize + memoryOffsetSize)) << endl;

	for (int i = 0; i < (1 << (memoryBlockIndexSize + memoryOffsetSize)); i++)
	{
		faCache.write(i, (char)(a + i));
	}

	std::cout << "Now reading:" << endl;

	for (int i = 0; i < (1 << (memoryBlockIndexSize + memoryOffsetSize)); i++)
	{
		std::cout << faCache.read(i);
	}
	std::cout << endl;
}

int main()
{
	directMappedTest();
	fullAssociativeTest();
	setAssociativeTest();

	system("Pause");
}