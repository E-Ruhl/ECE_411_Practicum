#include "Line.h"
#define MISS 0
#define HIT 1

#ifndef SET_H_20171123
#define SET_H_20171123

class Set
{
public:
	//Constructor and deconstructor function
	Set() {};
	Set(int a, int i, bool v, int aSize, int tSize, int oSize);
	~Set();

	//Public functions
	int read(unsigned int tag);
	int write(unsigned int tag);
	void invalidate(unsigned int tag);
	void readFromL2(unsigned int tag);
	void reset();
	void print();

private:
	//Private variables
	Line* lines;
	Line* first;
	Line* last;
	unsigned int associativity;
	bool verbose;
	int count;
	unsigned int index;
	unsigned int addressBitLength;
	unsigned int indexBitLength;
	unsigned int offsetBitLength;


	//Private functions
	void evict();
	void setLRU();
	unsigned int reconstructAddress(unsigned int tag);
	void handleWriteMiss(unsigned int tag);
	void handleReadMiss(unsigned int tag);
	bool isFull();
	std::string htos(unsigned int n);
	void add(unsigned int tag, MESI state);
	void remove(unsigned int tag);
	void touch(unsigned int tag);
};
#endif
