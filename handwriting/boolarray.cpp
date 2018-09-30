#include "boolarray.h"
CBoolArray::CBoolArray()
{
	const char *fname="boolarray.skp";
	FILE *f;
	f=fopen(fname,"rb");
	if(f)
	{
		fread(table,1875,sizeof(unsigned),f);
		fclose(f);
	}else
	{
		for(unsigned n=0;n<1875;n++)
			table[n]=0;
	}
}

CBoolArray::~CBoolArray()
{
}

void CBoolArray::save()
{
	const char *fname="skips.skp";
	FILE *f;
	f=fopen(fname,"wb");
	if(f)
	{
		fwrite(table,1875,sizeof(unsigned),f);
		fclose(f);
	};
}
void CBoolArray::set(unsigned x,bool f)
{
	unsigned b=1<<(x&31);
	if(f)
		table[x>>5]|=b;
	else table[x>>5]&=~b;
};
bool CBoolArray::get(unsigned x)
{
	unsigned v=table[x>>5];
	return v>>(x&31)&1;
};

