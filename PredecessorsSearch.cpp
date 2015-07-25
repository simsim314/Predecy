#include <iostream> 
#include <fstream>
#include <stdio.h> 
#include <vector>
#include <algorithm>
#include <time.h>     
#include <omp.h>
#include <string> 
#include <fstream>

class ScoredTile
{
public:

	ScoredTile(unsigned int in_key, unsigned long long in_score)
	{
		key = in_key;
		score = in_score;
	}
	
	bool ValAt(bool vals[5][5], int x, int y)
	{
		short total = 0; 
		
		for(int i = -1; i <= 1; i++)
			for(int j = -1; j <= 1; j++)
				if(vals[y + i][x + j])
					total++;
		
		if(total == 3)
			return true;
			
		if(total == 4 && vals[y][x])
			return true;
			
		return false;
	}	
	
	unsigned short CalcKey(bool vals[5][5], const short& xmin, const short& ymin, const short& width, const short& height, const std::vector<unsigned int>& ones)
	{
		int i = 0; 
		unsigned short result = 0; 
		
		for(int y = ymin; y < ymin + height; y++)
		{
			for(int x = xmin; x < xmin + width; x++)
			{
				if(vals[y][x])
				{
					result |= ones[i];
				}
				
				i++;
			}
		}
		
		return result;
	}
	
	void Init( const std::vector<unsigned int>& ones)
	{
		bool vals[5][5];
		
		int i = 0; 
	
		for(int y = 0; y < 5; y++)
		{
			for(int x = 0; x < 5; x++)
			{
				if((key & ones[i]) == ones[i])
					vals[y][x] = true;
				else 
					vals[y][x] = false;
				
				i++;
			}
		}
		
		upKey = CalcKey(vals, 0, 0, 5, 2, ones);
		bottomKey = CalcKey(vals, 0, 3, 5, 2, ones);
		righKey = CalcKey(vals, 3, 0, 2, 5, ones);
		leftKey = CalcKey(vals, 0, 0, 2, 5, ones);
		
		partialRightKey = CalcKey(vals, 3, 2, 2, 3, ones);
		partialLeftKey = CalcKey(vals, 0, 2, 2, 3, ones);
		
		i = 0; 
		innerKey = 0; 
		
		for(int y = 1; y < 4; y++)
		{
			for(int x = 1; x < 4; x++)
			{
				if(ValAt(vals, x, y))
					innerKey |= ones[i];
					
				i++;
			}
		}
	}
	
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
	
	void PrintKey(const std::vector<unsigned int>& ones)
	{
		PrintInputKey(key, ones, 5);
	}
	
	void PrintInnerKey(const std::vector<unsigned int>& ones)
	{
		PrintInputKey(innerKey, ones, 3);
	}
	
	unsigned int key; 
	unsigned long long score; 
	unsigned int depth; 
	
	unsigned short upKey;
	unsigned short bottomKey;
	unsigned short righKey;
	unsigned short leftKey;
	unsigned short innerKey;
	unsigned short partialLeftKey;
	unsigned short partialRightKey;
};

bool cmp(ScoredTile* a, ScoredTile* b) 
{
		return a->score > b->score;
}

class TileManager
{
public: 
	std::vector<std::vector<ScoredTile*> > sortedTile;
	std::vector<std::vector<std::vector<ScoredTile*> > > tileUp;
	std::vector<std::vector<std::vector<ScoredTile*> > > tileDown;
	std::vector<std::vector<std::vector<ScoredTile*> > > tileLeft;
	std::vector<std::vector<std::vector<ScoredTile*> > > tileRight;
	
	std::vector<std::vector<std::vector<ScoredTile*> > > tileUpLeft;
	std::vector<std::vector<std::vector<ScoredTile*> > > tileUpRight;
	
	void Load(std::string fname, std::vector<ScoredTile*>& vec)
	{
		std::ifstream infile(fname.c_str());
		unsigned long long a;
		int idx = 0;
		
		while (infile >> a)
		{
			vec.push_back(new ScoredTile(idx, a));
			idx++;
		}
	}
	
	void InitTileData(std::vector<ScoredTile*>& vec, const std::vector<unsigned int>& ones)
	{
		for(int i = 0; i < vec.size(); i++)
			vec[i]->Init(ones);
		
	}
	
	void InitSideRTiles()
	{
		for(int i = 0; i < sortedTile.size(); i++)
		{
			std::vector<std::vector<ScoredTile*> > tLeft;
			std::vector<std::vector<ScoredTile*> > tRight;
			
			for(int j = 0; j < (1024 * 64); j++)
			{
				tLeft.push_back(std::vector<ScoredTile*>());
				tRight.push_back(std::vector<ScoredTile*>());
			}
			
			tileUpLeft.push_back(tLeft);
			tileUpRight.push_back(tRight);
			
			for(int j = 0; j < sortedTile[i].size(); j++)
			{
				tileUpRight[i][((sortedTile[i][j]->upKey) << 6) | (sortedTile[i][j]->partialRightKey)].push_back(sortedTile[i][j]);
				tileUpLeft[i][((sortedTile[i][j]->upKey) << 6) | (sortedTile[i][j]->partialLeftKey)].push_back(sortedTile[i][j]);
			}
		}
	}
	
	void InitSideTiles()
	{
		for(int i = 0; i < sortedTile.size(); i++)
		{
			std::vector<std::vector<ScoredTile*> > tUp;
			std::vector<std::vector<ScoredTile*> > tDown;
			std::vector<std::vector<ScoredTile*> > tLeft;
			std::vector<std::vector<ScoredTile*> > tRight;
			
			for(int i = 0; i < 1024; i++)
			{
				tUp.push_back(std::vector<ScoredTile*>());
				tDown.push_back(std::vector<ScoredTile*>());
				tLeft.push_back(std::vector<ScoredTile*>());
				tRight.push_back(std::vector<ScoredTile*>());
			}
			
			tileUp.push_back(tUp);
			tileDown.push_back(tDown);
			tileLeft.push_back(tLeft);
			tileRight.push_back(tRight);
			
			for(int j = 0; j < sortedTile[i].size(); j++)
			{
				tileUp[i][sortedTile[i][j]->upKey].push_back(sortedTile[i][j]);
				tileDown[i][sortedTile[i][j]->bottomKey].push_back(sortedTile[i][j]);
				tileRight[i][sortedTile[i][j]->righKey].push_back(sortedTile[i][j]);
				tileLeft[i][sortedTile[i][j]->leftKey].push_back(sortedTile[i][j]);
			}
		}
	}
	void InitSortedTile(const std::vector<ScoredTile*>& vec)
	{
		for(int i = 0; i < 512; i++)
		{
			sortedTile.push_back(std::vector<ScoredTile*>());
		}
		
		for(int i = 0; i < vec.size(); i++)
			sortedTile[vec[i]->innerKey].push_back(vec[i]);
		
		for(int i = 0; i < sortedTile.size(); i++)
		{
			std::sort(sortedTile[i].begin(), sortedTile[i].end(), cmp);
			
			for(int j = 0; j < sortedTile[i].size(); j++)
				sortedTile[i][j]->depth = j;
		}
	}
	
	void Init(std::string fname, const std::vector<unsigned int>& ones)
	{
		
		std::vector<ScoredTile*> vec;
		
		std::cout << "Loading...\n";
		Load(fname, vec);
		std::cout << "Initializing Tiles...\n";
		InitTileData(vec, ones);
		std::cout << "Initializing Sorted Tiles...\n";
		InitSortedTile(vec);
		std::cout << "Initializing Side Tiles...\n";
		InitSideTiles();
		std::cout << "Initializing RSide  Tiles...\n";
		InitSideRTiles();
		std::cout << "Finished...\n";
	}
};

int main()
{
	 std::vector<unsigned int> ones; 
	
	for(int i = 0; i < 25; i++)
			ones.push_back(1 << i);					
		
	std::vector<ScoredTile*> vec;
	//Load("stats_bck.txt", vec);
	TileManager tileMan; 
	tileMan.Init("stats.txt", ones);
	
	int idx = 143;
	//int idx = 482;
	
	std::cout << "Size:" << tileMan.sortedTile[idx].size() << "\n\n";
	
	tileMan.sortedTile[idx][0]->PrintInnerKey(ones);
	std::cout << "\n\n";
	
	for(int i = 0; i < 10; i++)
	{
		tileMan.sortedTile[idx][i]->PrintKey(ones);
		std::cout << "\n\n";
	}
	
	getchar();
}