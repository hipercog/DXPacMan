/**	
 *	Author: Ben Cowley, 2006.
 *	Header file for universally available fields and functions.
 */

#ifndef INC_UNIVERSALS_H
#define INC_UNIVERSALS_H

#include "main.h"

// forward declaration of PacState class
class PacState;

////////////////////////////////////////////////////////////////////////////////////////////
//Hardcoded game values
////////////////////////////////////////////////////////////////////////////////////////////
#define PACMANSPRITE_START_POS_X	9		//Start position x offset for PacMan sprite
#define SHADOWSPRITE_START_POS_X	9		//Start position x offset for "Shadow" ghost sprite
#define BASHFULSPRITE_START_POS_X	9		//Start position x offset for "Bashful" ghost sprite
#define POKEYSPRITE_START_POS_X		10		//Start position x offset for "Pokey" ghost sprite
#define SPEEDYSPRITE_START_POS_X	10		//Start position x offset for "Speedy" ghost sprite
#define CHERRYSPRITE_START_POS_X	10		//Start position x offset for the cherry sprite

#define	PACMANSPRITE_START_POS_Y	13	//Start position y offset for PacMan sprite
#define	SHADOWSPRITE_START_POS_Y	8		//Start position y offset for "Shadow" ghost sprite
#define	BASHFULSPRITE_START_POS_Y	9		//Start position y offset for "Bashful" ghost sprite
#define	POKEYSPRITE_START_POS_Y		8		//Start position y offset for "Pokey" ghost sprite
#define	SPEEDYSPRITE_START_POS_Y	9		//Start position y offset for "Speedy" ghost sprite
#define	CHERRYSPRITE_START_POS_Y	11		//Start position y offset for the cherry sprite

#define GRIDTLX						120		//Top left x-coordinate of game grid
#define GRIDTLY						40		//Top left y-coordinate of game grid
#define SPRITE_WIDTH				20		//Sprite width (all sprites)
#define SPRITE_HEIGHT				20		//Sprite height (all sprites)
#define ASTAR_BOUND					300		//Number of iterations of the while loop for path search
#define NUM_GS						2		//Number of Ghosts we are using
#define POT_GS						16		//Number of potential next move permutations = 4 ^ NUM_GS

////////////////////////////////////////////////////////////////////////////////////////////
//Global game variables, that we need in more than one source file
////////////////////////////////////////////////////////////////////////////////////////////
extern int PacMoves;
extern int PacMap[20][20];
extern int Score;
extern int NumPills;
extern int NumDots;
extern int GameLives;
extern int Level;
extern int GhostFleeTimer;
extern int RowAnimation;
extern int cycles;
extern int randCherryInit;
extern char *out;
extern int RunOfGhostKills;
extern int GDMode;

extern unsigned int pacTik;
extern float gstTik;

extern int xIndex;
extern int yIndex;

extern bool CherryEaten;
extern bool GameOver;
extern bool GhostAttack;

extern bool ShadowEaten;
extern bool SpeedyEaten;
extern bool PokeyEaten;
extern bool BashfulEaten;

extern string gameDir;
extern FILE *fileout;	//Open an output File object
extern FILE *stateFile;	//Open an state data output File object

//Vectors for sprite positions (used by sprite Draw() function)
extern D3DXVECTOR3				g_PacmanSpritePos;	
extern D3DXVECTOR3				g_ShadowSpritePos;
extern D3DXVECTOR3				g_SpeedySpritePos;	
extern D3DXVECTOR3				g_PokeySpritePos;
extern D3DXVECTOR3				g_BashfulSpritePos;
extern D3DXVECTOR3				g_CherrySpritePos;

////////////////////////////////////////////////
// Shared functions for all source files
////////////////////////////////////////////////
void print(char* string);
void println(char* string);

double avgDistribution(int *xGhost, int *yGhost);
double lineDist(int x1, int y1, int x2, int y2);
int manhattanDist(int x1, int y1, int x2, int y2);
int myHeuristicDist(int x1, int y1, int x2, int y2);

void getSpritePos(D3DXVECTOR3 spritePos);
void logPos(D3DXVECTOR3 spritePos, char *name, char *endChar);
void getPlayerData();
deque<int> readFromCSVFile( char * filename );
void outputState(bool local);
void sleep(DWORD dur);

/**
 *	Description: struct holds the state of the game, including x,y position for all sprites (!cherry - x,y is fixed).
 *	This is a potential state, all the state variables for the state rendered to screen are declared & instantiated elsewhere (mostly universals.h)
 */
class PacState
{
public:
	
	int xPac, yPac, Lives, Points, Pills, Dots, MovedDir, Cycle;
	int xGhost[NUM_GS], yGhost[NUM_GS];
	bool CherryThere, PacAttack;
	int dtpMap[20][20];

	/**
	 *	Description: Creates a new PacState struct from the values of the relevant variables of the current game state
	 */
	PacState::PacState()
	{
		getSpritePos(g_ShadowSpritePos);
		xGhost[0] = xIndex; yGhost[0] = yIndex;
		if( NUM_GS > 1 )
		{
			getSpritePos(g_SpeedySpritePos);
			xGhost[1] = xIndex; yGhost[1] = yIndex;
			if( NUM_GS > 2 )
			{
				getSpritePos(g_PokeySpritePos);
				xGhost[2] = xIndex; yGhost[2] = yIndex;
				if( NUM_GS > 3 )
				{
					getSpritePos(g_BashfulSpritePos);
					xGhost[3] = xIndex; yGhost[3] = yIndex;
		}}}
		getSpritePos(g_PacmanSpritePos);
		xPac = xIndex; yPac = yIndex;

		Lives = GameLives;
		Points = Score;
		Pills = NumPills;
		Dots = NumDots;
		MovedDir = PacMoves;
		Cycle = cycles;
		CherryThere = !CherryEaten;
		PacAttack = !GhostAttack;
		
		for(int i = 0; i < 20; i++)
		for(int j = 0; j < 20; j++)
			dtpMap[i][j] = PacMap[i][j];
	}

	/**
	 *	Description: Creates a new PacState class from the values passed in
	 */
	PacState::PacState(int PacX, int PacY, int numLives, int numPoints, int numPills, int numDots, int moved, int cycle, int *ghostX, int *ghostY, bool cherryEaten, bool PacAtk, int Map[20][20])
	{
		xPac = PacX;
		yPac = PacY;
		Lives = numLives;
		Points = numPoints;
		Pills = numPills;
		Dots = numDots;
		MovedDir = moved;
		Cycle = cycle;
		for(int i = 0; i < NUM_GS; i++)
		{
			xGhost[i] = ghostX[i];
			yGhost[i] = ghostY[i];
		}
		CherryThere = !cherryEaten;
		PacAttack = !PacAtk;

		for(int i = 0; i < 20; i++)
		for(int j = 0; j < 20; j++)
			dtpMap[i][j] = Map[i][j];
	}
	/**/
	void PacState::operator=(PacState s)
	{
		xPac = s.xPac;
		yPac = s.yPac;
		Lives = s.Lives;
		Points = s.Points;
		Pills = s.Pills;
		Dots = s.Dots;
		MovedDir = s.MovedDir;
		Cycle = s.Cycle;
		for(int i = 0; i < NUM_GS; i++)
		{
			xGhost[i] = s.xGhost[i];
			yGhost[i] = s.yGhost[i];
		}
		CherryThere = s.CherryThere;
		PacAttack = s.PacAttack;

		for(int i = 0; i < 20; i++)
		for(int j = 0; j < 20; j++)
			dtpMap[i][j] = s.dtpMap[i][j];
	}
	/**/
}; // End state struct

/**
 * Data structure search functions
 */
// Return the index to the lowest value in an array
template <class myType>
int lowest(myType *aRay, int size)
{
	int minIndex = 0;
	myType low = aRay[minIndex];

	for(int i = 1; i < size; i++)
		if(aRay[i] < low)
		{
			low = aRay[i];
			minIndex = i;
		}
	return minIndex;
}

// Return the index to the lowest value in a deque
template <class myType>
int lowest(deque<myType> deQ)
{
	int minIndex = 0;
	myType low = deQ[minIndex];

	for(unsigned int i = 1; i < deQ.size(); i++)
		if(deQ[i] < low)
		{
			low = deQ[i];
			minIndex = i;
		}
	return minIndex;
}

//Return the index to the highest value in an array
template <class myType>
int highest(myType *aRay, int size)
{
	int maxIndex = 0;
	myType high = aRay[maxIndex];

	for(int i = 1; i < size; i++)
		if(aRay[i] > high)
		{
			high = aRay[i];
			maxIndex = i;
		}
	return maxIndex;
}

//Return the index to the highest value in a deque
template <class myType>
int highest(deque <myType> deQ)
{
	int maxIndex = 0;
	myType high = deQ[maxIndex];

	for(unsigned int i = 1; i < deQ.size(); i++)
		if(deQ[i] > high)
		{
			high = deQ[i];
			maxIndex = i;
		}
	return maxIndex;
}

extern deque <PacState> GameStates;

#endif /* INC_UNIVERSALS_H */