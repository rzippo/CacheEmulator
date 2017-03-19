#include <iostream>

#include "MemoryComponent.h"
#include "DirectMappedCache.h"
#include "FullAssociativeCache.h"
#include "SetAssociativeCache.h"
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

	for (int i = 0; i < (1 << (memoryBlockIndexSize + memoryOffsetSize)); i++)
	{
		dmCache.write(i, (char)(a + i));
	}

	bool passed = true;

	for (int i = 0; i < (1 << (memoryBlockIndexSize + memoryOffsetSize)); i++)
	{
		char b = dmCache.read(i);
		if (b != (char)(a + i))
		{
			passed = false;
			break;
		}
	}

	std::cout << "Direct mapped test: ";
	if (passed)
		std::cout << "PASSED" << endl;
	else
		std::cout << "FAILED" << endl;
}

void fullAssociativeTest()
{
	const unsigned memoryBlockIndexSize = 2;
	const unsigned memoryOffsetSize = 4;
	const unsigned cacheBlockIndexSize = 0;
	const unsigned tagSize = 2;
	const unsigned associativity = 2;

	RAM<memoryBlockIndexSize, memoryOffsetSize> ram;
	FullAssociativeCache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize, associativity> faCache(&ram, WriteStrategy::WriteBack);

	int a = (int)'A';

	for (int i = 0; i < (1 << (memoryBlockIndexSize + memoryOffsetSize)); i++)
	{
		faCache.write(i, (char)(a + i));
	}

	bool passed = true;

	for (int i = 0; i < (1 << (memoryBlockIndexSize + memoryOffsetSize)); i++)
	{
		char b = faCache.read(i);
		if (b != (char)(a + i))
		{
			passed = false;
			break;
		}
	}

	std::cout << "Full associative test: ";
	if (passed)
		std::cout << "PASSED" << endl;
	else
		std::cout << "FAILED" << endl;
}

void setAssociativeTest()
{
	const unsigned memoryBlockIndexSize = 2;
	const unsigned memoryOffsetSize = 4;
	const unsigned cacheBlockIndexSize = 1;
	const unsigned tagSize = 1;
	const unsigned associativity = 2;

	RAM<memoryBlockIndexSize, memoryOffsetSize> ram;
	SetAssociativeCache<memoryBlockIndexSize, memoryOffsetSize, cacheBlockIndexSize, tagSize, associativity> saCache(&ram, WriteStrategy::WriteBack);

	int a = (int)'A';

	for (int i = 0; i < (1 << (memoryBlockIndexSize + memoryOffsetSize)); i++)
	{
		saCache.write(i, (char)(a + i));
	}

	bool passed = true;

	for (int i = 0; i < (1 << (memoryBlockIndexSize + memoryOffsetSize)); i++)
	{
		char b = saCache.read(i);
		if (b != (char)(a + i))
		{
			passed = false;
			break;
		}
	}

	std::cout << "Set associative test: ";
	if (passed)
		std::cout << "PASSED" << endl;
	else
		std::cout << "FAILED" << endl;
}

void multiLayerCacheTest()
{
	const unsigned memoryOffsetSize = 4;		//16 bytes per block
	const unsigned memoryBlockIndexSize = 5;	//32 blocks in RAM 
	
	RAM<memoryBlockIndexSize, memoryOffsetSize> ram;

	//L3 is a set associative cache. 16 blocks, divided in 4 sets of 4 associate blocks
	const unsigned L3Associativity = 4;
	const unsigned L3CacheBlockIndexSize = 2;	
	const unsigned L3TagSize = 3;

	SetAssociativeCache<memoryBlockIndexSize, memoryOffsetSize, L3CacheBlockIndexSize, L3TagSize, L3Associativity> L3(&ram, WriteStrategy::WriteBack);

	//L2 is a full associative cache, with 8 blocks
	const unsigned L2Associativity = 8;
	const unsigned L2CacheBlockIndexSize = 0;
	const unsigned L2TagSize = 5;

	FullAssociativeCache<memoryBlockIndexSize, memoryOffsetSize, L2CacheBlockIndexSize, L2TagSize, L2Associativity> L2(&L3, WriteStrategy::WriteBack);

	//L1 is a direct mapped cache, with 4 blocks
	const unsigned L1CacheBlockIndexSize = 2;
	const unsigned L1TagSize = 3;

	DirectMappedCache<memoryBlockIndexSize, memoryOffsetSize, L1CacheBlockIndexSize, L1TagSize> L1(&L2, WriteStrategy::WriteBack);

	//Test

	int a = (int)'A';

	for (int i = 0; i < (1 << (memoryBlockIndexSize + memoryOffsetSize)); i++)
	{
		L1.write(i, (char)(a + i));
	}

	bool passed = true;

	for (int i = 0; i < (1 << (memoryBlockIndexSize + memoryOffsetSize)); i++)
	{
		char b = L1.read(i);
		if (b != (char)(a + i))
		{
			passed = false;
			break;
		}
	}

	std::cout << "Multi-layer cache test: ";
	if (passed)
		std::cout << "PASSED" << endl;
	else
		std::cout << "FAILED" << endl;
}

int main()
{
	directMappedTest();
	fullAssociativeTest();
	setAssociativeTest();
	multiLayerCacheTest();

	system("Pause");
}