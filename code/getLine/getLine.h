#ifndef getLine 
#define getLine
#include"../line/line.h"
#include"../cor/cor.h"
using namespace std;
void sort(Relate &relat);
bool MerPaLine(vector<LinePara> Pa,LinePara &LP);
//求中心线，有一对一和一对多的情况。一对多取距离最近，长度最长 
void getMiddLine(vector<Relate> relat,vector<LinePara> &MidLine)
{
	double threshold = 5e-1;
	for(int i=0;i<relat.size();i++)
	{
		LinePara current = relat[i].index;
		sort(relat[i]);
		vector<LinePara> tmp;
		double min;
		distance(current,relat[i].relate[0],min);
		for(int j=0;j<relat[i].relate.size();j++)
		{
			double dis;
			distance(current,relat[i].relate[j],dis);
			if(dis>min+threshold)
			{
				break;
			}
			else
			{
				Over over;
				LinePara LP;
				getRegion(current,relat[i].relate[j],over);
				getMiddPara(current,relat[i].relate[j],LP);
				if(RegionCross(LP,over))
				{
					tmp.push_back(LP);
				}
			}
		}
		if(tmp.size()==0)
			continue;
		LinePara LP;
		if(MerPaLine(tmp,LP))
		{
			MidLine.push_back(LP);
			for(int i=0;i<tmp.size();i++)
				if(!Para(LP,tmp[i]))
					MidLine.push_back(tmp[i]);
		}
		else
		{
			for(int i=0;i<tmp.size();i++)
			{
				MidLine.push_back(tmp[i]);
			}
		}
	}	
}
void sort(Relate &relat){
			vector<LinePara> *relate=&(relat.relate);
			LinePara index = relat.index;
			for(int i=0;i<(*relate).size();i++)
			{
				int min = i;
				double dimin;
				distance(index,(*relate)[i],dimin);
				for(int j=i;j<(*relate).size();j++)
				{
					double tmp;
					distance(index,(*relate)[j],tmp);
					if(tmp<dimin)
					{
						min = j;
						dimin = tmp;
					}
				}
				//交换i,min
				LinePara tmp;
				tmp = relat.relate[i];
				relat.relate[i]=relat.relate[min];
				relat.relate[min]=tmp;
			}
		}
//合并平行的多条边 
bool MerPaLine(vector<LinePara> Pa,LinePara &LP)
{
	bool back = 0;
	vector<LinePara> Chird;
	if(Pa.size()<=1)
		return 0;
	for(int i=0;i<Pa.size();i++)
	{
		int flag = 0;
		for(int j=i+1;j<Pa.size();j++)
			{
				Pa[i].PrL();
				Pa[j].PrL();
				if(Para(Pa[i],Pa[j]))
				{
					flag = 1;
					Chird.push_back(Pa[j]);
				}	
			}
		if(flag==1)
		{
			Chird.push_back(Pa[i]);
			back = 1;
		}
	}
	if(back==0)
		return 0;
	//得到长度最长的那条边的下标
	int max = 0;
	for(int i=0;i<Chird.size();i++)
	{
		if(Chird[i].length>Chird[max].length)
			max = i;
	}
	//投影后求最大最小的点 
	Point minP=Chird[max].Line.front();
	Point maxP=Chird[max].Line.back();
	Point update1;
	Point update2;
	for(int i=0;i<Chird.size();i++)
	{
		shadow(Chird[i].Line.front(),Chird[max],update1);
		shadow(Chird[i].Line.back(),Chird[max],update2);
		if(update1<minP)
			minP = update1;
		if(maxP<update2)
			maxP = update2;	
	} 
//	update1.PrD();
//	update2.PrD(); 
	getLinePara(minP,maxP,LP);
	return back;
}
#endif
