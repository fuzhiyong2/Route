//#include "stdafx.h"
#include <iostream>
#include <list>
#include <iomanip>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <vector>
#include <cstdio>
#include<math.h>
//#include <ctchar>
#define INT_MAX 2147483647
using namespace std;
/*It is used in Scanline , Point*/
class Point{
public:
	int x;
	int y;
};
/*It is used in Scanline*/
class Polygon{
public:
	int size;
	Point minp;
	Point maxp;
	vector<Point> coordinate;
};
/*It is used in Scanline*/
typedef struct XET{
	double x;
	double dx, ymax;
	XET* next;
}AET, NET;
/*It is used in Get Center line And connect Centerline*/
class DPoint{ 
public:
	double x;
	double y;
};
/* Mask is used to mark the edges of the same group and group them*/
struct Mask{
Mask *next;//Points to the same set of edges
int i;//Indicates the serial number of another edge in the same group
double dis;//Distance to corresponding side
};
/*Segment data structure¡£Flag = = 0 indicates that the slope does not exist */
struct DEdge{
DPoint A; //X of point A is less than point B 
DPoint B;
bool flag;
double length;
double k = INT_MAX; 
Mask *next;
};
/* MiddLine structure used to Dmiddle */
struct MiddLine{
	DPoint A;
	DPoint B;
	int i,j;//It is used to record the center line of which two sides
};
/* It is used for linePara Structure*/
struct mergeNode{
	mergeNode * next;
	int i;//Indicates the serial number of another edge Need to merge 
	double length;
};
/* It is used for linePara Structure, record the Point in LinePara */
struct extendNode{
	DPoint C;
	int type;//0 is the original starting point, - 1 is the extended point, 2 is the middle cutting point, 3 is the original end point
	extendNode *next; 
};
/* MiddLine structure used to MiddLP*/
struct LinePara{
	double k;
	double b;
	int flag;//flag = 0  mean vertical
	mergeNode * next; 
	extendNode *down;
	double length;
};
/* It is used for matrix*/
struct mask{
	int i;//Represents the position in Midd vector
	mask *next; 
 };
/* It is used for Midd vector*/
struct matrix{
	DPoint A;
	int flag;//0 means not traversed, 1 means traversed
	mask *next;
};  
//-------------------------Global variables used in the scanline section---------------- 
Polygon polygon;
const int weight=2500;
const int height = 5000;
unsigned long int arr[height][weight]={0};//Bitmap stores scan line scan results
string inFile = "./";//input file path
string OutFile = "./";//output file path
/* Record data range for calculation */
int minx = INT_MAX;
int miny = INT_MAX;
int maxx = -INT_MAX;
int maxy = -INT_MAX; 
double k;//Polygon reduction multiple
//---------------------------Global variables used in the get Centerline section---------------------
double thresholdK= 1e-1;//If the absolute value of the difference between slopes is less than this range, it is regarded as parallel . which is used in sameK function 
double thresholdPara = 3e-3;//If the absolute value of the difference between slopes is less than this range, it is regarded as parallel . which is used in Para function
double thresholdOverTmp = 5e-1;//If the coincident part of two parallel lines is less than this value, it is deemed that there is no coincident part. It is used when grouping divide.
double thresholdOverTmp1 = 2e-1;//If the coincident part of two parallel lines is less than this value, it is deemed that there is no coincident part. It is used when grouping divide.
double thresholdOver = 1e-1;//If the coincident part of two parallel lines is less than this value, it is deemed that there is no coincident part. It is used in function conin.
double thresholdZ = 1e3;//Lines with a slope greater than 2000 are considered perpendicular
double thresholdD = 0.5;//Threshold for merging the same set of center lines
double thresholdDis = 0.1;// When extending, it is used to find the length that the extension point can fall within the line segment. It is recommended to be the same as the threshold value within equal.
double thresholdE = 0.5;//Threshold of same Point
double thresholdEx = 10;//the max length of extend center line 
double thresholdM = 1;//when the length fo center line is short than thresholdM ,delete it
vector<DPoint> Dpoin;//the set of point in Polygon
vector<DEdge> Dpoly;//the set of boundary in Polygon
vector<MiddLine> Dmiddle; //the set of center line of Polygon
vector<LinePara> MiddLP;//the set of center line fo Polygon which is used in connect center line
vector<matrix> Midd;//the set of Point in center line which is used to switch center line to Point
bool staEndFlag = 0;//record  start and end edge
int start=0;//record the number of start edge
int End = 0;//record the number of end edge
/*  function */

/*  Get the distance between two points  */
double Getlength(DPoint start,DPoint end); 

void merge();//Merge the same set of edges, update the down in MiddLP;
bool equal(DPoint &A,DPoint &B);//Judge whether two points are the same (the difference between X and Y is less than threshold)
bool Para(LinePara E1,LinePara E2);//Judge whether line 1 and line 2 are parallel
double getLinePara(DPoint &A,DPoint &B,LinePara &LP);//Return the length of line AB, and get the parameter LP structure of line AB
bool Isgroup(LinePara &LP,double length,int j);//Determine whether it is the same group
bool conin(LinePara &LP,int i,mergeNode *p,double length);//0 indicates that two parallel lines can be merged into one, LP, I indicates the current parallel line, and P points to I of the line segment to be merged 
void getMiddlLP();//Generate a middlelp vector
bool getCross(LinePara &E1,LinePara &E2,DPoint &Cross);//We get the intersection of E1 and E2. Return 1 means two lines are parallel
void LP2DEdge(DEdge &E1,LinePara &LP);//
void extend();//Extend midline
void insert(int i,DPoint max);//After LP [i]. Down, insert the extended point
void deletePoint();//Delete the point with the same value (equal). If the later point is the same as the former, delete the later point
void Gematrix();//Generate the vector of midd, and modify the type value in middlp down
void swap(double &min,double &max);//sort min and max 
void Updatmatrix();//Update the next value in Midd vector;
void GeFinalFile();//Write result to OutPut File
int matrix2File(int i,FILE *fp);//Traversing matrxi ,turn center line to Point
//-----------------------the function in getMiddleLine -----------------------
void DPoint2DEdge();//get the Dpoly vector;
	void divide();//Group function, update the vector structure of dpoly, and use mindis [] to remove some stray edges
bool SameK(DEdge &E1,DEdge &E2);//Judge whether two line segments are parallel 
DPoint MiddlePoint(DPoint &A,DPoint &B);//Find the midpoint of two points
void shadow(DPoint &CT,double k,double b);//Find the projection of a point on a non vertical line 
bool overlap(DEdge E1,DEdge E2);//Judge whether there is overlap between line AB and CD; I indicates the sequence number of line AB in dpoly, and 0 indicates no overlap
bool PinPoly(DPoint A);//Judge whether point a is in the polygon. The internal parameter I represents the accuracy and can control the size
bool inPoly(DPoint A,DPoint B);// Judge whether line a and B are in the polygon (that is, judge whether two endpoints are in the polygon)
bool MiddleLine(DEdge &E1,DEdge &E2,int i,int j);//Pre finding the center line of two sides
double getdis(double k,double b,DPoint &A);//Distance from point to a line that is not vertical
void distinct(DEdge &E1,DEdge &E2,double &dis);//Find the distance between any two parallel lines
void getMiddleLine();//Get the center line which store in a vector of middle
double getNewDis(Mask *p,Mask *q);//Merge parallel lines < threshold D in the same group to generate a function of the new distance. P, Q points to the first parallel line and the last parallel line
void getParaLine(double &k,double &b,double dis);//Obtain a parallel line (direction down) parallel to line k, B and distance dis
void getLine(Mask *p,Mask *q,MiddLine &tmp,int i);//generate the center line
void add(int set);//Add a line parallel to the two edges around the start edge at the position of the start edge ,as the centerline
void InvokeGetMiddleLine();//Call to find the center line function
void free();//Free dpoly dynamically requested memory 
//-------------------------------------function in scanline sector-------------------------------------------------------------- 
void getK(double &k); // K is the global variable, indicating the reduction factor of ploygon
void set(int i,int j);//Store the value of coordinate (J, I) in bitmap
int get(int i,int j);//Get the value of coordinate (J, I) in bitmap
void txtToPolygon(double k,int offsetx,int offsety);//The input file is converted into the data structure of polygon, which is a structure used in scan line algorithm
void GetOrigion(int &MinX,int &MinY,int &MaxX,int &MaxY);/* Get the maximum and minimum values of the given points*/
/*
Scanline algorithm. Offsetx indicates the offset of arr, regardless. The default value is 0. 
Refrain forms a specific range of canvas, which is an absolute range, followed by polygons. Arr represents the returned array.
*/
void PolyScan(int &offsetx, int &offsety, int &refrain_minx, int &refrain_maxx, int &refrain_miny, int &refrain_maxy, int &MinY, int &MaxY,unsigned long int arr[][weight], Polygon &poly);
/*Call scanline to generate an array of scanlines*/
void invokeScanline();
//-------------------------------------function in scanline sector-------------------------------------------------------------- 
void PolyScan(int &offsetx, int &offsety, int &refrain_minx, int &refrain_maxx, int &refrain_miny, int &refrain_maxy, int &MinY, int &MaxY,unsigned long int arr[][weight], Polygon &poly){
	int i;
	int crosspoint1, crosspoint2;
	AET *pAET = new AET;
	pAET->next = nullptr; 
	NET *pNET[MaxY-MinY+1]; 
	for(i=MinY; i<=MaxY; i++){
		pNET[i-MinY] = new NET;
		pNET[i-MinY]->next = nullptr;
	}
	//Generate static edge and fill the static edge
	int count=0;
	for(i=MinY; i<=MaxY; i++){
		for(int j=0; j<poly.size; j++){
			if(poly.coordinate[j].y == i){
				if(poly.coordinate[(j-1+poly.size)%poly.size].y > poly.coordinate[j].y){
					NET *p = new NET;
					p->x = poly.coordinate[j].x;
					p->ymax = poly.coordinate[(j-1+poly.size)%poly.size].y;
					p->dx = (poly.coordinate[(j-1+poly.size)%poly.size].x - poly.coordinate[j].x) * 1.0f / (poly.coordinate[(j-1+poly.size)%poly.size].y - poly.coordinate[j].y);
					p->next = pNET[i-MinY]->next;
					pNET[i-MinY]->next = p;
					count ++;
				}
				if(poly.coordinate[(j+1+poly.size)%poly.size].y > poly.coordinate[j].y){
					NET *p = new NET;
					p->x = poly.coordinate[j].x;
					p->ymax = poly.coordinate[(j+1+poly.size)%poly.size].y;
					p->dx = (poly.coordinate[(j+1+poly.size)%poly.size].x - poly.coordinate[j].x) * 1.0f / (poly.coordinate[(j+1+poly.size)%poly.size].y - poly.coordinate[j].y);
					p->next = pNET[i-MinY]->next;
					pNET[i-MinY]->next = p;
					count ++;
				}
			}
		}
	}
	for(i=MinY; i<MaxY; i++){
		//Peat dynamic table peat is initially null
		//Update the x value of the edge in AET to enter the next cycle
		NET *p = pAET->next;
		while(p){
			p->x = p->x + p->dx;
			p = p->next;
		}
		
		//Sort X in AET table from small to large
		AET *tq = pAET;
		p = pAET->next;
		tq->next = NULL;
		while(p){
			while(tq->next && p->x >= tq->next->x)
                tq=tq->next;
            NET *s=p->next;
            p->next=tq->next;
            tq->next=p;
            p=s;
            tq=pAET;
		}
		//Delete the edge in AET satisfying y = ymax
		AET *q=pAET;
        p=q->next;
        while(p)
        {
            if(p->ymax==i)
            {
                q->next=p->next;
                delete p;
                p=q->next;
            }
            else
            {
                q=q->next;
                p=q->next;
            }
        }
		//Find the right place to insert pAET 
		p=pNET[i-MinY]->next;
        q=pAET;
        while(p)
        {
            while(q->next && p->x >= q->next->x)
                q=q->next;
            NET *s=p->next;
            p->next=q->next;
            q->next=p;
            p=s;
            q=pAET;
        }
		//Fill color
		p=pAET->next;
        while(p && p->next)
        {
			
            if(i>=refrain_miny && i< refrain_maxy){
				
				if((p->x < refrain_minx) && (p->next->x > refrain_minx) && (p->next->x < refrain_maxx)){
					crosspoint1 = refrain_minx;
					crosspoint2 = p->next->x;
				
				}else if((p->x < refrain_minx) && (p->next->x > refrain_maxx)){
					crosspoint1 = refrain_minx;
					crosspoint2 = refrain_maxx;
				
				}else if((p->x >= refrain_minx) && (p->next->x <= refrain_maxx)){
					crosspoint1 = p->x;
					crosspoint2 = p->next->x;
				
				}else if((p->x > refrain_minx) && (p->x < refrain_maxx) && (p->next->x > refrain_maxx)){
					crosspoint1 = p->x;
					crosspoint2 = refrain_maxx;
				}else{
					crosspoint1 = 0;
					crosspoint2 = -1;
				}
				for(int j=crosspoint1; j<=crosspoint2; ++j){
				set(i-offsety,j-offsetx);
				}
			}
            p=p->next->next;
        }
	}
	
	for(i=MinY; i<=MaxY; i++){
		delete(pNET[i-MinY]);
	}
	delete(pAET);

}  
void set(int i,int j){
	int shift = 8*(i%4) + (8-(j%8))-1;
	arr[i/4][j/8] |= 1 << (shift);
};
int get(int i,int j){
	int shift = 8*(i%4) + (8-(j%8))-1;
	int a = arr[i/4][j/8] & (1 << (shift));
	if(a!= 0)
		return 1;
	else
		return 0;
};
void txtToPolygon(double k){
	ifstream infile;
	infile.open(inFile);
	string type;
	int count = 0;
	Point tmp; 
	tmp.x=INT_MAX;
	tmp.y =INT_MAX;
	DPoint tmp1;
	while (infile !=NULL) 
        {   
            infile>>type;
            if(type[0]=='V')
            {
            double x;
            double y;
            string name;
            infile>>name;
            infile>>x;
            infile>>y;
            tmp1.x = x;
            tmp1.y = y;
            Dpoin.push_back(tmp1);
            x = (x -minx)/k +minx;
            y = (y -miny)/k +miny;
            if (tmp.x!=int(x) || tmp.y !=int(y))
            	{
				tmp.x=int(x);
            	tmp.y = int(y);
            	polygon.coordinate.push_back(tmp);
                count++;
               }
			}
			if(type[0] == 'E')
			{
				int x;
				int y;
				staEndFlag = 1;
				infile>>x;
				infile>>y;
				start = x<y?x:y;
				if(fabs(y-x)!=1)
				{
					start =x<y?y:x;
				}
				infile>>type;
				infile>>x;
				infile>>y;
				End=x<y?x:y;
				if(fabs(y-x)!=1)
				{
					End =x<y?y:x;
				}
			}
			type ="";
        }
    if(staEndFlag == 1)
    {
    	if(start>End)
    	{
    		int tmp = start;
			start = End;
			End = tmp; 
		}
	}
    polygon.maxp.x=(maxx-minx)/k+minx;
    polygon.maxp.y=(maxy-miny)/k+miny;
    polygon.minp.x=minx;
    polygon.minp.y=miny;
    polygon.size=count;
    infile.close();
}
void GetOrigion(){
	ifstream infile;
	infile.open(inFile);
	string type;
	while (infile !=NULL) 
        {   
            infile>>type;
            if(type[0]=='V')
            {
            double x;
            double y;
            infile>>type;
            infile>>x;
            infile>>y;
            if(int(x)> maxx)
				maxx = int(x);
			if(int(y)> maxy)
				maxy = int(y);
			if(int(x)< minx)
				minx = int(x);
			if(int(y)< miny)
				miny = int(y);
			}
        }
    maxy++;
    maxx++;
    infile.close();
}
void getK(double &k){
	double a=(maxy - miny)* 1.0f/(height*4);
	double c=(maxx - minx)* 1.0f/(weight*8);
	 k = a>c?a:c;
	if (k<1)
	  k =1;
}  
void invokeScanline(){
	GetOrigion();
	getK(k);
	cout<<k<<endl;
	txtToPolygon(k);
	PolyScan(minx, miny, polygon.minp.x, polygon.maxp.x, polygon.minp.y, polygon.maxp.y, polygon.minp.y, polygon.maxp.y, arr, polygon);
} 
bool SameK(DEdge &E1,DEdge &E2){
	if((E1.flag==0&&E2.flag==0) || fabs(E1.k-E2.k)<=thresholdK)
		return 1;
	if(fabs(E1.k)>1000&&fabs(E2.k)>=1000&&fabs(1/E1.k-1/E2.k)<thresholdK)
		return 1;
	if(fabs(E1.k/E2.k-1)<0.01)
		return 1; 
	return 0;
}
DPoint MiddlePoint(DPoint &A,DPoint &B){
	DPoint C;
	C.x = (A.x+B.x)/2;
	C.y = (A.y+B.y)/2;
	return C;
}
void shadow(DPoint &CT,double k,double b){
	if(fabs(k)>0)
	{
	double k1 = -1/k;
	double b1 = CT.y - k1*CT.x;
	CT.x = (b1-b)/(k-k1);
	CT.y = k*CT.x + b;
	}
	else
	{
		CT.y = b;
	}
	 
} 
double Getlength(DPoint start,DPoint end){
	return sqrt((start.x-end.x)*(start.x-end.x) + (start.y - end.y)*(start.y - end.y));
}
bool overlap(DEdge E1,DEdge E2){
	DPoint A = E1.A;
	DPoint B = E1.B; 
	DPoint C = E2.A;
	DPoint D = E2.B;
	double k =E1.k;
	int flag =E1.flag;
	double minxE1 = A.x;
	double maxxE1 = B.x;
	double minyE1 = A.y;
	double maxyE1 = B.y;
	double minxE2 = C.x;
	double maxxE2 = D.x;
	double minyE2 = C.y;
	double maxyE2 = D.y;
	swap(minyE1,maxyE1);
	swap(minyE2,maxyE2);
	double length1 =  Getlength(A,B);
	double length2 = Getlength(C,D);
	double length3 = Getlength(A,C);
	double maxLength = length1>length2?length1:length2;
	double thresholdOver = thresholdOverTmp;
	if(maxLength<=5)
		thresholdOver = thresholdOverTmp1;
	if(flag == 0)
	{
		if((minyE2>=maxyE1-thresholdOver || maxyE2-thresholdOver<=minyE1) &&(minyE2>=maxyE1-thresholdOver || maxyE2-thresholdOver<=minyE1))
			return 0;
	}
	else
	{
	DPoint CT = C;
	DPoint DT = D;
	double b = A.y - A.x*k;
	shadow(CT,k,b);
	shadow(DT,k,b);
	minxE2 = CT.x;
	maxxE2 = DT.x;
	minyE2 = CT.y;
	maxyE2 = DT.y;
	if((minxE2>=(maxxE1 - thresholdOver)||(maxxE2 - thresholdOver)<=minxE1) && (minyE2>=maxyE1-thresholdOver || maxyE2-thresholdOver<=minyE1))
			return 0;
	}
	return 1;
} 
bool PinPoly(DPoint A){
	//i means the point is in the polygon if one of the four points around the Point
	int i= 1 ;
	int y = int((A.y - miny)/k);
	int x = int((A.x - minx)/k);
	if((y-i)>=0&&(y+i)<4*height&&(x+i)<8*weight&&(x-i)>=0)
	{
		if( get(y,x)== 1 || get(y+i,x)== 1||get(y+i,x+i) == 1 ) 
			return 1;
		if(get(y-i,x) == 1||get(y,x+i)==1 ||get(y,x-i) == 1)
			return 1;
	}
	else if((y-i)<0&&(x-i)<0)
	{
		if(get(y,x)== 1||get(y+i,x)==1||get(y,x+i)==1)
			return 1;
	}
	else if((y+i)>=4*height&&(x-i)<0)
	{
		if(get(y,x)== 1||get(y-i,x)==1||get(y,x+i)==1)
			return 1;
	}
	else if((x-i)>=8*weight&&(y-i)<0)
	{
		if(get(y,x)== 1||get(y,x-i)==1||get(y+i,x)==1)
			return 1;
	}
	else if((x-i)>=8*weight&&(y+i)>=4*height)
	{
		if(get(y,x)== 1||get(y,x-i)==1||get(y-i,x)==1)
			return 1;
	}
	else if((x+i)>=8*weight)
	{
		if(get(y,x)== 1||get(y,x-i) == 1||get(y+i,x)==1||get(y-i,x)==1)
			return 1;
	}
	else if((x-i)<0)
	{
		if(get(y,x)== 1||get(y,x+i) == 1||get(y+i,x)==1||get(y-i,x)==1)
			return 1;
	}
	else if((y+i)>=4*height)
	{
		if(get(y,x)== 1||get(y,x+i) == 1||get(y,x-i)==1||get(y-i,x)==1)
			return 1;
	}
	else if((y-i)<0)
	{
		if(get(y,x)== 1||get(y,x+i) == 1||get(y,x-i)==1||get(y+i,x)==1)
			return 1;
	}
	else
	{
		return 0;
	}
}
bool inPoly(DPoint A,DPoint B){
	if( PinPoly(A) && PinPoly(B) )
		return 1;
	return 0;
} ;
bool MiddleLine(DEdge &E1,DEdge &E2){
	if(!SameK(E1,E2))
		return 0;
	if(!overlap(E1,E2))
		return 0;
	if(!inPoly(MiddlePoint(E1.A,E2.A),MiddlePoint(E1.B,E2.B)))
		return 0;
	return 1;
}
void DPoint2DEdge(){
	DEdge E;
	E.next = NULL;
	for(int i=0;i<Dpoin.size();i++)
	{
		int next = i+1;
		if(i == (Dpoin.size()-1))
		{
			next = 0;
		}
		DPoint A = Dpoin[next];
		DPoint B = Dpoin[i];
		DPoint tmp = A;
		int flag = 1;
		double k=INT_MAX;
		if(B.x<A.x)
		{
			tmp = A;
			A = B;
			B = tmp;
			k = (A.y - B.y)/(A.x - B.x);
		}
		else if(B.x==A.x)
		{
			if(B.y<A.y)
			{
			tmp = A;
			A = B;
			B = tmp;
			}
			flag = 0;
		}
		else
		{
			k = (A.y - B.y)/(A.x - B.x);
		}
		E.length = Getlength(A,B);
		E.A = A;
		E.B = B;
		E.flag = flag;
		E.k = k;
		if(fabs(E.k)>=thresholdZ)
		{
			E.flag = 0;
		}
		Dpoly.push_back(E);
	}
}
double getdis(double k,double b,DPoint &A){
	double dis = fabs(k*A.x - A.y + b)/sqrt(1+k*k);
	return dis;
}
void distinct(DEdge &E1,DEdge &E2,double &dis){
	if(E1.flag == 0)
	{
		dis = fabs(E1.A.x - E2.A.x);
	}
	else
	{
		double b=E1.A.y - E1.k*E1.A.x ;
		double dis1 = getdis(E1.k,b,E2.A);
		double dis2 = getdis(E1.k,b,E2.B);
		dis = (dis1+dis2)/2;
	}
}
void SameGroup(int i,int j,double mindis[])
{
	    double dis=0;
		if(MiddleLine(Dpoly[i],Dpoly[j]))
				{
					int a = i;
					int b = j;
					if(Dpoly[a].length<Dpoly[b].length) 
					{
						b = i;
						a = j;
					}
				    distinct(Dpoly[a],Dpoly[b],dis);
				    if(dis<mindis[a])
				    	mindis[a] = dis;
				    if(dis<mindis[b])
				    	mindis[b] =dis;
					Mask *p = (Mask*)malloc(sizeof(Mask));
					p->dis = dis;
					p->i=b;
					//Distance sorted from small to large
					Mask *q=Dpoly[a].next;
					Mask *t=Dpoly[a].next;
					int count = 0;
					while(q!=NULL)
					{
						if(p->dis<=q->dis)
							break;
						t = q;
						q = q->next;
						count++;
					} 
					if(count==0)
					{
					Mask *t = Dpoly[a].next;
					Dpoly[a].next = p;
					p->next = t;
					}
					else
					{
					q = t->next;
					t->next = p;
					p->next = q; 
					}	
				}
}
void divide(){
	double mindis[Dpoly.size()];
	for(int i=0;i<Dpoly.size();i++)
	{
		mindis[i] = INT_MAX;
	}
	if(staEndFlag==0)
	{ 
		for(int i = 0;i<Dpoly.size();i++)
		{
		for(int j=i+2;j<Dpoly.size();j++)
			{
				SameGroup(i,j,mindis);
			}
		}
    }
    else
    {
    	for(int i =start+1;i<End;i++)
    	{
    		for(int k=End+1;k<Dpoly.size()+start;k++)
    		{
    			int j = k%Dpoly.size();
    			if(abs(i-j)!=1) 
    			{
    				SameGroup(i,j,mindis);
				}
			}
		}
	}
	for(int i=0;i<Dpoly.size();i++)
	{
		if(Dpoly[i].next!=NULL&&Dpoly[i].next->dis-thresholdD>mindis[i])
			{
				Mask *p = Dpoly[i].next;
				Mask *t = Dpoly[i].next;
					while(p!=NULL)
					{
						p = p->next;
						free(t);
						t = p;
					}
				Dpoly[i].next = NULL;
			}
	}
}
double getNewDis(Mask *p,Mask *q){
	double sumLen=0;
	double tmp = 0;
	while(p!=q->next)
	{
		tmp += p->dis*Dpoly[p->i].length;
		sumLen+=Dpoly[p->i].length;
		p=p->next;
	}
	return tmp/sumLen;
}
void getParaLine(double &k,double &b,double dis){
	b = b - sqrt(1+k*k)*dis;
}
void getLine(Mask *p,Mask *q,MiddLine &tmp,int i){
	 double Newdis = getNewDis(p,q);
	 Mask *tp = p;
	 Mask *min=p;
	 Mask *max=p;
	 while(p!=q->next)
	 {
	 	if(Dpoly[p->i].A.x<Dpoly[min->i].A.x)
	 		min = p;
	 	if(Dpoly[p->i].B.x>Dpoly[min->i].B.x)
	 		max = p;
	 	p = p->next;
	 }
	 DPoint A = Dpoly[min->i].A;
	 DPoint B = Dpoly[max->i].B;
	 double k = Dpoly[i].k;
	 double b = 0;
	 //If it's vertical, compare x directly and subtract x
	 if(Dpoly[i].flag == 0)
	 {
	 	double sub1 = Newdis - min->dis;
	 	double sub2 = Newdis - max->dis;
	 	//Judge whether the point is on the left or right of the straight line
	 	if(Dpoly[i].A.x < Dpoly[min->i].A.x)
	 	{
	 		A.x = A.x +sub1;
	 		B.x = B.x + sub2;
		}
	 	else
	 	{
	 		A.x = A.x -sub1;
	 		B.x = B.x -sub2;
		}
	 }
	 else
	 {
	 	b = Dpoly[i].A.y - Dpoly[i].k*Dpoly[i].A.x;
	 	//Judge whether the point is below the straight line
	 	if(Dpoly[min->i].A.y < (Dpoly[min->i].A.x*k+b))
		{
			Newdis = -Newdis;
		}
		getParaLine(k,b,-Newdis);
		shadow(A,k,b);
		shadow(B,k,b);
	 }
	tmp.A = MiddlePoint(A,Dpoly[i].A);
	tmp.B = MiddlePoint(B,Dpoly[i].B);	  
}
void add(int set){
	DPoint startP;
	startP = MiddlePoint(Dpoly[set].A,Dpoly[set].B);
	DPoint endP1=startP;
	DPoint endP2=startP;
	double length = Getlength(Dpoly[set].A,Dpoly[set].B)/2;
	if(length<=5)
	{
		length =6;
	}
	double k = 0;
	double b = 0;
	if(Dpoly[(set+1)%Dpoly.size()].k== 0)
	{
		endP2.y =endP1.y = startP.y;
		endP1.x = startP.x+length;
		endP2.x = startP.x -length;
	}
	else if(Dpoly[(set+1)%Dpoly.size()].flag==0)
	{
		endP2.x =endP1.x = startP.x;
		endP1.y = startP.y+length;
		endP2.y = startP.y -length;
	}
	else
	{
		k = Dpoly[(set+1)%Dpoly.size()].k;
		b = startP.y - k*startP.x;
		endP1.x = startP.x - length/sqrt(1+k*k);
		endP1.y = k*endP1.x+b;
		endP2.x = startP.x + length/sqrt(1+k*k);
		endP2.y = k*endP2.x+b;
	}
	MiddLine tmp;
	if(!PinPoly(endP1))
	{
		endP1 = endP2;
	}
	if(PinPoly(endP1)){
	if(startP.x<endP1.x)
	{
		tmp.A = startP;
		tmp.B = endP1;
	}
	else if(startP.x>endP1.x)
	{
		tmp.A = endP1;
		tmp.B = startP;
	}
	else if(startP.y<endP1.y)
	{
		tmp.A =startP;
		tmp.B = endP1;
	}
	else
	{
		tmp.A = endP1;
		tmp.B = startP;
	}
	Dmiddle.push_back(tmp);
	}
}
void getMiddleLine(){
	Mask *p;
	MiddLine tmp;
	for(int i = 0;i<Dpoly.size();i++)
	{
		p = Dpoly[i].next;
		int count = 0;
		while(p!=NULL)
		{
			p = p->next;
			count++;
		}
		p = Dpoly[i].next;
		if(count==1)
		{
			tmp.A = MiddlePoint(Dpoly[i].A,Dpoly[p->i].A);
			tmp.B = MiddlePoint(Dpoly[i].B,Dpoly[p->i].B);
			tmp.i = i;
			tmp.j = p->i; 
			double length1 = Getlength(Dpoly[i].A,Dpoly[i].B);
			double length2 = Getlength(Dpoly[p->i].A,Dpoly[p->i].B);
			if(length1>=10*length2||length2>=10*length1)
				continue;
			if(Getlength(tmp.A,tmp.B)>=0.5)
			{
				Dmiddle.push_back(tmp);
			}
		}
		if(count>1)
		{
			while(p!=NULL&&p->next!=NULL)
			{
				if(fabs(p->next->dis-p->dis)>=thresholdD)	
					break;
				p=p->next;
			}  
			getLine(Dpoly[i].next,p,tmp,i);
			tmp.i = i;
			tmp.j = -1;
			if(Getlength(tmp.A,tmp.B)>=0.5)
			{
				Dmiddle.push_back(tmp);
			}
		}
	}
	if(staEndFlag == 1)
	{
		add(start);
		add(End);
	}
}
void InvokeGetMiddleLine(){
	DPoint2DEdge();
	divide(); 
   	getMiddleLine();
	free();
	Dpoin.clear();
	Dpoly.clear();
};
void free(){
	Mask * p;
	Mask * t;
	for(int i = 0;i<Dpoly.size();i++)
	{
		p = Dpoly[i].next;
		t = Dpoly[i].next;
		while(p!=NULL)
		{
			p = p->next;
			free(t);
			t = p;
		}
		Dpoly[i].next = NULL;
		
	}
}
//----------------------function Connect section -------------------
bool equal(DPoint &A,DPoint &B){
	if(fabs(A.x-B.x)<thresholdE&&fabs(A.y-B.y)<thresholdE)
		return 1;
	else
		return 0;
}  
bool Para(LinePara E1,LinePara E2){
	if((E1.flag==0&&E2.flag==0) || fabs(E1.k-E2.k)<thresholdPara)
		return 1;
	if(fabs(E1.k)>1000&&fabs(E2.k)>=1000&&fabs(E1.k/E2.k-1)<thresholdPara)
		return 1;
	if(fabs(E1.k/E2.k-1)<0.01 &&E2.k!=0)
		return 1; 
	return 0;
}
double getLinePara(DPoint &A,DPoint &B,LinePara &LP){
	double m = 0;
	double x2 = B.x;
	double x1 = A.x;
	double y2 =B.y;
	double y1 = A.y;
	m = x2 - x1; 
	if (0 == m)
	{
		LP.k = 4000;
		LP.flag = 0;
		LP.b = y1 - LP.k * x1;
	}
	else
	{
		LP.k = (y2 - y1) / (x2 - x1);
		LP.b = y1 - LP.k * x1;
		LP.flag = 1;
		if(fabs(LP.k) > 4000)
			LP.flag  = 0;
	}
	DPoint tmp;
	if(LP.flag == 1)
	{
		if(A.x>B.x)
		{
			tmp = A;
			A = B;
			B= tmp;
		}
	}
	else
	{
		if(A.y>B.y)
		{
			tmp =A;
			A = B;
			B = tmp;
		}
	}
	LP.next = NULL;
	extendNode *q = (extendNode *)malloc(sizeof(extendNode));
	q->C= A;
	q->type = 0;
	LP.down = q;
	extendNode *t = (extendNode *)malloc(sizeof(extendNode));
	t ->C = B;
	t->type = 3;
	q->next = t;
	t->next = NULL;
	LP.length  = Getlength(A,B);
	return LP.length; 
}
void GetSAndE(DPoint &min,DPoint &max,LinePara &LP)
{
	min = LP.down->C;
	extendNode *p = LP.down;
	while(p->next!=NULL)
	{
		p = p->next;
	}
	max = p->C;
}
bool conin(LinePara &LP,LinePara &LT){
	int z=3;
	double minlength = z*LT.length;
	if(LT.length>LP.length)
		minlength = z*LP.length;
	DPoint A;
	DPoint B;
	GetSAndE(A,B,LP);
	DPoint C;
	DPoint D;
	GetSAndE(C,D,LT);
	double minxE1 = A.x;
	double maxxE1 = B.x;
	double minyE1 = A.y>B.y?B.y:A.y;
	double maxyE1 = A.y>B.y?A.y:B.y;
	double minxE2 = C.x;
	double maxxE2 = D.x;
	double minyE2 = C.y>D.y?D.y:C.y;
	double maxyE2 = C.y>D.y?C.y:D.y;
	double length1 = Getlength(B,C);
	double length2 = Getlength(A,D);
	double dist = length1>length2?length2:length1;
	if(LP.flag == 0)
	{
		//No overlap and dis > = minlength
		if((minyE2>=maxyE1-thresholdOver || maxyE2-thresholdOver<=minyE1) && dist>=minlength )
			return 0;
	}
	else
	{
		//No overlap and dis > = minlength
		if( (minxE2>=(maxxE1 - thresholdOver)||(maxxE2 - thresholdOver)<=minxE1) && dist>=minlength)
			return 0;
	}
	return 1;
} 
bool PinEdge(DPoint A,DPoint B,DPoint C)
{
	if((C.x>A.x &&C.x<B.x)||(C.y>A.y&&C.y<B.y))
		return 1;
	return 0;
}  
bool Isgroup(LinePara &LP,double length,int j){
	int flag = 0;
	int record = -1;
	for(int i=0;i<MiddLP.size();i++)
	 {
	 	DPoint C;
		if(Para(LP,MiddLP[i]) && flag ==0 )
			{
				//Parallel situation
				DPoint M1;
				DPoint M2;
				DPoint A;
				M1 = MiddlePoint(Dmiddle[j].A,Dmiddle[j].B);
				M2 = MiddlePoint(LP.down->C,LP.down->next->C);
				A = MiddlePoint(M1,M2);
				if(LP.flag!=0 &&fabs(A.y-(MiddLP[i].k*A.x+MiddLP[i].b))>0.1)
					continue;
				else if(LP.flag == 0&&fabs(Dmiddle[i].A.x-Dmiddle[j].A.x)>0.1)
					continue;
				else
				{
					double length1 = Getlength(Dmiddle[j].A,Dmiddle[j].B);
					double length2 = Getlength(LP.down->C,LP.down->next->C);
					double length3 = Getlength(Dmiddle[j].B,LP.down->C);
					double length4 = Getlength(Dmiddle[j].A,LP.down->next->C);
					double over = length3>length4?length4:length3;
					if(over<10)
					{
						mergeNode *p=MiddLP[i].next;
						while(p!=NULL)
						{
							if(conin(LP,MiddLP[i]))
							{
						
								record = i;
								flag =1;
								insert(i,Dmiddle[j].A);
								insert(i,Dmiddle[j].B);
								extendNode *q = MiddLP[i].down;
								DPoint min=q->C;
								while(q->next!=NULL)
								{
									q = q->next;
								}
								DPoint max = q->C;
								q = MiddLP[i].down;
								extendNode *t = q;
								while(q !=NULL)
								{
									t = q;
									q = q->next;
									free(t);
								}
								getLinePara(min,max,MiddLP[i]);
								break;
							}
							p=p->next;
						}
					}
				}
			}
	}
	if(flag)
	{
			mergeNode *q = (mergeNode *)malloc(sizeof(mergeNode));
			q->i = j;
			q->length = length;
			q->next = MiddLP[record].next;
			MiddLP[record].next = q;

	}
	return flag;
}
void getMiddlLP(){
	for(int i=0;i<Dmiddle.size();i++)
	{ 
		LinePara LP;
		double length = getLinePara(Dmiddle[i].A,Dmiddle[i].B,LP);
		if(!Isgroup(LP,length,i))
		{
			mergeNode *p = (mergeNode *)malloc(sizeof(mergeNode));
			p->i = i;
			p->length = length;
			p->next = NULL;
			LP.next = p;
			MiddLP.push_back(LP);
		}
			
	}; 
}
void GetSPandEnd(extendNode *p,DPoint &start,DPoint &end)
{
		while(p!=NULL)
		{
			if(p->type == 0)
			{
				start = p->C;
			}
			if(p->type == 3)
			{
				end = p->C;
			}
			p = p->next;
		}
}
//Return0 indicates that there is an intersection point 
bool getCross(LinePara &E1,LinePara &E2,DPoint &Cross){
	if(E1.down==NULL||E2.down==NULL)
		return 1;
	if(fabs(E1.k-E2.k)<thresholdPara|| (E1.flag==0&&E2.flag == 0))
	{
		return 1;
	}
	else
	{
		if(E1.flag!=0&&E2.flag!=0)
		{
			if(E1.k==0)
			{
				Cross.y = E1.down->C.y;
				Cross.x = (Cross.y - E2.b)/E2.k;
			}
			else
			{
				double z = E2.k/E1.k;
				Cross.y = (E2.b-z*E1.b)/(1-z);
				Cross.x = (E1.b-E2.b)/(E2.k - E1.k);
			}
	    return 0;
		}
		else
		{
			//E1 vertical
			if(E1.flag == 0)
			{
				Cross.x = E1.down->C.x;
				Cross.y = E2.k*Cross.x + E2.b;
			} 
			else
			{
			//E2 vertical
				Cross.x = E2.down->C.x;
				Cross.y = E1.k*Cross.x + E1.b;
			}
		}
		return 0;
	} 
}
void UpdataMinMax(int j,DPoint C,DPoint &min,int &minJ,int k){
		//Find the start point and end point of line j
		extendNode *s=MiddLP[j].down;
		extendNode *e = MiddLP[j].down;
		while(e!=NULL)
		{
			if(s->type !=0)
					s = s->next;
			if(e->type !=3)
					e = e->next;
			else
				break;
		}
		if(MiddLP[j].flag !=0)
		{
			//Find the length to be extended  --- minlength
			double minlength = Getlength(C,s->C);
			if(C.x>s->C.x&&C.x<e->C.x)
				minlength = 0;
			else if(C.x<s->C.x)
				minlength = Getlength(C,s->C);
			else
				minlength = Getlength(C,e->C);
			double lengthJ = Getlength(s->C,e->C);
			// If the down conditions are met, update min and minJ
			if(minlength<k*lengthJ &&minlength<thresholdEx)
			{
				min = C;
				minJ = j;
			}
		}
		else
		{
			double minlength = Getlength(C,s->C);
			if(C.y>s->C.y&&C.y<e->C.y)
				minlength = 0;
			else if(C.y<s->C.y)
				minlength = Getlength(C,s->C);
			else
				minlength = Getlength(C,e->C);
			double lengthJ = Getlength(s->C,e->C);
			// If the down conditions are met, update min and minJ
			if(minlength<k*lengthJ &&minlength<thresholdEx)
			{
				min = C;
				minJ = j;
			}
		}
}
void extend(){
	double k=1;
	for(int i=0;i<MiddLP.size();i++)
	{
		DPoint min;
		DPoint max;
		DPoint start;
		DPoint end;
		int minJ=i;
		int maxJ=i;
		min.x = -INT_MAX;
		min.y = -INT_MAX;
		max.x = INT_MAX;
		max.y = INT_MAX; 
		extendNode * p = MiddLP[i].down;
		if(p==NULL)
			continue;
		while(p!=NULL)
		{
			if(p->type == 0)
			{
				start = p->C;
			}
			if(p->type == 3)
			{
				end = p->C;
			}
			p = p->next;
		}
		DPoint C;
		for(int j=0;j<MiddLP.size();j++)
		{
			C.x = INT_MAX;
			C.y = INT_MAX;
			if(MiddLP[j].down == NULL)
				continue;
			//vertical
			if(MiddLP[i].flag == 0)
			{
				if(!getCross(MiddLP[i],MiddLP[j],C))
				{
					if(C.y <=start.y+thresholdDis)
					{
						if(C.y>min.y)
						{
							UpdataMinMax(j,C,min,minJ,k);
						}
					}
					if(C.y>=end.y-thresholdDis)
					{
						if(C.y<max.y)
						{
						    UpdataMinMax(j,C,max,maxJ,k);
						}	
					}
				}
				else
				{
					//If there is no coincidence interval
					DPoint start1;
					DPoint end1;
					GetSAndE(start1,end1,MiddLP[j]);
					double thresholdOver = 0;
					if(end.y<start1.y-thresholdOver||start.y>end1.y+thresholdOver)
					{
						if(start.y<start1.y)
							C = MiddlePoint(start1,end);
						else
							C =MiddlePoint(start,end1);
					}
					if(fabs(start.x-C.x)<1)
						{
							if(C.y>min.y)
							{
								UpdataMinMax(j,C,min,minJ,k);
							}
							if(C.y<max.y)
							{
								UpdataMinMax(j,C,max,maxJ,k);
							}
						}
				}	
			}
			else
			{			
				if(!getCross(MiddLP[i],MiddLP[j],C))
				{
					if(C.x <=start.x+thresholdDis)
					{
						if(C.x>min.x)
						{
							UpdataMinMax(j,C,min,minJ,k);
						}
					}
					if(C.x>=end.x-thresholdDis)
					{
						if(C.x<max.x)
						{
							UpdataMinMax(j,C,max,maxJ,k);
						}
					}
				}

				else
				{
					DPoint start1;
					DPoint end1;
					GetSAndE(start1,end1,MiddLP[j]);
					double thresholdOver = 0;
					if(end.x<=start1.x-thresholdOver||start.x>=end1.x+thresholdOver)
					{
						if(start.x<start1.x)
							C = MiddlePoint(start1,end);
						else
							C =MiddlePoint(start,end1);
					}
					DPoint CT = C;
						shadow(CT,MiddLP[i].k,MiddLP[i].b);
						if(fabs(CT.y-C.y)<=1)
						{
							
								if(C.x>min.x)
								{
									UpdataMinMax(j,C,min,minJ,k);
								}
								if(C.x<max.x)
								{
									UpdataMinMax(j,C,max,maxJ,k);
								}
						}
				
				}	
			}	
		}
		double threshold = Getlength(start,end);
		double minlength = Getlength(min,start);
		double maxlength = Getlength(end,max);
		if(threshold/5<=1)
		{
			threshold = 5;	
		}
		DPoint A;
		DPoint B;
		GetSAndE(A,B,MiddLP[maxJ]);
		DPoint X;
		if(maxlength<=threshold/5 &&!getCross(MiddLP[i],MiddLP[maxJ],X) &&maxlength<=thresholdEx &&!PinEdge(A,B,max))
		{
			insert(i,max);
			insert(maxJ,max);
		}
		GetSAndE(A,B,MiddLP[minJ]);
		if(minlength<=threshold/5 &&!getCross(MiddLP[i],MiddLP[minJ],X) &&minlength<=thresholdEx&&!PinEdge(A,B,min))
		{
		
			insert(i,min);
			insert(minJ,min);	
		}
	} 
}
void insert(int i,DPoint max){
	extendNode *p = MiddLP[i].down;
	extendNode *t=p;
	extendNode *q =(extendNode *)malloc(sizeof(extendNode));
	while(p!=NULL)
			{
				if(MiddLP[i].flag == 0)
				{
					if(max.y < p->C.y)
					{
						if(p->type == 0)
							 q->type = -1;
						else if(p->type == 3)
							q->type = 2;
						else
							q->type =p->type;
						break;
					} 
				}
				else
				{
					if(max.x<p->C.x)
					{
						if(p->type == 0)
							 q->type = -1;
						else if(p->type == 3)
							q->type = 2;
						else
							q->type =p->type;
						break;
					}
				}
				t = p;
				p=p->next;
			}
	if(p == NULL)
		q->type = -1;
	if(t == p )
	{
	q -> C = max;
	q -> next = t;
	MiddLP[i].down = q;	
	}
	else
	{
	q -> C = max;
	q -> next = t->next;
	t -> next = q;	
	}
}
void deletePoint(){
	extendNode * q;
	extendNode * t;
	for(int i = 0;i<MiddLP.size();i++)
	{
		q = MiddLP[i].down;
		if(q==NULL)
			continue;
		t = q;
		DPoint tmp = q->C;
		q = q->next;
		while(q!=NULL)
		{
			if(equal(q->C,tmp) || q->type == 0||(q->type == 3&&q->next!=NULL))
			{
				t->next = q ->next;
				free(q);
				q = t->next;
				continue;
			}
			tmp = q->C;
			t = q;
			q = q->next;
		}
		if(equal(tmp,MiddLP[i].down->C)&&MiddLP[i].down->next==NULL)
		{
			free(MiddLP[i].down);
			MiddLP[i].down =NULL;
		}
	}
};
void Gematrix(){
	matrix tmp;
	extendNode * q;
	for(int i = 0;i<MiddLP.size();i++)
	{
		q = MiddLP[i].down;
		while(q!=NULL)
		{
			int fllag = 0;
			for(int i=0;i<Midd.size();i++)
			{
				if(equal(Midd[i].A,q->C))
				{
					q->type = i;
					fllag =1;
					break;
				}
			}
			if(fllag !=1)
			{
				q->type = Midd.size();
				tmp.A = q->C;
				tmp.flag = 0;
				tmp.next = NULL;
				Midd.push_back(tmp);
			}
			q = q->next;
		}
	}
}
void swap(double &min,double &max){
	double tmp = min;
	if(tmp>max)
	{
		min = max;
		max = tmp;
	}
}
void Updatmatrix(){
	extendNode *q;
	for(int i = 0;i<MiddLP.size();i++)
	{
		q = MiddLP[i].down;
		if(q==NULL)
			continue;
		while(q->next!=NULL)
		{
			int before = q->type;
			int after = q->next->type;
			swap(before,after); 
			mask *p=(mask *)malloc(sizeof(mask));
			p->next =NULL; 
			p->i = after;
			mask *t = Midd[before].next;
			Midd[before].next = p;
			p->next = t;
			//insert
			mask *z=(mask *)malloc(sizeof(mask));
			z->next =NULL; 
			z->i = before;
			mask *x = Midd[after].next;
			Midd[after].next = z;
			z->next = x;
			q = q->next; 
		}
	}
}
bool LenBig(int i)
{

	int flag = 1;
				if(Getlength(Midd[i].A,Midd[Midd[i].next->i].A)<=thresholdM&&Midd[i].next->next==NULL)
					flag =0;
	return flag;
}
bool isNext(mask *r){
	int flag = 0 ;
	while(r!=NULL)
	{
		if(Midd[r->i].flag == 0&&LenBig(r->i))
			flag =1;
		r = r->next;
	}
	return flag;
}
int matrix2File(int i,FILE *fp){
	if(Midd[i].flag == 1)
		return 0;
	mask * r = Midd[i].next;
	Midd[i].flag =1;
	//To deal with terminating and spurious edges
	int flagEnd = 1;
	mask * p = Midd[i].next;
	while(p!=NULL)
	{
		if(Midd[p->i].flag == 0)
		{
			flagEnd = 0;
		}
		p = p->next;
	}
	if(flagEnd)
	{
		p = Midd[i].next;
		double length = Getlength(Midd[i].A,Midd[p->i].A);
		if(length>=thresholdM)
		{
		DPoint C=Midd[i].A;
		fprintf(fp, "%f ",C.x);
		fprintf(fp, "%f ",C.y);
		fputc('\n', fp);
		}
	}
	else
	{
		fprintf(fp, "%f ",Midd[i].A.x);
		fprintf(fp, "%f ",Midd[i].A.y);
		fputc('\n', fp);
	}
	while(r!=NULL)
	{
		if(Midd[r->i].flag ==0)
		{
			matrix2File(r->i,fp);
			if(isNext(r->next)&&LenBig(r->i))
			{
				fputc('\n', fp);
				fprintf(fp, "%f ",Midd[i].A.x);
				fprintf(fp, "%f ",Midd[i].A.y);
				fputc('\n', fp);
			}
		}
		r = r->next;
	}
}
bool findNext(int &next,int &before)
{
	double minLength=INT_MAX;
	double length = INT_MAX; 
	for(int i=0;i<Midd.size();i++)
	{
		if(Midd[i].flag == 1)
			continue;
		double tmplength = INT_MAX;
		double tmpbefore = next;
		for(int j=0;j<Midd.size();j++)
		{
			if(Midd[j].flag == 0)
				continue;
			length = Getlength(Midd[i].A,Midd[j].A);
			if(length<tmplength)
			{
				tmplength = length;
				tmpbefore = j;
			}
		}
		if(tmplength<minLength)
		{
			next = i;
			before = tmpbefore;
			minLength = tmplength;	
		}	
	}
	if(Midd[next].flag == 1)
		return 0;
	if(Midd[next].flag == 0)
		return 1; 
}
int Readmatrix(int i){
	if(Midd[i].flag == 1)
		return 0;
	mask * r = Midd[i].next;
	Midd[i].flag =1;
	while(r!=NULL)
	{
		if(Midd[r->i].flag ==0)
		{
			Readmatrix(r->i);
		}
		r = r->next;
	}
}
void InsertMatrix()
{
	int j = 0;
	for(j;j<Midd.size();j++)
	{
		if(Midd[j].next!=NULL&&Midd[j].next->next==NULL)
		{
			if(j!=Midd[j].next->i)
				break;
		}
	}
	for(int i=0;i<Midd.size();i++)
	{
		int a= (i+j)%Midd.size();
		if(Midd[a].flag == 1)
			continue;
		Readmatrix(a);
		int after = j;
		int before = j;
		while(findNext(after,before))
		{
			mask *p=(mask *)malloc(sizeof(mask));
			p->next =NULL; 
			p->i = after;
			mask *t = Midd[before].next;
			Midd[before].next = p;
			p->next = t;
			//insert
			mask *z=(mask *)malloc(sizeof(mask));
			z->next =NULL; 
			z->i = before;
			mask *x = Midd[after].next;
			Midd[after].next = z;
			z->next = x;
			Readmatrix(after);
		}
	}
}
void setZero()
{
	for(int i=0;i<Midd.size();i++)
	{
		Midd[i].flag = 0;
	}
}
void GeFinalFile(){
	FILE *fp;
	string path =OutFile;
	cout<<OutFile<<endl;
	fp = fopen(path.c_str(), "w");
	int flag = 0;
	int j = 0;
	for(j;j<Midd.size();j++)
	{
		if(Midd[j].next!=NULL&&Midd[j].next->next==NULL)
		{
			if(j!=Midd[j].next->i&&Getlength(Midd[j].A,Midd[Midd[j].next->i].A)>thresholdM)
				break;
		}
	}
	for(int a=0;a<Midd.size();a++)
	{
		int i= (a+j)%Midd.size();
		if(flag !=0)
		{
		fputc('\n', fp);
		flag =0;
		}
		if(Midd[i].flag == 1)
			continue;
		matrix2File(i,fp);
		flag =1;
	}
}
void LP2DEdge(DEdge &E1,LinePara &LP)
{
	extendNode *p=LP.down;
	E1.A = LP.down->C;
	while(p->next!=NULL)
		{
			p=p->next;
		}
	E1.B = p->C;
	E1.flag = LP.flag;
	E1.k =LP.k;
	E1.next = NULL;
	E1.length =Getlength(E1.A,E1.B);
}
void  DeleteCross()
{
	for(int i=0;i<MiddLP.size();i++)
	{
		for(int j=0;j<MiddLP.size();j++)
		{
			DPoint C;
			if(MiddLP[i].down==NULL||MiddLP[j].down==NULL || i == j) 
				continue;
			DPoint A = MiddLP[j].down->C;
			DPoint B = MiddLP[j].down->next->C;
			DPoint E = MiddLP[i].down->C;
			DPoint F =MiddLP[i].down->next->C;
			if(!getCross(MiddLP[i],MiddLP[j],C))
			{
					if(PinEdge(A,B,C)&&PinEdge(E,F,C))
					{
						if(Getlength(A,C)>Getlength(E,C))
						{
							MiddLP[j].down->next->C = C;
						}
						else
						{
							MiddLP[j].down->next->C = C;
							MiddLP[j].down->C = E;
						}
						if(Getlength(F,C)>Getlength(B,C))
						{
							MiddLP[i].down->C = C;
						}
						else
						{
							MiddLP[i].down->C = C;
							MiddLP[i].down->next->C = B;
						}
					}
			}
		}
	}
}
void UpdataMiddLP()
{
	for(int i=0;i<MiddLP.size();i++)
	{
			if(MiddLP[i].down==NULL||MiddLP[i].down->next==NULL)
				continue;
			getLinePara(MiddLP[i].down->C,MiddLP[i].down->next->C,MiddLP[i]);
	}
}
void Deleteoverlap()
{
	double thresholdOver = 0;
	for(int i=0;i<MiddLP.size();i++)
	{
		for(int j=0;j<MiddLP.size();j++)
		{
			DPoint C;
			if(MiddLP[i].down==NULL||MiddLP[j].down==NULL || i == j) 
				continue;
			DPoint A = MiddLP[j].down->C;
			DPoint B = MiddLP[j].down->next->C;
			DPoint E = MiddLP[i].down->C;
			DPoint F =MiddLP[i].down->next->C;
			DEdge E1;
			DEdge E2;
			LP2DEdge(E1,MiddLP[j]);
			LP2DEdge(E2,MiddLP[i]);
			if(Para(MiddLP[j],MiddLP[i]))
			{ 
				DEdge E1;
				DEdge E2; 
				double dis=INT_MAX;
				LP2DEdge(E1,MiddLP[i]);
				LP2DEdge(E2,MiddLP[j]);
				distinct(E1,E2,dis);
				double length1 = Getlength(A,B);
				double length2 = Getlength(E,F);
				int flag = 0;
				if(MiddLP[i].flag != 0)
				{
					if(!(B.x<E.x-thresholdOver||A.x>F.x+thresholdOver) &&dis<=0.5)
					{
						if(E.x<A.x)
							A = E;
						if(F.x>B.x)
							B = F;
						flag =1;
					}
				}
				else
				{
					if(!(B.y<E.y-thresholdOver||A.y>F.y+thresholdOver) &&dis<=0.5)
					{
						if(E.y<A.y)
							A = E;
						if(F.y>B.y)
							B = F;
						flag =1;
					}
				}
				if(flag)
				{
				if(MiddLP[i].flag != 0)
				{
					double k=MiddLP[i].k;
					double b=MiddLP[i].b;
					if(length1>length2)
						{
							k = MiddLP[j].k;
							b = MiddLP[j].b;
						}
					shadow(A,k,b);
					shadow(B,k,b);
				}
				extendNode *p;
				extendNode *z;
				MiddLP[i].down->C = A;
				MiddLP[i].down->next->C = B;
				p = MiddLP[j].down;
				z = p;
				while(z!=NULL)
				{
					z = z->next;
					free(p);
					p = z;
				}
				MiddLP[j].down = NULL;
				}
				
		}
		}
	}
}
void freeLP(){
	mergeNode * p;
	mergeNode * t;
	extendNode *z;
	extendNode *d;
	for(int i = 0;i<MiddLP.size();i++)
	{
		p = MiddLP[i].next;
		t = MiddLP[i].next;
		while(p!=NULL)
		{
			p = p->next;
			free(t);
			t = p;
		}
		z = MiddLP[i].down;
		d = MiddLP[i].down;
		while(z!=NULL)
		{
			z = z->next;
			free(d);
			d = z;
		}
	MiddLP[i].next = NULL;
	MiddLP[i].down =NULL;
	}
}
void freeMidd(){
	for(int i=0;i<Midd.size();i++)
		{
			mask *p = Midd[i].next;
			mask *t = p;
			while(p!=NULL)
			{
				t = p;
				p=p->next;
				free(t);
			}
			Midd[i].next = NULL;
		}
}
void invokeConect(){
	getMiddlLP();
	Deleteoverlap();
	UpdataMiddLP();
	DeleteCross();
	UpdataMiddLP();
	extend();
	deletePoint();
	UpdataMiddLP();
	Gematrix();
	Updatmatrix();
	InsertMatrix();
	setZero();
	GeFinalFile();
	freeMidd();
	freeLP();
} 
int main(int argc,char*argv[])
{
	string fileName(argv[1]);
	int a = fileName.find_last_of('_');
	inFile = fileName; /*  Input file path  */
	OutFile = "./centerline_"+ fileName.substr(a+1);/*  Output file path  */
	invokeScanline();/*  Scanline algorithm  */
	InvokeGetMiddleLine();/* Center line  */
	invokeConect();/* Connected center line */
	return 0;
}
