#include <iostream> 
#include <fstream>
#include <stdio.h> 
#include <vector>
#include <algorithm>
#include <time.h>     
//#include <omp.h>
#include <string> 
#include <fstream>
#include <map> 
#include "Common.h"
#include <math.h>

void PrintInputKey(int inKey, const std::vector<unsigned int>& ones, int size)
{
	int i = 0; 

	for(int y = 0; y < size; y++)
	{
		for(int x = 0; x < size; x++)
		{
			if((inKey & ones[i]) == ones[i])
				std::cout << "O";
			else 
				std::cout << ".";
			
			i++;
		}
		
		std::cout << "\n";
	}
}

void GetVals(int inKey, bool* vals, const std::vector<unsigned int>& ones, int size, int ftype)
{
	int i = 0; 

	int i1 = ftype % 2; 
	int i2 = (ftype / 2) % 2; 
	int i3 = (ftype / 4) % 2; 
	
	for(int y = 0; y < size; y++)
	{
		for(int x = 0; x < size; x++)
		{
			int y1 = y; 
			int x1 = x; 
			
			if(i1 == 0)
				y1 = (size - 1) - y;
			
			if(i2 == 0)
				x1 = (size - 1) - x;
			
			bool v = ((inKey & ones[i]) == ones[i]);
			
			if(i3 == 0)
				vals[size * x1 + y1] = v;
			else 
				vals[size * y1 + x1] = v;
				
			i++;
		}
	}
}
	
void Load(std::string fname, std::vector<std::pair<long long, int> >& scoreVec)
{
	std::ifstream infile(fname.c_str());
	unsigned long long a;
	int idx = 0;
	
	while (infile >> a)
	{
		scoreVec.push_back(std::pair<long long, int>(a, idx));
		idx++;
	}
	
	//vec[0] = 2^63 - 1 (just max value as stats were ignoring it). 
	scoreVec[0].first = 9223372036854775807ULL;
}

bool AreSame(int inKey1, int inKey2, const std::vector<unsigned int>& ones, int size)
{
	int i = 0; 
	bool vals1[size * size];
	bool vals2[size * size];
	
	for(int i = 0; i < 8; i++)
	{
		GetVals(inKey1, vals1, ones, size, i);
		GetVals(inKey2, vals2, ones, size, 0);
		
		bool same = true; 
		
		for(int y = 0; y < size * size; y++)
		{
			if(vals1[y] != vals2[y])
				same = false;

		}
		
		if(same)
			return true;
	}
	
	return false;
}

int main()
{
	std::vector<std::pair<long long, int> > scoreVec;
	Load("stats.txt", scoreVec);
	
	std::vector<unsigned int> ones; 
	
	for(int i = 0; i < 25; i++)
		ones.push_back(1 << i);					
	
	std::sort(scoreVec.begin(), scoreVec.end(), std::greater<std::pair<long long, int> >());
	
	std::vector<std::pair<long long, int> > unique;
	
	for(int i = 0; i < 500; i++)
	{
		bool same = false; 
		
		for(int j = 0; j < unique.size(); j++)
		{
			if(AreSame(unique[j].second, scoreVec[i].second, ones, 5))
			{
				same = true;
				break;
			}
		}
		
		if(!same)
		{
			unique.push_back(scoreVec[i]);
			std::cout << unique.size() << " : " << (scoreVec[i].first / 1000000) << "M\n" ;
			PrintInputKey(scoreVec[i].second, ones, 5);
			std::cout << "\n\n";
		}
	}

	getchar();	
	
}