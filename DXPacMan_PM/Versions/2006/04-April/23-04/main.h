/**	
 *	Author: Ben Cowley, 2006.
 *	Header file for base of dependency tree - holds universally available fields and functions.
 */

#ifndef _MAIN_H
#define _MAIN_H

////////////////////////////////////////////////////////////////////////////////////////////
//Global game variables
////////////////////////////////////////////////////////////////////////////////////////////
int PacMap[20][20];
int Score;
int NumPills = 180;
int GameLives = 5;
int GhostFleeTimer = 300;
int RowAnimation = 0;

bool CherryEaten = false;
bool GameOver = false;
bool GhostAttack = true;

//Vectors for sprite positions (used by sprite Draw() function)
D3DXVECTOR3				g_PacmanSpritePos;	
D3DXVECTOR3				g_ShadowSpritePos;
D3DXVECTOR3				g_SpeedySpritePos;	
D3DXVECTOR3				g_PokeySpritePos;
D3DXVECTOR3				g_BashfulSpritePos;
D3DXVECTOR3				g_CherrySpritePos;

#endif