#include "net.h"
CNet::CNet()
{
	layerCount=0;
	//
	layerSize=0;
	neuron=0;
	neuronError=0;
	t=0;
	pweight=0;
	deltaWeight=0;
	weightTally=0;
};
CNet::~CNet()
{
	clean();
}
void CNet::clean()
{

	if(layerCount)	//if defined delete all buffers
	{
		for(unsigned m=1;m<layerCount;m++)
		{
			for(unsigned n=0;n<layerSize[m];n++)
			{
				delete[]deltaWeight[m][n];
			}
			delete[]deltaWeight[m];
		}
		delete[]pweight;
		delete[]weight;
		delete[]deltaWeight;
		//
		for(unsigned m=1;m<(layerCount-1);m++)
			delete[]neuron[m];
		delete[]neuron;
		//
		for(unsigned m=1;m<layerCount;m++)
			delete[]neuronError[m];
		delete[]neuronError;
		//
		delete[]layerSize;
	}
	layerCount=0;
	layerSize=0;
	neuron=0;
	weightTally=0;
};
void CNet::define(const unsigned *in)
{
	clean();
	layerCount=0;
	while(in[layerCount])layerCount++;		//count layers passed in
	layerSize=new unsigned[layerCount];		//store array of input layer sizes
	for(unsigned m=0;m<layerCount;m++)
		layerSize[m]=in[m];
	defineSub();
}
//
void CNet::operator = (const CNet &rhs)
{
	clean();
	layerCount=rhs.layerCount;
	layerSize=new unsigned[layerCount];		//store array of input layer sizes
	for(unsigned m=0;m<layerCount;m++)
		layerSize[m]=rhs.layerSize[m];
	defineSub();
	for(unsigned m=0;m<weightTally;m++)
		weight[m]=rhs.weight[m];
	eta=rhs.eta;
	alpha=rhs.alpha;
};
void CNet::defineSub()
{
	neuron=new double*[layerCount];			//define neuron pointers for each layer
	for(unsigned m=1;m<(layerCount-1);m++)
	{
		neuron[m]=new double[layerSize[m]];//define a double for each neuron in all hidden layer
	}
//
	neuronError=new double*[layerCount];				//define error pointer for each layer
	for(unsigned m=0;m<layerCount;m++)
	{
		neuronError[m]=new double[layerSize[m]];//define etrror double's for each neuron in input and hidden layers
	}
//	

	pweight=new double**[layerCount];		//pweight 		3d array[to layer][to pos][from pos]
	deltaWeight=new double **[layerCount];  //deltaWeight	"	"			"
	weightTally=0;
	for(unsigned m=1;m<layerCount;m++)
	{
		pweight[m]=new double *[layerSize[m]];
		deltaWeight[m]=new double *[layerSize[m]];
		for(unsigned n=0;n<layerSize[m];n++)
		{
			deltaWeight[m][n]=	new double[layerSize[m-1]+1];
			for(unsigned o=0;o<(layerSize[m-1]+1);o++)
				deltaWeight[m][n][o]=0.0;

			weightTally+=layerSize[m-1]+1;
		}
	}
	weight=new double[weightTally];			//define 1 dimentional array for all weights
	weightTally=0;
	for(unsigned m=1;m<layerCount;m++)
	{
		for(unsigned n=0;n<layerSize[m];n++)
		{
			pweight[m][n]=weight+weightTally; //set pointers in pweights to point into weights
			weightTally+=layerSize[m-1]+1;	
		}
	}
	//the weight's are stored in a 1 dimensional array so they are stored consecutively in memory
	//pweight is set to reference the weights as a 3d array [to layer][to pos][from pos]
}


//layerCount is the total number of layers 1 input layer 'n' hidden and 1 out
//layer '0' is the input layer,
//therefore the output layer is layerCount-1
void CNet::setInput(const void *in)
{
	neuron[0]=(double *)in;			//layer '0' is the input layer, set this to an array outisde CNet
};
void CNet::setOutput(const void *out)
{
	neuron[layerCount-1]=(double *)out;	//define the output layer
};
void CNet::setTarget(const void *target)
{		
	t=(double*)target;					//define the target
};
void CNet::randomize(double a,double b)	//fill the weight array with random numbers a<=n<b
{

	for(unsigned m=1;m<layerCount;m++)
	{
		for(unsigned n=0;n<layerSize[m];n++)
		{
			for(unsigned o=0;o<layerSize[m-1]+1;o++)
			{
				pweight[m][n][o]=a+(b-a)*(rand()/double(RAND_MAX));
			}
		}
	}
}
void CNet::forward()
{
	for(unsigned m=1;m<layerCount;m++)				//m loops throught layer 1 to output layer
	{
		for(unsigned n=0;n<layerSize[m];n++)		//n loops through each neuron in layer m
		{

			double sum=pweight[m][n][layerSize[m-1]];	//start with bias weight
			for(unsigned o=0;o<layerSize[m-1];o++)
			{	
				sum+=pweight[m][n][o]*neuron[m-1][o];//sum up weighted neurons in previous layer
			}
			neuron[m][n]=sig(sum);					//once summing complete store sigmoid of weighted sum
		}
	}
};

double CNet::back()
{
	error=0.0;

													//Calculate net error and error
													//of each neuron in output layer
	unsigned m=layerCount-1;						//m=output layer
	for(unsigned n=0;n<layerSize[m];n++)			//n loops through all output neurons
	{
		double delta=t[n]-neuron[m][n];				//square of diffrence 
		error+=delta*delta;							//sum up total output error
		neuronError[m][n]=delta*dsigdx(neuron[m][n]);//store error for each output neuron
	}

	error/=(layerSize[m]);
	error=sqrt(error);								//calculate net error
	
	//hidden layer gradients
	for(unsigned m=layerCount-2;m>0;m--)			//loop through each hidden layer neuron
													//and calculate it's error
	{
		for(unsigned n=0;n<layerSize[m];n++)
		{
			double dow=0;
			for(unsigned o=0;o<layerSize[m+1];o++)
			{
				dow+=pweight[m+1][o][n] * neuronError[m+1][o];
			}
			neuronError[m][n]=dow*dsigdx(neuron[m][n]);
		}
	}
	//update weights									
	double oldDeltaWeight;							//use the errors calculated above to 
	double newDeltaWeight;							//adjust the weights
	double myEta=eta;
	
	for(unsigned m=layerCount-1;m>0;m--)
	{
		myEta=eta/(pow(2,layerCount-m-1));
		for(unsigned n=0;n<layerSize[m];n++)
		{
			for(unsigned o=0;o<layerSize[m-1];o++)
			{
				oldDeltaWeight=deltaWeight[m][n][o];
				newDeltaWeight=
					myEta 	* neuron[m-1][o]*neuronError[m][n] +
					alpha	* oldDeltaWeight;

				deltaWeight[m][n][o]=newDeltaWeight;
				pweight[m][n][o]+=newDeltaWeight;
			}
			unsigned o=layerSize[m-1];
			oldDeltaWeight=deltaWeight[m][n][o];
			newDeltaWeight=
				myEta	* 1 *neuronError[m][n]+
				alpha *oldDeltaWeight;
			deltaWeight[m][n][o]=newDeltaWeight;
			pweight[m][n][o]+=newDeltaWeight;
		}
	}
	return error;
}

void CNet::save(const char *name)
{
	std::ofstream f(name);
	if(f.is_open())
	{
		f<<layerCount<<" ";
		for(unsigned n=0;n<layerCount;n++)
			f<<layerSize[n]<<" ";
		f<<"\n";
		f<<eta<<" "<<alpha<<"\n";
		for(unsigned n=0;n<weightTally;n++)
			f<<weight[n]<<"\n";
	}
	f.close();
};
bool CNet::load(const char *name)
{
	std::ifstream f(name);

	if(!f.is_open())return 0;

		clean();
		f>>layerCount;
		layerSize=new unsigned[layerCount];		//store array of input layer sizes
		for(unsigned m=0;m<layerCount;m++)
			f>>layerSize[m];

		f>>eta;
		f>>alpha;

		defineSub();
		for(unsigned m=0;m<weightTally;m++)
			f>>weight[m];
		f.close();	
	return 1;
}
void CNet::info()
{
	printf("Network\n-------\n");
	printf("Layers:%d\n",layerCount);
	for(int n=0;n<layerCount;n++)
		printf("%d ",layerSize[n]);
	printf("weightTally=%d\n",weightTally);
}
