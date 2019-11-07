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

#define	PACMANSPRITE_START_POS_Y	13		//Start position y offset for PacMan sprite
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
#define GDMode						0		//Sets the mode that determines how the Ghosts move. See main.cpp/GhostDriver() for mode details
#define NUM_GS						2		//Number of Ghosts we are using
#define POT_GS						16		//Number of potential next move permutations = 4 ^ NUM_GS
#define TEST_HARNESS				false
#define SERVER						false
#define NOMV						-1		//Value for entries in GhostMoves struct objects that are not valid moves

////////////////////////////////////////////////////////////////////////////////////////////
//Global game variables, that we need in more than one source file
////////////////////////////////////////////////////////////////////////////////////////////
extern int logIndex;
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
extern int gakProb;
extern int pacProb;

extern unsigned int pacTik;
extern float gstTik;

extern int xIndex;
extern int yIndex;

extern deque <int> aPathX;
extern deque <int> aPathY;

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
extern string logData;

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
void sleep(DWORD dur);

deque<int> GetClosestPill( PacState pS );
double avgDistribution(int *xGhost, int *yGhost);
void getSpritePos(D3DXVECTOR3 spritePos);
int localDots(int xCentre, int yCentre, int radius, int Map[20][20]);
int localGhosts(int x, int y, int radius, PacState pS);

double lineDist(int x1, int y1, int x2, int y2);
int manhattanDist(int x1, int y1, int x2, int y2);
int myHeuristicDist(int x1, int y1, int x2, int y2);

void print(char* string);
void print(string str);
void println(char* string);
void logPos(D3DXVECTOR3 spritePos, char *name, char *endChar);
void getPlayerData( char* name );
deque<int> readFromCSVFile( char * filename );
void outputState(bool local);
int runclient(const char *cmd, const char *data);
void endclient();

//The A* function
int a_Star(int src_x1, int src_y1, int trg_x2, int trg_y2, bool pacTravelling, PacState pS);

/**
 *	Description: Holds 256 permutations of ghost movement, thus details of location with four ghosts and an x,y axis
 *	perms = num of viable permutations in this struct. 
 *	If ghosts movement is random, Probability of any one permutation is 1/perms
 */
struct GhostMoves
{
public:
	GhostMoves() {}

	GhostMoves( int* xGhost, int* yGhost )
	{
		//Up to POT_GS possible states given any distinct PacAction (since we have NUM_GS ghosts with 4 poss move dirs each = 4^NUM_GS)
		for(int p1 = 0; p1 < NUM_GS; p1++)
			for(int p2 = 0; p2 < POT_GS; p2++)
			{
				ghostXYMove[p1][0][p2] = xGhost[p1];
				ghostXYMove[p1][1][p2] = yGhost[p1];
				ghostXYMove[p1][2][p2] = 0;
			}
		perms = 0;
	}

	//Data struct holding - Ghosts[1-N] x (x,y,p) x Permutations
	int ghostXYMove[NUM_GS][3][POT_GS];
	short perms;

	bool operator==(GhostMoves gM)
	{
		if(perms != gM.perms)
			return false;
		else{
			for(int i0 = 0; i0 < NUM_GS; i0++)
			{
			for(int i1 = 0; i1 < 3; i1++)
			{
			for(int i2 = 0; i2 < POT_GS; i2++)
			{
				if(ghostXYMove[i0][i1][i2] != gM.ghostXYMove[i0][i1][i2])
					return false;
			}}}
		}
		return true;
	}

	void operator=(GhostMoves gM)
	{
		perms = gM.perms;
		for(int i0 = 0; i0 < NUM_GS; i0++)
		{
		for(int i1 = 0; i1 < 3; i1++)
		{
		for(int i2 = 0; i2 < POT_GS; i2++)
		{
			ghostXYMove[i0][i1][i2] = gM.ghostXYMove[i0][i1][i2];
		}}}
	}

	short size()
	{
		perms = 0;
		for(int i = 0; i < POT_GS; i++)
			if(ghostXYMove[0][0][i] > NOMV)
				perms++;
		return perms;
	}
};
/* End GhostMoves struct */

/**
 *	Description: struct holds the state of the game, including x,y position for all sprites (!cherry - x,y is fixed).
 *	This is a potential state, all the state variables for the state rendered to screen are declared & instantiated elsewhere (mostly universals.h)
 */
class PacState
{
public:
	
	int State, xPac, yPac, Lives, Points, Pills, Dots, MovedDir, Cycle;
	int xGhost[NUM_GS], yGhost[NUM_GS];
	bool CherryThere, PacAttack;
	int dtpMap[20][20];
	GhostMoves gms;

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

		State = logIndex;
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
	PacState::PacState(int state, int PacX, int PacY, int numLives, int numPoints, int numPills, int numDots, int moved, int cycle, int *ghostX, int *ghostY, bool cherryThere, bool pacAtk, int Map[20][20])
	{
		State = state;
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
		CherryThere = cherryThere;
		PacAttack = pacAtk;

		for(int i = 0; i < 20; i++)
		for(int j = 0; j < 20; j++)
			dtpMap[i][j] = Map[i][j];
	}
	/**/
	void PacState::operator=(PacState s)
	{
		State = s.State;
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
	void setProb( int prob )
	{
		dtpMap[0][0] = prob;
	}
	int getProb()
	{
		return dtpMap[0][0];
	}
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

/**
 *	Description: Sum the contents of a deque. As long as the contents are summable (can utilise the = and + operators) then it should work.
 */
template <class myType>
myType sum(deque <myType> deQ)
{
	myType myte = 0;

	for(unsigned int i = 0; i < deQ.size(); i++)
		myte += deQ[i];

	return myte;
}

/**
 *	Description: Sum of deque elements from 1 to n(element 0 is for storing result). 
 *	As long as the contents are summable (can utilise the = and + operators) then it should work.
 */
template <class myType>
myType recordSum(deque <myType> deQ)
{
	for(unsigned int i = deQ.size()-1; i > 0; i--)
		deQ[0] += deQ[i];

	return deQ[0];
}

/**
 *	Description: Get average of deque elements from 1 to n (element 0 is for storing result). 
 *	As long as the contents are summable (can utilise the = and + operators) then it should work.
 */
template <class myType>
myType recAvg(deque <myType> deQ)
{
	for(unsigned int i = deQ.size()-1; i > 0; i--)
		deQ[0] += deQ[i];

	return deQ[0] / (deQ.size()-1);
}

/**
 *	Description: Get average of deque elements from (deque.size() - w0) to end of deque, inclusive (element 0 is for storing result).
 *	If w0 is larger than deque size then simply return, If w0 is too low it will be set to 1.
 *	As long as the contents are summable (can utilise the = and + operators) then it should work.
 */
template <class myType>
myType recAvgWindow(deque <myType> deQ, int w0 )
{
	int w1 = deQ.size()-1;
/* Old code
	if( w0 < 1 ) w0 = 1;
	if( w1 > s ) w1 = s;
*/
	// If the specified window is larger than the deque, just use the entire deque (after the first element)
	if( w0 >= w1 ) 
		w0 = 1;
	else
		w0 = w1 - w0;

	for(unsigned int i = w1; i > w0; i--)
		deQ[0] += deQ[i];

	return deQ[0] / (w1-w0);
}

extern deque <PacState> GameStates;

#endif /* INC_UNIVERSALS_H */