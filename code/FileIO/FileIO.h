#ifndef _FileIO 
#define _FileIO 
#include<list>
#include<vector>
#include<iostream>
#include <fstream>
#include<math.h>
#include<string>
#include"../struct/struct.h"
//文件名读成vector数组 
bool ReadFile(vector<Point> *input,string filename,int &start,int &end){
	ifstream infile;
	infile.open(filename);
	string type;
	//int count = 0;
	Point tmp; 
	tmp.x=INT_MAX;
	tmp.y =INT_MAX;
	while(infile!=NULL)
	{
			infile>>type;
            if(type[0]=='V')
            {
            string name;
            infile>>name;
            infile>>tmp.x;
            infile>>tmp.y;
            (*input).push_back(tmp);
			}
			if(type[0] == 'E')
			{
				int x;
				int y;
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
				end=x<y?x:y;
				if(fabs(y-x)!=1)
				{
					end =x<y?y:x;
				}
				if(start>end)
		    	{
		    		int tmp = start;
					start = end;
					end = tmp; 
				}
				return 1; 
			}
			type ="";
	}
	return 0; 
}
void write(list<Point> *Poly,string outpath)
{
	FILE *fp;
	fp = fopen(outpath.c_str(), "w");
	int count=0;
	for(list<Point>::iterator i=(*Poly).begin();i!=(*Poly).end();i++)
	{
		fprintf(fp,"V ");
		fprintf(fp,"%d ",count);
		fprintf(fp, "%f ",(*i).x);
		fprintf(fp, "%f ",(*i).y);
		fprintf(fp,"\n");
		count++;
	}
	
}
void writeEdge(vector<LinePara> &Edge,string outpath)
{
	FILE *fp;
	fp = fopen(outpath.c_str(), "w");
	for(int i=0;i<Edge.size();i++)
	{
		fprintf(fp, "%f ",Edge[i].Line.front().x);
		fprintf(fp, "%f ",Edge[i].Line.front().y);
		fprintf(fp, "%f ",Edge[i].Line.back().x);
		fprintf(fp, "%f ",Edge[i].Line.back().y);
		fprintf(fp,"\n");
	}
}
string input(string &File)
{
	cout<<"Enter the data order"<<endl;
	int a;
	cin>>a;
	string b = to_string(a);
	File = File+b+".txt";
	return b;
}
#endif
