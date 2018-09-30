#ifndef _BOOLARRAY
#define _BOOLARRAY
#include <stdio.h>

class CBoolArray
{
private:
	unsigned table[int(1875)];//60000 samples /32 bits = 1875
public:
	CBoolArray();
	~CBoolArray();
	void save();
	void set(unsigned x,bool f);
	bool get(unsigned x);
	void disp();
};
#endif

