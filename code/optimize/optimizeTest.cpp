
#include"./optimize.h"
using namespace std;
unsigned long int arr[height][weight]={0};
int main()
{

	//showRelat(relat);
	//��Edge��relat,��������߽ṹ
//	vector<LinePara> MiddLine;
//	vector<LinePara> next;
//	getMiddLine(relat,MiddLine);
//	del(MiddLine,next,arr,poPar);//Ӧ��ok
//	//һ��ת�� 
//	//rm(next,MiddLine);
//	//showEdge(MiddLine);
//	mer(next,MiddLine); 
//	ext(MiddLine);
//	writeEdge(MiddLine,OutFile);
	//showEdge(next);
	LinePara LP1;
	LinePara LP2;
	Point A(0,0);
	Point B(3,0); 
	Point C(2.5,-1);
	Point D(3.5,1);
	
	getLinePara(A,B,LP1);
	getLinePara(C,D,LP2);
	vector<LinePara> MiddLine;
	MiddLine.push_back(LP1);
	MiddLine.push_back(LP2);
	
	vector<LinePara> next;
	DelCro(MiddLine,next);
	for(int i=0;i<next.size();i++)
	{
		next[i].PrL();
		next[i].Line.front().PrD();
		next[i].Line.back().PrD();
	}
	//ext(next);
	//writeEdge(next,OutFile);
	//�Ż�����......�� 
}
