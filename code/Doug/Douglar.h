#ifndef DOUGLAGPEUCKER
#define DOUGLAGPEUCKER
#include<iostream>
#include <fstream>
#include<list>
#include<vector>
#include<string> 
#include<math.h>
#include"../struct/struct.h"
using namespace std;
double threshold = 0.1;
void distance(Point A,Point B,Point C,double *c)
{
	double k;
	double b;
	double flag;
	if(A.x==B.x || fabs((B.y-A.y)/(A.x-B.x))>4e23)
	{
		flag=0;
		(*c)=fabs(A.x - C.x);
	}
	else
	{
		flag=1;
		k = (B.y-A.y)/(A.x-B.x);
		b = A.y - A.x*k;
		(*c) = fabs(((-k)*C.x+C.y-b)/sqrt(k*k+1));
	}
}
int findMax(int i,int j,vector<Point> *input,double *length)
{
	int max=i+1;
	for(int c=i+1;c<j;c++)
	{
		double b;
		distance((*input)[i],(*input)[j],(*input)[c],&b);
		if(b>*length)
		{
			*length = b;
			max = c;
		}
		//cout<<b<<endl;
	}
	//cout<<*length<<endl;
	return max;
}
void Douglar(list<Point> *Poly,list<Point>::iterator begin,list<Point>::iterator end,int i,int j,vector<Point> *input)
{
	//找的最远的点
	double length=0;
	int max = findMax(i,j,input,&length);
	if(length>threshold)
	{
	//cout<<length<<endl;
	//插入
	list<Point>::iterator center;
	center=(*Poly).insert(end,(*input)[max]);
	//左边迭代一次
	Douglar(Poly,begin,center,i,max,input);
	Douglar(Poly,center,end,max,j,input);
	//右边迭代一次
	}
		
}

#endif
