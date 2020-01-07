#include<iostream>
#include"Douglar.h"
#include"../struct/struct.h"
#include"../FileIO/FileIO.h"
int main()
{
	list<Point> Poly;
	vector<Point> input;
	int num = 11;
	cin>>num; 
	string path("C:\\Users\\Administrator\\Desktop\\文件夹\\友谊第一_傅智勇.zip-c1ff51c0229b49898c038f7f08044590\\汇报\\第五周\\input\\boundary_"+to_string(num)+".txt");
	string outpath("C:\\Users\\Administrator\\Desktop\\文件夹\\友谊第一_傅智勇.zip-c1ff51c0229b49898c038f7f08044590\\汇报\\第五周\\input\\boundary_"+to_string(num+1000)+".txt");
	int start;
	int end;
	ReadFile(&input,path,start,end);
//	cout<<start<<endl;
//	cout<<End<<endl;
//	for(int i=0;i<input.size();i++)
//		Prin(input[i]);
	list<Point>::iterator a;
	list<Point>::iterator b;
	a = Poly.insert(Poly.begin(),input[0]);
	b = Poly.insert(Poly.end(),input[input.size()-1]);
	Douglar(&Poly,a,b,0,input.size()-1,&input);
//	Prin(Poly.front());
//	Prin(*a);
//	Prin(*b);
//	Prin(*(Poly.begin()));
	cout<<input.size()<<endl;
	cout<<Poly.size()<<endl;
	write(&Poly,outpath);
	return 0;
} 
