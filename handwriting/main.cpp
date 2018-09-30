//gcc -lglut -lGL -lGLU
#include <GL/freeglut.h>
#include <GL/gl.h>
#include "window.h"
#include "net.h"
#include "training.h"
#include "boolarray.h"
//
#define WINDOW_WIDTH 560
#define WINDOW_HEIGHT 400
//
unsigned window_width;
unsigned window_height;
bool train;
CNet net;
const unsigned netDef[]={28*28,200,10,0};
const char netName[]="net.txt";
double eta=0.02;
double alpha=0.5;
double *in;
double netOut[10];
double netTarget[10];

CWindow trainingWindow;
CWindow trainingInfoWindow;
CWindow trainingGraphWindow;
double trainingGrid[28*28];
CTraining trainingData;
unsigned trainingSample=0;
int trainingValue;
int netOutputValue;


CWindow editWindow;
double editGrid[28*28];


//
void reshape(int w,int h);
void key(unsigned char key,int x,int y);
void mouseClick(int button,int state,int x,int y);
void init();
void loop();
//
//void gridPlot();
void trainingInfo();
void paint(int x,int y,double intensity);
int trainNet();
//
int main(int argc,char *argv[])
{
	glutInit(&argc,argv);
	glutInitDisplayMode( GLUT_RGBA | GLUT_SINGLE );
	glutInitWindowSize( WINDOW_WIDTH , WINDOW_HEIGHT );
	glutCreateWindow("Handwritten Numbers");
	glutDisplayFunc(loop);
	glutIdleFunc(loop);
	glutKeyboardFunc(key);
	glutMouseFunc(mouseClick);
	glutReshapeFunc(reshape);
	init();
	glutMainLoop();
}
//
void reshape(int w,int h)
{
	window_width=w;
	window_height=h;
	trainingWindow.setWindowHeight(h);
	glClear(GL_COLOR_BUFFER_BIT);
	trainingWindow.clear(0.3,0.3,0.3);
	editWindow.clear(0.4,0.4,0.4);
	glFlush();
}
//
void key(unsigned char key,int x,int y)
{
	if(key=='q')
	{
		trainingSample=(trainingSample+59999)%60000;
		trainingValue=trainingData.read(trainingSample,trainingGrid);
	};
	if(key=='w')
	{
		trainingSample=(trainingSample+1)%60000;
		trainingValue=trainingData.read(trainingSample,trainingGrid);
	};
	if((key>='0')&&(key<='9'))paint(x,y,(key-'0')/9.0);
	if(key=='x'){net.save(netName);exit(0);};

	if(key=='t')train=!train;
}
//
void mouseClick(int button,int state,int x,int y)
{
};
//
void paint(int x,int y,double intensity)
{
	if(editWindow.isHover(x,y))
	{
		int dx=int((editWindow.screenToMouseX(x)-2)/8.0);
		int dy=27-int((editWindow.screenToMouseY(y)-2)/8.0);
		if((dx>=0)&&(dx<28)&&(dy>=0)&&(dy<28))
		{
			editGrid[dx+28*dy]=intensity;
		}
	}
};

void init()
{
	trainingWindow.create(10,10,225,225);
	trainingWindow.range(0,0,225,225);
	trainingWindow.setVisible(1);
	trainingWindow.adjust();

	trainingInfoWindow.create(10,330,225,70);
	trainingInfoWindow.range(0,0,225,70);
	trainingInfoWindow.setVisible(1);
	trainingInfoWindow.adjust();

	trainingGraphWindow.create(10,240,225,60);
	trainingGraphWindow.range(0,0,225,60);
	trainingGraphWindow.setVisible(1);
	trainingGraphWindow.adjust();

	editWindow.create(238,10,225,225);
	editWindow.range(0,0,225,225);
	editWindow.setVisible(1);
	editWindow.adjust();


	reshape(WINDOW_WIDTH,WINDOW_HEIGHT);
//	for(unsigned n=0;n<100;n++)
//		trainingGrid[rand()%(28*28)]=rand()/double(RAND_MAX);

	if(!net.load(netName))
	{
		net.define(netDef);
		net.randomize(-0.4,0.4);
	}
	net.setEtaAlpha(eta,alpha);
	net.setOutput(netOut);
	net.setTarget(netTarget);
	trainingValue=trainingData.read(trainingSample,trainingGrid);
	printf("Weight count %d\n",net.getWeightCount());

	train=0;
}
//
int correct=0;
int total=0;
int itt=0;

void loop()
{
	trainingInfoWindow.clear(0.3,0.3,0.3);
	trainingGraphWindow.clear(0.3,0.3,0.3);
	trainingInfo();
	for(int n=0;n<10;n++)
	{
		correct+=trainNet();
		total++;
	}
	if(total>=10000)
	{
		correct/=2;
		total=5000;
		itt++;
	};
	glFlush();
}
//
int trainNet()
{
	double grid[28*28];
	double out[10];
	double target[10];
	unsigned sample=rand()%60000;
	int value=trainingData.read(sample,grid);
	net.setInput(grid);
	net.setOutput(out);
	net.setTarget(target);
	for(int n=0;n<10;n++)
	{
		if(n==value)target[n]=1.0;
		else target[n]=0.0;
	}
	net.forward();
	int o=0;
	double outVal=out[o];
	for(int n=0;n<10;n++)
		if(out[n]>outVal){outVal=out[n];o=n;};
	if(o==value)return 1;
	if(train)net.back();

	return 0;
}
void trainingInfo()
{
	net.setInput(trainingGrid);
	net.setOutput(netOut);
	net.forward();

	netOutputValue=0;
	double netOutputLevel=netOut[0];
	for(unsigned n=0;n<10;n++)
	{	
		if(netOut[n]>netOutputLevel)
		{
			netOutputLevel=netOut[n];
			netOutputValue=n;

		}
		trainingGraphWindow.print(n*16,40-int(netOut[n]*35.0),(char*)"%d",n);
	}
	trainingGraphWindow.print(netOutputValue*16,0,(char*)"*");

	trainingInfoWindow.print(0,0,(char*)"Training sample (%d) ",trainingSample);
	trainingInfoWindow.print((char*)"Training value = %d  ",trainingValue);
	trainingInfoWindow.print((char*)"Net Output =%d  ",netOutputValue);
	trainingInfoWindow.print((char*)"Average =%f ",correct/double(total));

	for(unsigned y=0;y<28;y++)
	{
		for(unsigned x=0;x<28;x++)
		{
			if(trainingValue==netOutputValue)
				trainingWindow.dot(4+x*8,220-y*8,7,	0,trainingGrid[x+28*y],0);
			else
				trainingWindow.dot(4+x*8,220-y*8,7,	trainingGrid[x+28*y],0,0);
		}
	}

	for(unsigned y=0;y<28;y++)
	{
		for(unsigned x=0;x<28;x++)
		{
			editWindow.dot(4+x*8,220-y*8,7,	0,editGrid[x+28*y],0);
		}
	}
}

