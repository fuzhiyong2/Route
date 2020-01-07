#ifndef scanline
#define scanline
#include"../struct/struct.h"
const int weight=2500;
const int height = 5000;
void set(int i,int j,unsigned long int arr[][weight]){
	int shift = 8*(i%4) + (8-(j%8))-1;
	arr[i/4][j/8] |= 1 << (shift);
};
int get(int i,int j,unsigned long int arr[][weight]){
	int shift = 8*(i%4) + (8-(j%8))-1;
	int a = arr[i/4][j/8] & (1 << (shift));
	if(a!= 0)
		return 1;
	else
		return 0;
};
void PolyScan(int &offsetx, int &offsety, int &refrain_minx, int &refrain_maxx, int &refrain_miny, int &refrain_maxy, int &MinY, int &MaxY,unsigned long int arr[][weight], Polygon &poly){
	int i;
	int crosspoint1, crosspoint2;
	AET *pAET = new AET;
	pAET->next = nullptr; 
	NET *pNET[MaxY-MinY+1]; 
	for(i=MinY; i<=MaxY; i++){
		pNET[i-MinY] = new NET;
		pNET[i-MinY]->next = nullptr;
	}
	//生成静态边 ，把静态边给填充起来 
	int count=0;
	for(i=MinY; i<=MaxY; i++){
		for(int j=0; j<poly.size; j++){
			if(poly.coordinate[j].y == i){
				if(poly.coordinate[(j-1+poly.size)%poly.size].y > poly.coordinate[j].y){
					NET *p = new NET;
					p->x = poly.coordinate[j].x;
					p->ymax = poly.coordinate[(j-1+poly.size)%poly.size].y;
					p->dx = (poly.coordinate[(j-1+poly.size)%poly.size].x - poly.coordinate[j].x) * 1.0f / (poly.coordinate[(j-1+poly.size)%poly.size].y - poly.coordinate[j].y);
					p->next = pNET[i-MinY]->next;
					pNET[i-MinY]->next = p;
					count ++;
				}
				if(poly.coordinate[(j+1+poly.size)%poly.size].y > poly.coordinate[j].y){
					NET *p = new NET;
					p->x = poly.coordinate[j].x;
					p->ymax = poly.coordinate[(j+1+poly.size)%poly.size].y;
					p->dx = (poly.coordinate[(j+1+poly.size)%poly.size].x - poly.coordinate[j].x) * 1.0f / (poly.coordinate[(j+1+poly.size)%poly.size].y - poly.coordinate[j].y);
					p->next = pNET[i-MinY]->next;
					pNET[i-MinY]->next = p;
					count ++;
				}
			}
		}
	}
	for(i=MinY; i<MaxY; i++){
		//pAET动态表  pAET最开始是null
		// 更新AET中边的x值，进入下一循环
		NET *p = pAET->next;
		while(p){
			p->x = p->x + p->dx;
			p = p->next;
		}
		//对AET表中的x进行一个从小到大的排序 
		AET *tq = pAET;
		p = pAET->next;
		tq->next = NULL;
		while(p){
			while(tq->next && p->x >= tq->next->x)
                tq=tq->next;
            NET *s=p->next;
            p->next=tq->next;
            tq->next=p;
            p=s;
            tq=pAET;
		}
		//删除AET中满足y=ymax的边
		AET *q=pAET;
        p=q->next;
        while(p)
        {
            if(p->ymax==i)
            {
                q->next=p->next;
                delete p;
                p=q->next;
            }
            else
            {
                q=q->next;
                p=q->next;
            }
        }
		//找到合适的位置插入pAET 
		p=pNET[i-MinY]->next;
        q=pAET;
        while(p)
        {
            while(q->next && p->x >= q->next->x)
                q=q->next;
            NET *s=p->next;
            p->next=q->next;
            q->next=p;
            p=s;
            q=pAET;
        }
		//填充色彩 
		p=pAET->next;
        while(p && p->next)
        {
			
            if(i>=refrain_miny && i< refrain_maxy){
				
				if((p->x < refrain_minx) && (p->next->x > refrain_minx) && (p->next->x < refrain_maxx)){
					crosspoint1 = refrain_minx;
					crosspoint2 = p->next->x;
				
				}else if((p->x < refrain_minx) && (p->next->x > refrain_maxx)){
					crosspoint1 = refrain_minx;
					crosspoint2 = refrain_maxx;
				
				}else if((p->x >= refrain_minx) && (p->next->x <= refrain_maxx)){
					crosspoint1 = p->x;
					crosspoint2 = p->next->x;
				
				}else if((p->x > refrain_minx) && (p->x < refrain_maxx) && (p->next->x > refrain_maxx)){
					crosspoint1 = p->x;
					crosspoint2 = refrain_maxx;
				}else{
					crosspoint1 = 0;
					crosspoint2 = -1;
				}
				for(int j=crosspoint1; j<=crosspoint2; ++j){
					set(i-offsety,j-offsetx,arr);
				}
			}
            p=p->next->next;
        }
	}
	
	for(i=MinY; i<=MaxY; i++){
		delete(pNET[i-MinY]);
	}
	delete(pAET);

} 
void getScanPara(int &offsetx,int &offsety,int &minx,int &miny,int &maxx,int &maxy,vector<Point> in)
{
	offsetx=minx=maxx=in[0].x;
	offsety=miny=maxy=in[0].y;
	for(int i=0;i<in.size();i++)
	{
		int x = in[i].x;
		int y = in[i].y;
		if(int(x)> maxx)
				maxx = int(x);
		if(int(y)> maxy)
				maxy = int(y);
		if(int(x)< minx)
				minx = int(x);
		if(int(y)< miny)
				miny = int(y);	
	}
	offsetx = minx;
	offsety = miny;
} 
void getK(double &k,int minx,int miny,int maxx,int maxy){
	double a=(maxy - miny)* 1.0f/(height*4);
	double c=(maxx - minx)* 1.0f/(weight*8);
	 k = a>c?a:c;
	if (k<1)
	  k =1;
}
void getPoly(double k,int minx,int miny,int maxx,int maxy,vector<Point> in,Polygon &poly)
{
	Point tmp(minx-1,miny-1);
	poly.size = 0;
	for(int i=0;i<in.size();i++)
	{
		Point A(int((in[i].x-minx)/k+minx),int((in[i].y-miny)/k+miny));
		if(A == tmp)
			continue;
		tmp = A;
		poly.coordinate.push_back(A);
		poly.size++;
	}
	poly.maxp.x=int((maxx-minx)/k+minx);
    poly.maxp.y=int((maxy-miny)/k+miny);
    poly.minp.x=minx;
    poly.minp.y=miny;
}
#endif
