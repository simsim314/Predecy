#include <iostream> 
#include <fstream>
#include <stdio.h> 
#include <vector>
#include <algorithm>
#include <time.h>     
#include <omp.h>
#include <string> 
#include <fstream>
#include <map> 

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
	
	void CalcVals(std::vector<bool>& vec, const std::vector<unsigned int>& ones)
	{
		int i = 0; 
	
		for(int y = 0; y < 5; y++)
		{
			for(int x = 0; x < 5; x++)
			{
				if((key & ones[i]) == ones[i])
					vec[5 * y + x] = true;
				else 
					vec[5 * y + x] = false;
				
				i++;
			}
		}
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
	
	std::vector<std::map<int, std::vector<ScoredTile*> > > tileUpLeft;
	std::vector<std::map<int, std::vector<ScoredTile*> > > tileUpRight;
	std::vector<std::map<int, std::vector<ScoredTile*> > > tileDownLeft;
	std::vector<std::map<int, std::vector<ScoredTile*> > > tileDownRight;
	
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
		
		//vec[0] = 2^63 - 1 (just max value as stats were ignoring it). 
		vec[0]->score = 9223372036854775807ULL;
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
			std::map<int,std::vector<ScoredTile*> > tLeft;
			std::map<int,std::vector<ScoredTile*> > tRight;
			std::map<int,std::vector<ScoredTile*> > tDLeft;
			std::map<int,std::vector<ScoredTile*> > tDRight;
			
			tileUpLeft.push_back(tLeft);
			tileUpRight.push_back(tRight);
			tileDownLeft.push_back(tDLeft);
			tileDownRight.push_back(tDRight);
			
			for(int j = 0; j < sortedTile[i].size(); j++)
			{
				tileUpRight[i][((sortedTile[i][j]->upKey) << 6) | (sortedTile[i][j]->partialRightKey)].push_back(sortedTile[i][j]);
				tileUpLeft[i][((sortedTile[i][j]->upKey) << 6) | (sortedTile[i][j]->partialLeftKey)].push_back(sortedTile[i][j]);
				tileDownLeft[i][((sortedTile[i][j]->bottomKey) << 6) | (sortedTile[i][j]->partialLeftKey)].push_back(sortedTile[i][j]);
				tileDownRight[i][((sortedTile[i][j]->bottomKey) << 6) | (sortedTile[i][j]->partialRightKey)].push_back(sortedTile[i][j]);
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

enum NeighborType { NONE, UP, DOWN, LEFT, RIGHT, UP_RIGHT, UP_LEFT, DOWN_RIGHT, DOWN_LEFT };

class BacktrackerUnit
{
public:

	BacktrackerUnit() { curIdx = -1; tile3x3 = 0; curVal = NULL; }
	BacktrackerUnit(short tileKey) { curIdx = -1; tile3x3 = tileKey; curVal = NULL; }
	int xLoc, yLoc;
	
	bool StepNext(TileManager& tileman, int maxDepth)
	{
		if(curVal != NULL)
		{
			if(curIdx >=0 && curIdx + 1 < curVal->size())
			{	
				curIdx++;
				
				if(CurVal()-> depth <= maxDepth)
				{	
					return true;
				}
				else
				{
					curIdx = -1;
					return false;
				}
			}
			
			if(curIdx + 1 >= curVal->size())
			{
				curIdx = -1;
				return false;
			}
		}
		
		return InitInternal(tileman);
	}
	
	bool InitInternal(TileManager& tileman)
	{
		int key;
		
		switch(constrains) 
		{
			case NONE: 
				curVal = &(tileman.sortedTile[tile3x3]);
				break;
			case UP:
				curVal = &(tileman.tileUp[tile3x3][neighbors[0]->CurVal()->bottomKey]);
				break;
			case DOWN:
				curVal = &(tileman.tileDown[tile3x3][neighbors[0]->CurVal()->upKey]);
				break;
			case LEFT:
				curVal = &(tileman.tileLeft[tile3x3][neighbors[0]->CurVal()->righKey]);
				break;
			case RIGHT:
				curVal = &(tileman.tileRight[tile3x3][neighbors[0]->CurVal()->leftKey]);
				break;
			case UP_RIGHT:
				key = (neighbors[0]->CurVal()->bottomKey << 6) | (neighbors[1]->CurVal()->partialLeftKey);
				
				if (tileman.tileUpRight[tile3x3].count(key))
					curVal = &(tileman.tileUpRight[tile3x3][key]);
				else
					curVal = NULL;
				
				break;
			case UP_LEFT:
				key = (neighbors[0]->CurVal()->bottomKey << 6) | (neighbors[1]->CurVal()->partialRightKey);
				
				if (tileman.tileUpLeft[tile3x3].count(key))
					curVal = &(tileman.tileUpLeft[tile3x3][key]);
				else
					curVal = NULL;
				
				break;
			case DOWN_RIGHT:
				key = (neighbors[0]->CurVal()->upKey << 6) | (neighbors[1]->CurVal()->partialLeftKey);
				
				if (tileman.tileDownRight[tile3x3].count(key))
					curVal = &(tileman.tileDownRight[tile3x3][key]);
				else
					curVal = NULL;
				
				break;
			case DOWN_LEFT:
				key = (neighbors[0]->CurVal()->upKey << 6) | (neighbors[1]->CurVal()->partialRightKey);
				
				if (tileman.tileDownLeft[tile3x3].count(key))
					curVal = &(tileman.tileDownLeft[tile3x3][key]);
				else
					curVal = NULL;
				break;
		}
		
		if(curVal == NULL)
			return false;
			
		if(curVal->size() > 0)
		{
			curIdx = 0;
			return true;
		}
		
		curVal = NULL;
		return false;
	}
	
	ScoredTile* CurVal()
	{
		return curVal->at(curIdx);
	}
	
	void Init()
	{
		constrains = NONE;
	}
	
	void Init(NeighborType nType, BacktrackerUnit* neighbor5x2)
	{
		neighbors.push_back(neighbor5x2);
		constrains = nType;
	}
	
	void Init(NeighborType nType, BacktrackerUnit* neighbor5x2,  BacktrackerUnit* neighbor3x2)
	{
		neighbors.push_back(neighbor5x2);
		neighbors.push_back(neighbor3x2);
		constrains = nType;
	}

	int curIdx; 
	NeighborType constrains;
	std::vector<BacktrackerUnit*> neighbors;
	std::vector<ScoredTile*> *curVal; 
	short tile3x3; 
};

class BacktrackerSearch
{
public:
	std::vector<BacktrackerUnit*> trackerData; 
	int curIdx;
	TileManager* tileMan; 
	int maxDepth; 
	int width, height;
	
	void Init(TileManager* in_tileMan, int in_maxDepth)
	{
		tileMan = in_tileMan;
		maxDepth = in_maxDepth;
		curIdx = 0;
		width = -1;
		height = -1;
	}
	
	//Lower better
	int ResultScore()
	{
		int totalDepth = 0;
		
		for(int i = 0; i <= curIdx; i++)
		{
			int curDep = trackerData[i]->CurVal()->depth;
		
			totalDepth += curDep;
		}
		
		return totalDepth;
	}
	
	void FillDataVec(const std::vector<unsigned int>& ones, std::vector<std::vector<bool> >& keyVals)
	{
		int maxIdx = curIdx;
		
		for(int i = 0; i < 3 * height + 2; i++)
		{
			std::vector<bool> vec(3 * width + 2, false);
			keyVals.push_back(vec);
		}
		for(int i = 0; i <= maxIdx; i++)
		{
			std::vector<bool> vals(25);
			
			trackerData[i]->CurVal()->CalcVals(vals, ones);
			
			for(int x = 0; x < 5; x++)
				for(int y = 0; y < 5; y++)
					keyVals[3 * (trackerData[i]->yLoc - 1) + y][3 * (trackerData[i]->xLoc - 1) + x] = vals[5 * y + x];

		}
		
	}
	
	BacktrackerUnit* FindByLocation(int x, int y)
	{
		for(int i = 0; i < trackerData.size(); i++)
			if(trackerData[i]->xLoc == x && trackerData[i]->yLoc == y)
				return trackerData[i];
				
		return NULL;
	}
	
	void SetupBoxValue(int idx, int val)
	{	
		trackerData[idx]->tile3x3 = val;
	}
	
	void SetupBoxValue(int x, int y, int val)
	{	
		for(int idx = 0; idx < trackerData.size(); idx++)
			if(trackerData[idx]->xLoc == x && trackerData[idx]->yLoc == y)
				trackerData[idx]->tile3x3 = val;
	}
	
	void SetupBox(int m, int n)
	{
		if(width == -1)
		{
			width = m;
			height = n;
		}
		
		if(m == 1 && n == 1)
		{
			BacktrackerUnit* start = new BacktrackerUnit();
			start->xLoc = 1;
			start->yLoc = 1;
			start->Init();
			trackerData.push_back(start);
		}
		else if(m >= n)
		{
			SetupBox(m - 1, n);
			
			BacktrackerUnit* left = FindByLocation(m - 1, 1);
			
			BacktrackerUnit* unit = new BacktrackerUnit();
			unit->xLoc = m;
			unit->yLoc = 1;
			
			unit->Init(LEFT, left);
			
			trackerData.push_back(unit);
			
			BacktrackerUnit* prev = unit;
			
			for(int i = 2; i <= n; i++)
			{
				left = FindByLocation(m - 1, i);
				unit = new BacktrackerUnit();
				unit->xLoc = m;
				unit->yLoc = i;
				
				unit->Init(UP_LEFT, prev, left);
				trackerData.push_back(unit);
				prev = unit;
			}
		}
		else
		{
			SetupBox(m, n - 1);
			
			BacktrackerUnit* up = FindByLocation(1, n - 1);
			
			BacktrackerUnit* unit = new BacktrackerUnit();
			unit->xLoc = 1;
			unit->yLoc = n;
			
			unit->Init(UP, up);
			
			trackerData.push_back(unit);
			
			BacktrackerUnit* prev = unit;
			
			for(int i = 2; i <= m; i++)
			{
				up = FindByLocation(i, n - 1);
				unit = new BacktrackerUnit();
				unit->xLoc = i;
				unit->yLoc = n;
				
				unit->Init(UP_LEFT, up, prev);
				trackerData.push_back(unit);
				prev = unit;
			}
		}
	}
	
	int StepNext()
	{
		bool step = trackerData[curIdx]->StepNext(*tileMan, maxDepth);
		
		if(step)
		{
			if(curIdx == trackerData.size() - 1)
				return 1;
			else 
			{
				curIdx++;
				return 0;
			}
		}
		else
		{
			if(curIdx == 0)
				return -1;
			else
			{
				curIdx--;
				return 0;
			}
		}
	}
};

void PrintDataVec(std::vector<std::vector<bool> >& keyVals)
{

	for(int y = 0; y < keyVals.size(); y++)
	{
		for(int x = 0; x < keyVals[y].size(); x++)
		{
			if(keyVals[y][x])
				std::cout << "O";
			else 
				std::cout << ".";
		}
		
		std::cout << "\n";
	}
}
int main()
{
	 std::vector<unsigned int> ones; 
	
	for(int i = 0; i < 25; i++)
		ones.push_back(1 << i);					
		
	std::vector<ScoredTile*> vec;
	//Load("stats_bck.txt", vec);
	TileManager tileMan; 
	tileMan.Init("stats.txt", ones);
	
	BacktrackerSearch srch;
	srch.Init(&tileMan, 320);
	srch.SetupBox(3, 7);
	srch.SetupBoxValue(1, 1, 256);
	srch.SetupBoxValue(2, 1, 0);
	srch.SetupBoxValue(3, 1, 0);
	srch.SetupBoxValue(1, 2, 256);
	srch.SetupBoxValue(2, 2, 27);
	srch.SetupBoxValue(3, 2, 0);
	srch.SetupBoxValue(1, 3, 0);
	srch.SetupBoxValue(2, 3, 314);
	srch.SetupBoxValue(3, 3, 0);
	srch.SetupBoxValue(1, 4, 0);
	srch.SetupBoxValue(2, 4, 340);
	srch.SetupBoxValue(3, 4, 1);
	srch.SetupBoxValue(1, 5, 0);
	srch.SetupBoxValue(2, 5, 101);
	srch.SetupBoxValue(3, 5, 0);
	srch.SetupBoxValue(1, 6, 0);
	srch.SetupBoxValue(2, 6, 118);
	srch.SetupBoxValue(3, 6, 0);
	srch.SetupBoxValue(1, 7, 0);
	srch.SetupBoxValue(2, 7, 0);
	srch.SetupBoxValue(3, 7, 0);
	
	std::vector< std::pair< int, std::vector<std::vector<bool> > > > results;
	
	while(true)
	{
		int next = srch.StepNext();
		
		if(next == 1)
		{	
			std::vector<std::vector<bool> > vec;
			srch.FillDataVec(ones, vec);
			int score = srch.ResultScore();
			
			results.push_back(std::pair<int, std::vector<std::vector<bool> > >(score, vec));
			
			if(results.size() % 1000 == 0)
			{
				std::cout << "Fount "<< results.size() << " solutions\n";
			}
		}
		
		if(next == -1 || results.size() > 1000000)
			break;
	}
	
	std::sort(results.begin(), results.end());
	
	std::cout << "\n\n";
	
	int idx = results.size();

	if(idx > 100)
		idx = 100;
		
	for(int i = 0; i < idx; i++)
	{
		PrintDataVec(results[i].second);
		std::cout << "\n\n\n\n\n" ;
	}
	
	std::cout << "Finished...";
	getchar();
	
	/*
	std::cout << "Size:" << tileMan.sortedTile[idx1].size() << "\n\n";
	
	tileMan.sortedTile[idx1][0]->PrintInnerKey(ones);
	std::cout << "\n\n";
	
	for(int i = 0; i < 10; i++)
	{
		tileMan.sortedTile[idx1][i]->PrintKey(ones);
		std::cout << "\n Tile: "<<  i <<"  \n";
		
		int max = 10;
		
		
		for(int j = 0; j < 10; j++)
		{
			if(j == tileMan.tileLeft[idx2][tileMan.sortedTile[idx1][i]->righKey].size())
			{
				std::cout << "\nBreaked at " << j << "\n\n";
				break;
			}
				
			tileMan.tileLeft[idx2][tileMan.sortedTile[idx1][i]->righKey][j]->PrintKey(ones);
			std::cout << "\n\n";
			
		}
		
	}
	
	getchar();
	
	getchar();
	*/
}