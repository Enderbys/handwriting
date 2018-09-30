#ifndef _WINDOWS
#define _WINDOWS
#include <stdio.h>
#include <stdarg.h>
#include <string>
#include <math.h>
#include <GL/freeglut.h>
#include <GL/gl.h>

class CWindow
{
private:
	bool visible;
	static int windowHeight;
	int cursory;
public:
	int posx;
	int posy;
	int width;
	int height;
	double xmin,ymin,xmax,ymax;
	CWindow();
	void create(int x,int y,int w,int h);
	void range(double minx,double miny,double maxx,double maxy);
	void setWindowHeight(int h){windowHeight=h;};
	void setVisible(bool v);
	bool isVisible(){return visible;};
	void adjust();
	void clear(double r,double g,double b);
	void axis(double r,double g,double b);
	void print(int x, int y, char *string, ...);
	void print(char *string, ...);
	void dot(double x,double y,double size,double r,double g,double b);
	void line(double x1,double y1,double x2,double y2,double r,double g,double b);
	double xStep();
	double yStep();
	double screenToMouseX(int x);
	double screenToMouseY(int y);
	double ratio();
	void function(double(*f)(double),double r,double g,double b);
	bool isHover(int x,int y);
};
#endif

