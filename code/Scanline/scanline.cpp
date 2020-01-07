#include"scanline.h"
#include"../FileIO/FileIO.h"
using namespace std;
unsigned long int arr[height][weight]={0};
void show()
{

}
int main(){
	string File = "../../input/boundary_";
	input(File);
	//cout<<File<<endl;
	vector<Point> in;
	int start;
	int end;
	int flag = ReadFile(&in,File,start,end);
	cout<<flag<<endl;
	PolyPara poPar;
	Polygon poly;
	getScanPara(poPar.offsetx,poPar.offsety,poPar.minx,poPar.miny,poPar.maxx,poPar.maxy,in);
	getK(poPar.k,poPar.minx,poPar.miny,poPar.maxx,poPar.maxy);
	poPar.PrPar();
	getPoly(poPar.k,poPar.minx,poPar.miny,poPar.maxx,poPar.maxy,in,poly);
	
	
	cout<<poly.coordinate.size()<<endl;
//	cout<<in.size()<<endl;
	PolyScan(poPar.offsetx,poPar.offsety,poPar.minx,poPar.maxx,poPar.miny,poPar.maxy,poPar.miny,poPar.maxy,arr,poly);
//	poPar.PrPar();
//	cout<<get(9,10,arr)<<endl;
}
