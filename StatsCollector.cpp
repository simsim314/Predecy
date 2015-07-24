#include <iostream> 
#include <fstream>
#include <stdio.h> 
#include <vector>
#include <algorithm>
#include <time.h>     
#include <omp.h>
#include <string> 

bool NextVal(const std::vector<std::vector<bool> > &vals, int x, int y)
{
	int sum = 0;
	
	for(int i = -1; i <= 1; i++)
		for(int j = -1; j <= 1; j++)
			if(vals[y + i][x + j])
				sum ++;
	
	if(sum == 3)
		return true;
		
	if(vals[y][x] && sum == 4)
		return true;
	
	return false;
}
bool Iterate(std::vector<std::vector<bool> > &vals, int& minx, int& miny, int& maxx, int& maxy)
{
	std::vector<bool> prev2(maxx - minx + 1, false);
	std::vector<bool> prev(maxx - minx + 1, false);
	std::vector<bool> cur(maxx - minx + 1, false);
	
	int cminx = maxx + 1;
	int cminy = maxy + 1;
	int cmaxx = minx - 1;
	int cmaxy = miny - 1;
	
	for(int y = miny; y <= maxy + 2; y++)
	{
		int i = 0; 
		
		for(int x = minx; x <= maxx; x++, i++)
		{
			cur[i] = NextVal(vals, x, y);
			
			if(cur[i] && x < cminx)
				cminx = x;
			
			if(cur[i] && x > cmaxx)
				cmaxx = x;
				
			if(cur[i] && y < cminy)
				cminy = y;
			
			if(cur[i] && y > cmaxy)
				cmaxy = y;
				
			vals[y - 2][x] = prev2[i];
		}
		
		prev2.swap(prev);
		cur.swap(prev);
	}
	
	if(cminy - 3 < 0)
		return false;
	
	if(cminx - 2 < 0)
		return false;
	
	if(cmaxy + 3 >= vals.size())
		return false;
	
	if(cmaxx + 2 >= vals[0].size())
		return false;
		
	minx = cminx - 1;
	miny = cminy - 1;
	
	maxx = cmaxx + 1;
	maxy = cmaxy + 1;
}

void Init(std::vector<std::vector<bool> >& vec, int size)
{
	for(int i = 0; i < size; i++)
		vec.push_back(std::vector<bool>(size, false));
}

void Print(std::vector<std::vector<bool> >& vec)
{
	for(int i = 0; i < vec.size(); i++)
	{
		for(int j = 0; j < vec[i].size(); j++)
		{
			if(vec[i][j])
				std::cout << "X";
			else
				std::cout << "_";
		}
		
		std::cout << std::endl;
		
	}
	
	std::cout << std::endl;
}

void PlaceGlider(std::vector<std::vector<bool> >& vec, int x, int y, int gliderType)
{
	if(gliderType == 0)
	{
		vec[y + 0][x + 1] = true;
		vec[y + 1][x + 0] = true;
		vec[y + 2][x + 0] = true;
		vec[y + 2][x + 1] = true;
		vec[y + 2][x + 2] = true;
	}
	else if(gliderType == 1)
	{
		vec[y + 1][x + 0] = true;
		vec[y + 1][x + 2] = true;
		vec[y + 2][x + 0] = true;
		vec[y + 2][x + 1] = true;
		vec[y + 3][x + 1] = true;

	}
	else if(gliderType == 2)
	{
		vec[y + 1][x + 0] = true;
		vec[y + 2][x + 0] = true;
		vec[y + 2][x + 2] = true;
		vec[y + 3][x + 0] = true;
		vec[y + 3][x + 1] = true;
	}
	else if(gliderType == 3)
	{
		vec[y + 1][x + 1] = true;
		vec[y + 2][x + -1] = true;
		vec[y + 2][x + 0] = true;
		vec[y + 3][x + 0] = true;
		vec[y + 3][x + 1] = true;

	}
	else if(gliderType == 4)
	{
		vec[y + 0][x + 1] = true;
		vec[y + 1][x + 2] = true;
		vec[y + 2][x + 0] = true;
		vec[y + 2][x + 1] = true;
		vec[y + 2][x + 2] = true;
	}
	else if(gliderType == 5)
	{
		vec[y + 1][x + 0] = true;
		vec[y + 1][x + 2] = true;
		vec[y + 2][x + 1] = true;
		vec[y + 2][x + 2] = true;
		vec[y + 3][x + 1] = true;
	}
	else if(gliderType == 6)
	{
		vec[y + 1][x + 2] = true;
		vec[y + 2][x + 0] = true;
		vec[y + 2][x + 2] = true;
		vec[y + 3][x + 1] = true;
		vec[y + 3][x + 2] = true;
	}
	else if(gliderType == 7)
	{
		vec[y + 1][x + 1] = true;
		vec[y + 2][x + 2] = true;
		vec[y + 2][x + 3] = true;
		vec[y + 3][x + 1] = true;
		vec[y + 3][x + 2] = true;
	}
	else if(gliderType == 8)
	{
		vec[y + 0][x + 0] = true;
		vec[y + 0][x + 1] = true;
		vec[y + 0][x + 2] = true;
		vec[y + 1][x + 2] = true;
		vec[y + 2][x + 1] = true;
	}
	else if(gliderType == 9)
	{
		vec[y + -1][x + 1] = true;
		vec[y + 0][x + 1] = true;
		vec[y + 0][x + 2] = true;
		vec[y + 1][x + 0] = true;
		vec[y + 1][x + 2] = true;
	}
	else if(gliderType == 10)
	{
		vec[y + -1][x + 1] = true;
		vec[y + -1][x + 2] = true;
		vec[y + 0][x + 0] = true;
		vec[y + 0][x + 2] = true;
		vec[y + 1][x + 2] = true;
	}
	else if(gliderType == 11)
	{
		vec[y + -1][x + 1] = true;
		vec[y + -1][x + 2] = true;
		vec[y + 0][x + 2] = true;
		vec[y + 0][x + 3] = true;
		vec[y + 1][x + 1] = true;
	}
	else if(gliderType == 12)
	{
		vec[y + 0][x + 0] = true;
		vec[y + 0][x + 1] = true;
		vec[y + 0][x + 2] = true;
		vec[y + 1][x + 0] = true;
		vec[y + 2][x + 1] = true;
	}
	else if(gliderType == 13)
	{
		vec[y + -1][x + 1] = true;
		vec[y + 0][x + 0] = true;
		vec[y + 0][x + 1] = true;
		vec[y + 1][x + 0] = true;
		vec[y + 1][x + 2] = true;
	}
	else if(gliderType == 14)
	{
		vec[y + -1][x + 0] = true;
		vec[y + -1][x + 1] = true;
		vec[y + 0][x + 0] = true;
		vec[y + 0][x + 2] = true;
		vec[y + 1][x + 0] = true;
	}
	else if(gliderType == 15)
	{
		vec[y + -1][x + 0] = true;
		vec[y + -1][x + 1] = true;
		vec[y + 0][x + -1] = true;
		vec[y + 0][x + 0] = true;
		vec[y + 1][x + 1] = true;
	}
}

void PlaceRandomGlidersStream(std::vector<std::vector<bool> >& vec, int dx, int dy, int dtype)
{
	int center = vec.size() / 2;
	
	int d = 8;
	int ngliders = 3 + rand() % 4;
	
	for(int i = 0; i < ngliders; i++)
	{
		PlaceGlider(vec, center + dx * d, center + dy * d + rand() % 16 - 8, dtype + rand() % 4);
		d += 6;
	}
}

void PlaceRandomGliders(std::vector<std::vector<bool> >& vec)
{
	PlaceRandomGlidersStream(vec, 1, -1, 0);
	PlaceRandomGlidersStream(vec, -1, -1, 4);
	PlaceRandomGlidersStream(vec, -1, 1, 8);
	PlaceRandomGlidersStream(vec, 1, 1, 12);
}

int Key(std::vector<std::vector<bool> >& vec, int xp, int yp, const std::vector<unsigned int>& ones, int size)
{
	unsigned int key = 0; 
	int i = 0; 
	
	for(int y = yp; y < yp + size; y++)
	{
		for(int x = xp; x < xp + size; x++)
		{
			if(vec[y][x])
			{
				key |= ones[i];
			}
			
			i++;
		}
	}
	
	return key;
}

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
	//srand (time(NULL));
	
	
	/*
	int i = 0; 
	while(true)
	{
		
		if(++i % 100 == 0)
			std::cout << i << "\n";
			
		if(i%100 == 0)
		{
			std::vector<unsigned int> stats2 = stats;
			std::sort(stats2.begin(), stats2.end(), std::greater<unsigned int>());
			
			for(int j = 0; j < stats2.size(); j++)
			{
				if(stats2[j] == 0)
				{
					std::cout << "stats for " << j << "\n";
					break;
				}
			}
		}
		
	}
	*/
}