#include <vector>
#include <algorithm>   
#include <string> 
#include <iostream> 

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

void Clear(std::vector<std::vector<bool> >& vec, int minx, int miny, int maxx, int maxy)
{
	for(int i = miny; i < maxy + 1; i++)
		for(int j = minx; j < maxx + 1; j++)
			vec[i][j] = false;
	
}
void Print(std::vector<std::vector<bool> >& vec, int minx, int miny, int maxx, int maxy)
{
	for(int i = miny; i < maxy + 1; i++)
	{
		for(int j = minx; j < maxx + 1; j++)
		{
			if(vec[i][j])
				std::cout << "O";
			else
				std::cout << ".";
		}
		
		std::cout << std::endl;
		
	}
	
	std::cout << std::endl;
}

void Print(std::vector<std::vector<bool> >& vec)
{
	Print(vec, 0, 0, vec[0].size() - 1, vec.size() - 1);
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

bool Parse(std::vector<std::vector<bool> >& vec, const char* rle, int dx, int dy, int& minx, int& miny, int& maxx, int& maxy)
{
	char ch;
	int cnt, i, j; 
	int x, y;
	x = 0;
	y = 0;
	cnt = 0;
	i = 0;
	minx = vec[0].size() + 1;
	miny = vec.size() + 1;
	maxx =  -1;
	maxy =  -1;
	
	while((ch = rle[i]) != '\0')
	{

		if(ch >= '0' && ch <= '9')
		{
			cnt *= 10;
			cnt += (ch - '0');
		}
		else if(ch == 'o')
		{
			
			if(cnt == 0)
				cnt = 1;
				
			for(j = 0; j < cnt; j++)
			{
				if(y + dy >= vec.size() || x + dx >= vec[y + dy].size() || y + dy < 0 || x + dx < 0)
					return false;
					
				vec[y + dy][x + dx] = true;
				
				if(y + dy > maxy)
					maxy = y + dy;
				
				if(y + dy < miny)
					miny = y + dy;
				
				if(x + dx > maxx)
					maxx = x + dx;
				
				if(x + dx < minx)
					minx = x + dx;
				
				x++;
			}
			
			cnt = 0;
		}
		else if(ch == 'b')
		{
			if(cnt == 0)
				cnt = 1;
			
			x += cnt;
			cnt = 0;
			
		}
		else if(ch == '$')
		{
			if(cnt == 0)
				cnt = 1;
				
			y += cnt;
			x = 0;
			cnt = 0;
		}
		else if(ch == '!')
		{
			break;
		}
		else
		{
			return false;
		}
		
		i++;
	}
	
	minx--;
	miny--;
	maxx++;
	maxy++;
	
	return true;
}

bool PlaceState(std::vector<std::vector<bool> >& vec, std::vector<std::vector<bool> >& result, int dx, int dy, int& minx, int& miny, int& maxx, int& maxy)
{
	minx = vec[0].size() + 1;
	miny = vec.size() + 1;
	maxx =  -1;
	maxy =  -1;
	
	for(int y = 0; y < result.size(); y++)
	{
		for(int x = 0; x < result[y].size(); x++)
		{
			if(result[y][x])
			{
				vec[y + dy][x + dx] = true;
				
				if(y + dy > maxy)
					maxy = y + dy;
				
				if(y + dy < miny)
					miny = y + dy;
				
				if(x + dx > maxx)
					maxx = x + dx;
				
				if(x + dx < minx)
					minx = x + dx;
				
			}
		}
	}
	
	minx--;
	miny--;
	maxx++;
	maxy++;
	
	return true;
}