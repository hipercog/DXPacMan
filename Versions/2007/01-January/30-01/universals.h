/**	
 *	Author: Ben Cowley, 2006.
 *	Header file for universally available fields and functions.
 */

#ifndef INC_UNIVERSALS_H
#define INC_UNIVERSALS_H

#include "main.h"

////////////////////////////////////////////////////////////////////////////////////////////
//Hardcoded game values
////////////////////////////////////////////////////////////////////////////////////////////
#define GRIDTLX						120		//Top left x-coordinate of game grid
#define GRIDTLY						40		//Top left y-coordinate of game grid
#define SPRITE_WIDTH				20		//Sprite width (all sprites)
#define SPRITE_HEIGHT				20		//Sprite height (all sprites)
#define PACMANSPRITE_START_POS_X	9		//Start position x offset for PacMan sprite
#define PACMANSPRITE_START_POS_Y	13		//Start position y offset for PacMan sprite
#define SHADOWSPRITE_START_POS_X	9		//Start position x offset for "Shadow" ghost sprite
#define SHADOWSPRITE_START_POS_Y	8		//Start position y offset for "Shadow" ghost sprite
#define BASHFULSPRITE_START_POS_X	9		//Start position x offset for "Bashful" ghost sprite
#define BASHFULSPRITE_START_POS_Y	9		//Start position y offset for "Bashful" ghost sprite
#define POKEYSPRITE_START_POS_X		10		//Start position x offset for "Pokey" ghost sprite
#define POKEYSPRITE_START_POS_Y		8		//Start position y offset for "Pokey" ghost sprite
#define SPEEDYSPRITE_START_POS_X	10		//Start position x offset for "Speedy" ghost sprite
#define SPEEDYSPRITE_START_POS_Y	9		//Start position y offset for "Speedy" ghost sprite
#define CHERRYSPRITE_START_POS_X	10		//Start position x offset for the cherry sprite
#define CHERRYSPRITE_START_POS_Y	11		//Start position y offset for the cherry sprite

////////////////////////////////////////////////////////////////////////////////////////////
//Global game variables, that we need in more than one source file
////////////////////////////////////////////////////////////////////////////////////////////
extern int PacMoves;
extern int PacMap[20][20];
extern int Score;
extern int NumPills;
extern int NumDots;
extern int GameLives;
extern int GhostFleeTimer;
extern int RowAnimation;
extern int cycles;
extern int randCherryInit;
extern char *out;
extern int RunOfGhostKills;

extern int xIndex;
extern int yIndex;

extern bool CherryEaten;
extern bool GameOver;
extern bool GhostAttack;

extern bool ShadowEaten;
extern bool SpeedyEaten;
extern bool PokeyEaten;
extern bool BashfulEaten;

extern FILE *fileout;	//Open an output File object

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
void getSpritePos(D3DXVECTOR3 spritePos);
void logPos(D3DXVECTOR3 spritePos, char *name, char *endChar);
void getPlayerData();
void print(char* string);
void println(char* string);

/**
 * Data structure manipulation functions
 */
// Return the index to the lowest value in an array
template <class myType>
int lowest(myType *array, int size)
{
	int minIndex = 0;
	myType low = array[minIndex];

	for(int i = 1; i < size; i++)
	{
		if(array[i] < low)
		{
			low = array[i];
			minIndex = i;
		}
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
	{
		if(deQ[i] < low)
		{
			low = deQ[i];
			minIndex = i;
		}
	}
	return minIndex;
}

//Return the index to the highest value in an array
template <class myType>
int highest(myType *array, int size)
{
	int maxIndex = 0;
	myType high = array[maxIndex];

	for(int i = 1; i < size; i++)
	{
		if(array[i] > high)
		{
			high = array[i];
			maxIndex = i;
		}
	}
	return maxIndex;
}

#endif /* INC_UNIVERSALS_H */