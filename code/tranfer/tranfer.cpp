#include"./tranfer.h"
#include"../cor/cor.h" 
#include"../getLine/getLine.h" 
#include"../scanline/scanline.h"
#include"../tranfer/tranfer.h"
#include"../Doug/Douglar.h"
#include"../optimize/optimize.h"
#include <iomanip>
using namespace std;
unsigned long int arr[height][weight]={0};
void getRelate(vector<Relate> &relat,vector<LinePara> Edge,int flag,int start,int end)
{
	Relate tmp;
	int mask[Edge.size()];
	for(int i=0;i<Edge.size();i++)
	{
		mask[i] = 0;
	}
	if(flag == 0)
	{
		for(int i = 0;i<Edge.size();i++)
		{
			tmp.index = Edge[i];
			tmp.size = 0;
			tmp.relate.clear();
			for(int j=i+2;j<Edge.size();j++)
				{
					if(SameGroup(Edge[i],Edge[j]))
					{
						tmp.relate.push_back(Edge[j]);
						tmp.size++;	
						mask[j] =1;
					}
				}
			if(tmp.size!=0)
			{
				relat.push_back(tmp);
				mask[i]=1;
			}
		};
		//��ƽ����������� 
		for(int i = 0;i<Edge.size();i++)
		{
			tmp.index = Edge[i];
			tmp.size = 0;
			tmp.relate.clear();
			if(mask[i]==1)
				continue;
			for(int j=i+2;j<Edge.size();j++)
				{
					if(mask[j]==1)
						continue;
					if(SameGroupA(Edge[i],Edge[j]))
					{
						tmp.relate.push_back(Edge[j]);
						tmp.size++;	
					}
				}
			if(tmp.size!=0)
			{
				relat.push_back(tmp);
			}
		}
	}
	//����ʼ�� 
	else
	{
		for(int i =start+1;i<end;i++)
    	{
    		tmp.index = Edge[i];
			tmp.size = 0;
			tmp.relate.clear();
    		for(int k=end+1;k<Edge.size()+start;k++)
    		{
    			int j = k%Edge.size();
    			if(abs(i-j)!=1) 
    				{
    					if(SameGroup(Edge[i],Edge[j]))
						{
							tmp.relate.push_back(Edge[j]);
							tmp.size++;	
							mask[j] =1;
						}
					}
			}
			if(tmp.size!=0)
			{
				relat.push_back(tmp);
				mask[i] =1;
			}
		};
		for(int i =start+1;i<end;i++)
    	{
    		if(mask[i]==1)
    			continue;
    		tmp.index = Edge[i];
			tmp.size = 0;
			tmp.relate.clear();
    		for(int k=end+1;k<Edge.size()+start;k++)
    		{
    			if(mask[k]==1)
    				continue;
    			int j = k%Edge.size();
    			if(abs(i-j)!=1) 
    				{
    					if(SameGroupA(Edge[i],Edge[j]))
						{
							tmp.relate.push_back(Edge[j]);
							tmp.size++;	
						}
					}
			}
			if(tmp.size!=0)
			{
				relat.push_back(tmp);
			}
		} 
	}
}
void showRelat(vector<Relate> relat)
{
	for(int i=0;i<relat.size();i++)
		relat[i].show();
	cout<<"OK"<<endl;
}
void showEdge(vector<LinePara> Edge)
{
	cout<<Edge.size()<<endl;
	for(int i=0;i<Edge.size();i++)
	{
		cout<<i<<" ";
		Edge[i].PrL();
		Edge[i].Line.front().PrD();
		Edge[i].Line.back().PrD();
	}
	cout<<"OK"<<endl;
}
void showMatrix(vector<matrix> Midd)
{
	for(int i=0;i<Midd.size();i++)
	{
		cout<<i<<" ";
		Midd[i].A.PrD();
		for(int j=0;j<Midd[i].con.size();j++)
		{
			cout<<Midd[i].con[j]<<" ";
		}
		cout<<endl;
	}
}
int main()
{
	//��ȡ�ļ� 
		for(int i=576;i<=576;i++)
	{
		string b = to_string(i);
		string File = "../../input/boundary_";
		File = File+b+".txt"; 
		string OutFile = "../../output1/centerline_"+b+".txt";
		cout<<File<<endl;
		cout<<OutFile<<endl;
	vector<Point> in;
	int start=0;
	int end=0;
	int flag = ReadFile(&in,File,start,end);
	cout<<start<<","<<end<<endl;
	//ɨ�����㷨 
	PolyPara poPar;
	Polygon poly;
	getScanPara(poPar.offsetx,poPar.offsety,poPar.minx,poPar.miny,poPar.maxx,poPar.maxy,in);
	getK(poPar.k,poPar.minx,poPar.miny,poPar.maxx,poPar.maxy);
	poPar.PrPar();
	getPoly(poPar.k,poPar.minx,poPar.miny,poPar.maxx,poPar.maxy,in,poly);
	PolyScan(poPar.offsetx,poPar.offsety,poPar.minx,poPar.maxx,poPar.miny,poPar.maxy,poPar.miny,poPar.maxy,arr,poly);
	poly.coordinate.clear();
	//Ѱ���໥��Ӧ��ϵ�ı� 
	cout<<"RUN"<<endl;
	vector<LinePara> Edge;
	//ȥ�ӵ�
//	list<Point> Poly;
//	list<Point>::iterator be;
//	list<Point>::iterator en;
//	be = Poly.insert(Poly.begin(),in[0]);
//	en = Poly.insert(Poly.end(),in[in.size()-1]);
//	Douglar(&Poly,be,en,0,in.size()-1,&in);
//	
//	
//	traPo2Line(Poly,Edge);
	traIn2Line(in,Edge);
	//showEdge(Edge);
	cout<<Edge.size()<<endl;
	vector<Relate> relat;
	getRelate(relat,Edge,flag,start,end);
	cout<<"getRelate compeltely"<<endl;
	//showRelat(relat);
	//��Edge��relat,��������߽ṹ
	vector<LinePara> MiddLine;
	vector<LinePara> next;
	getMiddLine(relat,MiddLine);
	del(MiddLine,next,arr,poPar);//Ӧ��ok
	//һ��ת�� 
	//rm(next,MiddLine);
	//showEdge(MiddLine);
	mer(next,MiddLine);
	DelCro(MiddLine,next);
//	MiddLine = next;
	ext(next);
	DPinLine(next);
	AddCro(next);
	writeEdge(next,OutFile);
	vector<matrix> Mitrix; 
	GeMatrix(next,Mitrix);
	
	//showEdge(MiddLine);
	showMatrix(Mitrix);
	
	//cout<<findNext(1,Mitrix)<<endl; 
	//cout<<st<<endl;
	FILE *fp;
	string path = "../../output2/referline_"+b+".txt";
	fp = fopen(path.c_str(), "w");
	int st=0;
	while(st!=-1)
	{
		st =-1;
		for(int i=0;i<Mitrix.size();i++)
		{
			if(Mitrix[i].con.size()==1&&Mitrix[i].flag==0)
			{
				st=i;
				break;
			}
		}
		if(st !=-1)
		{
		//Matrix2PointT(st,Mitrix,-1);
		WritPonit(st,Mitrix,-1,fp);
		fprintf(fp, "%f ",-3.0);
		fprintf(fp, "%f ",-3.0);
		fputc('\n', fp);
		}
	}
	//showEdge(next);
	
	//ext(next);
	//writeEdge(next,OutFile);
	}
	//�Ż�����......�� 
}
