//#include "stdafx.h"
#include <iostream>
#include <list>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <vector>
#include <cstdio>
#include<math.h>
//#include <ctchar>
//#define INT_MAX 2147483647
using namespace std;
class Point{
public:
	int x;
	int y;
};
//�����С�ĵ㣬�Լ����е������ 
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
//Mask���ڱ��ͬһ������ıߣ����ڰѱ߷��� 
struct Mask{
Mask *next;//ָ��ͬһ��ı�
int i;//��ʾͬһ�������һ���� 
double dis;//����Ӧ�ߵľ��� 
};
//�߽������ݽṹ���߶Σ���A��xҪ��С����B��xҪ������������ ��x��ͬ����y��С���С�flag == 0��ʾб�ʲ����ڣ������б߽��߳����߽�б�� 
struct DEdge{
DPoint A; //��A��xС�ڵ�B 
DPoint B;
bool flag;
double length;
double k = INT_MAX; 
Mask *next;
};
//ë��������ߴ洢��ʽ 
struct MiddLine{
	DPoint A;
	DPoint B;
	int i,j;//���ڼ�¼�������ߵ����ߣ����ڵ��� 
};
//-------------���ھ�������---------- 
struct mergeNode{
	mergeNode * next;
	int i;//��Ҫ�ϲ��߶εı� 
	double length;
};
struct extendNode{
	DPoint C;
	int type;//0��ʾԭ������㣬-1��ʾ��չ�ĵ㣬2��ʾ�м��и�ĵ� ��3��ʾԭ�����յ�(д���ļ��ľ���1��չ�ĵ� �����referline����2+1������0����)
	extendNode *next; 
};
struct LinePara{
	double k;
	double b;
	int flag;//flag == 0 ��ʾб�ʲ����� 
	mergeNode * next; 
	extendNode *down;
};
struct mask{
	int i;//��ʾ��Ӧ�ĵ��λ��
	mask *next; 
 };
struct matrix{
	DPoint A;
	int flag;//0��ʾ��δ������1��ʾ�������� 
	mask *next;
};  
//-------------------------ɨ���߲���ʹ�õ���ȫ�ֱ���---------------- 
Polygon polygon;
const int weight=2500;
const int height = 5000;
unsigned long int arr[height][weight]={0};//λͼ�洢ɨ����ɨ���� 
string num="0";//num��ʾ�ļ���ţ�������� ,k��ʾ���ű�����
//��¼���ݷ�Χ�����ڼ��� 
int minx = INT_MAX;
int miny = INT_MAX;
int maxx = -INT_MAX;
int maxy = -INT_MAX; 
double k;//��С���� 
//---------------------------�������㷨---------------------
double thresholdK= 1e-1;//б��֮��ľ���ֵС�������Χ�� ����Ϊƽ�� 
double thresholdPara = 1e-3;//�ϲ�ƽ���ߣ���ֵҪ��thresholdK��С 
double thresholdOverTmp = 5e-1;//����ƽ�����غϲ���С�����ֵ�ģ���Ϊû���غϲ���, ����divide��ʱ��ʹ�� 
double thresholdOver = 1e-1;//����ƽ�����غϲ���С�����ֵ�ģ���Ϊû���غϲ��֣�extend �е�coin��ʹ�� ,�������Ǻ�thresholdTmpһ�� 
double thresholdZ = 2e3;//б�ʴ���2000���߶���Ϊ���� 
double thresholdD = 1.5;//�ϲ�ͬһ���ߵ���ֵ���Լ����� 
double thresholdDis = 0.5;//extend��չ��ʱ���ã�����չ����������߶��ڵĳ��ȣ������equal�ڵ���ֵһ���� 
double thresholdE = 0.5;//equal����ֵ 
double thresholdlength = 1.5;//�ٽ��߽��ߵ���ֵ 
double thresholdEx = 10;//��k*minlengthһ����ƿ����ӳٵĳ��� 
double a = 1.5; // �ӳ���ʼ�ߵĲ��� 
vector<DPoint> Dpoin;//��ȡ�ļ�ʱ���γɵ�һ���߽����ɵ����� 
vector<DEdge> Dpoly;//�߽��߽ṹ��ɵ�һ��vector�ṹ 
vector<MiddLine> Dmiddle; //�洢���߽ṹ��һ�����ݽṹ 
vector<LinePara> MiddLP;//һ���洢���ߺ������ϵĵ�Ļ��νṹ�� ��vector���� 
vector<matrix> Midd;//һ�������ж��Ƿ���ͨ�Ļ��ε����ݽṹ 
bool staEndFlag = 0;//��ȡ�ļ����ļ��Ƿ�����ʼ�� 
int start=0;//��¼��ʼ������Ž�С����� 
int End = 0;//��¼��ʼ������Žϴ����� 
//---------------------���в�����Ҫ�õ��ĺ���---------------------------
//string input();//��ȫ�ֱ���num��ֵ�����ڿ��������ĸ��ļ� �����ļ���ţ��������
double Getlength(DPoint start,DPoint end);//��������֮��ľ��� 
bool check(int argc, char* argv[]);//������ 
//---------------------���в�����Ҫ�õ��ĺ���---------------------------
//--------------------���������㷨���躯��----------------
void merge();//�ϲ�ͬһ��ıߣ�������LP�е�down; 
bool equal(DPoint &A,DPoint &B);//�ж��������Ƿ�Ϊͬһ���㣨x,y�Ĳ�С��0.1�� 
bool Para(LinePara E1,LinePara E2);//�ж���1����2�Ƿ�ƽ��  
double getLinePara(DPoint &A,DPoint &B,LinePara &LP);////�����߶γ���,�õ��߶�AB�Ĳ���LP�ṹ�� 
bool Isgroup(LinePara &LP,double length,int j);//��ʾ�Ƿ���Ժϲ�������ƽ���ߣ�LP��MiddLP[j]�� ����0��ʾ��ͬ�飬1��ʾͬһ�鲢�ҿ��Ժϲ� 
bool conin(LinePara &LP,int i,mergeNode *p,double length);//0��ʾ����ƽ���߿��Ժϲ���һ����LP,i��ʾ��ǰƽ���ߣ�pָ����Ҫ�ϲ����߶ε�i 
void getMiddlLP();//����һ��MiddlLP��vector�����Ҹ�����LP�е�next;
bool getCross(LinePara &E1,LinePara &E2,DPoint &Cross);//�õ�E1,E2�Ľ��㡣����1��ʾ��ֱ��ƽ�� 

void extend();//�����ͨ��չ��������MiddLP�����е�down������extend()����һ��k ,k�����ſ����ӳٵĳ��ȣ�minlenght * k�� 
void Delete2File(string num);//��deletePoint���MiddLP�еĽ��д�뵽DeReferlineFinal 
void insert(int i,DPoint max);//��LP[i].down�󣬲�����չ�ĵ㣨��x,y�Ĵ�С˳�����У� 
void deletePoint();//ɾ��ֵһ���ĵ㣨equal),����ĵ��ǰ��һ������ɾ������� 
void showMergeLP();//���� show merge ���MiddLP �е�down�� 
void MergeLP2File(string num);//��Merge���MiddLP�е�down���� ReferlinePara���ļ����� 
void Gematrix();//����Midd��vector��ͬʱ�޸ĸ�MiddLP down�е�typeֵ
void swap(int &min,int &max);//���������������һ���Ǹ�С���� 
void Updatmatrix();//����Midd�е�nextֵ��
void showUpdataMidd();//���ԣ�չʾUpdatamatrix��Midd��ֵ 
int Matrix2Point(int i);//���ԣ������˳��ĵ� 
void GeFinalFile(string num);//������matrix2File,��д��������ͨ���߶� 
int matrix2File(int i,FILE *fp);//�ѽ��������ļ��� 
//-----------------------�������㷨��Ҫ�õ��ĺ���-----------------------
void txt2DPoint();//��ȡ�ı������ɵ�Dpoin��һ��vector 
void DPoint2DEdge();//��ȡ�ı�������Dpoly��һ��vector�Ľṹ��Dpoly�Ľڵ��У�A,B�������� 
	void divide();//���麯����������Dpoly��vector�ṹ ������mindis[]ȥ��һЩ�ӱ� 
bool SameK(DEdge &E1,DEdge &E2);//�ж������߶��Ƿ�ƽ�� 
DPoint MiddlePoint(DPoint &A,DPoint &B);//����������е� 
void shadow(DPoint &CT,double k,double b);//��һ������һ������ֱ�����ϵ�ͶӰ�����Ż� 
double Getlength(DPoint start,DPoint end);//��������֮��ľ��� 
bool overlap(DPoint A,DPoint B,DPoint C,DPoint D,int i);//�ж��߶�AB,CD֮���Ƿ���overlap; i��ʾ�߶�AB��Dpoly�е���ţ�����0��ʾû��overlap,���Ż� 
bool PinPoly(DPoint A);//�жϵ�A�Ƿ��ڶ�����ڣ��ڲ�����i��ʾ���ȣ��ɿ��ƴ�С 
bool inPoly(DPoint A,DPoint B);//�ж��߶�A,B�Ƿ��ڶ�����ڣ����ж������˵��Ƿ��ڶ�����ڣ� 
bool MiddleLine(DEdge &E1,DEdge &E2,int i,int j);//Ԥ�������ߵ����ߣ����ж��Ƿ��������������ڶ�����ڲ�    0��ʾû�С�  i���ڵ��� 
double getdis(double k,double b,DPoint &A);//�㵽һ�����Ǵ�ֱ�ߵľ��� 
void distinct(DEdge &E1,DEdge &E2,double &dis);//����������ƽ���ߵľ��� 
	void getMiddleLine();//��ȡ���ߣ�����һ��Dmiddle��vector 
double getNewDis(Mask *p,Mask *q);//�ϲ�ͬһ���У����<thresholdD��ƽ���ߣ������¾���ĺ�����p,qָ���һ��ƽ���ߺ����һ��ƽ���� 
void getParaLine(double &k,double &b,double dis);//��ȡ����k,bƽ�У�����Ϊdis�ģ��������£���ƽ���� 
void getLine(Mask *p,Mask *q,MiddLine &tmp,int i);//�������ߵĺ�����������һ��Dmiddle��vector�е�һ��С�ڵ㣩
void add(int set);//����ʼ�ߵ�λ�����һ������ʼ����Χ������ƽ�е��� 
	void Dmid2File(string num);//�ѽṹ��������߽ṹд��Referline�ļ����� 
void InvokeGetMiddleLine();//���������ߺ��� 
void free();//�ͷ�Dpoly��̬������ڴ� 
void dedis(DEdge &E1,DEdge &E2);//����������ƽ������overlap��ƽ���ߵľ��� ���������߲�Ϊ��ֱ�ߣ� 
void showDpoint();//���ԣ������ȡ�ĵ��DPoin��ֵ
void showDEdge();//���ԣ����DPoly��ֵ
void showD();//���ԣ���������DPoly��ֵ 
void showMidd();//���ԣ����Dmiddle��ֵ 
void deGetLine(Mask *p,Mask *q,Mask *min,Mask *max,MiddLine &tmp,int i,double dis,double &k,double &b,DPoint &AA,DPoint &AB);//�������ɺϲ�ͬһ��ߺ����ɵ��±� 
//-------------------------------------ɨ�����㷨�õ��ĺ��������ݽṹ-------------------------------------------------------------- 
void getK(double &k); //kΪȫ�ֱ�������ʾ��С���� 
void set(int i,int j);//i��ʾy,j��ʾx
int get(int i,int j);//���i��jλ�õ�i 
void txtToPolygon(double k,int offsetx,int offsety,string num);//�������ļ�ת���� polygon�����ݽṹ ������ɨ�����㷨  polygon��һ���ṹ�� 
void GetOrigion(int &MinX,int &MinY,int &MaxX,int &MaxY,string num);//����������е������Сֵ 
void array2file(string num);//��ɨ���ߵĽ������ ת�����ļ�  
void PolyScan(int &offsetx, int &offsety, int &refrain_minx, int &refrain_maxx, int &refrain_miny, int &refrain_maxy, int &MinY, int &MaxY,unsigned long int arr[][weight], Polygon &poly);//ɨ�����㷨��offsetx��ʾarr��ƫ�ƣ�������ᣬĬ��Ϊ0��refrain����һ���ض���Χ�Ļ�����Ϊ���Է�Χ������ľ��Ƕ���Σ�arr��ʾ���ص����顣
void invokeScanline();//����Scanline,����һ��ɨ���ߵ����� 

bool check(int argc, char* argv[]){
    if (argc != 2)
    {
        printf("Usage: ./lineExt boundary_x.txt\n");
        return false;
    }
    // other check...
    return true;
}
//-------------------------------------ɨ�����㷨�õ��ĺ��������ݽṹ-------------------------------------------------------------- 
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
	//���ɾ�̬�� ���Ѿ�̬�߸�������� 
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
	//showPaint();
	//showPoint(poly,offsetx,offsety);
	//������� 
	for(i=MinY; i<MaxY; i++){
		//pAET��̬��  pAET�ʼ��null
		// ����AET�бߵ�xֵ��������һѭ��
		NET *p = pAET->next;
		while(p){
			p->x = p->x + p->dx;
			p = p->next;
		}
		//��AET���е�x����һ����С��������� 
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
		//ɾ��AET������y=ymax�ı�
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
		//�ҵ����ʵ�λ�ò���pAET 
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
		//���ɫ�� 
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
void txtToPolygon(double k,int offsetx,int offsety,string num){
	ifstream infile;
	string path = "./boundary_"+num;
	infile.open(path);
	string type;
	int count = 0;
	int minx = INT_MAX;
	int miny = INT_MAX;
	int maxx = 0;
	int maxy = 0;
	Point tmp;
	tmp.x=-1;
	tmp.y =-1;
	//cout<<"edsaffa";
	while (infile !=NULL) // line�в�����ÿ�еĻ��з�  
        {   
        	//cout<<"test";
            infile>>type;
            if(type[0]=='V')
            {
            double x;
            double y;
            string name;
            infile>>name;
            infile>>x;
            infile>>y;
            x = (x -offsetx)/k +offsetx;
            y = (y -offsety)/k +offsety;
            //cout<<"x :"<< x <<" y "<< y<<endl;
            if (tmp.x!=int(x) || tmp.y !=int(y))
            	{
				tmp.x=int(x);
            	tmp.y = int(y);
            	polygon.coordinate.push_back(tmp);
            	if(int(x)> maxx)
					maxx = int(x);
				if(int(y)> maxy)
					maxy = int(y);
				if(int(x)< minx)
					minx = int(x);
				if(int(y)< miny)
					miny = int(y);
                count++;
               }
            //cout<<"x :"<< tmp.x <<" y "<< tmp.y<<endl;
			}
			type ="";
        }
    polygon.maxp.x=maxx;
    polygon.maxp.y=maxy;
    polygon.minp.x=minx;
    polygon.minp.y=miny;
    polygon.size=count;
    //showPoint(polygon,minx,miny);
    infile.close();
}
void GetOrigion(int &MinX,int &MinY,int &MaxX,int &MaxY,string num){
	ifstream infile;
	string path = "./boundary_"+num;
	infile.open(path);
	string type;
	int minx = INT_MAX;
	int miny = INT_MAX;
	int maxx = -minx;
	int maxy = -maxy; 
	while (infile !=NULL) // line�в�����ÿ�еĻ��з�  
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
    MinX=minx;
    MinY=miny;
    MaxX=maxx;
    MaxY=maxy;
    infile.close();
}
//-------------------------------------ɨ�����㷨�õ��ĺ��������ݽṹ-------------------------------------------------------------- 
string input(string a){
	return a;
}
void getK(double &k){
	double a=(maxy - miny)* 1.0f/(height*4);
	double c=(maxx - minx)* 1.0f/(weight*8);
	 k = a>c?a:c;
	if (k<1)
	  k =1;
}  
void invokeScanline(){
	//num = input(a);
	GetOrigion(minx,miny,maxx,maxy,num);
	getK(k);
	cout<<"1"<<endl;
	int offsetx = minx;
	int offsety = miny;
	txtToPolygon(k,offsetx,offsety,num);
	cout<<"1"<<endl;
	PolyScan(offsetx, offsety, polygon.minp.x, polygon.maxp.x, polygon.minp.y, polygon.maxp.y, polygon.minp.y, polygon.maxp.y, arr, polygon);
	cout<<"1"<<endl;
	//cout<<"------------------------"<<endl;
	//array2file(num);
} 
//-------------------------------------�����㷨�õ��ĺ��������ݽṹ-------------------------------------------------------------- 
bool SameK(DEdge &E1,DEdge &E2){
	if((E1.flag==0&&E2.flag==0) || fabs(E1.k-E2.k)<=thresholdK)
		return 1;
	if(fabs(E1.k)>1000&&fabs(E2.k)>=1000&&fabs(E1.k/E2.k-1)<thresholdK)
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
	//��CT������
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
bool overlap(DPoint A,DPoint B,DPoint C,DPoint D,int i){
	double minxE1 = A.x;
	double maxxE1 = B.x;
	double minyE1 = A.y>B.y?B.y:A.y;
	double maxyE1 = A.y>B.y?A.y:B.y;
	double minxE2 = C.x;
	double maxxE2 = D.x;
	double minyE2 = C.y>D.y?D.y:C.y;
	double maxyE2 = C.y>D.y?C.y:D.y;
	double length1 =  Getlength(A,B);
	double length2 = Getlength(C,D);
	double maxLength = length1>length2?length1:length2;
	double thresholdOver = thresholdOverTmp;
	if(maxLength<=5)
		thresholdOver = 1e-1;
	if( (minxE2>=(maxxE1 - thresholdOver)||(maxxE2 - thresholdOver)<=minxE1) && (minyE2>=maxyE1-thresholdOver || maxyE2-thresholdOver<=minyE1))
		return 0;
	//��ֱֱ�ӱȽ�y 
	if(Dpoly[i].flag == 0)
	{
		if((minyE2>=maxyE1-thresholdOver || maxyE2-thresholdOver<=minyE1))
			return 0;
	}
	else
	{
			//�������ͶӰ��Ƚ�x������ 
	DPoint CT = C;
	DPoint DT = D;
	double k = Dpoly[i].k;
	double b = Dpoly[i].A.y - Dpoly[i].A.x*k;
	shadow(CT,k,b);
	shadow(DT,k,b);
	minxE2 = CT.x;
	maxxE2 = DT.x;
	if((minxE2>=(maxxE1 - thresholdOver)||(maxxE2 - thresholdOver)<=minxE1))
			return 0;
	}
	//ˮƽ�Ƚ�x
	if(fabs(Dpoly[i].k)<thresholdK)
	{
		if((minxE2>=(maxxE1 - thresholdOver)||(maxxE2 - thresholdOver)<=minxE1))
			return 0;
	}
	return 1;
}

bool PinPoly(DPoint A)
{
	//i��ʾ���� 
	int i= 1 ;
	if( get(int((A.y - miny)/k),int((A.x - minx)/k))== 1 || (get(int((A.y - miny)/k+i),int((A.x - minx)/k)) == 1&&int((A.y - miny)/k+i<height))|| (get(int((A.y - miny)/k),int((A.x - minx)/k+i))== 1&&int((A.x - minx)/k+i)<weight)) 
		return 1;
	if((get(int((A.y - miny)/k)-i,int((A.x - minx)/k))== 1 &&int((A.y - miny)/k)-i>=0)||(get(int((A.y - miny)/k),int((A.x - minx)/k)-i) == 1&&int((A.x - minx)/k)-i>=0))
		return 1;
	return 0;
}
bool inPoly(DPoint A,DPoint B){
	if( PinPoly(A) && PinPoly(B) )
		return 1;
	return 0;
} ;
bool MiddleLine(DEdge &E1,DEdge &E2,int i,int j){
	if(!SameK(E1,E2))
		return 0;
	if(!overlap(E1.A,E1.B,E2.A,E2.B,i))
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
	cout<<"------------------show ����------------------------"<<endl;
	Mask * p;
	for(int i = 0;i<Dpoly.size();i++)
	{
		
		//cout<<"i="<<i<<"  "<<Dpoly[i].A.x - minx<<" , "<<Dpoly[i].A.y - miny<< " | "<<Dpoly[i].B.x- minx<< " , " << Dpoly[i].B.y-miny<<" length = "<<Dpoly[i].length<<endl;
		cout<<"i="<<i<<"  "<<Dpoly[i].A.x<<" , "<<Dpoly[i].A.y << " | "<<Dpoly[i].B.x<< " , " << Dpoly[i].B.y<<" length = "<<Dpoly[i].length<<" k "<<Dpoly[i].k<<endl;
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
void txt2DPoint(){
	ifstream infile;
	string dir = ".";
	string path = dir+"/boundary"+num;
	infile.open(path);
	string type;
	DPoint tmp;
	tmp.x=-1;
	tmp.y =-1;
	while (infile !=NULL) // line�в�����ÿ�еĻ��з�  
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
            tmp.x = x;
            tmp.y = y;
            Dpoin.push_back(tmp);
            //cout<<"x :"<< x <<" y "<< y<<endl;
            //cout<<"x :"<< tmp.x <<" y "<< tmp.y<<endl;
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
//				cout<<x<<","<<y<<","<<start<<endl; 
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
    infile.close();
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
		E.length = sqrt(((Dpoin[next].y - Dpoin[i].y)*(Dpoin[next].y - Dpoin[i].y) + (Dpoin[next].x - Dpoin[i].x)*(Dpoin[next].x - Dpoin[i].x)));
		if(Dpoin[i].x<Dpoin[next].x)
		{
			E.A = Dpoin[i];
			E.B =Dpoin[next];
			E.flag =1;
			E.k = (Dpoin[next].y - Dpoin[i].y)/(Dpoin[next].x - Dpoin[i].x);
		}
		else if(Dpoin[i].x>Dpoin[next].x)
		{
			E.A = Dpoin[next];
			E.B =Dpoin[i];
			E.flag =1;
			E.k = (Dpoin[next].y - Dpoin[i].y)/(Dpoin[next].x - Dpoin[i].x);
		}
		else
		{
			if(Dpoin[next].y<Dpoin[i].y)
			{
				E.A = Dpoin[next];
				E.B =Dpoin[i];
			}
			else
			{
				E.A = Dpoin[i];
				E.B = Dpoin[next];
			}
			E.flag = 0;
			E.k = INT_MAX;
		}
		if(fabs(E.k)>=thresholdZ)
		{
			E.flag = 0;
		}
		Dpoly.push_back(E);
	}
}
void dedis(DEdge &E1,DEdge &E2){
	double b1=E1.A.y -E1.k*E1.A.x;
	double b2=E2.A.y-E2.k*E2.A.x;
//	cout<<"b1 "<< E1.A.y -E1.k*E1.A.x<<"  "<<"b2 "<<E2.A.y-E2.k*E2.A.x<<endl;
//	cout<<E1.A.y<<","<<E1.A.x<<" k "<<E1.k<<endl;
//	cout<<E2.A.y<<" , "<<E2.A.x<<" k "<<E2.k<<endl;
	cout<<"fabs  "<<fabs ((b2-b1)/sqrt(1+E1.k*E1.k))<<endl;
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
void divide(){
	double dis=0;
	double mindis[Dpoly.size()]={INT_MAX};
	for(int i=0;i<Dpoly.size();i++)
	{
		mindis[i] = INT_MAX;
	}
	if(staEndFlag==0)
	{
	for(int i = 0;i<Dpoly.size();i++)
	{
		for(int j=i+1;j<Dpoly.size();j++)
			{
				//cout<< i << " ============ "<< j<<endl;һ����ѱ߲嵽������һ�߲�����¼ 
				if(MiddleLine(Dpoly[i],Dpoly[j],i,j))
				{
					int a = i;
					int b = j;
					if(Dpoly[a].length<Dpoly[b].length) 
					{
						b = i;
						a = j;
					}
					//cout<<"RUN"<<endl;
				    distinct(Dpoly[a],Dpoly[b],dis);
				    if(dis<mindis[a])
				    	mindis[a] = dis;
				    if(dis<mindis[b])
				    	mindis[b] =dis;
					//dis =1;
					//dedis(Dpoly[i],Dpoly[j]);
					Mask *p = (Mask*)malloc(sizeof(Mask));
					p->dis = dis;
					p -> next =NULL;
					p->i=b;
					//���밴��С��������
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
					//����t��qǰ
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
		}
    }
    else
    {
    	for(int i =start+1;i<End;i++)
    	{
    		for(int k=End+1;k<Dpoly.size()+start;k++)
    		{
    			int j = k%Dpoly.size();
    			if(MiddleLine(Dpoly[i],Dpoly[j],i,j))
				{
					int a = i;
					int b = j;
					if(Dpoly[a].length<Dpoly[b].length) 
					{
						b = i;
						a = j;
					}
					//cout<<"RUN"<<endl;
				    distinct(Dpoly[a],Dpoly[b],dis);
				    if(dis<mindis[a])
				    	mindis[a] = dis;
				    if(dis<mindis[b])
				    	mindis[b] =dis;
					//dis =1;
					//dedis(Dpoly[i],Dpoly[j]);
					Mask *p = (Mask*)malloc(sizeof(Mask));
					p->dis = dis;
					p -> next =NULL;
					p->i=b;
					//���밴��С��������
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
					//����t��qǰ
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
void deGetLine(Mask *p,Mask *q,Mask *min,Mask *max,MiddLine &tmp,int i,double dis,double &k,double &b,DPoint &AA,DPoint &AB){
	 DPoint A = Dpoly[min->i].A;
	 DPoint B = Dpoly[max->i].B;
	cout<<"---------------deGetLine--------------------"<<i<<endl;
	cout<<"tmp"<<tmp.A.x<<","<<tmp.A.y<<endl;
	cout<<"tmpB"<<tmp.B.x<<","<<tmp.B.y<<endl;
	cout<<""<<Dpoly[i].A.x<<","<<Dpoly[i].A.y<<endl;
	cout<<"Bshadow"<<A.x<<","<<A.y<<endl;
	cout<<"Bshadow"<<B.x<<","<<B.y<<endl;
	cout<<"Ashadow"<<AA.x<<","<<AA.y<<endl;
	cout<<"Ashadow"<<AB.x<<","<<AB.y<<endl;
	cout<<"dis"<<dis<<",k"<<k<<endl;
	cout<<"b "<< b<<endl;
	cout<<" b original"<<Dpoly[i].A.y - Dpoly[i].k*Dpoly[i].A.x<<endl;
	cout<<"p q �Ƿ���ȷ"<<endl;
	while(p!=q->next)
	 {
	 	cout<<"dis"<<p->dis<<endl;
	 	p = p->next;
	 } 
	cout<<"------------------end-----------------------"<<endl;
}
void getLine(Mask *p,Mask *q,MiddLine &tmp,int i){
	 //�ҵ�x��С�ĵ�,��x���ı�,���±�i����¼ (��ȡ���ȷ�Χ)
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
	 //��ֱ�Ļ� ��ֱ�ӱȽ�x,��x
	 double k = Dpoly[i].k;
	 double b = 0;
	 if(Dpoly[i].flag == 0)
	 {
	 	double sub1 = Newdis - min->dis;
	 	double sub2 = Newdis - max->dis;
	 	//����ֱ����߻����ұ� 
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
	 	//����ֱ���·� 
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
	cout<<PinPoly(endP1)<<endl;
	cout<<PinPoly(endP2)<<endl;
	cout<<"End1"<<endP1.x<<","<<endP1.y<<endl;
	cout<<"End2"<<endP2.x<<","<<endP2.y<<endl;
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
			Dmiddle.push_back(tmp);
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
			tmp.j = -1;//��ʾ���� 
			cout<<tmp.i<<","<<tmp.j<<endl;
			Dmiddle.push_back(tmp);
		}
	}
	if(staEndFlag == 1)
	{
		add(start);
		add(End);
	}
} 
void InvokeGetMiddleLine(){
	txt2DPoint();
	DPoint2DEdge();
	divide();
   	getMiddleLine();
	free();
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
		
	}
}
//----------------------���������㷨�ĺ���-------------------
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
	return 0;
}
double getLinePara(DPoint &A,DPoint &B,LinePara &LP){
	double m = 0;
	double x2 = B.x;
	double x1 = A.x;
	double y2 =B.y;
	double y1 = A.y;
	// �������  
	m = x2 - x1;
	//�����k��ʾ����� 
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
//		if(fabs(LP.k) > 4000)
//			LP.flag  = 0;
	}
	LP.next = NULL;
	LP.down = NULL;
	return sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1)); 
}
bool conin(LinePara &LP,int i,mergeNode *p,double length){
	int z=3;
	double minlength = z*p->length;
	if(p->length>length)
		minlength = z*length;
	DPoint A=Dmiddle[i].A; 
	DPoint B=Dmiddle[i].B;
	DPoint C=Dmiddle[p->i].A;
	DPoint D=Dmiddle[p->i].B;
	double minxE1 = A.x;
	double maxxE1 = B.x;
	double minyE1 = A.y>B.y?B.y:A.y;
	double maxyE1 = A.y>B.y?A.y:B.y;
	double minxE2 = C.x;
	double maxxE2 = D.x;
	double minyE2 = C.y>D.y?D.y:C.y;
	double maxyE2 = C.y>D.y?C.y:D.y;
	double length1 = sqrt((B.x-C.x)*(B.x-C.x)+(B.y-C.y)*((B.y-C.y)));
	double length2 = sqrt((D.x-A.x)*((D.x-A.x)) + (D.y-A.y)*(D.y-A.y));
	double dist = length1>length2?length2:length1;
	//cout<<"run"<<" dist "<<dist<<" length "<<minlength<<endl;
	if(LP.flag == 0)
	{
		//û���غ� 
		if((minyE2>=maxyE1-thresholdOver || maxyE2-thresholdOver<=minyE1) && dist>=minlength )
			return 0;
	}
	else
	{
		if( (minxE2>=(maxxE1 - thresholdOver)||(maxxE2 - thresholdOver)<=minxE1) && dist>=minlength)
			return 0;
	}
	//cout<<"return 0";
	return 1;
	
	//��������ߵĳ��� 
} 
bool Isgroup(LinePara &LP,double length,int j){
	int flag = 0;
	int record = -1;
	for(int i=0;i<MiddLP.size();i++)
	{
		if(Para(LP,MiddLP[i]) && flag ==0 )
			{
				//b���
				//cout<<"run---------"<<endl; 
				if(LP.flag!=0 &&fabs(Dmiddle[j].A.y-(MiddLP[i].k*Dmiddle[j].A.x+MiddLP[i].b))>0.1)
					continue;
				else if(LP.flag == 0&&fabs(Dmiddle[i].A.x-Dmiddle[j].A.x)>0.1)
					continue;
				else
				{
					mergeNode *p=MiddLP[i].next;
					while(p!=NULL)
					{
						if(conin(LP,j,p,length))
						{
							//����
							record = i;
							flag =1;
							break;
						}
						p=p->next;
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
void merge(){
	mergeNode * p;
	DPoint minP;
	DPoint maxP; 
	for(int i = 0;i<MiddLP.size();i++)
	{
		p = MiddLP[i].next;
		minP = Dmiddle[p->i].A;
		maxP = Dmiddle[p->i].B;
		//�ҵ������С�ĵ㣬���� extendNode
		while(p!=NULL)
		{
			if(MiddLP[i].flag == 0)
			{
				if(Dmiddle[p->i].B.y>maxP.y)
					maxP = Dmiddle[p->i].B;
				if(Dmiddle[p->i].A.y<minP.y)
					minP = Dmiddle[p->i].A;
			}
			else
			{
				if(Dmiddle[p->i].B.x>maxP.x)
					maxP = Dmiddle[p->i].B;
				if(Dmiddle[p->i].A.x<minP.x) 
					minP = Dmiddle[p->i].A;
			}
			p = p->next;
		}
		extendNode *q = (extendNode *)malloc(sizeof(extendNode));
		q->C= minP;
		q->type = 0;
		MiddLP[i].down = q;
		extendNode *t = (extendNode *)malloc(sizeof(extendNode));
		t ->C = maxP;
		t->type = 3;
		q->next = t;
		t->next = NULL;
	}
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
bool getCross(LinePara &E1,LinePara &E2,DPoint &Cross){
	if(fabs(E1.k-E2.k)<thresholdPara)
	{
		return 1;
	}
	else if(fabs(E1.k-E2.k)<thresholdK || (E1.flag==0&&E2.flag == 0))
	{
		extendNode *p=E1.down;
		DPoint start;
		DPoint end;
		DPoint start1;
		DPoint end1;
		GetSPandEnd(p,start,end);
		double length1 = Getlength(start,end);
		p = E2.down;
		GetSPandEnd(p,start1,end1);
	    double length2 = Getlength(start1,end1);
	    double thresholdDis = length1>length2?length2:length1;
	    double dis=INT_MAX;
	    length1 = Getlength(start,end1);
		length2 = Getlength(start1,end);
		if(length1<length2) 
		{
			dis = length1;
			Cross = MiddlePoint(start,end1);
		}
		else
		{
			dis = length2;
			Cross = MiddlePoint(start1,end);
		}
	    if(thresholdDis>dis&&dis<thresholdEx)
	    	return 0;
		return 1;
	}
	else
	{
		if(E1.flag!=0&&E2.flag!=0)
		{
		if(fabs(E2.k-E1.k)>=thresholdK || (fabs(E2.k-E1.k)>fabs(E2.k)&&fabs(E2.k-E1.k)>fabs(E1.k)))
		{
			Cross.x =(E1.b-E2.b)/(E2.k - E1.k);
			Cross.y = E1.k*Cross.x + E1.b;
		}
		else if(E1.k == 0)
		{
			Cross.y = E1.down->C.y;
			Cross.x = (Cross.y - E2.b)/E2.k;
		}
		else if(E2.k == 0)
		{
			Cross.y = E2.down->C.y;
			Cross.x = (Cross.y - E1.b)/E1.k;
		}
		else if(fabs(E1.k)>fabs(E2.k))
		{
			double z = E2.k/E1.k;
			Cross.y = (E2.b-z*E1.b)/(1-z);
			Cross.x = (Cross.y - E1.b)/E1.k;
		}
		else
		{
			double z = E1.k/E2.k;
			Cross.y = (E1.b-z*E2.b)/(1-z);
			Cross.x = (Cross.y - E2.b)/E2.k;
		}
	    return 0;
		}
		else
		{
			//E1��ֱ 
			if(E1.flag == 0)
			{
				Cross.x = E1.down->C.x;
				Cross.y = E2.k*Cross.x + E2.b;
			} 
			else
			{
			//E2��ֱ 
				Cross.x = E2.down->C.x;
				Cross.y = E1.k*Cross.x + E1.b;
			}
		}
		return 0;
	} 
}
void UpdataMinMax(int j,DPoint C,DPoint &min,int &minJ,int k){
		//�ҵ�j��Ӧ�������յ� 
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
		//�ҵ���Ҫ��չ�ĳ���minlength 
		double minlength = Getlength(C,s->C);
		if(C.x>=s->C.x&&C.x<=e->C.x)
			minlength = 0;
		else if(C.x<s->C.x)
			minlength = Getlength(C,s->C);
		else
			minlength = Getlength(C,e->C);
		//������������������� 
		if(minlength<k*Getlength(s->C,e->C) &&minlength<thresholdEx)
		{
			min = C;
			minJ = j;
		}
}
void extend(){
	//�ӳ��ߵĳ��ȣ���k*minlength ��  5���� 
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
			}	
		}
		double threshold = Getlength(start,end);
		double minlength = Getlength(min,start);
		double maxlength = Getlength(end,max);
		//���ĵ��ҵ����ĵط����롣 
		if(maxlength<=threshold)
		{
			//�������� 
			insert(i,max);
			insert(maxJ,max);
		}
		if(minlength<=threshold)
		{
		 	//��������
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
		//cout<<"----------------"<<endl; 
	}
};
void showMergeLP(){
	cout<<"------------show Merge��� MiddLP-----------------"<<endl;
	extendNode * q;
	for(int i = 0;i<MiddLP.size();i++)
	{
		cout<<"  i  "<<i<<"  k:  "<<MiddLP[i].k<<"  b:  "<<MiddLP[i].b<<"  flag:  "<<MiddLP[i].flag<<"   :   "<<endl;
		q = MiddLP[i].down;
		while(q!=NULL)
		{
			cout<<"  ��:  "<<q->C.x<<"  �� "<<q->C.y<<", type "<< q->type<<endl;
			q = q->next;
		}
		cout<<"----------------"<<endl; 
	}
}
void showExtendLP(){
	cout<<"------------show Extend��� MiddLP-----------------"<<endl;
	extendNode * q;
	for(int i = 0;i<MiddLP.size();i++)
	{
		cout<<"  i  "<<i<<"  k:  "<<MiddLP[i].k<<"  b:  "<<MiddLP[i].b<<"  flag:  "<<MiddLP[i].flag<<"   :   "<<endl;
		q = MiddLP[i].down;
		while(q!=NULL)
		{
			cout<<"  ��:  "<<q->C.x<<"  �� "<<q->C.y<<", type "<< q->type<<endl;
			q = q->next;
		}
		cout<<"----------------"<<endl; 
	}
}
void showDeleteLP(){
	cout<<"------------show Delete��� MiddLP-----------------"<<endl;
	extendNode * q;
	for(int i = 0;i<MiddLP.size();i++)
	{
		cout<<"  i  "<<i<<"  k:  "<<MiddLP[i].k<<"  b:  "<<MiddLP[i].b<<"  flag:  "<<MiddLP[i].flag<<"   :   "<<endl;
		q = MiddLP[i].down;
		while(q!=NULL)
		{
			cout<<"  ��:  "<<q->C.x<<"  �� "<<q->C.y<<", type "<< q->type<<endl;
			q = q->next;
		}
		cout<<"----------------"<<endl; 
	}
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
			//cout<<"  ��:  "<<q->C.x<<"  �� "<<q->C.y<<", type "<< q->type<<endl;
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
void swap(int &min,int &max){
	int tmp = min;
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
			//��Midd[q->type]�����p 
			mask *t = Midd[before].next;
			Midd[before].next = p;
			p->next = t;
		//	q = q->next;
			//����
			mask *z=(mask *)malloc(sizeof(mask));
			z->next =NULL; 
			z->i = before;
			//��Midd[q->type]�����p 
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
bool isNext(mask *r){
	int flag = 0 ;
	while(r!=NULL)
	{
		if(Midd[r->i].flag == 0)
			flag =1;
		r = r->next;
	}
	return flag;
}
bool JPinPoly(DPoint A){
	//i��ʾ���� ��Χ��4������һ���ڶ�����ھ��� 
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
//�ı�A��ֱ��outPoly 
DPoint addLength(DPoint A,DPoint B)
{
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
				return addLength(C,B);
		}
		else
		{
			C.y = A.y+a;
			if(!JPinPoly(C))
				return A;
			else
				return addLength(C,B);
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
				return addLength(C,B);
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
				return addLength(C,B);
			}
		}
	}
}
int matrix2File(int i,FILE *fp){
//	cout<<" i "<< i<<" "<<Midd[i].A.x<<" , "<<Midd[i].A.y<<endl;
	if(Midd[i].flag == 1)
		return 0;
	mask * r = Midd[i].next;
	Midd[i].flag =1;
//������ֹ�ߺ��ӱߵĲ��� 
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
		cout<<"length ��:"<<length<<endl;
		if(length>=thresholdlength)
		{
		DPoint C=Midd[i].A; 
		C = addLength(Midd[i].A,Midd[p->i].A);
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
			if(isNext(r->next))
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
void GeFinalFile(string num){
	FILE *fp;
	string path = "./centerline_"+num;
	cout<<path<<endl;
	fp = fopen(path.c_str(), "w");
	int flag = 0;
	for(int i=0;i<Midd.size();i++)
	{
		if(flag !=0)
		{
//		fprintf(fp, "%f ",-3.0);
//		fprintf(fp, "%f ",-3.0);
		fputc('\n', fp);
		flag =0;
		}
		if(Midd[i].flag == 1)
			continue;
		cout<<"i"<<i<<endl;
		matrix2File(i,fp);
		flag =1;
		//cout<<"i"<<endl;
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
		}
}
void showDmidd(){
	cout<<"--------------ë������----------------------"<<endl; 
	for(int i=0;i<Dmiddle.size();i++)
		{			cout<<Dmiddle[i].A.x<<","<<Dmiddle[i].A.y<<endl; 

		}
}
void invokeConect(){
	getMiddlLP();
	merge();
	//showMergeLP();
//	MergeLP2File(num);
	extend();
//	showExtendLP();//extendLP 
	deletePoint();
	//showDeleteLP();
//	Delete2File(num);
	Gematrix();
	Updatmatrix();
//	showUpdataMidd();
	GeFinalFile(num);
	freeMidd();
	freeLP();
} 
int main(int argc , char* argv[])
{
	if (!check(argc, argv))
    {
        return -1;
    }
    //���ļ�
    string fileName(argv[1]);
	int a = fileName.find_last_of('_');
	num = fileName.substr(a);
	cout<<num<<endl;
	invokeScanline();
	cout<<num<<endl; 
	InvokeGetMiddleLine();
	invokeConect();
	return 0;
}
