#include<iostream>
#include"line.h"
using namespace std;
int main(){
	Point A(0,0);
	Point B(0.1,1);
	LinePara AB;
	getLinePara(A,B,AB);
	AB.PrL(); 
	Point C(1,0);
	double dis;
	getdis(C,AB,dis);
	cout<<dis<<endl;
//	cout<<"--";
//	Point A(923.656,547.176);
//	Point B(972.41 , 642.743);
//	Point C(965.572 , 602.747);
//	Point D(972.41 , 642.743);
//	Point A(976.1,646.737);
//	Point B(1051.24,666.612);
//	
//	Point C(979.1,646.737);
//	Point D(1059.94,668.92);
////	Point test = center(A,B);
//	
//	LinePara LP;
//	LinePara LP1;
//	LinePara LP2;
//	
//	getLinePara(A,B,LP);
//	getLinePara(C,D,LP1);
//	
//	Point E,F;
//	
//	EPoint(LP,LP1,E,F);
//	E.PrD();
//	F.PrD(); 
//
//	double disleng;
//	test.PrD();
//	LP.PrL();
//	getVerticalLine(test,LP,LP1);
//	LP1.PrL();
//	P2Line(test,LP,disleng);
//	cout<<disleng<<endl;
//	cout<<"test"<<endl;
//	Point Cro; 
//	LinePara LP;
//	LinePara LP1;
//	LinePara LA;
//	double dis; 
//	getLinePara(B,A,LP);
//	getLinePara(C,D,LP1);
//	LP.PrL();
//	LP1.PrL();
	//���Խ�ƽ���� 
//	LinePara LA=LP1; 
//	cout<<getAngleLine(LP,LP1,LA);
//	LA.PrL();
//	LA.Line.front().PrD();
//	LA.Line.back().PrD();

//����ƽ�� 
//	cout<<Para(LP,LP1)<<endl; 
//	cout<<getCross(LP,LP1)
//�������߶εĽ���
//	cout<<getCross(LP,LP1,Cro)<<endl;
//	Cro.PrD();
//���Թ�һ������
//	getVerticalLine(A,LP1,LA);
//	LA.PrL();
//	cout<<LA.Line.size()<<endl;
//���Ե����߶��ϵ�ͶӰ
//	shadow(A,LP1,C);
//	C.PrD();
	
//���������߶ε���̾���
//	double dis;
//	distance(LP,LP1,dis);
//	cout<<dis<<endl;
//	cout<<LA.Line.size()<<endl; 
//	double dis;
//	distance(C,D,A,B,dis);
//	cout<<dis<<endl;
	return 0;
}
