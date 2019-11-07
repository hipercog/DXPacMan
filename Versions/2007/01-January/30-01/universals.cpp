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
int NumDots = 176;
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

// Uses the console to get the player name, appends a random session number, and creates an output file thereby named
void getPlayerData()
{
string input;
string news("evalData/printout-");

/**/
cout << "Enter your name:" << endl;
cin >> input;
news += input;
/** /
cout << "Enter your session #:" << endl;
cin >> input;
news += input;
/**/
itoa(rand(), out, 10);
news += out;
/**/
news += ".txt";

fileout = fopen(news.c_str(), "wt");
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
	print(itoa(xIndex, numBin, 10));
	if(yIndex < 10) print(",  ");
	else print(", ");
	print(itoa(yIndex, numBin, 10));
	print(endChar);
}