#include "training.h"
CTraining::CTraining()
{
	imageFileName="train-images.idx3-ubyte";
	labelFileName="train-labels.idx1-ubyte";
	mode="rb";
	imageFile=fopen(imageFileName,mode);
	labelFile=fopen(labelFileName,mode);
}
CTraining::~CTraining()
{
	fclose(imageFile);
	fclose(labelFile);
}
int CTraining::read(int n,double *data)
{
	fseek(imageFile,16+(28*28)*n,SEEK_SET);
	fread(d,28,28,imageFile);
	
	fseek(labelFile,8+n,SEEK_SET);
	fread(&x,1,1,labelFile);
	for(unsigned n=0;n<(28*28);n++)
	{
		data[n]=double(d[n])/255.0;
	}
	return int(x);
}	

