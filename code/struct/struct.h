#ifndef _TEST_ISTREAM
#define _TEST_ISTREAM 1
#include<iostream>
#include<list>
#include<math.h>
#include<vector>
using namespace std;
class Point{
	public:
		double x;
		double y;
		int type;//��ʾ��������Ƕ˵㻹���м�ĵ� ,1��ʾ����չ�ĵ㡣 
		Point(){
			this->type = 0; 
		};
		Point(double x,double y)
		{
			this->x = x;
			this->y = y;
			this->type = 0; 
		}
		bool operator==(const Point &f)
		{
			if (sqrt((f.x-this->x)*(f.x-this->x)+(f.y - this->y)*(f.y - this->y))<=0.01){
			return true;
			}
			return false;
		}
		bool operator< (const Point&f){
		int flag=1;
		if(f.x==this->x)
			flag = 0;
		else
		{
			if(fabs((f.y-this->y)/(f.x-this->x))>=4e4)
				flag = 0;
		}
		if(flag == 1)
		{
			if (f.x < this->x){
				return false;
			}
			else if(f.x==this->x)
			{
				if(f.y<this->y)
					return false;
			}
			return true;
		}
		else
		{
			if(f.y<this->y)
				return false;
			else
				return true;
		}
		
		}
		bool operator> (const Point&f){
		if (f.x < this->x){
			return true;
		}
		else if(f.x==this->x)
		{
			if(f.y<this->y)
				return true;
		}
		return false;
		}
		void PrD()
		{
			cout<<"Point A("<<this->x<<","<<this->y<<")"<<endl;	
		} 
};
class Polygon{
public:
	int size;
	Point minp;
	Point maxp;
	vector<Point> coordinate;
};
typedef struct XET{
	double x;
	double dx, ymax;
	XET* next;
}AET, NET;
class LinePara{
	public:
		double k;
		double b;
		int flag;//flag == 0 ��ʾб�ʲ����� 
		list<Point> Line;
		double length; 
		void PrL()
		{
			cout<<"k = "<<k<<" b= "<<b<<" flag= "<<flag<<endl;
		}
};
class Over{
	public:
		vector<Point> region;
		void PrO()
		{
			for(int i=0;i<region.size();i++)
				region[i].PrD();
		}
};
class PolyPara{
	public:
		double k;
		int minx;
		int maxx;
		int miny;
		int maxy;
		int offsetx;
		int offsety;
		void PrPar()
		{
			cout<<"k = "<<k<<endl;
			cout<<"minx = "<<minx<<endl;
			cout<<"maxx = "<<maxx<<endl;
			cout<<"miny = "<<miny<<endl;
			cout<<"maxy = "<<maxy<<endl;
			cout<<"offsetx = "<<offsetx<<endl;
			cout<<"offsety = "<<offsety<<endl;
		}
};
//�ߺͱߵ��໥��Ӧ��ϵ ����ϣ���Ǹ��ݵ�index�ߵľ����������� 
class Relate{
	public:
		int size;
		LinePara index;
		vector<LinePara> relate;
		void show(){
			cout<<"----------"<<endl;
			index.PrL();
			for(int i=0;i<relate.size();i++)
				relate[i].PrL();
			cout<<endl;
		}
};
//�ڽӾ���
struct matrix{
	Point A;
	int flag;//0��ʾ��δ������1��ʾ�������� 
	vector<int> con;
}; 
#endif 
