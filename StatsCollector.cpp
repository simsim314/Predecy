#include <iostream> 
#include <fstream>
#include <stdio.h> 
#include <vector>
#include <algorithm>
#include <time.h>     
#include <omp.h>
#include <string> 
#include "Common.h"

void GainStats(std::vector<unsigned int>& stats, std::vector<std::vector<bool> >& vec, const std::vector<unsigned int>& ones)
{
	for(int i = 0; i < vec.size(); i++)
		std::fill(vec[i].begin(),vec[i].end(), false);
	
	int size = vec.size(); 
	
	int minx = 5; 
	int miny = 5;
	int maxx = size - 5;
	int maxy = size - 5;
	
	PlaceRandomGliders(vec);
	
	for(int i = 0; i < 450; i++)
	{
		Iterate(vec, minx, miny, maxx, maxy);
		
		if(minx <= 5 || maxx >= size - 5 || miny <= 5 || maxy >= size - 5)
			break;

		for(int y = miny - 4; y < maxy; y++)
		{
			for(int x = minx - 4; x < maxx; x++)
			{
				unsigned int key = Key(vec, x, y, ones, 5);
				
				if(key > 0)
					stats[key]++;
			}
		}
		
	}
}

void CollectStats(std::vector<unsigned int>& stats, int numIters)
{
	std::vector<unsigned int> ones; 
	
	for(int i = 0; i < 25; i++)
		ones.push_back(1 << i);
		
	std::vector<std::vector<bool> > vec;
	
	int size = 150; 
	Init(vec, size);
	
	for(int i = 0; i < numIters; i++)
		GainStats(stats, vec, ones);
	
}

void SaveStats(const std::vector<float>& stats, std::string fileName)
{
	std::ofstream f;
	
	f.open(fileName.c_str());
	
	for(int i = 0; i < stats.size(); i++)
		f << (unsigned long long)(stats[i] * 1000) << "\n";
	
	f.close();
}

int main()
{
	std::vector<float> sharedStats(33554432, 0); 
	int counter = 0; 
	
	#pragma omp parallel shared(sharedStats)
	{
		int tid = omp_get_thread_num();
		srand (time(NULL) % 10000  + tid * 17 + 318);
		
		#pragma omp for
		for(int i = 0; i < 1000000;i++)
		{
			std::vector<unsigned int> stats(33554432, 0); 
			CollectStats(stats, 5000);
			
			#pragma omp critical
			{
				for(int j = 0; j < stats.size(); j++)
					sharedStats[j] += stats[j] / 1000.0;
			
				counter++;
				std::cout << "Iterated:" << counter * 5000 << "\n";
				
				if(counter % 10 == 0)
				{
					SaveStats(sharedStats, "stats.txt");
					std::cout << "writing" << counter << "\n";
				}
			}
		}
		
	}
}