#ifndef _TRAINING
#define _TRAINING
#include <stdio.h>

class CTraining
{
private:
	const char *imageFileName;
	const char *labelFileName;
	const char *mode;
	FILE *imageFile;
	FILE *labelFile;
	unsigned char d[28*28];
	unsigned char x;

public:
	CTraining();
	~CTraining();
	int read(int n,double *data);
};
#endif

