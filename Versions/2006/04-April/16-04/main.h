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

#endif