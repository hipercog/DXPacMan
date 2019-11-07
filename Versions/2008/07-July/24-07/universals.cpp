/**	
 *	Author: Ben Cowley, 2006.
 *	Source file for universal functions and fields.
 */

#include "universals.h"
#include "wcomm.h"

/* Universal variables */
int logIndex = 0;
int PacMoves = 0;
int PacMap[20][20];
int Score;
int NumPills = 4;
int NumDots = 175;
int GameLives = 5;
int Level = 0;
int GhostFleeTimer = 340;
int RowAnimation = 0;
int cycles = 0;
int randCherryInit = 0;
char *out = new char[99];
int RunOfGhostKills = 1;
int GDMode = 0;		//Sets the mode that determines how the Ghosts move. See main.cpp/GhostDriver() for mode details

unsigned int pacTik = 96;
float gstTik = (float)pacTik*2.5;

int xIndex = 0;
int yIndex = 0;

deque <int> aPathX;
deque <int> aPathY;

bool CherryEaten = true;
bool GameOver = false;
bool GhostAttack = true;

bool ShadowEaten = false;
bool SpeedyEaten = false;
bool PokeyEaten = false;
bool BashfulEaten = false;

//Networking functions & fields
WComm w;
//Local output files and data structures
string gameDir = "C:/PacPlayd/Logs/";
FILE *fileout;	//Open an output File object
FILE *stateFile;	//Open an state data output File object
deque <PacState> GameStates;
string logData = "";

//Vectors for sprite positions (used by sprite Draw() function)
D3DXVECTOR3				g_PacmanSpritePos;	
D3DXVECTOR3				g_ShadowSpritePos;
D3DXVECTOR3				g_SpeedySpritePos;	
D3DXVECTOR3				g_PokeySpritePos;
D3DXVECTOR3				g_BashfulSpritePos;
D3DXVECTOR3				g_CherrySpritePos;

void sleep(DWORD dur)
{
	dur += GetTickCount();
	while( GetTickCount() < dur )
	{}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////             FUNCTIONS FOR STATE ATTRIBUTE CALCULATION                      /////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 *	Description: Return the x,y of the closest Pill to Pacman in the state passed as argument
 */
deque<int> GetClosestPill( PacState pS )
{
	deque<int> xy;
	xy.assign(2, 0);

	deque <int> mDist; //Deque of distances between things
	deque <int> xPill;
	deque <int> yPill;

	//Get the closest pill's x and y
	if(pS.dtpMap[1][3] == 5){
		mDist.push_back( myHeuristicDist(pS.xPac, pS.yPac, 1, 3) );
		xPill.push_back(1);
		yPill.push_back(3);
	}
	if(pS.dtpMap[18][3] == 5){
		mDist.push_back( myHeuristicDist(pS.xPac, pS.yPac, 18, 3) );
		xPill.push_back(18);
		yPill.push_back(3);
	}
	if(pS.dtpMap[1][15] == 5){
		mDist.push_back( myHeuristicDist(pS.xPac, pS.yPac, 1, 15) );
		xPill.push_back(1);
		yPill.push_back(15);
	}
	if(pS.dtpMap[18][15] == 5){
		mDist.push_back( myHeuristicDist(pS.xPac, pS.yPac, 18, 15) );
		xPill.push_back(18);
		yPill.push_back(15);
	}
	if( mDist.size() > 0 ){
		mDist.push_front(lowest(mDist));
		xy[0] = xPill[mDist[0]];
		xy[1] = yPill[mDist[0]];
	}

	return xy;
}

/**
 * Description: returns a value for the distribution of the ghosts
 */
double avgDistribution(int *xGhost, int *yGhost)
{
	//centroid
	int Xes = 0, Yes = 0;
	for(int i = 0; i < NUM_GS; i++)
	{
		Xes += xGhost[i]; Yes += yGhost[i];
	}
	(int)Xes /= NUM_GS; (int)Yes /= NUM_GS;
	int manhattan = 0;
	for(int i = 0; i < NUM_GS; i++)
	{
		manhattan += manhattanDist(xGhost[i], yGhost[i], Xes, Yes);
	}

	//Circumference using average radius = avg(r)*2*Pi
	return (manhattan / 4) * (44 / 7);
}

/**
 * Description: function that counts the number of dots within a given radius of an x,y coord
 */
int localDots(int xCentre, int yCentre, int radius, int Map[20][20])
{
	int dots = 0;
	
	int x = 0, y = 0, x2 = 19, y2 = 19;
	if(xCentre - radius > 0)
		x = xCentre - radius;
	if(yCentre - radius > 0)
		y = yCentre - radius;

	if(xCentre + radius < 19)
		x2 = xCentre + radius;
	if(yCentre + radius < 19)
		y2 = yCentre + radius;
	
	while(x <= x2)
	{
		while(y <= y2)
		{
			if(Map[x][y] == 4)
				dots++;
			y++;
		}
		x++;
	}
	return dots;
}

/**
 * Description: function that counts the number of ghosts within a given radius of an x,y coord
 */
int localGhosts(int x, int y, int radius, PacState pS)
{
	int ghosts = 0;
	if(x < 0)
		x = 0;
	else if(x > 19)
		x = 19;
	if(y < 0)
		y = 0;
	else if(y > 19)
		y = 19;
	for(int i = 0; i < NUM_GS; i++)
		if(myHeuristicDist(pS.xGhost[i], pS.yGhost[i], x, y) <= radius)
			ghosts++;
	return ghosts;
}

/**
 *	Translates Sprite positions into x & y index values for the map.
 */
void getSpritePos(D3DXVECTOR3 spritePos)
{
	//Obtain x and y pos values from the PacMan sprite pointer
	FLOAT xPosInitial = spritePos.x;
	FLOAT yPosInitial = spritePos.y;

	//Adjust position values to obtain PacMap index values
	xIndex = (int)((xPosInitial - GRIDTLX)/SPRITE_WIDTH);
	yIndex = (int)((yPosInitial - GRIDTLY)/SPRITE_HEIGHT);
}

/***********************************************************
 *	DISTANCE MEASURING FUNCTIONS
 ***********************************************************/

/**
 * Distance measures. manhattanDist and lineDist are self explanatory.
 */
int manhattanDist(int x1, int y1, int x2, int y2)
{
	return abs(x1 - x2) + abs(y1 - y2);
}

double lineDist(int x1, int y1, int x2, int y2)
{
	double dist = 0.0;
	dist = sqrt(double((x2 - x1)^2 + (y2 - y1)^2));
	return dist;
}

/**
 * Description: Heuristic distance counts the number of open and closed squares along four paths - left & right L's, and left & right biased diagonals
	Test loop for myHeuristicDist;
	for(int i = 0; i < 20; i++)
	{
		for(int j = 0; j < 20; j++)
		{
			sprintf_s(out, 99, "%d", myHeuristicDist(i, j, 10, 11)); print("\nHeuristic dist: "); print(out);
		}
	}
*/
int myHeuristicDist(int x1, int y1, int x2, int y2)
{
	if(x1 == x2 && y1 == y2)
		return 0;
	if(x1 < 0) x1 = 0;
	if(y1 < 0) y1 = 0;
	if(x2 < 0) x2 = 0;
	if(y2 < 0) y2 = 0;
	if(x1 > 19) x1 = 19;
	if(y1 > 19) y1 = 19;
	if(x2 > 19) x2 = 19;
	if(y2 > 19) y2 = 19;

	int count[4] = {0};
	int opx = 1, opy = 1;
	if(x1 > x2)	opx = -1;
	if(y1 > y2)	opy = -1;
	int x = 0, y = 0;
	
	for(x = x1; x != x2; x+=opx)
	{
		if(PacMap[x][y1] < 3)
			count[0]++;
	}
	for(y = y1; y != y2; y+=opy)
	{
		if(PacMap[x][y] < 3)
			count[0]++;
	}

	for(y = y1; y != y2; y+=opy)
	{
		if(PacMap[x1][y] < 3)
			count[1]++;
	}
	for(x = x1; x != x2; x+=opx)
	{
		if(PacMap[x][y] < 3)
			count[1]++;
	}
/**/
	if(abs(x1 - x2) > 1 && abs(y1 - y2) > 1)
	{
		x = x1;
		y = y1;
		//Step once in x, then in y, etc. Count PacMap 1's and alternate
		while(x != x2 && y != y2)
		{
			if(PacMap[x][y] < 3)
				count[2]++;
			x += opx;
			if(PacMap[x][y] < 3)
				count[2]++;
			y += opy;
		}
		if(x == x2)
		{
			while(y != y2)
			{
				if(PacMap[x][y] < 3)
					count[2]++;
				y += opy;
			}
		}else if(y == y2)
		{
			while(x != x2)
			{
				if(PacMap[x][y] < 3)
					count[2]++;
				x += opx;
			}
		}
		//Alternating
		x = x1;
		y = y1;
		while(x != x2 && y != y2)
		{
			if(PacMap[x][y] < 3)
				count[3]++;
			y += opy;
			if(PacMap[x][y] < 3)
				count[3]++;
			x += opx;
		}
		if(x == x2)
		{
			while(y != y2)
			{
				if(PacMap[x][y] < 3)
					count[3]++;
				y += opy;
			}
		}else if(y == y2)
		{
			while(x != x2)
			{
				if(PacMap[x][y] < 3)
					count[3]++;
				x += opx;
			}
		}
	}else{
		count[2] = count[3] = 99;
	}
/**/
	sort(count, count+4);

	return abs(x1 - x2) + abs(y1 - y2) + count[lowest(count, 4)];
}

/**
 * Description:	Implement the A* pathfinding algorithm - optimised for Pacman map. Stores path in global deques aPathX and aPathY
 * Arguments:	Ints give the source x,y and the target x,y; 
 *				bool sets whether it is a path for Pacman - i.e. 2's not navigable;
 *				PacState is the state to pathfind on.
 * Return Val:	number of moves in path.
 */
int a_Star(int src_x1, int src_y1, int trg_x2, int trg_y2, bool pacTravelling, PacState pS)
{
	if( src_x1 == trg_x2 && src_y1 == trg_y2 )
		return 0;

	if(src_x1 < 0) src_x1 = 1;
	if(src_y1 < 1) src_y1 = 1;
	if(trg_x2 < 0) trg_x2 = 1;
	if(trg_y2 < 1) trg_y2 = 1;
	if(src_x1 > 19) src_x1 = 18;
	if(src_y1 > 18) src_y1 = 18;
	if(trg_x2 > 19) trg_x2 = 18;
	if(trg_y2 > 18) trg_y2 = 18;

	int limit = 1;
	if( pacTravelling )
		limit++;

	while( pS.dtpMap[src_x1][src_y1] <= limit )
	{
		if( trg_x2 < src_x1 )
			src_x1--;
		else if( trg_x2 > src_x1 )
			src_x1++;
		else if( trg_y2 < src_y1 )
			src_y1--;
		else
			src_y1++;
	}

	while( pS.dtpMap[trg_x2][trg_y2] <= limit )
	{
		if( src_x1 < trg_x2 )
			trg_x2--;
		else if( src_x1 > trg_x2 )
			trg_x2++;
		else if( src_y1 < trg_y2 )
			trg_y2--;
		else
			trg_y2++;
	}

	if( src_x1 == trg_x2 && src_y1 == trg_y2 )
		return 0;

	//Initialisations
	int x1 = src_x1; 
	int y1 = src_y1;
	aPathX.clear();
	aPathY.clear();

	//List of directions, starting from x1, y1
	int path[ASTAR_BOUND];
	int closedNodesX[ASTAR_BOUND] = {0};
	int closedNodesY[ASTAR_BOUND] = {0};
	int openNodesX[ASTAR_BOUND][4] = {0};	//four directions X
	int openNodesY[ASTAR_BOUND][4] = {0};	//four directions Y
	int openNodesF[4];	//four Final scores = G + H
	int fScoresHist[ASTAR_BOUND][4] = {0};

	for(int i = 0; i < ASTAR_BOUND; i++)
		for(int j = 0; j < 4; j++)
			fScoresHist[i][j] = INT_MAX;
	
	int iter = 0;
	int dir = -1, closedDir = -1;
	int G = 1;//, F = 0, H = 0, currNode = 0;
	int tempTrgX = -1;

	deque<int> indexArr;
	deque<int> valueArr;
	int temp = 0;
/**/
	//Some functionality to handle the teleporters
	if(src_y1 == 9)
		if(src_x1 < 5)
			if(trg_x2 > 9+src_x1)
			{
				for(int i = src_x1; i > -1; i--)
				{
					closedNodesX[iter] = i;
					closedNodesY[iter] = 9;
					iter++; G++;
				}
				closedDir = 2;
				x1 = 19;
			}
		else if(src_x1 > 14)
			if(trg_x2 < 10-(20-src_x1))
			{
				for(int i = src_x1; i < 20; i++)
				{
					closedNodesX[iter] = i;
					closedNodesY[iter] = 9;
					iter++; G++;
				}
				closedDir = 3;
				x1 = 0;
			}
/**/
	//And for the target being near teleporter
	if(trg_y2 == 9)
		if(trg_x2 < 5 && src_x1 > 12)
		{
			tempTrgX = trg_x2;
			trg_x2 = 19;
		}else if(trg_x2 > 14 && src_x1 < 7)
		{
			tempTrgX = trg_x2;
			trg_x2 = 0;
		}
/**/
	//Check our path and update path[]
	while(iter < ASTAR_BOUND)
	{
		bool deadEnd = true;
		openNodesF[0] = INT_MAX, openNodesF[1] = INT_MAX, openNodesF[2] = INT_MAX, openNodesF[3] = INT_MAX;
		//Seal off the way we came from investigation.
		closedNodesX[iter] = x1;
		closedNodesY[iter] = y1;

		if( manhattanDist( x1, y1, trg_x2, trg_y2 ) == 1 )
			break;

		//Find available directions and calculate pathfinding indices for each dir
		if(pS.dtpMap[x1][y1-1] > limit && closedDir != 0)
		{
			deadEnd = false;
			openNodesX[iter][0] = x1; 
			openNodesY[iter][0] = y1-1;
			fScoresHist[iter][0] = openNodesF[0] = G + myHeuristicDist(x1, y1-1, trg_x2, trg_y2);
		}
		if(pS.dtpMap[x1][y1+1] > limit && closedDir != 1)
		{
			deadEnd = false;
			openNodesX[iter][1] = x1; 
			openNodesY[iter][1] = y1+1;  
			fScoresHist[iter][1] = openNodesF[1] = G + myHeuristicDist(x1, y1+1, trg_x2, trg_y2); 
		}
		if(pS.dtpMap[x1+1][y1] > limit && closedDir != 2)
		{
			deadEnd = false;
			openNodesX[iter][2] = x1+1; 
			openNodesY[iter][2] = y1;  
			fScoresHist[iter][2] = openNodesF[2] = G + myHeuristicDist(x1+1, y1, trg_x2, trg_y2);
		}
		if(pS.dtpMap[x1-1][y1] > limit && closedDir != 3)
		{
			deadEnd = false;
			openNodesX[iter][3] = x1-1; 
			openNodesY[iter][3] = y1; 
			fScoresHist[iter][3] = openNodesF[3] = G + myHeuristicDist(x1-1, y1, trg_x2, trg_y2);
		}


		/*Need to add parents array:
		@ iter, parent of X,Y is closedDir or something
		at end, have aPathX[end] = trg_x, aPathX[end-1] = trg_x.parent, ...
		In other words aPathX[end] = closedNodesX[end].parent, aPathX[end-1] = closedNodesX[end].parent, ...
		That should give a seamless path...
		*/
/* PRINT OUTS * /
sprintf_s(out, 99, "%d", lowest(fScoresHist[iter], 4)); print("\ndir of F score @ iter: "); print(out);
sprintf_s(out, 99, "%d", fScoresHist[iter][lowest(fScoresHist[iter], 4)]); print("\nlowest F score @ iter: "); print(out);
/**/		
		//Now look at all the F scores for the smallest one...going back through all previously tested nodes : maybe we shud just go thru nodes on path?
		//If indices are (i, j) then node to pick is: x1 = openNodesX[i][j], y1 = openNodesY[i][j]
		//dir will be equal to j, closedDir its opposite
		for(int i = 0; i <= iter; i++)
		{
			temp = lowest( fScoresHist[i], 4 );
			indexArr.push_back( temp );
			valueArr.push_back( fScoresHist[i][temp] );
		}

		if( deadEnd )
		{}
		
		temp = lowest(valueArr);
		dir = indexArr[temp];
		if(dir%2 == 0)
			closedDir = dir + 1;
		else
			closedDir = dir - 1;
		x1 = openNodesX[temp][dir];
		y1 = openNodesY[temp][dir];

/*
if(fScoresHist[temp][dir] < openNodesF[lowest(openNodesF, 4)])
{	sprintf_s(out, 99, "%d", fScoresHist[temp][dir]); print("\nlowest f scores hist: "); print(out);
}else if(fScoresHist[temp][dir] == openNodesF[lowest(openNodesF, 4)])
{	sprintf_s(out, 99, "%d", fScoresHist[temp][dir]); print("\nequal f scores: "); print(out);
}else{
	sprintf_s(out, 99, "%d", openNodesF[lowest(openNodesF, 4)]); print("\nlowest F from openNodes: "); print(out);
}
*/
		//Put the chosen node on the closed lists
		fScoresHist[temp][dir] = INT_MAX;
		openNodesX[temp][dir] = -1;
		openNodesY[temp][dir] = -1;
/** /
		//Pick the dir with smallest F value - i.e. shortest path
		//Set the next testable node to this, and close the direction we came from
		dir = lowest(openNodesF, 4);
		switch(dir)
		{
		case 0:
			{
			y1--;
			closedDir = 1;
			break;
			}
		case 1:
			{
			closedDir = 0;
			y1++;
			break;
			}
		case 2:
			{
			closedDir = 3;
			x1++;
			break;
			}
		case 3:
			{
			closedDir = 2;
			x1--;
			break;
			}
		}

/** /		//Implement backtracking capability
		for(int i = 0; i < iter-1; i++) //Test up until current - 1 : to prevent oscillation
		{
			if(fScoresHist[i] < openNodesF[dir])
			{
				//Set the current node back to the 1st one with lower F score
				//Test if any nodes are available - not blocked by walls, not on closedList
				if(pS.dtpMap[closedNodesX[i]][closedNodesY[i] - 1] > 2 && ((closedNodesX[i] != closedNodesX[i-1] && closedNodesY[i] - 1 != closedNodesY[i-1]) || (closedNodesX[i] != closedNodesX[i+1] && closedNodesY[i] - 1 != closedNodesY[i+1])))
				{
					x1 = closedNodesX[i];
					y1 = closedNodesY[i] - 1;
					G = iter = i; G++;
					closedDir = 1;
					break;
				}
				if(pS.dtpMap[closedNodesX[i]][closedNodesY[i] + 1] > 2 && ((closedNodesX[i] != closedNodesX[i-1] && closedNodesY[i] + 1 != closedNodesY[i-1]) || (closedNodesX[i] != closedNodesX[i+1] && closedNodesY[i] + 1 != closedNodesY[i+1])))
				{
					x1 = closedNodesX[i];
					y1 = closedNodesY[i] + 1;
					G = iter = i; G++;
					closedDir = 0;
					break;
				}
				if(pS.dtpMap[closedNodesX[i] + 1][closedNodesY[i]] > 2 && ((closedNodesX[i] + 1 != closedNodesX[i-1] && closedNodesY[i] != closedNodesY[i-1]) || (closedNodesX[i] + 1 != closedNodesX[i+1] && closedNodesY[i] != closedNodesY[i+1])))
				{
					x1 = closedNodesX[i] + 1;
					y1 = closedNodesY[i];
					G = iter = i; G++;
					closedDir = 3;
					break;
				}
				if(pS.dtpMap[closedNodesX[i] - 1][closedNodesY[i]] > 2 && ((closedNodesX[i] - 1 != closedNodesX[i-1] && closedNodesY[i] != closedNodesY[i-1]) || (closedNodesX[i] - 1 != closedNodesX[i+1] && closedNodesY[i] != closedNodesY[i+1])))
				{
					x1 = closedNodesX[i] - 1;
					y1 = closedNodesY[i];
					G = iter = i; G++;
					closedDir = 2;
					break;
				}
				//Else -> Continue looking for lower F scores until i = iter+1
			}
		}
/**/
		path[iter] = dir;
		iter++;
		G++;
		indexArr.clear();
		valueArr.clear();
	}//End While Loop

	//Cut out any loops that have appeared in the closedNodes arrays.
/** /	for(int pt = 0; pt < iter-1; pt++)
		{
			for(int luk = pt+1; luk < iter; luk++)
			{
				if(closedNodesX[pt] == closedNodesX[luk] && closedNodesY[pt] == closedNodesY[luk])
				{
					int t = pt;
					int l = luk;
					while( l < iter )
					{
						closedNodesX[t] = closedNodesX[l];
						closedNodesY[t] = closedNodesY[l];
						t++; l++;
					}
					iter -= (luk - pt);
					break;
				}
			}
		}
/**/
	//If the target was in one of the teleport deadends, we may have teleported there, need to finish off.
	if(tempTrgX != -1)
	{
		trg_x2 = tempTrgX;
		if(tempTrgX < 5)
			for(int i = 0; i < tempTrgX; i++)
			{
				closedNodesX[iter] = i;
				closedNodesY[iter] = 9;
				iter++;
			}
		else if(tempTrgX > 14)
			for(int i = 19; i > tempTrgX; i--)
			{
				closedNodesX[iter] = i;
				closedNodesY[iter] = 9;
				iter++;
			}
	}

	//When we have our path, iterate back from the target and push down the parent of each path node.
	aPathX.push_front(trg_x2);
	aPathY.push_front(trg_y2);

	for(int i = iter; i > -1; i--)
		if(manhattanDist(aPathX.front(), aPathY.front(), closedNodesX[i], closedNodesY[i]) == 1)
		{
			aPathX.push_front(closedNodesX[i]);
			aPathY.push_front(closedNodesY[i]);
		}

//sprintf_s(out, 99, "%d", iter); print("\nWhile Loop iter's: "); println(out);
	iter = (int)aPathX.size();
/** /
	print("Source: ");
	sprintf_s(out, 99, "%d", src_x1); print(out); print(", ");
	sprintf_s(out, 99, "%d", src_y1); print(out); print("\n");
/** /
	sprintf_s(out, 99, "%d", iter); print("path length: "); println(out); 
	for(int i = 0; i < iter; i++)
	{
		print("Iter: ");
		sprintf_s(out, 99, "%d", i); print(out); print(";\t");
		sprintf_s(out, 99, "%d", aPathX[i]); print(out); print(", ");
		sprintf_s(out, 99, "%d", aPathY[i]); print(out); print(";\t");
		if(i > 0){
			sprintf_s(out, 99, "%d", path[i-1]); print(out); } print("\n");
	}
/** /
	print("Target: ");
	sprintf_s(out, 99, "%d", trg_x2); print(out); print(", ");
	sprintf_s(out, 99, "%d", trg_y2); println(out);
/**/
	return iter;
}

/*****************************************************************
 *	FILE I/O AND NETWORKING FUNCTIONS
 *****************************************************************/

/**
 *	Logging functions and fields for outputting debug info.
 */
// Puts its string argument to a global output stream.
void print(char* string)
{
	if(string == NULL)
		return;
	if(fileout != NULL)
		fputs(string, fileout);
}

// Puts its string argument and a carriage return to a global output stream.
void println(char* string)
{
	if(string == NULL)
		return;
	if(fileout != NULL)
	{
		fputs(string, fileout);
		fputs("\n", fileout);
	}
}
/**
 *	Uses the console to get the player name, appends a random session number, and creates an output file thereby named
 */
void getPlayerData()
{
	string news("evalData/analysisData-");

	/** /
	string input;
	cout << "Enter your name:" << endl;
	cin >> input;
	news += input;
	/** /
	cout << "Enter your session #:" << endl;
	cin >> input;
	news += input;
	/**/
	sprintf_s( out, 99, "%d", rand() );
	news += out;
	/**/
	news += ".csv";

	// Open for write
	if( fopen_s( &fileout, news.c_str(), "wt" ) != 0 )
	  cout << "The file " << news.c_str() << "was not opened." << endl;
}

/**
 *	Print out the sprites name and coordinates in map index values
 */
void logPos(D3DXVECTOR3 spritePos, char *name, char *endChar)
{
	getSpritePos(spritePos);

	char numBin[25];
	print(name);
	if(xIndex < 10) print(" ");
	sprintf_s( numBin, 25, "%d", xIndex);
	print( numBin );
	if(yIndex < 10) print(",  ");
	else print(", ");
	sprintf_s( numBin, 25, "%d", xIndex);
	print( numBin );
	print(endChar);
}

/**
 *	Description: Reads in PacStates from a .csv file, named in argument.
 *	Returns:	deque of ints holding values from the .csv, need to be interpreted as PacState fields by caller
 */
deque<int> readFromCSVFile( char * filename )
{
	int c;
	FILE *states;
	deque <int> csv;
	string state("");

	// Open for read (will fail if 'filename'.csv does not exist)
	if( fopen_s( &states, filename, "r" ) != 0 )
		cout << "The file " << filename << "was not opened." << endl;

	while( true )
	{
		if( (c = fgetc(states)) == EOF )
			break;
		state += c;
		if( c == ',' )
		{
			csv.push_back( atoi(state.c_str()) );
			state.clear();
		}
		if( c == '\n' )
		{
			csv.push_back( atoi(state.c_str()) );
			state.clear();
			csv.push_back( INT_MAX );
		}
	}

	return csv;
}

/**
 *	Description: Writes all the state information from all the states in the GameStates global deque, in CSV format.
 */
void outputState(bool local)
{
	string news("");

	for( int i = 0; i < GameStates.size(); i++ )
	{
		sprintf_s( out, 99, "%d", i );
		news += out; news += ",";
		sprintf_s( out, 99, "%d", GameStates[i].xPac );
		news += out; news += ",";
		sprintf_s( out, 99, "%d", GameStates[i].yPac );
		news += out; news += ",";
		for( int j = 0; j < NUM_GS; j++ )
		{
			sprintf_s( out, 99, "%d", GameStates[i].xGhost[j] );
			news += out; news += ",";
			sprintf_s( out, 99, "%d", GameStates[i].yGhost[j] );
			news += out; news += ",";
		}
		sprintf_s( out, 99, "%d", GameStates[i].Dots );
		news += out; news += ",";
		sprintf_s( out, 99, "%d", GameStates[i].Lives );
		news += out; news += ",";
		sprintf_s( out, 99, "%d", GameStates[i].Pills );
		news += out; news += ",";
		sprintf_s( out, 99, "%d", GameStates[i].Points );
		news += out; news += ",";
		if( GameStates[i].CherryThere )
			news += "1,";
		else
			news += "0,";
		if( GameStates[i].PacAttack )
			news += "1,";
		else
			news += "0,";
		sprintf_s( out, 99, "%d", GameStates[i].MovedDir );
		news += out; news += ",";
		sprintf_s( out, 99, "%d", GameStates[i].Cycle );
		news += out; news += ",";
/**/	for( int x = 0; x < 20; x++ )
			for( int y = 0; y < 20; y++ )
			{
				sprintf_s( out, 99, "%d", GameStates[i].dtpMap[x][y] );
				news += out; news += ",";
			}
/**/	news += "\n";
	}

	sprintf_s(out, 99, "%d", Level);
	string file = "/Level-";
	file += out;
	file += ".csv";
	if(local)
	{
		string temp = gameDir;
		temp += file;
		file = "C:/PacPlayd/Logs/";
		file += temp;
		if( fopen_s( &stateFile, file.c_str(), "wt" ) != 0 )
			cout << "The file " << gameDir.c_str() << " was not opened." << endl;
		else
			//Put the latest game state data onto the output file stream
			fputs(news.c_str(), stateFile);
	}else{
		//send the file name to save - Level-[the level#].csv
		runclient("FileSend", file.c_str());
		//Send the game state data.
		runclient("DataSend", news.c_str());
	}
}

/**
 *	Create a client interface to a server running on 
 *	Arguments : ip	-	IP addr for a remote machine
 *				cmd -	DirSend, the name of a directory to create for this game
 *						FileSend, the name of the file equivalent to one game level
 *						DataSend, the actual game state data
 *				data -	the data, either a directory name, file name, or game play data.
 */
int runclient(const char *cmd, const char *data)
{
	char *rec = new char[32];

	if(!w.open)
	{
		// Connect To Server - Port number is hardcoded to 4276 usually, 88 on Nigel's server.
		if( w.connectServer("193.61.166.253", 88) == -1 )	// 127.0.0.1	193.61.166.20/253
		{
			cout << "Not connected to server!\n";
			return -1;
		}
		else
			cout << "Connected to server!\n";
	}

	//Send command
	w.sendData(cmd);	w.recvData(rec,32);
	if(strcmp(rec, "OK1") == 0)
		cout << "Did " << cmd << "; Received Ack" << endl;

	//Send data
	w.sendData(data);	w.recvData(rec,32);
	if(strcmp(rec, "OK2") == 0)
		cout << cmd << " payload sent; Received Ack" << endl;

	return 0;
}

void endclient()
{
	char *rec = new char[32];
	// Send Close Connection Signal
	w.sendData("EndConnection"); w.recvData(rec,32);
	if(strcmp(rec, "END") == 0)
		cout << "Connection ended!\n";
}
//End of file
