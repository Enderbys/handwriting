#ifndef _NET
#define _NET
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <math.h>
class CNet
{
private:
	double error;
//	double recentAverageError;
//	double recentAverageSmoothingFactor;
	unsigned layerCount;			//number of layers in network
	unsigned *layerSize;			//array of layer sizes
	unsigned weightTally;			//number of weights
	double **neuron;				//neuron array[layer][neurons in layer]
	double **neuronError;			//error  array[layer][neurons in layer]
	double *weight;					//pointer to 1d array of all weights 
	double ***pweight;				//pointers into weights
									//so pweights[layer][pos in layer][pos in previous layer]
	double ***deltaWeight;			//same dimensions as pweight
	double *t;						//target array
	static double sig(double x){return 1/(1+pow(2.71828,-x));};
	static double dsigdx(double x){return sig(x)*(1-sig(x));};
	double eta;
	double alpha;
public:
	CNet();
	void clean();
	void define(const unsigned *in);
	void defineSub();
	void setEtaAlpha(double e,double a){eta=e;alpha=a;};
	void setInput(const void *in);
	void setOutput(const void *out);
	void setTarget(const void *target);
	void randomize(double a,double b);
	void forward();
	double back();
	void operator = (const CNet &rhs);
	~CNet();
	//
	int getLayerCount(){return layerCount;};
	int getLayerSize(int m){return layerSize[m];};
	double getNeuron(int m,int n){return neuron[m][n];};
	double getWeight(int m,int n,int o){return pweight[m][n][o];};
	double* getWeight(int n){return &weight[n];};
	unsigned getWeightCount(){return weightTally;};
	void save(const char *name);
	bool load(const char *name);
	void info();
};
#endif
