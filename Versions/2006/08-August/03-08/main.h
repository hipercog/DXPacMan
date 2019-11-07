/**	
 *	Author: Ben Cowley, 2006.
 *	Header file for base of dependency tree - holds universally available fields and functions.
 */

#ifndef _MAIN_H
#define _MAIN_H

#include <algorithm>
#include <deque>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>

using namespace std;

////////////////////////////////////////////////////////////////////////////////////////////
//Global game variables
////////////////////////////////////////////////////////////////////////////////////////////
int PacMap[20][20];
int Score;
int NumPills = 180;
int GameLives = 5;
int GhostFleeTimer = 300;
int RowAnimation = 0;
int cycles = 0;
int randCherryInit = 0;
char *out = new char[99];
int RunOfGhostKills = 1;

bool CherryEaten = true;
bool GameOver = false;
bool GhostAttack = true;

bool ShadowEaten = false;
bool SpeedyEaten = false;
bool PokeyEaten = false;
bool BashfulEaten = false;

//Vectors for sprite positions (used by sprite Draw() function)
D3DXVECTOR3				g_PacmanSpritePos;	
D3DXVECTOR3				g_ShadowSpritePos;
D3DXVECTOR3				g_SpeedySpritePos;	
D3DXVECTOR3				g_PokeySpritePos;
D3DXVECTOR3				g_BashfulSpritePos;
D3DXVECTOR3				g_CherrySpritePos;

#endif