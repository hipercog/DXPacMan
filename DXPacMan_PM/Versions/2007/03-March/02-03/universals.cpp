/**	
 *	Author: Ben Cowley, 2006.
 *	Source file for universal functions and fields.
 */

#include "universals.h"

/* Universal variables */
int PacMoves = 0;
int PacMap[20][20];
int Score;
int NumPills = 4;
int NumDots = 175;
int GameLives = 5;
int GhostFleeTimer = 300;
int RowAnimation = 0;
int cycles = 0;
int randCherryInit = 0;
char *out = new char[99];
int RunOfGhostKills = 1;

int xIndex = 0;
int yIndex = 0;

bool CherryEaten = true;
bool GameOver = false;
bool GhostAttack = true;

bool ShadowEaten = false;
bool SpeedyEaten = false;
bool PokeyEaten = false;
bool BashfulEaten = false;

FILE *fileout;	//Open an output File object
FILE *stateFile;	//Open an state data output File object
deque <PacState> GameStates;

//Vectors for sprite positions (used by sprite Draw() function)
D3DXVECTOR3				g_PacmanSpritePos;	
D3DXVECTOR3				g_ShadowSpritePos;
D3DXVECTOR3				g_SpeedySpritePos;	
D3DXVECTOR3				g_PokeySpritePos;
D3DXVECTOR3				g_BashfulSpritePos;
D3DXVECTOR3				g_CherrySpritePos;

/**
 *	Logging functions and fields for outputting debug info.
 */
// Puts its string argument to a global output stream.
void print(char* string)
{
	fputs(string, fileout);
}

// Puts its string argument and a carriage return to a global output stream.
void println(char* string)
{
	fputs(string, fileout);
	fputs("\n", fileout);
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

// Uses the console to get the player name, appends a random session number, and creates an output file thereby named
void getPlayerData()
{
	string input;
	string news("evalData/printout-");

	/** /
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

	// Open for read (will fail if file news.c_str() does not exist)
	if( fopen_s( &fileout, news.c_str(), "wt" ) != 0 )
	  cout << "The file " << news.c_str() << "was not opened." << endl;
}

// Translates Sprite positions into x & y index values for the map.
void getSpritePos(D3DXVECTOR3 spritePos)
{
	//Obtain x and y pos values from the PacMan sprite pointer
	FLOAT xPosInitial = spritePos.x;
	FLOAT yPosInitial = spritePos.y;

	//Adjust position values to obtain PacMap index values
	xIndex = (int)((xPosInitial - GRIDTLX)/SPRITE_WIDTH);
	yIndex = (int)((yPosInitial - GRIDTLY)/SPRITE_HEIGHT);
}

// Print out the sprites name and coordinates in map index values
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
void outputState()
{
	string news("stateData/stateData-1.csv");

	// Open for read (will fail if file news.c_str() does not exist)
	if( fopen_s( &stateFile, news.c_str(), "wt" ) != 0 )
	  cout << "The file " << news.c_str() << "was not opened." << endl;

	for( int i = 0; i < GameStates.size(); i++ )
	{
		sprintf_s( out, 99, "%d", i );
		news = out; news += ",";
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
		sprintf_s( out, 99, "%d", GameStates[i].CherryThere );
		news += out; news += ",";
		sprintf_s( out, 99, "%d", GameStates[i].PacAttack );
		news += out; news += ",";
		sprintf_s( out, 99, "%d", GameStates[i].MovedDir );
		news += out; news += ",";
/**/	for( int x = 0; x < 20; x++ )
			for( int y = 0; y < 20; y++ )
			{
				sprintf_s( out, 99, "%d", GameStates[i].dtpMap[x][y] );
				news += out; news += ",";
			}
/**/	news += "\n";
		fputs(news.c_str(), stateFile);
	}
}

void sleep(DWORD dur)
{
	dur += GetTickCount();
	while( GetTickCount() < dur )
	{}
}