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
		int type;//表示点的类型是端点还是中间的点 ,1表示是扩展的点。 
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
		int flag;//flag == 0 表示斜率不存在 
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
//边和边的相互对应关系 ，并希望是根据到index边的距离有序排列 
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
//邻接矩阵
struct matrix{
	Point A;
	int flag;//0表示还未遍历，1表示遍历过了 
	vector<int> con;
}; 
#endif 
