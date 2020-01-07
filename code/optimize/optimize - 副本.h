#include"../cor/cor.h"
#include"../line/line.h"
#include"../scanline/scanline.h"
using namespace std;
//const int weight=2500;
//const int height = 5000;
/*ȥ���ӱ�
1.����С��һ����ֵ1
2.���ڶ�����ڵı� 
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
//ɾ������Զ���ӱ�
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
//ȥ���໥ƽ�������غϵı�
//1ƽ��
//2������Ϳ� 
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
						//�ϲ����� 
						flag[j]=1;
					//	cout<<"RUN"<<endl;
						//���Ƚϳ��ıߵ�k��bΪ׼���������������ϵ�ͶӰ 
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
				//�ӳ��ϲ�ƽ���ߵĴ��� 
				else
				{
					LinePara  mer1;
					LinePara mer2;
					getLinePara(MiddLine[i].Line.front(),MiddLine[j].Line.back(),mer1);
					getLinePara(MiddLine[i].Line.front(),MiddLine[j].Line.front(),mer2);
					if(i==0&&j==1)
					{
						mer1.PrL();
						mer2.PrL();
						MiddLine[i].PrL();
						MiddLine[j].PrL();
						cout<<Para(mer1,MiddLine[j])<<endl;
						mer1.Line.front().PrD();
						mer1.Line.back().PrD();
						MiddLine[j].Line.front().PrD();
						MiddLine[j].Line.back().PrD();
					}
					if(Para(mer1,MiddLine[j])&&Para(mer1,mer2))
					{
							double dis;
						distance(MiddLine[i],MiddLine[j],dis); 
						if(dis<extlength)
						{
							//�ϲ����� 
							flag[j]=1;
						//	cout<<"RUN"<<endl;
							//���Ƚϳ��ıߵ�k��bΪ׼���������������ϵ�ͶӰ 
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
//ȥ���ӳ������
void ext(vector<LinePara> &MiddLine)
{
	for(int i=0;i<MiddLine.size();i++)
	{
		Point min(-INT_MAX,-INT_MAX);
		min.type = 1;
		Point max(INT_MAX,INT_MAX);
		max.type = 1; 
		Point Cro;
		Cro.type = 1;
		double record = INT_MAX;
		for(int j=0;j<MiddLine.size();j++)
		{
			if(i==j)
				continue;
			if(getCross(MiddLine[i],MiddLine[j],Cro))
			{
				double dis;
				P2Line(Cro,MiddLine[j],dis);
				if(dis>extlength)
					continue;
				//�������� 
				if(Cro<MiddLine[i].Line.front()||Cro==MiddLine[i].Line.front())
				{
					if(Cro>min)
					{
						min = Cro;
					}
				}
				else if(Cro>MiddLine[i].Line.back()||Cro==MiddLine[i].Line.back())
				{
					if(Cro<max)
					{
						max = Cro;
					}
				}
				//������i���ڲ� 
				else
				{
					distance(MiddLine[i],MiddLine[j],dis);
					if(dis<record)
						record = dis;
					MiddLine[i].Line.push_back(Cro);
				}
			}
			else
			{
				//ƽ�е������������ 
				continue;
			}
		}
		double lengthMin = Getlength(min,MiddLine[i].Line.front());
		double lengthMax = Getlength(max,MiddLine[i].Line.back());
		if(lengthMin<=extlength &&lengthMin<=record)
		{
			MiddLine[i].Line.push_back(min);
		}
		if(lengthMax<=extlength &&lengthMax<=record)
		{
			MiddLine[i].Line.push_back(max);
		}
		MiddLine[i].Line.sort();
	}
} 
//���Ӵ���
 
