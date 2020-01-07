#ifndef cor
#define cor
#include<iostream>
//#include"../struct/struct.h"
#include"../line/line.h"
#include"../scanline/scanline.h"
using namespace std;
//�������ߺ���
bool shadLine(LinePara AB,LinePara CD,LinePara &LP);
//���غ�����ĺ���,A,B,C,D�������� 
void getRegionC(LinePara LP1,LinePara LP2,Over &over)
{
	Point A = LP1.Line.front();
	Point B = LP1.Line.back();
	Point C = LP2.Line.front();
	Point D = LP2.Line.back();
	LinePara AV1;
	LinePara BV1;
	LinePara CV2;
	LinePara DV2;
	getVerticalLine(A,LP1,AV1);
	getVerticalLine(B,LP1,BV1);
	getVerticalLine(C,LP2,CV2);
	getVerticalLine(D,LP2,DV2);
	Point C1;
	Point C2;
	Point C3;
	Point C4;
	//BV1.PrL();
	getCross(AV1,CV2,C1);
	getCross(AV1,DV2,C2);
	getCross(BV1,CV2,C4);
	getCross(BV1,DV2,C3);
	over.region.push_back(C1);
	over.region.push_back(C2);
	over.region.push_back(C3);
	over.region.push_back(C4);
} 
void getRegionP(LinePara AB,LinePara CD,Over &over)
{
	LinePara LP1;
	LinePara LP2;
	shadLine(AB,CD,LP1);
	shadLine(CD,AB,LP2);
	over.region.push_back(LP1.Line.front());
	over.region.push_back(LP1.Line.back());
	over.region.push_back(LP2.Line.back());
	over.region.push_back(LP2.Line.front());	 
}
void getRegion(LinePara AB,LinePara CD,Over &over)
{
	if(Para(AB,CD))
	{
		getRegionP(AB,CD,over);
	}
	else
	{
		getRegionP(AB,CD,over);
	}
}
void getMiddPara(LinePara LP1,LinePara LP2,LinePara &LP)
{
	if(Para(LP1,LP2))
	{
		getCenterLine(LP1,LP2,LP);
	}
	else
	{
		getAngleLine(LP1,LP2,LP);
	}
}
//û���غϷ���0���з���1 
bool RegionCross(LinePara &LP,Over over)
{
	int count=0;
	LinePara LP1;
	Point C;
	vector<Point> L;
	for(int i=0;i<over.region.size();i++)
	{
		getLinePara(over.region[i],over.region[(i+1)%over.region.size()],LP1);
		if(getCrossL(LP,LP1,C))
		{
			L.push_back(C);
			count ++;
		};
		
	}
	//cout<<"---"<<count<<endl;
	if(count==2)
	{
		getLinePara(L.front(),L.back(),LP1);
		LP=LP1;
		return 1;	
	} 
	return 0;
}
//����1��ͶӰ��һ������,�����߶�AB���߶�CDͶӰ���غ��߶� ,��ͬʱ���㻥��ͶӰ�ĳ��Ⱦ��������߳�����С���Ǹ���0.1��ʱ����Ϊ���غ����� 
bool shadLine(LinePara AB,LinePara CD,LinePara &LP)
{
	LP.Line.clear();
	Point A = AB.Line.front();
	Point B = AB.Line.back();
	Point C = CD.Line.front();
	Point D = CD.Line.back();
	Point T1;
	Point T2;
	Point tmp;
	bool flag1 = shadow(A,CD,T1);
	bool flag2 = shadow(B,CD,T2);
	if(!flag1&&!flag2)
	{
		if(T1<C&&T2>D)
		{
			LP = CD;
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else if(flag1&&flag2)
	{
		getLinePara(T1,T2,LP);
		return 1;
	}
	else if(flag1&&!flag2)
	{
		getLinePara(T1,D,LP);
		return 1;
	}
	else if(!flag1&&flag2)
	{
		getLinePara(T2,C,LP);
		return 1;
	};
	
} 
double thresholdlen =0.3;
//ƽ�л�����ͶӰ���غ������н��� 
bool SameGroup(LinePara LP1,LinePara LP2)
{
	LinePara L1;
	LinePara L2;
	Point A =LP1.Line.front();
	Point B =LP1.Line.back();
	Point C =LP2.Line.front();
	Point D =LP2.Line.back(); 
	if(!Para(LP1,LP2))
		return 0;
	if(shadLine(LP1,LP2,L1) && shadLine(LP2,LP1,L2))
	{
		if(L1.length<=thresholdlen&&L2.length<=thresholdlen)
		{
			return 0;
		}
		Over over;
		LinePara LP;
		getRegion(LP1,LP2,over);
		getMiddPara(LP1,LP2,LP);
		if(RegionCross(LP,over))
		{
			//if(inPoly(LP.Line.front(),LP.Line.back(),))
			return 1;
		}
		
	}
	return 0;
		//if(Para(LP1.Line.front(),LP1.Line.back(),LP2.Line.front(),LP2.Line.back()))
}
//��ƽ�е����������ƽ�еĻ������ڿ��ǣ�ά��һ��������Ϊn������
bool SameGroupA(LinePara LP1,LinePara LP2)
{
	LinePara L1;
	LinePara L2;
	Point A =LP1.Line.front();
	Point B =LP1.Line.back();
	Point C =LP2.Line.front();
	Point D =LP2.Line.back(); 
	if(shadLine(LP1,LP2,L1) && shadLine(LP2,LP1,L2))
	{
		if(L1.length<=thresholdlen&&L2.length<=thresholdlen)
		{
			return 0;
		}
		Over over;
		LinePara LP;
		getRegion(LP1,LP2,over);
		getMiddPara(LP1,LP2,LP);
		if(RegionCross(LP,over))
		{
			//if(inPoly(LP.Line.front(),LP.Line.back(),))
			return 1;
		}
		
	}
	return 0;
}  
//����2��ƽ�У�line.h�Ѿ�ʵ�֣���ȥ 

//����3��Ԥ�����������ڶ�����ڲ���scanlineʵ��
bool PinPoly(Point A,double k,int minx,int miny,int maxx,int maxy,unsigned long int arr[][weight]){
	int i= 1 ;
	int y = int((A.y - miny)/k);
	int x = int((A.x - minx)/k);
	if(get(y,x,arr)== 0&&get(y-i,x,arr)==0&&get(y,x+i,arr)==0&&get(y,x-i,arr)==0)
	//if(get(y,x,arr)==0&&get(y-i,x,arr)==0)
		return 0;
	return 1;
}
bool inPoly(Point A,Point B,double k,int minx,int miny,int maxx,int maxy,unsigned long int arr[][weight]){
	if( PinPoly(A,k,minx,miny,maxx,maxy,arr) && PinPoly(B,k,minx,miny,maxx,maxy,arr))
		return 1;
	return 0;
};
// 
#endif

