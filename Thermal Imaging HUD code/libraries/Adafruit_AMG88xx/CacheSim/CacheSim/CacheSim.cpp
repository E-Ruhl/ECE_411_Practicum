#include <iomanip>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <exception>
#include <string>
#include "Cache.h"

using namespace std;

#define NUM_SETS	16384 //because 16K sets = 2^14
#define INSTR_ASSOC	2
#define DATA_ASSOC	16

#ifdef LINUX
bool linuxOp = true;
#else
bool linuxOp = false;
#endif

void ResetCache();
bool checkFlags(char* arg, string flag);
void printCaches();
void printCachesStats();

Cache* instructionCache;
Cache* dataCache;

int main(int argc, char* argv[])
{
	string line;
	ifstream file;
	bool debug = false;
	bool verbose = false;
	int count = 0;
	
	for (int i = 0; i < argc; i++)
	{
		debug = debug | checkFlags(argv[i], "-D");
		debug = debug | checkFlags(argv[i], "-d");
	}
	for (int i = 0; i < argc; i++)
	{
		verbose = verbose | checkFlags(argv[i], "-A");
		verbose = verbose | checkFlags(argv[i], "-a");
	}
	cout << "Initializing Instruction Cache\n";
	instructionCache = new Cache(NUM_SETS, INSTR_ASSOC, debug);
	cout << "Initializing Data Cache\n\n";
	dataCache = new Cache(NUM_SETS, DATA_ASSOC, debug);
	
	if (verbose) cout << "Opening file\n";

	try
	{
		file.open(argv[1]);
		if (file.is_open())
		{
			if (verbose) cout << argv[1] << " was opened successfully\n";
			while (getline(file, line))
			{
				istringstream iss(line);
				unsigned int instruction, address;
				count++;
				if (!(iss >> instruction))
				{
					cerr << "failed to read instruction code at line: " << count << endl << endl;
					continue;
				}
				else if (instruction < 0 || instruction > 4 && instruction < 8 || instruction > 9)
				{
					cerr << "not actual code, skipping instruction code and address at line: " << count << endl << endl;
					continue;
				}
				else if ((instruction != 9 || instruction != 8) && !(iss >> hex >> address))
				{
					cerr << "ignoring address after instruction code: " << instruction << ", THIS IS NOT AN ACTUAL ERROR\n\n";
				}

				// this is were we actually handle cache instructions
				//if (verbose) cout << endl << instruction << ',' << hex << address << '\n';

				switch (instruction)
				{
				case 0:
					//read data
					dataCache->readData(address);
					break;
				case 1:
					// write data
					dataCache->writeData(address);
					break;
				case 2:
					// instruction fetch
					// instruction read
					instructionCache->readData(address);
					break;
				case 3:
					// invalidate
					dataCache->invalidate(address);
					break;
				case 4:
					dataCache->readFromL2(address);
					instructionCache->readFromL2(address);
					break;
				case 8:
					// clear reset
					dataCache->resetAll();
					instructionCache->resetAll();
					break;
				case 9:
					// print all
					printCaches();
					continue;
				default:
					cout << instruction << " found at line " << count << ", is not a valid instruction for a cache" << endl;
					break;
				}

				if (verbose)
				{
					cout << endl;
					printCaches();
					char next = getchar();
					if (next == 'Q' || next == 'q' || next == '')
					{
						cout << "Now terminating simulation\n\n";
						break;
					}
				}
			}
		}
	}
	catch (exception e)
	{
		cout << e.what() << "\n\n";
	}
	cout << "\n\nCACHE SIMULATION HAS ENDED\nNOW PRINTING FINAL CACHE CONTENTS AND STATISTICS\n\n";
	printCaches();
	printCachesStats();
	file.close();
	if (!linuxOp) getchar();
	return 0;
}

void printCaches()
{
	cout << "Here is are the valid contents of the L1 Instruction Cache\n";
	instructionCache->printCache();
	cout << "\n\n";
	cout << "Here is are the valid contents of the L1 Data Cache\n";
	dataCache->printCache();
	cout << "\n\n";
}

void printCachesStats()
{
	cout << "The statistics for the L1 Instruction Cache are ";
	instructionCache->printStatistics();
	cout << "The statistics for the L1 Data Cache are ";
	dataCache->printStatistics();
}

bool checkFlags(char* arg, string flag)
{
	string str = string(arg);
	return str == flag;
}

void ResetCache()
{
	instructionCache->resetAll();
	dataCache->resetAll();
}
