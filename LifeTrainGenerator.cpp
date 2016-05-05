#include <iostream> 
#include <fstream>
#include <stdio.h> 
#include <vector>
#include <algorithm>
#include <time.h>     
#include <omp.h>
#include <string> 
#include <algorithm> 
#include <unordered_set> 

#include "Common.h"

using std::unordered_set;
using std::cout;
using std::string;
using std::vector;
using std::to_string;
using std::min;
void GainStats(std::vector<string>& instats, std::vector<string>& outstats, std::vector<std::vector<bool> >& vec, int maxIter, int borderSize, int outputrange)
{
	static unordered_set<string> hash;
	
	for(int i = 0; i < vec.size(); i++)
		std::fill(vec[i].begin(),vec[i].end(), false);
	
	int size = vec.size(); 
	
	int minx = borderSize; 
	int miny = borderSize;
	int maxx = size - borderSize;
	int maxy = size - borderSize;
	
	PlaceRandomGliders(vec);
	
	bool changed = true;
	
	for(int i = 0; i < maxIter; i++)
	{
		if(!changed)
			break; 
			
		changed = false; 
		vector<vector<bool> > prev = vec;
		Iterate(vec, minx, miny, maxx, maxy);
		
		if(minx <= borderSize + 5 || maxx >= size - borderSize  - 5 || miny <= borderSize + 5 || maxy >= size - borderSize - 5)
			break;

		for(int y = miny - 5; y <= maxy + 5; y++)
		{
			for(int x = minx - 5; x <= maxx + 5; x++)
			{
				string output = "";
				string input = "";
				int numactive = 0; 
				
				for(int x1 = -3; x1 <= 3; x1++)
				{						
					for(int y1 = -3; y1 <= 3; y1++)
					{
						if(vec[y + y1][x + x1])
							numactive++;
					}
					
					if(numactive >= 1)
						break;

				}
				
				if(numactive < 1)
					continue; 
					
				for(int x1 = -borderSize; x1 <= borderSize; x1++)
				{
					for(int y1 = -borderSize; y1 <= borderSize; y1++)
					{
						bool val = vec[y + y1][x + x1];
						input += to_string((int)(val));
					}
				}
				
				if(hash.find(input) == hash.end())
					hash.insert(input);
				else 
					continue;
				
				changed = true; 
		 
				for(int x1 = -outputrange; x1 <= outputrange; x1++)
					for(int y1 = -outputrange; y1 <= outputrange; y1++)
						output += to_string((int)(prev[y + y1][x + x1])) ;

				instats.push_back(input);
				outstats.push_back(output);
			}
		}
	}
}

void SaveStats(const vector<string>& instats, const vector<string>& outstats, const std::string& fileName, int inputdim, int outputdim)
{
	std::ofstream f;
	f.open(fileName.c_str());
	
	f << instats.size() << " " << inputdim << " " << outputdim << "\n";
	
	for(int i = 0; i < instats.size(); i++)
	{
		f << instats[i] << "\n";
		f << outstats[i] << "\n";
	}
	
	
	f.close();
}

class TreeNode
{
public: 
	int parentID;
	int ID; 
	
	int idx; 
	bool isLeft; 
	
	TreeNode(int parent, int id, int index, bool left)
	{
		parentID = parent;
		ID = id; 
		idx = index; 
		isLeft = left;
	}
};

void FilterStats(vector<string>& instats, vector<string>& outstats, vector<string>& newinstats, vector<string>& newoutstats, int min_error_idx, char filterChar)
{
	newinstats.clear();
	newoutstats.clear();
	
	for(int j = 0; j < instats.size(); j++)
	{
		if(instats[j][min_error_idx] == filterChar)
		{
			newinstats.push_back(instats[j]);
			newoutstats.push_back(outstats[j]);
		}
	}
		
}
void FindPivot(vector<string>& instats, vector<string>& outstats, int& maxID, int parentID, vector<TreeNode>& nodes)
{
	float epsilon = 1.0 / (instats[0].size());
	float min_error = 1.0; 
	int min_error_idx; 
	bool continueLeft; 
	bool continueRight; 
	
	for(int i = 0; i < instats[0].size(); i++)
	{
		float cntL0 = 0;
		float cntL1 = 0;
		float cntR0 = 0;
		float cntR1 = 0;
		
		for(int j = 0; j < instats.size(); j++)
		{
			if(outstats[j][0] == '0' && instats[j][i] == '0')
				cntL0+=epsilon;
			
			if(outstats[j][0] == '0' && instats[j][i] == '1')
				cntR0+=epsilon;
			
			if(outstats[j][0] == '1' && instats[j][i] == '0')
				cntL1+=epsilon;
			
			if(outstats[j][0] == '1' && instats[j][i] == '1')
				cntR1+=epsilon;						
		}
		
		float error = (float)(cntL0 * cntL1 + cntR0 * cntR1);
		error /= (int)(instats.size());
		
		min_error = min(error, min_error);
		
		if(error == min_error)
		{
			min_error_idx = i;
			continueLeft = min(cntL0, cntL1) > 0;
			continueRight = min(cntR0, cntR1) > 0;
		}
	}
	
	vector<string> newinstats; 
	vector<string> newoutstats;
	
	if(continueLeft)
	{
		FilterStats(instats, outstats, newinstats, newoutstats, min_error_idx, '0');
		TreeNode node(parentID, maxID++, min_error_idx, true);
		nodes.push_back(node);
		
		FindPivot(newinstats, newoutstats, maxID, maxID - 1, nodes);
	}		

	if(continueRight)
	{
		FilterStats(instats, outstats, newinstats, newoutstats, min_error_idx, '1');
		TreeNode node(parentID, maxID++, min_error_idx, false);
		nodes.push_back(node);
		
		FindPivot(newinstats, newoutstats, maxID, maxID - 1, nodes);
	}		
}

int main()
{
	srand (time(NULL));
		
	int counter = 0; 
	std::vector<std::vector<bool> > vec;
	int gridSize = 150;
	int border = 9;
	int iterNum = 300;
	int outputrange  = 0;
	
	Init(vec, gridSize);
	
	vector<string> instats;
	vector<string> outstats;
	
	for(int i = 0; i < 7; i++)
		GainStats(instats, outstats, vec, iterNum, border, outputrange);
	
	cout << "Soup finish\n"; 
	int maxID = 0;
	vector<TreeNode> nodes;
	
	FindPivot(instats, outstats, maxID, -1, nodes);
	cout << instats.size() << "," << nodes.size() << "," << 100 * float(nodes.size()) / instats.size();
	getchar();
	//SaveStats(instats, outstats, "life_single.train", (1 + 2 * border) * (1 + 2 * border), (1 + 2 * outputrange) * (1 + 2 * outputrange));
	
}