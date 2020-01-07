#include"getLine.h"
using namespace std;
int main()
{
	Point A(0,0);
	Point B(-1,1);
	LinePara AB;
	getLinePara(A,B,AB);
	
	Point C(1,0);
	Point D(0,1);
	LinePara CD;
	getLinePara(C,D,CD);
	
	Point G(0,1);
	Point H(-1,2);
	LinePara GH;
	getLinePara(G,H,GH);
//	
//	Point E(1,0.5);
//	Point F(1.4,1.5);
//	LinePara EF;
//	getLinePara(E,F,EF);
	
	Relate relat;
	relat.index = AB;
	
	relat.relate.push_back(CD);
//	relat.relate.push_back(EF);
	relat.relate.push_back(GH);
	//relat.show();
	sort(relat);
//	relat.show();
	vector<LinePara> MiddLine;
	vector<Relate> relate;
	relate.push_back(relat);
	getMiddLine(relate,MiddLine);
	cout<<MiddLine.size();
	for(int i=0;i<MiddLine.size();i++)
	{
		MiddLine[i].PrL();
		MiddLine[i].Line.front().PrD();
		MiddLine[i].Line.back().PrD();
	}
//	LinePara LP;
//	cout<<MiddLine.size()<<endl;
//	cout<<MerPaLine(MiddLine,LP)<<endl;
//	LP.PrL();
//	LP.Line.front().PrD();
//	LP.Line.back().PrD(); 
} 
