#include"../cor/cor.h"
#include"../line/line.h"
#include"../scanline/scanline.h"
using namespace std;
//const int weight=2500;
//const int height = 5000;
/*去掉杂边
1.长度小于一个阈值1
2.不在多边形内的边 
*/
void del(vector<LinePara> &MiddLine,vector<LinePara> &result,unsigned long int arr[][weight],PolyPara para)
{
	double threshold = 1;
	for(vector<LinePara>::iterator it=MiddLine.begin();it!=MiddLine.end();it++)
	{
		if((*it).length<threshold)
		{
			continue;
		}
		if(!inPoly((*it).Line.front(),(*it).Line.back(),para.k,para.minx,para.miny,para.maxx,para.maxy,arr))
		{
			continue;
		}
		result.push_back(*it);
	}
	MiddLine.clear();
} 
//1.删掉距离远的杂边
void rm(vector<LinePara> &MiddLine,vector<LinePara> &rm)
{
	for(int i=0;i<MiddLine.size();i++)
	{
		double min=INT_MAX;
		double dis;
		for(int j=0;j<MiddLine.size();j++)
		{
			if(i==j)
				continue;
			distance(MiddLine[i],MiddLine[j],dis);
			if(dis<min)
				min = dis;
		}
		if(min<MiddLine[i].length)
			rm.push_back(MiddLine[i]);
	}
	MiddLine.clear();	
} 
double extlength  = 5;
//2.合并平行线去掉相互平行且有重合的边
//1平行
//2距离近就可 
void mer(vector<LinePara> &MiddLine,vector<LinePara> &result)
{
	double threshold = 0.5;
	int flag[MiddLine.size()];
	for(int i=0;i<MiddLine.size();i++)
	{
		flag[i] = 0; 
	}
	for(int i=0;i<MiddLine.size();i++)
	{
		if(flag[i]==1)
			continue;
		for(int j=i+1;j<MiddLine.size();j++)
		{
			if(flag[j] == 1)
				continue;
			if(Para(MiddLine[i],MiddLine[j]))
			{
				LinePara LP;
				if(shadLine(MiddLine[i],MiddLine[j],LP))
				{
					double dis;
					distance(MiddLine[i],MiddLine[j],dis); 
					if(dis<threshold)
					{
						//合并操作 
						flag[j]=1;
					//	cout<<"RUN"<<endl;
						//长度较长的边的k，b为准，其他的是其在上的投影 
						int max=i;
						int min=j;
						if(MiddLine[max].length<MiddLine[j].length)
						{
							max = j;
							min = i;
						}	
						Point update1;
						Point update2;
						shadow(MiddLine[min].Line.front(),MiddLine[max],update1);
						shadow(MiddLine[min].Line.back(),MiddLine[max],update2);
						if(update1>MiddLine[max].Line.front())
							update1 = MiddLine[max].Line.front();
						if(update2<MiddLine[max].Line.back())
							update2 = MiddLine[max].Line.back();
						getLinePara(update1,update2,MiddLine[i]);
					}
				} 
			}
		}
	}
	for(int i=0;i<MiddLine.size();i++)
	{
		if(flag[i]==0)
		{
			result.push_back(MiddLine[i]);
		}
	}
	MiddLine.clear();
} 
//去掉延长处理边 
//3.删交线
void DelCro(vector<LinePara> &Middle,vector<LinePara> &next)
{
	int flag[Middle.size()];
	for(int i=0;i<Middle.size();i++)
	{
		flag[i]=0;
	}
	for(int i=0;i<Middle.size();i++)
	{
		if(flag[i]==1)
			continue;
		for(int j=i+1;j<Middle.size();j++)
		{
			Point C;
			if(getCrossL(Middle[i],Middle[j],C)&&getCrossL(Middle[j],Middle[i],C))
			{
				double dis1 = Getlength(Middle[i].Line.front(),C);
				double dis2 = Getlength(Middle[j].Line.front(),C);
				
				double dis3 = Getlength(Middle[i].Line.back(),C);
				double dis4 = Getlength(Middle[j].Line.back(),C);
				
				int flag1 = 0;
				int flag2 = 0;
				
				Point A  =Middle[i].Line.front();
				Point B = Middle[i].Line.back();
				Point E = Middle[j].Line.front();
				Point F = Middle[j].Line.back();
				if(dis1>=dis2)
				{
					flag1 = 1;
				}
				if(dis3>=dis4)
				{
					flag2 = 1;
				}
				if(flag1==1&&flag2==1)
				{
					flag[j] =1;
				}
				else if(flag1==0&&flag2==0)
				{
					flag[i] = 1;
				}
				else if(flag1==1&&flag2 ==0)
				{
					getLinePara(A,C,Middle[i]);
					getLinePara(F,C,Middle[j]);
				}
				else
				{
					getLinePara(E,C,Middle[j]);
					getLinePara(C,B,Middle[i]);
				}
			}
		}
		if(flag[i]==0)
		{
			next.push_back(Middle[i]);
		}
	}
	Middle.clear();
} 
//4.延长和拉伸处理

//4.1拉伸两个平行线更短的那条 
void change(LinePara &LP,LinePara &LP1)
{
	//LP的长度更大 
	int flag1;
	int flag2;
	EPoint(LP,LP1,flag1,flag2);
	
	Point update1;
	Point update2;
	if(flag1 == 0)
	{
		update1 = LP.Line.front();
	}
	else
	{
		update1 = LP.Line.back();
	}
	if(flag2 == 0)
	{
		update2 = LP1.Line.front();
	}
	else
	{
		update2 = LP1.Line.back();
	}
	if(LP.length>LP1.length)
	{
		if(0 == flag2)
		{
			Point C =LP1.Line.back();
			getLinePara(update1,C,LP1);
		}
		else
		{
			Point C =LP1.Line.front();
			getLinePara(update1,C,LP1);
		}
	}
	else
	{
		if(0 == flag1)
		{
			Point C =LP.Line.back();
			getLinePara(update2,C,LP);
		}
		else
		{
			Point C =LP.Line.front();
			getLinePara(update2,C,LP);
		}
	}
}
double extlengthK = 1;
//4.2拉伸处理 
void ext(vector<LinePara> &MiddLine)
{
	for(int i=0;i<MiddLine.size();i++)
	{
		Point A = MiddLine[i].Line.front();
		Point B = MiddLine[i].Line.back();
		double minA = INT_MAX;
		int miA = i;
		double minB = INT_MAX;
		int miB = i;
		double dis;
		for(int j=0;j<MiddLine.size();j++)
		{
			if(i==j)
				continue;
			P2Line(A,MiddLine[j],dis);
			if(dis<minA)
			{
				miA = j;
				minA = dis;
			}
			P2Line(B,MiddLine[j],dis);
			if(dis<minB)
			{
				miB = j;
				minB=dis;
			} 
		}
		if(minA<extlengthK*MiddLine[i].length)
		{
			Point Cro;
			Cro.type = 1;
			if(getCross(MiddLine[i],MiddLine[miA],Cro))
			{
				double Cdis;
				P2Line(Cro,MiddLine[i],Cdis);
				if(Cdis<=2*minA)
				{
					MiddLine[i].Line.push_back(Cro);
					MiddLine[miA].Line.push_back(Cro); 
					MiddLine[miA].Line.sort();
				}
				else
				{
					//这里并不是拉升，而是连接起来 
					change(MiddLine[i],MiddLine[miA]);
				}
			}
			//平行的情况 
			else
			{
				change(MiddLine[i],MiddLine[miA]);
			}
		}
		MiddLine[i].Line.sort();
		if(minB<extlengthK*MiddLine[i].length)
		{
			Point Cro;
			Cro.type = 1;
			if(getCross(MiddLine[i],MiddLine[miB],Cro))
			{
				double Cdis;
				P2Line(Cro,MiddLine[i],Cdis);
				if(Cdis<=2*minB)
				{
					MiddLine[i].Line.push_back(Cro);
					MiddLine[miB].Line.push_back(Cro);
					MiddLine[miB].Line.sort();
				}
				else
				{
					change(MiddLine[i],MiddLine[miB]);
				}
			}
			//平行的情况 
			else
			{
				change(MiddLine[i],MiddLine[miB]);
			}
		}
		MiddLine[i].Line.sort();
	}
} 
 
