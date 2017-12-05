#include <string>

using namespace std;

#ifndef LINE_H_20171123
#define LINE_H_20171123

enum MESI
{
	MODIFIED = 0,
	INVALID = 1,
	SHARED = 2,
	EXCLUSIVE = 3,
};

class Line 
{
public:
	Line();
	~Line();
	void invalidate();
	string printMESI();
	
	int tag;
	int LRU;
	MESI mesi;
	Line* prev;
	Line* next;
private:
};

#endif
