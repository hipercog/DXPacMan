/**
 *	Author: Ben Cowley, 2007.
 *	Information Metrics Player Modelling functions for predicting Challenge and Skill values
 */
#include "universals.h"

void GetVectors(int x, int y)
{
	int dir[4] = {y, y, x, x};

	while(true)
	{
		if(PacMap[x][--y] == 1)
			break;
		else
			dir[0]--;
	}

	while(true)
	{
		if(PacMap[x][++y] == 1)
			break;
		else
			dir[1]++;
	}

	while(true)
	{
		if(PacMap[++x][y] == 1)
			break;
		else
			dir[2]++;
	}

	while(true)
	{
		if(PacMap[--x][y] == 1)
			break;
		else
			dir[3]--;
	}
}

int GetRelativeDist(int x1, int y1, int x2, int y2)
{
	int h = myHeuristicDist(x1, y1, x2, y2);
	if( h < 4 )
	{
//		h = a_Star(x1, y1, x2, y2);
	}

	return h;
}

int GetGoalMap(int x, int y)
{
	int info = 0;
	int xn = max(0, x - 4);
	int yn = max(0, y - 4);
	int xp = min(20, x + 4);
	int yp = min(20, y + 4);

	for( xn; xn < xp; xn++ )
		for( yn; yn < yp; yn++ )
		{
			if( PacMap[xn][yn] == 4 )
			{}
			if( PacMap[xn][yn] == 5 )
			{}
			if( PacMap[xn][yn] == 3 )
			{}
		}

	return info;
}
//end of file