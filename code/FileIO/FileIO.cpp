#include"../FileIO/FileIO.h"
#include"../tranfer/tranfer.h"
using namespace std;
void showPoint(vector<Point> L)
{
	for(int i=0;i<L.size();i++)
	{
		L[i].PrD();
	}
}
int main()
{
	string File = "../../input/boundary_";
	string OutFile = "../../output/centerline_";
	string b = input(File);
	OutFile = OutFile +b+".txt";
	//cout<<File<<endl;
	vector<Point> in;
	int start = 0;
	int end = 0;
	int flag = ReadFile(&in,File,start,end);
	showPoint(in);
	cout<<start<<","<<end<<endl;
	vector<LinePara> Edge;
//	traIn2Line(in,Edge);
//	writeEdge(Edge,OutFile);
}
