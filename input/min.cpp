#include <iostream>
#include <list>
#include <iomanip>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <vector>
#include <cstdio>
#include<math.h>
//#include <ctchar>
//#define INT_MAX 2147483647
using namespace std;

int main()
{
	string inFile = "result2";
	ifstream infile;
	infile.open(inFile);
	double x;
	double tmp=100;
	int i = 80;
	int test = 80;
	while (infile !=NULL)
	{
		infile>>x;
		if(tmp>x)
		{
			tmp = x;
			test = i;
			}
		i++;
	} 
	cout<<test<<endl;
	cout <<tmp<<endl;
	return 0;
 } 
