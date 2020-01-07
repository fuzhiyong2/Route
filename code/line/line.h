#ifndef line
#define line
#include"../struct/struct.h"
#include<iostream>
#include<math.h>
using namespace std;
double thresholdPara = 1e-2;
void getVerticalLine(Point A, LinePara LB, LinePara & LP); 
double Getlength(Point start,Point end){
	return sqrt((start.x-end.x)*(start.x-end.x) + (start.y - end.y)*(start.y - end.y));
}
//由点获取线段参数 
double getLinePara(Point &A,Point &B,LinePara &LP){
	LP.Line.clear();
	double threshold = 1e3;
	double m = 0;
	double x2 = B.x;
	double x1 = A.x;
	double y2 =B.y;
	double y1 = A.y;
	// 计算分子  
	m = x2 - x1;
	//这里的k表示无穷大 
	if (0 == m)
	{
		LP.k = threshold;
		LP.flag = 0;
		LP.b = y1 - LP.k * x1;
	}
	else
	{
		LP.k = (y2 - y1) / (x2 - x1);
		LP.b = y1 - LP.k * x1;
		LP.flag = 1;
		if(fabs(LP.k) >= threshold)
		{
			LP.flag  = 0;
			LP.k = threshold;	
		}
	}
	LP.Line.push_back(A);
	LP.Line.push_back(B);
	LP.Line.sort();
	LP.length = Getlength(A,B);
	return LP.length; 
}
//判断两条线段是否平行 
bool Para(LinePara E1,LinePara E2){
//	E1.PrL();
//	E2.PrL();
//	if(E1.flag==0&&E2.flag!=0)
//		return 0;
//	if(E1.flag==1&&E2.flag==0)
//		return 0;
	LinePara E3,E4;
//	void getVerticalLine(Point A, LinePara LB, LinePara & LP);
	getVerticalLine(E1.Line.front(),E1,E3);
	getVerticalLine(E2.Line.front(),E2,E4);
	if((E3.flag==0&&E4.flag==0) || fabs(E3.k-E4.k)<thresholdPara||fabs((E3.k/E4.k)-1)<thresholdPara)
		return 1;
	if((E1.flag==0&&E2.flag==0) || fabs(E1.k-E2.k)<thresholdPara||fabs((E1.k/E2.k)-1)<thresholdPara)
		return 1;
	return 0;
}
//求两条直线的交点,0表示没用交点 
bool getCross(LinePara E1,LinePara E2,Point &Cross){
	Point A=E1.Line.front();
	Point C=E2.Line.front();
	if(Para(E1,E2))
	{
		return 0;
	}
	else
	{
		if(E1.flag!=0&&E2.flag!=0)
		{
			if(E1.k==0)
			{
				Cross.y = A.y;
				Cross.x = (Cross.y - E2.b)/E2.k;
			}
			else
			{
				double z = E2.k/E1.k;
				Cross.y = (E2.b-z*E1.b)/(1-z);
				Cross.x = (E1.b-E2.b)/(E2.k - E1.k);
			}
	    return 1;
		}
		else
		{
			//E1垂直 
			if(E1.flag == 0)
			{
				Cross.x = A.x;
				Cross.y = E2.k*Cross.x + E2.b;
			} 
			else
			{
			//E2垂直 
				Cross.x = C.x;
				Cross.y = E1.k*Cross.x + E1.b;
			}
		}
		return 1;
	} 
} 
//点在线段上,有问题的函数 
bool PinEdge(LinePara LP,Point C)
{
	if(LP.flag==0)
	{
		if(LP.Line.front().y<=C.y && LP.Line.back().y>=C.y)
			return 1;
	}
	else
	{
		if(LP.Line.front().x<=C.x && LP.Line.back().x>=C.x)
			return 1;
	}
	return 0;
} 
//求直线AB和线段CD的交点,0表示没用交点 
bool getCrossL(LinePara AB,LinePara CD,Point &Cross)
{
	if(getCross(AB,CD,Cross))
	{
		if(PinEdge(CD,Cross))
			return 1;
	}
	return 0;
}
//求两条直线的角平分线或者中线 OK
bool getAngleLine(LinePara LB,LinePara LP,LinePara &LA)
{
	//求LB,LP的交点（LB,LP不平行）
	LA.Line.clear();
	Point C;
	if(!getCross(LB,LP,C)) 
	{
		return 0;
	}
	//再求角平分线过原点的平行线的斜率k
	Point e1;
	Point e2;
	Point O(0,0);
	if(LB.flag==0)
	{
		e1.y = 1;
		e1.x = 0;
	}
	else
	{
		e1.x = 1;
		e1.y = LB.k*e1.x;
		double length=Getlength(O,e1);
		e1.x = e1.x/length;
		e1.y = e1.y/length; 
	}
	if(LP.flag==0)
	{
		e2.y = 1;
		e2.x = 0;
	}
	else
	{
		e2.x = 1;
		e2.y = LP.k*e2.x;
		double length=Getlength(O,e2);
		e2.x = e2.x/length;
		e2.y = e2.y/length; 
	}
	Point next(C.x+e1.x+e2.x,C.y+e1.y+e2.y);
	getLinePara(C,next,LA);
   	return 1; 
}
Point center(Point A,Point B)
{
	Point C((A.x+B.x)/2,(A.y+B.y)/2);
	return C;
}
//求两条平行线的中心线 
bool getCenterLine(LinePara LA,LinePara LB,LinePara &LP)
{
		LP.Line.clear();
		if(!Para(LA,LB))
			return 0;
		Point A = center(LA.Line.front(),LB.Line.front());
		Point B = center(LA.Line.back(),LB.Line.back());
		getLinePara(A,B,LP);
		return 1;
}
//1表示没用问题 ,检查是否是线段。 
bool checkLP(LinePara &LP)
{
	if(LP.Line.size()>1)
		return 1;
	Point A = LP.Line.front();
	if(LP.flag==0)
	{
		Point B(A.x,A.y+1);
		LP.Line.push_back(B);
	}
	else
	{
		Point B;
		B.x = A.x+1;
		B.y = B.x*LP.k+LP.b;
		LP.Line.push_back(B);
	}	
} 
//过一点求另外一条变得垂线,LP为返回得边得参数 
void getVerticalLine(Point A, LinePara LB, LinePara & LP)
{
	double big=4e4;
	double x1 = A.x;
	double y1 = A.y; 
	LP.flag =1;
	if(LB.flag == 0 )
		LP.k=0;
	else if(LB.k==0)
	{
		LP.k=big;
		LP.flag = 0;
	}
	else
		LP.k=-1/LB.k;
	if(fabs(LP.k)>big)
		LP.flag = 0;
	LP.b=y1-LP.k*x1;
	LP.Line.push_back(A);
	checkLP(LP);
}
//一个点在一线段上的投影
bool shadow(Point A,LinePara LP,Point &Cro)
{
	if(!checkLP(LP))
	{
		return 0;
	}
	LinePara V;
	Cro = A;
	Point B=LP.Line.front(); 
	Point C=LP.Line.back();
	if(LP.flag == 0)
	{
		Cro.x = center(B,C).x;
		if(A.y>=B.y&&A.y<=C.y)
		{
			return 1;
		}
		else
			return 0;
	}
	else
	{
		getVerticalLine(Cro,LP,V);
		if(getCrossL(V,LP,Cro))
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
}
//求点到线段的最短距离
void P2Line(Point A,LinePara BC,double &dis)
{
	Point Cro;
	if(shadow(A,BC,Cro))
	{
		dis = Getlength(A,Cro);
	}
	else
	{
		Point B = BC.Line.front();
		Point C = BC.Line.back();
		double lengthB=Getlength(A,B);
		double lengthC=Getlength(A,C);
		dis=lengthB<lengthC?lengthB:lengthC; 
	}
}
//求点到直线的距离
void getdis(Point A,LinePara BC,double &dis)
{
	Point C = A;
	if(BC.flag == 0)
	{
		dis = fabs(A.x - BC.Line.front().x);	
	}
	else
	{
		//点到直线的距离公式
		double k = BC.k;
		double b = BC.b;
		dis = fabs(((-k)*C.x+C.y-b)/sqrt(k*k+1)); 
	}
} 
//求两线段的最短距离 
void distance(LinePara AB,LinePara CD,double &dis)
{
	Point A = AB.Line.front();
	Point B = AB.Line.back();
	Point C = CD.Line.front();
	Point D = CD.Line.back();
	double length[4];
	P2Line(A,CD,length[0]);
	P2Line(B,CD,length[1]);
	P2Line(C,AB,length[2]);
	P2Line(D,AB,length[3]);
	dis = length[0];
	for(int i=0;i<4;i++)
	{
		if(length[i]<dis)
			dis = length[i];
	}
}
//取两条线段，距离最近的两个端点
void EPoint(LinePara AB,LinePara CD,int &A,int &C)
{
	Point A1 = AB.Line.front();
	Point B1 = AB.Line.back();
	Point C1 = CD.Line.front();
	Point D1 = CD.Line.back();
	double a [4]; 
	a[0] = Getlength(A1,C1); 
	a[1] = Getlength(A1,D1);
	a[2] = Getlength(B1,C1);
	a[3] = Getlength(B1,D1);
	double min = a[0];
	int m=0;
	for(int i=0;i<4;i++)
	{
		if(a[i]<min)
		{
			m=i;
			min = a[i];
		}
	}
	if(m==0)
	{
		A = 0;
		C = 0;
	}
	if(m==1)
	{
		A=0;
		C=1;
	}
	if(m==2)
	{
		A=1;
		C=0; 
	}
	if(m==3)
	{
		A=1;
		C=1; 
	}	
} 
#endif
