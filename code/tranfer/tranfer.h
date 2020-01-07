#ifndef _tranfer 
#define _tranfer
#include <iomanip>
#include <fstream>
#include <iostream>
#include"../FileIO/FileIO.h"
#include"../line/line.h"
using namespace std;
void traIn2Line(vector<Point> &in,vector<LinePara> &Line)
{
	LinePara LP;
	for(int i=0;i<in.size();i++)
	{
		getLinePara(in[i],in[(i+1)%in.size()],LP);
		Line.push_back(LP);
		LP.Line.clear();
	}
}
void traPo2Line(list<Point> &in,vector<LinePara> &Line)
{
	LinePara LP;
	for(list<Point>::iterator i=in.begin();i!=in.end();i++)
	{
		list<Point>::iterator j = next(i);
		getLinePara(*i,*(j),LP);
		Line.push_back(LP);
		LP.Line.clear();
	}
}
// ȥ���߶�֮�ڵ��ӵ� 
void DPinLine(vector<LinePara> &MiddLine)
{
	for(int i=0;i<MiddLine.size();i++)
	{
		if(MiddLine[i].Line.size()==2)
			continue;
		Point A = MiddLine[i].Line.front();
		Point B = MiddLine[i].Line.back();
		MiddLine[i].Line.clear();
		MiddLine[i].Line.push_back(A);
		MiddLine[i].Line.push_back(B);
	}
}
//���߶�֮��Ľ��㣬���ѽ�������
void AddCro(vector<LinePara> &MiddLine)
{
	Point Cro;
	for(int i=0;i<MiddLine.size();i++)
	{
		for(int j=i+1;j<MiddLine.size();j++)
		{
			if(getCrossL(MiddLine[i],MiddLine[j],Cro)&&getCrossL(MiddLine[j],MiddLine[i],Cro))
			{
				MiddLine[i].Line.push_back(Cro);
				MiddLine[j].Line.push_back(Cro);
				MiddLine[i].Line.sort();
				MiddLine[j].Line.sort();	
			}	
		}	
	}	
}

//��ȵĺ��� 
bool equal(Point A,Point B,Point &Center)
{
	if(Getlength(A,B)<=0.6)
		return 1;
	else
		return 0;
}
//�ڽӾ�������
void GeMatrix(vector<LinePara> &MiddLine,vector<matrix> &Mitrix)
{
	for(int i=0;i<MiddLine.size();i++)
	{
		for(list<Point>::iterator j=(MiddLine[i].Line).begin();j!=(MiddLine[i].Line).end();j++)
		{
			int flag = 0;
			for(int k=0;k<Mitrix.size();k++)
			{
				Point A = Mitrix[k].A;
				Point C;
				if(equal((*j),A,C))
				{
					flag = 1;
					(*j).type = k;
				}
			}
			if(flag == 0)
			{
				matrix tmp;
				(*j).type = Mitrix.size();
				tmp.A = (*j);
				tmp.flag = 0;
				
				Mitrix.push_back(tmp);
			}
		}
	}
	for(int i=0;i<MiddLine.size();i++)
	{
		for(list<Point>::iterator j=(MiddLine[i].Line).begin();j!=(MiddLine[i].Line).end();j++)
		{
			list<Point>::iterator nextP = next(j);
			if(nextP == (MiddLine[i].Line).end())
				break;
			Point C;
			if(equal((*nextP),(*j),C))
				continue; 
			Mitrix[(*j).type].con.push_back((*nextP).type);
			Mitrix[(*nextP).type].con.push_back((*j).type);
		}
	}
 }
int findNext(int i,vector<matrix> &Midd)
{
	for(int j=0;j<Midd[i].con.size();j++)
	{
		int index = Midd[i].con[j];
		if(Midd[index].flag == 0)
		{
			return index;
		}
	}
	return -1;	
} 
 int Matrix2PointT(int i,vector<matrix> &Midd,int before){
	if(Midd[i].flag == 1)
		return 0;
	int next = findNext(i,Midd);
	if(before == -1||next==-1)
	{
		Midd[i].A.PrD();	
	}
	else
	{
		LinePara LP;
		LinePara LP1;
//		getLinePara(Midd[next].A,Midd[before].A,LP);
		getLinePara(Midd[before].A,Midd[next].A,LP1); 
//		if(!Para(LP,LP1))
//		{
//			Midd[i].A.PrD();
//		}
		double dis;
		getdis(Midd[i].A,LP1,dis);
		if(dis>0.1)
		{
			Midd[i].A.PrD(); 
		}
	}
	before = i;
	Midd[i].flag =1;
	for(int j=0;j<Midd[i].con.size();j++)
	{
		int index = Midd[i].con[j];
		if(Midd[index].flag ==0)
		{
			//�ҵ��˱�ʾ��һ������index, 
			Matrix2PointT(index,Midd,before); 
			//������δ�����ʹ��Ķ���
			int flag = 0;
			for(int k = j;k<Midd[i].con.size();k++)
			{
				if(Midd[Midd[i].con[k]].flag==0)
					flag =1;	
			} 
			if(flag==1)
			{
				cout<<"-2 , -2"<<endl; 
				before = i;
				Midd[i].A.PrD();
			}
			
		}
	}
	
}
 int WritPonit(int i,vector<matrix> &Midd,int before,FILE *fp){
	if(Midd[i].flag == 1)
		return 0;
	int next = findNext(i,Midd);
	if(before == -1||next==-1)
	{
		fprintf(fp, "%f ",Midd[i].A.x);
		fprintf(fp, "%f ",Midd[i].A.y);
		fputc('\n', fp);	
	}
	else
	{
		LinePara LP;
		LinePara LP1;
//		getLinePara(Midd[next].A,Midd[before].A,LP);
		getLinePara(Midd[before].A,Midd[next].A,LP1); 
//		if(!Para(LP,LP1))
//		{
//			Midd[i].A.PrD();
//		}
		double dis;
		getdis(Midd[i].A,LP1,dis);
		if(dis>0.01)
		{
			fprintf(fp, "%f ",Midd[i].A.x);
			fprintf(fp, "%f ",Midd[i].A.y);
			fputc('\n', fp);
		}	
	}
	before = i;
	Midd[i].flag =1;
	for(int j=0;j<Midd[i].con.size();j++)
	{
		int index = Midd[i].con[j];
		if(Midd[index].flag ==0)
		{
			//�ҵ��˱�ʾ��һ������index, 
			WritPonit(index,Midd,before,fp); 
			//������δ�����ʹ��Ķ���
			int flag = 0;
			for(int k = j;k<Midd[i].con.size();k++)
			{
				if(Midd[Midd[i].con[k]].flag==0)
					flag =1;	
			} 
			if(flag==1)
			{
				fprintf(fp, "%f ",-2.0);
				fprintf(fp, "%f ",-2.0);
				fputc('\n', fp);
				before = i;
				fprintf(fp, "%f ",Midd[i].A.x);
				fprintf(fp, "%f ",Midd[i].A.y);
				fputc('\n', fp);
			}
			
		}
	}
	
}
#endif
