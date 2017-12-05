#include "Set.h"
#include <math.h>

#ifndef CACHE_H_20171123
#define CACHE_H_20171123

#define PROC_SIZE 32 //because Processor is 32-bit 
#define OFFSET_BITS 6 //because 64B lines = (2^6)B
#define TAG_BITS 12 //because sets + lines = 14+6=20, 32-20=12

class Cache
{
public:
	Cache(int setSize, int associativity, bool verbose);  //constructor
	~Cache(); //destructor
	
	//public function declarations (primitives, prototypes)
	void readData(int address);
	void writeData(int address);
	void invalidate(int address);
	void readFromL2(int address);
	void resetAll();
	void printCache();
	void printStatistics();

private:
	Set** sets;
	int setSize;
	int associativity;
	bool verbose;
	int HitCount;
	int MissCount;
	int CacheReads;
	int CacheWrites;
	long totalOperations;
	
	unsigned int getIndex(unsigned int address);
	unsigned int getTag(unsigned int address); //pass this to Dustin
};


//test

#endif
