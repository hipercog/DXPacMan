/**	
 *	Author: Ben Cowley, 2006.
 *	Header file for base of dependency tree - holds universally available fields and functions.
 */

#ifndef INC_MAIN_H
#define INC_MAIN_H

#include <algorithm>
#include <deque>
#include <string>
#include <fstream>
#include <iostream>
#include <d3dx9math.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef _USE_OLD_OSTREAMS
using namespace std;
#endif

////////////////////////////////////////////////////////////////////////////////////////////
//Global game variables, that we need in more than one source file
////////////////////////////////////////////////////////////////////////////////////////////
extern int PacMoves;
extern int PacMap[20][20];
extern int Score;
extern int NumPills;
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
// Shared functions from main.cpp
////////////////////////////////////////////////
void getSpritePos(D3DXVECTOR3 spritePos);
void logPos(D3DXVECTOR3 spritePos, char *name, char *endChar);
void getPlayerData();
void print(char* string);

#endif /* INC_MAIN_H */