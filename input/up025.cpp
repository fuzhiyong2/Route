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
class Point{
public:
	int x;
	int y;
};
//最大最小的点，以及所有点的坐标 
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
class DPoint{ 
public:
	double x;
	double y;
};
//Mask用于标记同一个分组的边，用于把边分组 
struct Mask{
Mask *next;//指向同一组的边
int i;//表示同一组的另外一条边 
double dis;//到对应边的距离 
};
//边界线数据结构。线段，点A的x要更小，点B的x要更大，有序排列 ，x相同，按y大小排列。flag == 0表示斜率不存在，还存有边界线长，边界斜率 
struct DEdge{
DPoint A; //点A的x小于点B 
DPoint B;
bool flag;
double length;
double k = INT_MAX; 
Mask *next;
};
//毛坯结果中线存储方式 
struct MiddLine{
	DPoint A;
	DPoint B;
	int i,j;//用于记录哪两条边的中线，用于调试 
};
//-------------用于精修中线---------- 
struct mergeNode{
	mergeNode * next;
	int i;//需要合并线段的边 
	double length;
};
struct extendNode{
	DPoint C;
	int type;//0表示原来的起点，-1表示扩展的点，2表示中间切割的点 ，3表示原来的终点(写入文件的就是1扩展的点 ，输出referline就是2+1，遇到0跳过)
	extendNode *next; 
};
struct LinePara{
	double k;
	double b;
	int flag;//flag == 0 表示斜率不存在 
	mergeNode * next; 
	extendNode *down;
	double length;
};
struct mask{
	int i;//表示对应的点的位置
	mask *next; 
 };
struct matrix{
	DPoint A;
	int flag;//0表示还未遍历，1表示遍历过了 
	mask *next;
};  
//-------------------------扫描线部分使用到的全局变量---------------- 
Polygon polygon;
const int weight=2500;
const int height = 5000;
unsigned long int arr[height][weight]={0};//位图存储扫描线扫描结果 
string num="0";//num表示文件序号，方便调试 ,k表示缩放倍数，
string inFile = "0";
string OutFile = "0"; 
//记录数据范围，用于计算 
int minx = INT_MAX;
int miny = INT_MAX;
int maxx = -INT_MAX;
int maxy = -INT_MAX; 
double k;//缩小倍数 
//---------------------------求中线算法---------------------
double thresholdK= 1e-1;//斜率之差的绝对值小于这个范围的 ，视为平行 
double thresholdPara = 3e-3;//合并平行线，阈值要比thresholdK更小 
double thresholdOverTmp = 5e-1;//两条平行线重合部分小于这个值的，视为没有重合部分, 分组divide的时候使用
double thresholdOverTmp1 = 2e-1;
double thresholdOver = 1e-1;//两条平行线重合部分小于这个值的，视为没有重合部分，extend 中的coin有使用 ,本质上是和thresholdTmp一样 
double thresholdZ = 1e3;//斜率大于2000的线段视为垂线 
double thresholdD = 0.5;//合并同一组线的阈值，自己懂的 
double thresholdDis = 0.1;//extend扩展的时候用，求扩展点可以落在线段内的长度，建议和equal内的阈值一样。 
double thresholdE = 0.5;//equal的阈值 
double thresholdlength = 1.5;//临近边界线的阈值 
double thresholdEx = 10;//和k*minlength一起控制可以延迟的长度 
double thresholdM = 7;
double a = 1.5; // 延长起始边的步长 
vector<DPoint> Dpoin;//读取文件时，形成的一个边界点组成的数组 
vector<DEdge> Dpoly;//边界线结构组成的一个vector结构 
vector<MiddLine> Dmiddle; //存储中线结构的一个数据结构 
vector<LinePara> MiddLP;//一个存储中线和中线上的点的畸形结构体 的vector数组 
vector<matrix> Midd;//一个用于判断是否连通的畸形的数据结构 
bool staEndFlag = 0;//读取文件，文件是否有起始边 
int start=0;//记录起始边中序号较小的序号 
int End = 0;//记录起始边中序号较大的序号 
//---------------------所有部分需要用到的函数---------------------------
string input();//给全局变量num赋值，用于控制输入哪个文件 输入文件序号，方便调试
double Getlength(DPoint start,DPoint end);//求两个点之间的距离 
//---------------------所有部分需要用到的函数---------------------------
//--------------------精修中线算法所需函数----------------
void merge();//合并同一组的边，更新了LP中的down; 
bool equal(DPoint &A,DPoint &B);//判断两个点是否为同一个点（x,y的差小于0.1） 
bool Para(LinePara E1,LinePara E2);//判断线1和线2是否平行  
double getLinePara(DPoint &A,DPoint &B,LinePara &LP);////返回线段长度,得到线段AB的参数LP结构体 
bool Isgroup(LinePara &LP,double length,int j);//表示是否可以合并这两条平行线（LP和MiddLP[j]） 返回0表示不同组，1表示同一组并且可以合并 
bool conin(LinePara &LP,int i,mergeNode *p,double length);//0表示两条平行线可以合并成一条，LP,i表示当前平行线，p指向需要合并的线段的i 
void getMiddlLP();//生成一个MiddlLP的vector。并且更新了LP中的next;
bool getCross(LinePara &E1,LinePara &E2,DPoint &Cross);//得到E1,E2的交点。返回1表示两直线平行 
void LP2DEdge(DEdge &E1,LinePara &LP);
void extend();//完成连通扩展，即更新MiddLP中所有的down。其中extend()中有一个k ,k控制着可以延迟的长度（minlenght * k） 
void Delete2File(string num);//把deletePoint后的MiddLP中的结果写入到DeReferlineFinal 
void insert(int i,DPoint max);//在LP[i].down后，插入扩展的点（按x,y的大小顺序排列） 
void deletePoint();//删掉值一样的点（equal),后面的点和前面一样，则删掉后面的 
void showMergeLP();//调试 show merge 后的MiddLP 中的down点 
void MergeLP2File(string num);//把Merge后的MiddLP中的down存入 ReferlinePara的文件夹中 
void Gematrix();//生成Midd的vector，同时修改稿MiddLP down中的type值
void swap(double &min,double &max);//输入两个数，输出一个是更小的数 
void Updatmatrix();//更新Midd中的next值；
void showUpdataMidd();//调试，展示Updatamatrix后Midd的值 
int Matrix2Point(int i);//调试，输出有顺序的点 
void GeFinalFile(string num);//调用了matrix2File,可写入多个不连通的线段 
int matrix2File(int i,FILE *fp);//把结果输出到文件中 
//-----------------------求中线算法需要用到的函数-----------------------
void txt2DPoint();//读取文本，生成点Dpoin的一个vector 
void DPoint2DEdge();//读取文本，生成Dpoly的一个vector的结构。Dpoly的节点中，A,B有序排列 
	void divide();//分组函数，更新了Dpoly的vector结构 ，并用mindis[]去除一些杂边 
bool SameK(DEdge &E1,DEdge &E2);//判断两条线段是否平行 
DPoint MiddlePoint(DPoint &A,DPoint &B);//求两个点的中点 
void shadow(DPoint &CT,double k,double b);//求一个点在一条不垂直的线上的投影，可优化 
double Getlength(DPoint start,DPoint end);//求两个点之间的距离 
bool overlap(DEdge E1,DEdge E2);//判断线段AB,CD之间是否有overlap; i表示线段AB在Dpoly中的序号，返回0表示没有overlap,可优化 
bool PinPoly(DPoint A);//判断点A是否在多边形内，内部参数i表示精度，可控制大小 
bool inPoly(DPoint A,DPoint B);//判断线段A,B是否在多边形内（即判断两个端点是否在多边形内） 
bool MiddleLine(DEdge &E1,DEdge &E2,int i,int j);//预求两条边的中线，即判断是否用中线且中线在多边形内部    0表示没有。  i用于调试 
double getdis(double k,double b,DPoint &A);//点到一条不是垂直线的距离 
void distinct(DEdge &E1,DEdge &E2,double &dis);//求任意两条平行线的距离 
	void getMiddleLine();//获取中线，成了一个Dmiddle的vector 
double getNewDis(Mask *p,Mask *q);//合并同一组中，相距<thresholdD的平行线，生成新距离的函数。p,q指向第一个平行线和最后一个平行线 
void getParaLine(double &k,double &b,double dis);//获取和线k,b平行，距离为dis的（方向向下）的平行线 
void getLine(Mask *p,Mask *q,MiddLine &tmp,int i);//生成中线的函数（生成了一个Dmiddle的vector中的一个小节点）
void add(int set);//在起始边的位置添加一条和起始边周围两条边平行的线 
	void Dmid2File(string num);//把结构求出的中线结构写入Referline文件夹下 
void InvokeGetMiddleLine();//调用求中线函数 
void free();//释放Dpoly动态申请的内存 
void dedis(DEdge &E1,DEdge &E2);//调试求两条平行且有overlap的平行线的距离 ，（两条线不为垂直线） 
void showDpoint();//调试，输出读取的点的DPoin的值
void showDEdge();//调试，输出DPoly的值
void showD();//调试，输出分组后DPoly的值 
void showMidd();//调试，输出Dmiddle的值 
void deGetLine(Mask *p,Mask *q,Mask *min,Mask *max,MiddLine &tmp,int i,double dis,double &k,double &b,DPoint &AA,DPoint &AB);//调试生成合并同一组边后，生成的新边 
//-------------------------------------扫描线算法用到的函数和数据结构-------------------------------------------------------------- 
void getK(double &k); //k为全局变量，表示缩小倍数 
void set(int i,int j);//i表示y,j表示x
int get(int i,int j);//获得i，j位置的i 
void txtToPolygon(double k,int offsetx,int offsety);//把输入文件转化成 polygon的数据结构 ，用于扫描线算法  polygon是一个结构体 
void GetOrigion(int &MinX,int &MinY,int &MaxX,int &MaxY);//获得所给点中的最大最小值 
void array2file(string num);//把扫描线的结果数组 转化成文件  
void PolyScan(int &offsetx, int &offsety, int &refrain_minx, int &refrain_maxx, int &refrain_miny, int &refrain_maxy, int &MinY, int &MaxY,unsigned long int arr[][weight], Polygon &poly);//扫描线算法。offsetx表示arr的偏移，不用理会，默认为0。refrain构成一个特定范围的画布，为绝对范围，后面的就是多边形，arr表示返回的数组。
void invokeScanline();//调用Scanline,生成一个扫描线的数组 
//-------------------------------------扫描线算法用到的函数和数据结构-------------------------------------------------------------- 
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
	//生成静态边 ，把静态边给填充起来 
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
		//pAET动态表  pAET最开始是null
		// 更新AET中边的x值，进入下一循环
		NET *p = pAET->next;
		while(p){
			p->x = p->x + p->dx;
			p = p->next;
		}
		//对AET表中的x进行一个从小到大的排序 
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
		//删除AET中满足y=ymax的边
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
		//找到合适的位置插入pAET 
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
		//填充色彩 
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
				//showTian(i-offsety,crosspoint1-offsetx,crosspoint2-offsetx);
				for(int j=crosspoint1; j<=crosspoint2; ++j){
				//	arr[i-offsety][j-offsetx] = 1;
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
//已经缩小了k倍 
void txtToPolygon(double k){
	ifstream infile;
	infile.open(inFile);
	string type;
	int count = 0;
	Point tmp; 
	tmp.x=INT_MAX;
	tmp.y =INT_MAX;
	DPoint tmp1;
	while (infile !=NULL) // line中不包括每行的换行符  
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
	while (infile !=NULL) // line中不包括每行的换行符  
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
//-------------------------------------扫描线算法用到的函数和数据结构-------------------------------------------------------------- 
//string input(){
//	cout<<"Enter the data order"<<endl;
//	int a;
//	cin>>a;
//	char b[1];
//	itoa(a,b,10);
//	string num = b;
//	inFile = "./boundary_"+num+".txt";
//	OutFile ="./centerline_"+num+".txt";
//	return num;
//}
void getK(double &k){
	double a=(maxy - miny)* 1.0f/(height*4);
	double c=(maxx - minx)* 1.0f/(weight*8);
	 k = a>c?a:c;
	if (k<1)
	  k =1;
}  
void invokeScanline(){
	//num = input();
	GetOrigion();
	getK(k);
	cout<<k<<endl;
	cout<<"RUN READ"<<endl;
	txtToPolygon(k);
	cout<<"RUN PolyScan"<<endl;
	PolyScan(minx, miny, polygon.minp.x, polygon.maxp.x, polygon.minp.y, polygon.maxp.y, polygon.minp.y, polygon.maxp.y, arr, polygon);
} 
//-------------------------------------中线算法用到的函数和数据结构-------------------------------------------------------------- 
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
	//过CT，求垂线
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
//	if(A.x==-31146.02&&C.x==-31149.02)
//		{
//				cout<<"test"<<(minyE2>=maxyE1-thresholdOver || maxyE2-thresholdOver<=minyE1)<<endl;
//				cout<<minyE1<<","<<minyE2<<endl;
//				cout<<maxyE1<<","<<maxyE2<<endl;
//				cout<<"1" <<((minxE1>minxE2&&minxE1<maxxE2)||(minyE1>minyE2&&minyE1<maxyE2))<<endl;
//			}
//	if( !(minxE2>=(maxxE1)||(maxxE2)<=minxE1) || !((minyE2>=maxyE1|| maxyE2<=minyE1)) )
//	{
//		return 1;
//	}
//	double dis;
//	distinct(E1,E2,dis);
//	double S = sqrt(fabs((C.x-A.x)*(D.y-B.y)));
////	cout<<S<<endl;
//	cout<<fabs(S/length2)<<",l"<<length2<<endl;
//	if(fabs( (length1/length2)-1)<0.1 && fabs(S/length2)>length2) 
//	{
//		thresholdOver = -length2/2;
//	//	cout<<dis<<endl;
//	}
	if(flag == 0)
	{
//		if(A.x==-804.98&&B.x==-801.98)
//			cout<<"test"<<(minyE2>=maxyE1-thresholdOver || maxyE2-thresholdOver<=minyE1)<<endl;
		if((minyE2>=maxyE1-thresholdOver || maxyE2-thresholdOver<=minyE1) &&(minyE2>=maxyE1-thresholdOver || maxyE2-thresholdOver<=minyE1))
			return 0;
	}
	else
	{
	//其他情况投影后比较x就行了 
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
//bool PinPoly(DPoint A)
//{
//	//i表示精度 
//	int i= 1 ;
//	if( get(int((A.y - miny)/k),int((A.x - minx)/k))== 1 || (get(int((A.y - miny)/k+i),int((A.x - minx)/k)) == 1&&int((A.y - miny)/k+i<height))|| (get(int((A.y - miny)/k),int((A.x - minx)/k+i))== 1&&int((A.x - minx)/k+i)<weight)) 
//		return 1;
//	if((get(int((A.y - miny)/k)-i,int((A.x - minx)/k))== 1 &&int((A.y - miny)/k)-i>=0)||(get(int((A.y - miny)/k),int((A.x - minx)/k)-i) == 1&&int((A.x - minx)/k)-i>=0))
//		return 1;
//	return 0;
//}
//??????????????????????起作用了吗？ 
bool PinPoly(DPoint A){
	//return 1;
	//i表示精度 周围的4个点有一个在多边形内就算 
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
//	if((y-i)>=0&&(y+i)<4*height&&(x+i)<8*weight&&(x-i)>=0)
//	{
//		if( get(y,x)== 1 || get(y+i,x)== 1||get(y+i,x+i) == 1 ) 
//			return 1;
//		if(get(y-i,x) == 1||get(y,x+i)==1 ||get(y,x-i) == 1)
//			return 1;
//	}
//	else if((y-i)>=0)
//	{
//		if(get(y,x)== 1||get(y-i,x)==1)
//			return 1;
//	}
//	else if((y+i)<4*height)
//	{
//		if(get(y,x)== 1||get(y+i,x)==1)
//			return 1;
//	}
//	else if((x-i)>=0)
//	{
//		if(get(y,x)== 1||get(y,x-i)==1)
//			return 1;
//	}
//	else if((x+i)<8*weight)
//	{
//		if(get(y,x)== 1||get(y,x+i) == 1)
//			return 1;
//	}
//	else
//	{
//		return 0;
//	}
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
void showDpoint(){
	cout<<"------------------show Point------------------------"<<endl;
	cout<<"Dpoint size "<< Dpoin.size()<<endl;
	for(int i=0;i<Dpoin.size();i++){
		cout<<Dpoin[i].x<<" , "<<Dpoin[i].y<<endl;
	}
	cout<<"------------------end-----------------------------"<<endl;
}
void showDEdge(){
	cout<<"------------------show DEdge------------------------"<<endl;
	cout<<"DEdge size "<< Dpoly.size()<<endl;
	for(int i=0;i<Dpoly.size();i++){
		cout<<" i=  "<< i <<"  "<<Dpoly[i].A.x<<" , "<<Dpoly[i].A.y<< " | "<<Dpoly[i].B.x<< " , " << Dpoly[i].B.y<<endl;
	}
	cout<<"------------------end-----------------------------"<<endl;
} 
void showD(){
	cout<<"------------------show 分组------------------------"<<endl;
	Mask * p;
	for(int i = 0;i<Dpoly.size();i++)
	{
		
		//cout<<"i="<<i<<"  "<<Dpoly[i].A.x - minx<<" , "<<Dpoly[i].A.y - miny<< " | "<<Dpoly[i].B.x- minx<< " , " << Dpoly[i].B.y-miny<<" length = "<<Dpoly[i].length<<endl;
		cout<<"i="<<i<<"  "<<fixed<<setprecision(2)<<Dpoly[i].A.x<<" , "<<Dpoly[i].A.y << " | "<<Dpoly[i].B.x<< " , " << Dpoly[i].B.y<<" length = "<<Dpoly[i].length<<" k "<<Dpoly[i].k<<"flag"<<Dpoly[i].flag<<endl;
		p = Dpoly[i].next;
		while(p!=NULL)
		{
			cout<<p->i<<" dis "<<p->dis<<"  ,  ";
			p = p->next;
		}
		cout<<endl;
		
	}
	cout<<"------------------end-----------------------------"<<endl;
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
		//cout<<(b2-b1)<<endl;
		dis = (dis1+dis2)/2;
	}
}
void SameGroup(int i,int j,double mindis[])
{
	    double dis=0;
	    //cout<<i<<","<<j<<endl;
		if(MiddleLine(Dpoly[i],Dpoly[j]))
				{
				//	cout<<"RUN sameGroup"<<i<<" "<<j<<endl;
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
					//距离按从小到大排序
					Mask *q=Dpoly[a].next;
					Mask *t=Dpoly[a].next;
					int count = 0;
//					 if(i==760&&j==768)
//				    {
//				    	cout<<"OK"<<endl;
//					}
					while(q!=NULL)
					{
						if(p->dis<=q->dis)
							break;
						t = q;
						q = q->next;
						count++;
					} 
//					 if(i==760&&j==769)
//				    {
//				    	cout<<"OK4"<<endl;
//					}
					//插入t后，q前
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
//					 if(i==760&&j==769)
//				    {
//				    	cout<<"OK5"<<endl;
//					}
				//	cout<<"RUN MiddLELine"<<endl;	
				}
}
//相邻的边不可能互相对应 
void divide(){
	double mindis[Dpoly.size()];
	for(int i=0;i<Dpoly.size();i++)
	{
		mindis[i] = INT_MAX;
	}
	//cout<<"Dpoly.size"<<endl;
	if(staEndFlag==0)
	{
		//int count=0;
		//cout<<"Dpoly.size"<<Dpoly.size()<<endl; 
		for(int i = 0;i<Dpoly.size();i++)
		{
		//	count++;
		for(int j=i+2;j<Dpoly.size();j++)
			{
				SameGroup(i,j,mindis);
			}
		}
	//	cout<<"i= "<<count<<endl;
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
    				//cout<<"RUN samGroup2"<<endl;
    				SameGroup(i,j,mindis);
				}
			}
		}
	}
	cout<<"RUN out divide"<<endl;
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
	 //找到x最小的点,和x最大的边,用下标i来记录 (获取长度范围)
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
	 //垂直的话 ，直接比较x,减x
	 double k = Dpoly[i].k;
	 double b = 0;
	 if(Dpoly[i].flag == 0)
	 {
	 	double sub1 = Newdis - min->dis;
	 	double sub2 = Newdis - max->dis;
	 	//点在直线左边还是右边 
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
	 	//点在直线下方 
	 	if(Dpoly[min->i].A.y < (Dpoly[min->i].A.x*k+b))
		{
			Newdis = -Newdis;
		}
		getParaLine(k,b,-Newdis);
		shadow(A,k,b);
		shadow(B,k,b);
	 }
//	 tmp.A = MiddlePoint(Dpoly[min->i].A,Dpoly[i].A);
//	 tmp.B = MiddlePoint(Dpoly[max->i].B,Dpoly[i].B);
	tmp.A = MiddlePoint(A,Dpoly[i].A);
	tmp.B = MiddlePoint(B,Dpoly[i].B);
	//deGetLine(tp,q,min,max,tmp,i,Newdis,k,b,A,B);
	  
}
void showMidd(){
	cout<<"-------------------show Middle line-----------"<<endl;
	for(int i=0;i<Dmiddle.size();i++)
	{ 
		cout<<i<<" "<<"i = "<<Dmiddle[i].i<< ",j ="<<Dmiddle[i].j<<"("<<Dmiddle[i].A.x<<" , "<<Dmiddle[i].A.y<<" , "<< Dmiddle[i].B.x<<" ,"<<Dmiddle[i].B.y<<")"<<endl;
	}; 
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
//	cout<<PinPoly(endP1)<<endl;
//	cout<<PinPoly(endP2)<<endl;
//	cout<<"End1"<<endP1.x<<","<<endP1.y<<endl;
//	cout<<"End2"<<endP2.x<<","<<endP2.y<<endl;
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
//? 
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
			//加一个判定 
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
			tmp.j = -1;//表示多条 
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
void Dmid2File(string num){
	FILE *fp;
	string path = "../Referline/referline_"+num+".txt";
	fp = fopen(path.c_str(), "w"); 
	for(int i=0;i<Dmiddle.size();i++)
	{ 
		//cout<<"("<<Dmiddle[i].A.x<<" , "<<Dmiddle[i].A.y<<" , "<< Dmiddle[i].B.x<<" ,"<<Dmiddle[i].B.y<<")"<<endl;
		fprintf(fp, "%f ",Dmiddle[i].A.x);
		fprintf(fp, "%f ",Dmiddle[i].A.y);
		fprintf(fp, "%f ",Dmiddle[i].B.x);
		fprintf(fp, "%f ",Dmiddle[i].B.y);
		fputc('\n', fp);
	};
}
void PrD(DPoint A)
{
	cout<<fixed<< setprecision(2)<<A.x<<","<<A.y<<endl;
}
void InvokeGetMiddleLine(){
	cout<<"DPoint2DEdge OK"<<endl;
	DPoint2DEdge();
	cout<<"divide OK"<<endl;
	divide();
//	DEdge E1 = Dpoly[1658];
//	DEdge E2 = Dpoly[1656];
//	cout<<"lenght"<<E1.length<<endl;
//	cout<<E1.k<<endl;
//	PrD(E1.A);
//	PrD(E2.A);
//	PrD(MiddlePoint(E1.A,E2.A)) ;
//	PrD(MiddlePoint(E1.B,E2.B));
//	DPoint A = MiddlePoint(E1.A,E2.A);
//	cout<<PinPoly(MiddlePoint(E1.A,E2.A))<<endl;
//	cout<<PinPoly(MiddlePoint(E1.B,E2.B))<<endl;
//	cout<<inPoly(MiddlePoint(E1.A,E2.A),MiddlePoint(E1.B,E2.B))<<endl;
//	if(!SameK(E1,E2))
//		cout<<"not Paraller"<<endl;
//	if(!overlap(E1,E2))
//		cout<<"not overlap"<<endl;
//	if(!inPoly(MiddlePoint(E1.A,E2.A),MiddlePoint(E1.B,E2.B)))
//		cout<<"not in Poly"<<endl; 
//	int y = int((A.y - miny)/k);
//	int x = int((A.x - minx)/k);
//	cout<<x<<","<<y<<endl;
//	cout<<maxx-minx<<","<<maxy-miny<<endl;
//		int  i =1;
//	cout<<((y-i)>=0&&(y+i)<4*height&&(x+i)<8*weight&&(x-i)>=0)<<endl;
//	cout<<get(y,x)<<endl;
//	cout<<get(y,x-i)<<endl;
//	cout<<get(y,x+i)<<endl;
//	cout<<get(y+i,x)<<endl;
//	cout<<get(y-i,x)<<endl; 
	showD();
	cout<<"getMiddLeLine OK"<<endl;
   	getMiddleLine();
   	//Dmid2File(num);
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
//----------------------精修中线算法的函数-------------------
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
	// 计算分子  
	m = x2 - x1;
	//这里的k表示无穷大 
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
	//cout<<"run"<<" dist "<<dist<<" length "<<minlength<<endl;
	if(LP.flag == 0)
	{
		//没有重合 
		if((minyE2>=maxyE1-thresholdOver || maxyE2-thresholdOver<=minyE1) && dist>=minlength )
			return 0;
	}
	else
	{
		//没有重合且dis>=minlength 
		if( (minxE2>=(maxxE1 - thresholdOver)||(maxxE2 - thresholdOver)<=minxE1) && dist>=minlength)
			return 0;
	}
	//cout<<"return 0";
	return 1;
	
	//求出两条边的长度 
} 
bool PinEdge(DPoint A,DPoint B,DPoint C)
{
	if((C.x>A.x &&C.x<B.x)||(C.y>A.y&&C.y<B.y))
		return 1;
	return 0;
}
//想改  
bool Isgroup(LinePara &LP,double length,int j){
	int flag = 0;
	int record = -1;
	for(int i=0;i<MiddLP.size();i++)
	 {
	 	DPoint C;
		if(Para(LP,MiddLP[i]) && flag ==0 )
			{
				//平行的情况 
				//cout<<"run---------"<<endl;
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
					//double threshold = length1>length2?
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
								//插入
								record = i;
								flag =1;
								//插入点
								insert(i,Dmiddle[j].A);
								insert(i,Dmiddle[j].B);
								extendNode *q = MiddLP[i].down;
								//更新LP中的k和B
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
					//插入mergeNode 
//					mergeNode *p=MiddLP[i].next;
//					while(p!=NULL)
//					{
//						if(conin(LP,MiddLP[i]))
//						{
//							//插入
//							record = i;
//							flag =1;
//							//插入点
//							insert(i,Dmiddle[j].A);
//							insert(i,Dmiddle[j].B);
//							extendNode *q = MiddLP[i].down;
//							//更新LP中的k和B
//							DPoint min=q->C;
//							while(q->next!=NULL)
//							{
//								q = q->next;
//							}
//							DPoint max = q->C;
//							q = MiddLP[i].down;
//							extendNode *t = q;
//							while(q !=NULL)
//							{
//								t = q;
//								q = q->next;
//								free(t);
//							}
//							getLinePara(min,max,MiddLP[i]);
//							break;
//						}
//						p=p->next;
//					}
				}
			}
//		else
//			{
//				if(!getCross(MiddLP[i],LP,C))
//				{
//					DPoint A = LP.down->C;
//					DPoint B = LP.down->next->C;
//					DPoint E = MiddLP[i].down->C;
//					DPoint F =MiddLP[i].down->next->C;
//					if(PinEdge(A,B,C)&&PinEdge(E,F,C))
//					{
//						if(Getlength(A,C)>Getlength(B,C))
//						{
//							LP.down->next->C = C;
//						}
//						else
//						{
//							LP.down->C = C; 
//						}
//						if(Getlength(E,C)>Getlength(F,C))
//						{
//							MiddLP[i].down->next->C = C;
//						}
//						else
//						{
//							MiddLP[i].down->C = C; 
//						}
//						
//					}
//				}
//			}
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
//0表示不平行有交点 
bool getCross(LinePara &E1,LinePara &E2,DPoint &Cross){
	if(E1.down==NULL||E2.down==NULL)
		return 1;
	if(fabs(E1.k-E2.k)<thresholdPara|| (E1.flag==0&&E2.flag == 0))
	{
		return 1;
	}
//	else if(fabs(E1.k-E2.k)<thresholdK || (E1.flag==0&&E2.flag == 0))
//	{
//		extendNode *p=E1.down;
//		DPoint start;
//		DPoint end;
//		DPoint start1;
//		DPoint end1;
//		GetSPandEnd(p,start,end);
//		double length1 = Getlength(start,end);
//		p = E2.down;
//		GetSPandEnd(p,start1,end1);
//		if(overlap(start,end,start1,end1,E1.k,E1.flag))
//			return 1;
//	    double length2 = Getlength(start1,end1);
//	    double thresholdDis = length1>length2?length2:length1;
//	    double dis=INT_MAX;
//	    length1 = Getlength(start,end1);
//		length2 = Getlength(start1,end);
//		if(length1<length2) 
//		{
//			dis = length1;
//			Cross = MiddlePoint(start,end1);
//		}
//		else
//		{
//			dis = length2;
//			Cross = MiddlePoint(start1,end);
//		}
//	    if(thresholdDis>dis&&dis<thresholdEx)
//	    	return 0;
//		return 1;
//	}
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
//		if(fabs(E2.k-E1.k)>=thresholdK || (fabs(E2.k-E1.k)>fabs(E2.k)&&fabs(E2.k-E1.k)>fabs(E1.k)))
//		{
//			Cross.x =(E1.b-E2.b)/(E2.k - E1.k);
//			Cross.y = E1.k*Cross.x + E1.b;
//		}
//		else if(E1.k == 0)
//		{
//			Cross.y = E1.down->C.y;
//			Cross.x = (Cross.y - E2.b)/E2.k;
//		}
//		else if(E2.k == 0)
//		{
//			Cross.y = E2.down->C.y;
//			Cross.x = (Cross.y - E1.b)/E1.k;
//		}
//		else if(fabs(E1.k)>fabs(E2.k))
//		{
//			double z = E2.k/E1.k;
//			Cross.y = (E2.b-z*E1.b)/(1-z);
//			Cross.x = (Cross.y - E1.b)/E1.k;
//		}
//		else
//		{
//			double z = E1.k/E2.k;
//			Cross.y = (E1.b-z*E2.b)/(1-z);
//			Cross.x = (Cross.y - E2.b)/E2.k;
//		}
	    return 0;
		}
		else
		{
			//E1垂直 
			if(E1.flag == 0)
			{
				Cross.x = E1.down->C.x;
				Cross.y = E2.k*Cross.x + E2.b;
			} 
			else
			{
			//E2垂直 
				Cross.x = E2.down->C.x;
				Cross.y = E1.k*Cross.x + E1.b;
			}
		}
		return 0;
	} 
}
void UpdataMinMax(int j,DPoint C,DPoint &min,int &minJ,int k){
		//找到j对应的起点和终点 
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
			//找到需要扩展的长度minlength 
			double minlength = Getlength(C,s->C);
			if(C.x>s->C.x&&C.x<e->C.x)
				minlength = 0;
			else if(C.x<s->C.x)
				minlength = Getlength(C,s->C);
			else
				minlength = Getlength(C,e->C);
			double lengthJ = Getlength(s->C,e->C);
//			if(k*lengthJ<=5)
//			{
//				lengthJ = 5;
//			}
			//符合以上条件，则更新 
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
//			if(k*lengthJ<=5)
//			{
//				lengthJ = 5;
//			}
			//符合以上条件，则更新 
			if(minlength<k*lengthJ &&minlength<thresholdEx)
			{
				min = C;
				minJ = j;
			}
		}
}
void extend(){
	//延长线的长度，由k*minlength 和  5控制 
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
		//cout<<"RUN extend"<<endl;
		DPoint C;
		for(int j=0;j<MiddLP.size();j++)
		{
			C.x = INT_MAX;
			C.y = INT_MAX;
			if(MiddLP[j].down == NULL)
				continue;
			//平行 
			//相交 
			if(i==j)
				continue;
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
//				DEdge E1;
//				DEdge E2;
//				LP2DEdge(E1,MiddLP[i]);
//				LP2DEdge(E2,MiddLP[j]);
//				if(SameK(E1,E2))
				else
				{
					//如果没有重合区间则 
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
//			
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
//				DEdge E1;
//				DEdge E2;
//				LP2DEdge(E1,MiddLP[i]);
//				LP2DEdge(E2,MiddLP[j]);
//				if(SameK(E1,E2))
				else
				{
					//int flag = 0;
					DPoint start1;
					DPoint end1;
					GetSAndE(start1,end1,MiddLP[j]);
					double thresholdOver = 0;
					if(end.x<=start1.x-thresholdOver||start.x>=end1.x+thresholdOver)
					{
						//flag =1;
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
		//最大的点找到最大的地方插入。
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
			//两个插入 
			insert(i,max);
			insert(maxJ,max);
		}
		GetSAndE(A,B,MiddLP[minJ]);
		if(minlength<=threshold/5 &&!getCross(MiddLP[i],MiddLP[minJ],X) &&minlength<=thresholdEx&&!PinEdge(A,B,min))
		{
		 	//两个插入
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
		//cout<<"  i  "<<i<<"  k:  "<<MiddLP[i].k<<"  b:  "<<MiddLP[i].b<<"  flag:  "<<MiddLP[i].flag<<"   :   "<<endl;
		//DPoint tmp =
		q = MiddLP[i].down;
		//cout<<"Yes"<<endl;
		if(q==NULL)
			continue;
//		if(MiddLP[i].length<0.6)
//		{
//			MiddLP[i].down=NULL;
//			continue;
//		}
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
//		if(equal(tmp,MiddLP[i].down->C))
//			insert(i,tmp);
		//cout<<"----------------"<<endl; 
	}
};
void showMergeLP(){
	cout<<"------------show Merge后的 MiddLP-----------------"<<endl;
	extendNode * q;
	for(int i = 0;i<MiddLP.size();i++)
	{
		cout<<"  i  "<<i<<"  k:  "<<MiddLP[i].k<<"  b:  "<<MiddLP[i].b<<"  flag:  "<<MiddLP[i].flag<<"   :   length "<<MiddLP[i].length<<endl;
		q = MiddLP[i].down;
		while(q!=NULL)
		{
			cout<<"  点:  "<<q->C.x<<"  ， "<<q->C.y<<", type "<< q->type<<endl;
			q = q->next;
		}
		cout<<"----------------"<<endl; 
	}
}
void showExtendLP(){
	cout<<"------------show Extend后的 MiddLP-----------------"<<endl;
	extendNode * q;
	for(int i = 0;i<MiddLP.size();i++)
	{
		cout<<"  i  "<<i<<"  k:  "<<MiddLP[i].k<<"  b:  "<<MiddLP[i].b<<"  flag:  "<<MiddLP[i].flag<<"   :   "<<endl;
		q = MiddLP[i].down;
		while(q!=NULL)
		{
			cout<<"  点:  "<<q->C.x<<"  ， "<<q->C.y<<", type "<< q->type<<endl;
			q = q->next;
		}
		cout<<"----------------"<<endl; 
	}
}
void showDeleteLP(){
	cout<<"------------show Delete后的 MiddLP-----------------"<<endl;
	extendNode * q;
	for(int i = 0;i<MiddLP.size();i++)
	{
		cout<<"  i  "<<i<<"  k:  "<<MiddLP[i].k<<"  b:  "<<MiddLP[i].b<<"  flag:  "<<MiddLP[i].flag<<"   :   "<<endl;
		q = MiddLP[i].down;
		while(q!=NULL)
		{
			cout<<"  点:  "<<q->C.x<<"  ， "<<q->C.y<<", type "<< q->type<<endl;
			q = q->next;
		}
		cout<<"----------------"<<endl; 
	}
}
void MergeLP2File(string num){
	FILE *fp;
	string path = "../ReferlinePara/referline_"+num+".txt";
	fp = fopen(path.c_str(), "w"); 
	extendNode * q;
	for(int i=0;i<MiddLP.size();i++)
	{ 
		//cout<<"("<<Dmiddle[i].A.x<<" , "<<Dmiddle[i].A.y<<" , "<< Dmiddle[i].B.x<<" ,"<<Dmiddle[i].B.y<<")"<<endl
		q = MiddLP[i].down;
		if(q==NULL)
			continue;
		while(q!=NULL)
		{
			fprintf(fp, "%f ",q->C.x);
			fprintf(fp, "%f ",q->C.y);
			q = q->next;
		}
		fputc('\n', fp);
	};
}
void Gematrix(){
	matrix tmp;
	extendNode * q;
	for(int i = 0;i<MiddLP.size();i++)
	{
		//cout<<"  i  "<<i<<"  k:  "<<MiddLP[i].k<<"  b:  "<<MiddLP[i].b<<"  flag:  "<<MiddLP[i].flag<<"   :   "<<endl;
		q = MiddLP[i].down;
		while(q!=NULL)
		{
			//cout<<"  点:  "<<q->C.x<<"  ， "<<q->C.y<<", type "<< q->type<<endl;
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
		//cout<<"----------------"<<endl; 
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
//		int before = q->type;
//		int after = q->next->type;
		if(q==NULL)
			continue;
		while(q->next!=NULL)
		{
			int before = q->type;
			int after = q->next->type;
			swap(before,after); 
//			if(i == 1)
//				cout<<"adfaf"<<before<<" , "<<after<<endl;
			mask *p=(mask *)malloc(sizeof(mask));
			p->next =NULL; 
			p->i = after;
			//在Midd[q->type]后插入p 
			mask *t = Midd[before].next;
			Midd[before].next = p;
			p->next = t;
		//	q = q->next;
			//插入
			mask *z=(mask *)malloc(sizeof(mask));
			z->next =NULL; 
			z->i = before;
			//在Midd[q->type]后插入p 
			mask *x = Midd[after].next;
			Midd[after].next = z;
			z->next = x;
			q = q->next; 
		}
	}
}
void showUpdataMidd(){
	cout<<"show------Updata---Midd----------------------"<<endl;
	for(int i=0;i<Midd.size();i++)
		{
			cout<<i<<","<<Midd[i].A.x<<","<<Midd[i].A.y<<" flag "<<Midd[i].flag<<endl;
			mask *p = Midd[i].next;
			while(p!=NULL)
			{
				cout<<"  i "<<p->i<<"  ";
				p=p->next;
			}
			cout<<endl;
		}
}
int Matrix2Point(int i){
	if(Midd[i].flag == 1)
		return 0;
	mask * r = Midd[i].next;
	cout<<"i"<<i<<" "<<Midd[i].A.x<<","<<Midd[i].A.y<<endl; 
	Midd[i].flag =1;
//	mask *p = r;
//	if(p->next ==NULL &Midd[r->i].flag==1)
//		cout<<"-2 , -2"<<endl; 
	while(r!=NULL)
	{
		if(Midd[r->i].flag ==0)
		{
			Matrix2Point(r->i); 
		if(r->next!=NULL &&Midd[r->next->i].flag==0)
			{
				cout<<"-2 , -2"<<endl; 
				cout<<"i"<<i<<" "<<Midd[i].A.x<<","<<Midd[i].A.y<<endl; 
			}
		}
		r = r->next;
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
bool JPinPoly(DPoint A){
	//i表示精度 周围的4个点有一个在多边形内就算 
	int i= 1 ;
	double y = int((A.y - miny)/k);
	double x = int((A.x - minx)/k);
	if( !(x/8>0&&x/8<=weight&&y/4>0&&y/4<=height) )
	{
		return 0;
	}
	else
	{
		if(get(y,x)== 1)
			return 1;
	}
	return 0;
}
//改变A点直到outPoly 
DPoint addLength(DPoint A,DPoint B,int &time)
{
	time++;
	if(time == 4)
	   return A;
	cout<<A.x<<","<<A.y<<endl;
	DPoint C;
	if(A.x == B.x)
	{
		C.x = A.x;
		if(A.y<B.y)
		{
			C.y = A.y-a;
			if(!JPinPoly(C))
				return A;
			else
				return addLength(C,B,time);
		}
		else
		{
			C.y = A.y+a;
			if(!JPinPoly(C))
				return A;
			else
				return addLength(C,B,time);
		}
	}
	else
	{
		double k=(A.y - B.y)/(A.x-B.x);
		double b =A.y - k*A.x;
		if(A.x<B.x)
		{
			C.x = A.x -a;
			C.y = k*C.x + b;
			if(!JPinPoly(C))
			{
				return A;
			}
			else
			{
				return addLength(C,B,time);
			}
		}
		else
		{
			C.x =A.x +a;
			C.y = k*C.x + b;
			if(!JPinPoly(C))
			{
				return A;
			}
			else
			{
				return addLength(C,B,time);
			}
		}
	}
}
int matrix2File(int i,FILE *fp){
//	cout<<" i "<< i<<" "<<Midd[i].A.x<<" , "<<Midd[i].A.y<<endl;
//	if (i == 208)
//		cout<<"test happy"<<endl;
	if(Midd[i].flag == 1)
		return 0;
	mask * r = Midd[i].next;
	Midd[i].flag =1;
//处理终止边和杂边的步骤 
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
		cout<<"length 终:"<<length<<endl;
		if(length>=thresholdM)
		{
		DPoint C=Midd[i].A;
		PrD(C);
		int time = 0;
		//C = addLength(Midd[i].A,Midd[p->i].A,time);
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
//				fprintf(fp, "%f ",-2.0);
//				fprintf(fp, "%f ",-2.0);
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
		//cout<<"a"<<a<<endl;
		Readmatrix(a);
		int after = j;
		int before = j;
		while(findNext(after,before))
		{
			mask *p=(mask *)malloc(sizeof(mask));
			p->next =NULL; 
			p->i = after;
			//在Midd[q->type]后插入p 
			mask *t = Midd[before].next;
			Midd[before].next = p;
			p->next = t;
		//	q = q->next;
			//插入
			mask *z=(mask *)malloc(sizeof(mask));
			z->next =NULL; 
			z->i = before;
			//在Midd[q->type]后插入p 
			mask *x = Midd[after].next;
			Midd[after].next = z;
			z->next = x;
//			cout<<"next"<<next<<endl;
//			cout<<"before"<<before<<endl;
			cout<<"j"<<j<<endl;
			Readmatrix(after);
		}
		//cout<<"i"<<endl;
	}
}
void setZero()
{
	for(int i=0;i<Midd.size();i++)
	{
		Midd[i].flag = 0;
	}
}
void GeFinalFile(string num){
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
		//cout<<"a  "<<a<<" i "<<i<<endl;
		if(flag !=0)
		{
//		fprintf(fp, "%f ",-3.0);
//		fprintf(fp, "%f ",-3.0);
		fputc('\n', fp);
		flag =0;
		}
		if(Midd[i].flag == 1)
			continue;
//		cout<<"i"<<i<<endl;
//		int next = j;
//		int before = j;
//		while(findNext(next,before))
//		{
//
//					fputc('\n', fp);
//					fprintf(fp, "%f ",Midd[before].A.x);
//					fprintf(fp, "%f ",Midd[before].A.y);
//					fputc('\n', fp);
//			cout<<"next"<<next<<endl;
//			cout<<"before"<<before<<endl;
//			cout<<"j"<<j<<endl;
//			matrix2File(next,fp);
//		}
		matrix2File(i,fp);
		flag =1;
		//cout<<"i"<<endl;
	}
}
void Delete2File(string num){
	FILE *fp;
	string path = "../DeReferlineFinal/referline_"+num+".txt";
	fp = fopen(path.c_str(), "w");
	for(int i=0;i<MiddLP.size();i++)
	{
		extendNode *down;
		down = MiddLP[i].down;
		if(down==NULL)
			continue;
		fprintf(fp, "%f ",down->C.x);
		fprintf(fp, "%f ",down ->C.y);
		while(down->next!=NULL)
		{
			down=down->next;
		}
		fprintf(fp, "%f ",down->C.x);
		fprintf(fp, "%f ",down ->C.y);
		fputc('\n', fp);
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
		//cout<<"i"<<i<<endl;
		for(int j=0;j<MiddLP.size();j++)
		{
			//第一种情况有交点 
			DPoint C;
			if(MiddLP[i].down==NULL||MiddLP[j].down==NULL || i == j) 
				continue;
			DPoint A = MiddLP[j].down->C;
			DPoint B = MiddLP[j].down->next->C;
			DPoint E = MiddLP[i].down->C;
			DPoint F =MiddLP[i].down->next->C;
			if(!getCross(MiddLP[i],MiddLP[j],C))
			{
					if(i==91&&j==92)
						{
							cout<<"inEdge"<<(PinEdge(A,B,C)&&PinEdge(E,F,C))<<endl;
						cout<<"j"<<j<<endl;
						cout<<"i"<<i<<endl; 
						PrD(C);
						}
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
//					cout<<"j"<<j<<endl;
//					cout<<"i"<<i<<endl; 
//					cout<<"有交点"<<endl;
//					cout<<C.x<<","<<C.y<<endl;
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
		//cout<<"i"<<i<<endl;
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
			//deletePoint();
			//if(SameK(E1,E2))
			if(Para(MiddLP[j],MiddLP[i]))
			{ 
				if(i==66&&j==67)
				{
					PrD(A);
					PrD(B);
					PrD(E);
					PrD(F);
					cout<<(B.x<=E.x-thresholdOver||A.x>=F.x+thresholdOver)<<endl;
					cout<<MiddLP[i].k<<endl;
					cout<<MiddLP[j].k<<endl;
				}
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
				if(i==3&&j==0)
				{
					cout<<"-----"<<endl;
					PrD(A);
					PrD(B);
//					PrD(E);
//					PrD(F);
				}
				if(flag)
				{
//				cout<<"minAnd max"<<endl;
//				cout<<A.x<<","<<A.y<<endl;
//				cout<<B.x<<","<<B.y<<endl;
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
				//getLinePara(A,B,MiddLP[i]);
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
			//cout<<i<<","<<Midd[i].A.x<<","<<Midd[i].A.y<<endl;
			mask *p = Midd[i].next;
			mask *t = p;
			while(p!=NULL)
			{
				t = p;
				p=p->next;
				free(t);
			}
			//cout<<endl;
			Midd[i].next = NULL;
		}
}
void showDmidd(){
	cout<<"--------------毛坯中线----------------------"<<endl; 
	for(int i=0;i<Dmiddle.size();i++)
		{
			cout<<"i"<<i<<" "<<Dmiddle[i].A.x<<","<<Dmiddle[i].A.y<<endl;
		}
}
void invokeConect(){
	showDmidd();
	getMiddlLP();
//	cout<<"run merge"<<endl;
	showMergeLP();
//	MergeLP2File(num);
//	DeleteCross();
//	UpdataMiddLP();
	Deleteoverlap();
//	MergeLP2File(num);
//	showMergeLP();
	cout<<"showMergLP wang"<<endl;
//	showMergeLP();
	UpdataMiddLP();
	DeleteCross();
	UpdataMiddLP();
	showMergeLP();
//	MergeLP2File(num);
//	Deleteoverlap();
//	UpdataMiddLP();
	cout<<"RUN showMergeLP"<<endl; 
	//showMergeLP();
//	cout<<"RUN OK"<<endl;
//	DPoint Cross;
//	int i = 1;
//	int j = 2;
//	getCross(MiddLP[i],MiddLP[j],Cross);
//	PrD(MiddLP[i].down->C);
//	PrD(MiddLP[i].down->next->C);
//	PrD(MiddLP[j].down->C);
//	PrD(MiddLP[j].down->next->C);
//	cout<<"Cross"<<endl;
//	cout<<MiddLP[i].k<<endl;
//	cout<<MiddLP[j].k<<endl;
//	PrD(Cross);
//	cout<<PinEdge(MiddLP[i].down->C,MiddLP[i].down->next->C,Cross)<<endl;
//	cout<<PinEdge(MiddLP[j].down->C,MiddLP[j].down->next->C,Cross)<<endl;
//	DeleteCross();

//	MergeLP2File(num);
	cout<<"RUN extend"<<endl;
	showMergeLP();
	cout<<"RUN extend after"<<endl;
	extend();
//	showMergeLP();
//	Delete2File(num);
//	showMergeLP();
//	//Deleteoverlap();
//	//showExtendLP();//extendLP
//	cout<<"RUN deletePoint"<<endl; 
	deletePoint();
	cout<<"RUN deletePoint"<<endl;
	UpdataMiddLP();
	showMergeLP();
//	showDeleteLP();
//	cout<<"Delete2File"<<endl;
//	showMergeLP();
//	Delete2File(num);
//	cout<<"run Gematrix"<<endl;
	Gematrix();
//	cout<<"run Updata"<<endl;
	Updatmatrix();
	cout<<"run showUpadataMidd"<<endl;
	showUpdataMidd();
	InsertMatrix();
	setZero();
//	cout<<"run showUpadataMidd"<<endl;
//	showUpdataMidd();
	GeFinalFile(num);
	freeMidd();
	freeLP();
} 
int main(int argc,char*argv[])
{
	string fileName(argv[1]);
	//cout<<fileName<<endl;
	int a = fileName.find_last_of('_');
	inFile = fileName;
	OutFile = "./centerline_"+ fileName.substr(a+1);
	//cout<<fileName.substr(a+1,1)<<endl;
	invokeScanline(); //OK 
	cout<<"ScanLine OK"<<endl;
	InvokeGetMiddleLine();//OK
	cout<<"InvokeGetMiddleLine OK"<<endl;
	invokeConect();
	return 0;
}
