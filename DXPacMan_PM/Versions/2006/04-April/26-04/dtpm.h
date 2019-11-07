/**	
 *	Author: Ben Cowley, 2006.
 *	Header file for the specific Decision Theory Player Modelling functions.
 */

#ifndef _DTPM_H
#define _DTPM_H

#include <stdio.h>
#include <stdlib.h>
#include "main.h"

/**
 *	Function prototypes
 */
void mainDTPM();
void init();
double utility(int xPacAct, int yPacAct/*var gameState*/);
double utilityOfState(int xPacAct, int yPacAct, int *ghostsX, int *ghostsY);
void probabilityOfState(unsigned int recurseLimit, int *baseGhostX, int *baseGhostY);//int xPacLoc, int yPacLoc);
double planOfActions(int timeHorizon);
void availableActions();

int a_Star(int src_x1, int src_y1, int trg_x2, int trg_y2);
void fillIndices();
void getSpritePos(D3DXVECTOR3 spritePos);
double lineDist(int x1, int y1, int x2, int y2);
int localDots(int x, int y, int radius);
int localGhosts(int x, int y, int radius);
int manhattanDist(int x1, int y1, int x2, int y2);

void printToFile();
void print(char* string);
void logPos(D3DXVECTOR3 spritePos, char *name, char *endChar);

/**
 *	Global fields.
 */
int xIndex = 0;
int yIndex = 0;
//int GhostMove[4][2][256];
//float fProbs[4][4][4][4];
//Ghost 1-4 Positions, X[i] == i.x, Y[i] == i.y
int ghostIndicesX[4];
int ghostIndicesY[4];
char out[25];
int ShadowMap[20][20];

/**
 *	Decision Theory functions and fields for predicting player utility
 */
//Going Java style! (We have an algorithm to run, need a clean place to run it)
void mainDTPM()
{
	print("\n\n");
/*	logPos(g_ShadowSpritePos, "ShadowXY: ", ";\t");
	logPos(g_SpeedySpritePos, "SpeedyXY: ", ";\t");
	logPos(g_PokeySpritePos, "PokeyXY: ", ";\t");
	logPos(g_BashfulSpritePos, "BashfulXY: ", ";\n");
*/
	logPos(g_PacmanSpritePos, "PacmanXY: ", ";\n");
	init();
	availableActions();
}

//Initialise all the local data structures to hold the real (ie current state) values
void init()
{
	//Copy the map of the real game state
	for(int i = 0; i < 20; i++)
	{
		for(int j = 0; j < 20; j++)
		{
			ShadowMap[i][j] = PacMap[i][j];
		}
	}
	//Call this first to update array of Ghost position xy's
	fillIndices();
}

void availableActions()
{
	getSpritePos(g_PacmanSpritePos);

	//Checking: Up, Down, Right, Left / North, South, East, West / y-1, y+1, x+1, x-1 / 0, 1, 2, 3
	double movUtil[5] = {0.0, 0.0, 0.0, 0.0, 0.0};
	if(ShadowMap[xIndex][yIndex-1] > 2)
		movUtil[0] = 1.0;
	if(ShadowMap[xIndex][yIndex+1] > 2)
		movUtil[1] = 1.0;
	if(ShadowMap[xIndex+1][yIndex] > 2)
		movUtil[2] = 1.0;
	if(ShadowMap[xIndex-1][yIndex] > 2)
        movUtil[3] = 1.0;

	//Either iterate through first moves in available directions to find best utility, or
	//Project to end of plans(seq of moves), heuristic the ghosts and pick best utility (more human-natural)
	//Check for Ghosts & Pill both nearby - strategic sweet spot

	/**
	 *	Pseudocode - what state is Pacman's utility in? 
	 */
	if(GhostAttack) //Normal Pacman behaviour - utility for eating dots, negative for losing life.
	{
		for(int i = 0; i < 4; i++)
		{
			if((xIndex == ghostIndicesX[i] && (yIndex == ghostIndicesY[i] + 1 || yIndex == ghostIndicesY[i] - 1)) || 
				((xIndex == ghostIndicesX[i] + 1 || xIndex == ghostIndicesX[i] - 1) && yIndex == ghostIndicesY[i]))
				//If there's a Ghost in the way of this direction...we can't go that way.
				movUtil[i] = 0.0;
		}
	}
	if(movUtil[0] == 1.0)
		movUtil[0] = utility(xIndex, yIndex - 1);
	if(movUtil[1] == 1.0)
		movUtil[1] = utility(xIndex, yIndex + 1);
	if(movUtil[2] == 1.0)
		movUtil[2] = utility(xIndex + 1, yIndex);
	if(movUtil[3] == 1.0)
		movUtil[3] = utility(xIndex - 1, yIndex);

	//It is here we can decide how many actions ahead to judge utility on.

sprintf(out, "%G", movUtil[0]); print("\tnorth:"); print(out);
sprintf(out, "%G", movUtil[1]); print("\tsouth:"); print(out);
sprintf(out, "%G", movUtil[2]); print("\teast:"); print(out);
sprintf(out, "%G", movUtil[3]); print("\twest:"); print(out);

	int dir = 0;
	movUtil[4] = movUtil[0];
	for(int i = 1; i < 4; i++)
	{
		if(movUtil[4] <= movUtil[i])
		{
			movUtil[4] = movUtil[i];
			dir = i;
		}
	}
	sprintf(out, "%G", movUtil[4]); print("\nUtility:"); print(out);
	switch(dir)
	{
	case 0:
		print(" Prediction: North\n");
		break;
	case 1:
		print(" Prediction: South\n");
		break;
	case 2:
		print(" Prediction: East\n");
		break;
	case 3:
		print(" Prediction: West\n");
		break;
	default:
		break;
	}
}

//Considers all Pacman's actions for a plan and all the heuristics relevant
double planOfActions(int timeHorizon)
{
	double utilityOfPlan = 0.0;
	
	//Obtain the states available after #timeHorizon actions
	while( timeHorizon > 0 )
	{
		timeHorizon--;
		//currentState->addChildren(possibleStates);
		//currentState = currentState.mostUtileChild;
	}
	if(!GhostAttack)
	{
		//Consider the case where Pacman has eaten a PowerPill
	}

	//Apply the Decision Theory formula
//	utilityOfPlan = Sum of : (the utilities of states possible after this action)x(the probabilities of those states occuring)

	return utilityOfPlan;
}

//Utility of the ghost's state
double utilityOfState(int xPacAct, int yPacAct, int *ghostsX, int *ghostsY)
{
	double util = 0.0;
	//Simply find each ghost distance to Pacman (this is a projected location for both parties)
	//If distance is close (getting closer?) with GhostAttack == true, utility--, and vice versa.
	for(int i = 0; i < 4; i++)
		util += manhattanDist(xPacAct, yPacAct, ghostsX[i], ghostsY[i]);
	if(GhostAttack)//Distance is good
		return util;
	else			//Distance is bad
		return 50-util;
}

//Utility of one move. Function only works globally - may need to add ghost XY argument
double utility(int xPacAct, int yPacAct/*var gameState*/)
{
	double util = 0.0;
	//Util is: Sum(probabilityOf(utilityOf(allStatesPossibleAt(xPacAct, yPacAct))))
	//Random ghost move means we can probably cut out probabilityOf
	int totalPills = NumPills;

	for(int i = 0; i < 4; i++)
	{
		if(xPacAct == ghostIndicesX[i] && yPacAct == ghostIndicesY[i])
		{		
			util -= 500;
			break;
		}
		//Ghost will be within 2 moves - 0-1x, 0-1y
		if(abs(ghostIndicesX[i] - xPacAct) < 2 && abs(ghostIndicesY[i] - yPacAct) < 2)
		{
			util -= 100;
			break;
		}
	}

	if (ShadowMap[xPacAct][yPacAct] == 4)
	{
		util += 5;
		totalPills--;
	}
	else if (ShadowMap[xPacAct][yPacAct] == 5)
	{
//		GhostAttack = false; // ghosts now run away - or at least need to be programmed to
		//The eager Pacman will get a number of points equal to number of Ghosts in vicinity * 100
		util += localGhosts(xPacAct, yPacAct, 8)*100;

		util += 50;
		totalPills--;
	}
	else if (ShadowMap[xPacAct][yPacAct] == 3)
	{
        //No immediate utility but teleporting may be the precursor to some plan - test if possible utilities
		if(xPacAct == 0)
		{
			for(int i = 0; i < 4; i++)
			{
				if(ghostIndicesY[i] == 9 && ghostIndicesX[i] < 4)
					util += 50;
			}
			if(localGhosts(5, 9, 1) > 0)
				util += 20;
			if(localDots(15, 9, 4) > 1)
				util += localDots(15, 9, 4) - localGhosts(15, 9, 4);
		}else{
			for(int i = 0; i < 4; i++)
			{
				if(ghostIndicesY[i] == 9 && ghostIndicesX[i] > 15)
					util += 50;
			}
			if(localGhosts(14, 9, 1) > 0)
				util += 20;
			if(localDots(4, 9, 4) > 1)
				util += localDots(4, 9, 4) - localGhosts(4, 9, 4);
		}
	}
	
	//Extra utility for moving toward cherry if still uneaten...
/*	if(!CherryEaten)
	{
		getSpritePos(g_CherrySpritePos);
		int x1 = xIndex;
		int y1 = yIndex;
		getSpritePos(g_PacmanSpritePos);
		if(manhattanDist(x1, y1, xIndex, yIndex) > manhattanDist(xPacAct, yPacAct, xIndex, yIndex))
			//Closer to cherry
			util += 50;
		else
			util -= 50;
	}
*/
	return util;
}

//Discover all possible combinations of ghost movement, and the corresponding probability of each
//Passing all data structs in to enable recursion - array sizes are set, no need to pass
void probabilityOfState(unsigned int recurseLimit, int *baseGhostX, int *baseGhostY/*[Ghost 1-4][x-y]*/)
{
	//256 possible states given any distinct PacAction
	int sttSpc = 0;
	float fProbs[4][4][4][4]; 
	int GhostMove[4][2][256];

	//All states have equal prob in a random distribution
	for(int p1 = 0; p1 < 4; p1++)
	{
		for(int p2 = 0; p2 < 4; p2++)
		{
			for(int p3 = 0; p3 < 4; p3++)
			{
				for(int p4 = 0; p4 < 4; p4++)
				{
					fProbs[p1][p2][p3][p4] = 1;
				}
			}
		}
	}

	for(int p1 = 0; p1 < 4; p1++)
	{
		for(int p2 = 0; p2 < 256; p2++)
		{
			GhostMove[p1][0][p2] = baseGhostX[p1];
			GhostMove[p1][1][p2] = baseGhostY[p1];
		}
	}

	int count = -1;
	for(int g1 = 0; g1 < 4; g1++)
	{
	for(int g2 = 0; g2 < 4; g2++)
	{
	for(int g3 = 0; g3 < 4; g3++)
	{
	for(int g4 = 0; g4 < 4; g4++)
	{
		count++;
		switch(g1)
		{
		case 0:
			if( ShadowMap[baseGhostX[0]][baseGhostY[0] - 1 ] == 1 )
				fProbs[g1][g2][g3][g4] = 0;
			else
				GhostMove[0][1][count]--;
			break;
		case 1:
			if( ShadowMap[baseGhostX[0]][baseGhostY[0] + 1 ] == 1 )
				fProbs[g1][g2][g3][g4] = 0;
			else
				GhostMove[0][1][count]++;
			break;
		case 2:
			if( ShadowMap[baseGhostX[0] + 1 ][baseGhostY[0]] == 1 )
				fProbs[g1][g2][g3][g4] = 0;
			else
				GhostMove[0][0][count]++;
			break;
		case 3:
			if( ShadowMap[baseGhostX[0] - 1 ][baseGhostY[0]] == 1 )
				fProbs[g1][g2][g3][g4] = 0;
			else
				GhostMove[0][0][count]--;
			break;
		default:
			break;
		}
		//Check all around 2nd Ghost
		switch(g2)
		{
		case 0:
			if( ShadowMap[baseGhostX[1]][baseGhostY[1] - 1 ] == 1 )
				fProbs[g1][g2][g3][g4] = 0;
			else
				GhostMove[1][1][count]--;
			break;
		case 1:
			if( ShadowMap[baseGhostX[1]][baseGhostY[1] + 1 ] == 1 )
				fProbs[g1][g2][g3][g4] = 0;
			else
				GhostMove[1][1][count]++;
			break;
		case 2:
			if( ShadowMap[baseGhostX[1] + 1 ][baseGhostY[1]] == 1 )
				fProbs[g1][g2][g3][g4] = 0;
			else
				GhostMove[1][0][count]++;
			break;
		case 3:
			if( ShadowMap[baseGhostX[1] - 1 ][baseGhostY[1]] == 1 )
				fProbs[g1][g2][g3][g4] = 0;
			else
				GhostMove[1][0][count]--;
			break;
		default:
			break;
		}
		//Check all around 3rd Ghost
		switch(g3)
		{
		case 0:
			if( ShadowMap[baseGhostX[2]][baseGhostY[2] - 1 ] == 1 )
				fProbs[g1][g2][g3][g4] = 0;
			else
				GhostMove[2][1][count]--;
			break;
		case 1:
			if( ShadowMap[baseGhostX[2]][baseGhostY[2] + 1 ] == 1 )
				fProbs[g1][g2][g3][g4] = 0;
			else
				GhostMove[2][1][count]++;
			break;
		case 2:
			if( ShadowMap[baseGhostX[2] + 1 ][baseGhostY[2]] == 1 )
				fProbs[g1][g2][g3][g4] = 0;
			else
				GhostMove[2][0][count]++;
			break;
		case 3:
			if( ShadowMap[baseGhostX[2] - 1 ][baseGhostY[2]] == 1 )
				fProbs[g1][g2][g3][g4] = 0;
			else
				GhostMove[2][0][count]--;
			break;
		default:
			break;
		}
		//Check all around 4th Ghost
		switch(g4)
		{
		case 0:
			if( ShadowMap[baseGhostX[3]][baseGhostY[3] - 1 ] == 1 )
				fProbs[g1][g2][g3][g4] = 0;
			else
				GhostMove[3][1][count]--;
			break;
		case 1:
			if( ShadowMap[baseGhostX[3]][baseGhostY[3] + 1 ] == 1 )
				fProbs[g1][g2][g3][g4] = 0;
			else
				GhostMove[3][1][count]++;
			break;
		case 2:
			if( ShadowMap[baseGhostX[3] + 1 ][baseGhostY[3]] == 1 )
				fProbs[g1][g2][g3][g4] = 0;
			else
				GhostMove[3][0][count]++;
			break;
		case 3:
			if( ShadowMap[baseGhostX[3] - 1 ][baseGhostY[3]] == 1 )
				fProbs[g1][g2][g3][g4] = 0;
			else
				GhostMove[3][0][count]--;
			break;
		default:
			break;
		}
	}
	}}}

	for(int p1 = 0; p1 < 4; p1++)
	{
		for(int p2 = 0; p2 < 4; p2++)
		{
			for(int p3 = 0; p3 < 4; p3++)
			{
				for(int p4 = 0; p4 < 4; p4++)
				{
					if( fProbs[p1][p2][p3][p4] == 1 )
						sttSpc++;
				}
			}
		}
	}

	print("\n#P(S): 1/"); itoa(sttSpc, out, 10); print(out);
	print("\t3:0, 4:0\t\t\t\t3:0, 4:1\t\t\t\t3:0, 4:2\t\t\t\t3:0, 4:3\t\t\t\t3:1, 4:0\t\t\t\t3:1, 4:1\t\t\t\t3:1, 4:2\t\t\t\t3:1, 4:3\t\t\t\t3:2, 4:0\t\t\t\t3:2, 4:1\t\t\t\t3:2, 4:2\t\t\t\t3:2, 4:3\t\t\t\t3:3, 4:0\t\t\t\t3:3, 4:1\t\t\t\t3:3, 4:2\t\t\t\t3:3, 4:3\n");
	print("-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------");

	count = -1;
	for(int p1 = 0; p1 < 4; p1++)
	{
		for(int p2 = 0; p2 < 4; p2++)
		{
			print("\n0:"); itoa(p1, out, 10); print(out);
			print(", 1:"); itoa(p2, out, 10); print(out);
			print("|\t");
			for(int p3 = 0; p3 < 4; p3++)
			{
				for(int p4 = 0; p4 < 4; p4++)
				{
					count++;
					fProbs[p1][p2][p3][p4] /= sttSpc;
					if(fProbs[p1][p2][p3][p4] == 0)
						print("0\t\t\t\t\t");
					else
					{
/**/					for(int g2 = 0; g2 < 4; g2++)
						{
							print("G"); itoa(g2, out, 10); print(out); print(":");
							for(int g3 = 0; g3 < 2; g3++)
							{
								itoa(GhostMove[g2][g3][count], out, 10); 
								if(GhostMove[g2][g3][count] < 10) print(" ");
								print(out); print(",");
							}
						}
						print("\t");
						//If we want to go through the Ghost states produced, branching for new states at each one, this is how...
/**/					if(recurseLimit > 0)
						{
							int ghostBranchX[4] = {GhostMove[0][0][count], GhostMove[1][0][count], GhostMove[2][0][count], GhostMove[3][0][count]};
							int ghostBranchY[4] = {GhostMove[0][1][count], GhostMove[1][1][count], GhostMove[2][1][count], GhostMove[3][1][count]};
							probabilityOfState(--recurseLimit, ghostBranchX, ghostBranchY);
						}
/**/				}
				}
			}
		}
	}
}

/**
 *	Functions that do things.
 */

//Implement the A* pathfinding algorithm
int a_Star(int src_x1, int src_y1, int trg_x2, int trg_y2)
{
	if(src_x1 == trg_x2 && src_y1 == trg_y2)
		return 0;

	int x1 = src_x1, y1 = src_y1, x2 = trg_x2, y2 = trg_y2;

	//List of directions, starting from x1, y1
	static int path[20] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
	static int iter = 0;
	int openNodesX[4] = {-1, -1, -1, -1};	//four directions X
	int openNodesY[4] = {-1, -1, -1, -1};	//four directions Y
	int openNodesG[4] = {0, 0, 0, 0};	//four Goto values
	int openNodesH[4] = {0, 0, 0, 0};	//four heuristics to the target (manhattan distance)
	int openNodesF[4] = {99, 99, 99, 99};	//four Final scores = G + H
	int closedNodesX[20];
	int closedNodesY[20];
	int closedDir = -1;
	int G = 0, F = 0, H = 0, currNode = 0;

	//Check our path and update path[]
	while(true)
	{
		//Seal off the way we came from investigation.
		closedNodesX[iter] = x1;
		closedNodesY[iter] = y1;

		//Find available directions and calculate pathfinding indices for each dir
		if(ShadowMap[x1][y1-1] > 2 || closedDir != 0)
		{
			openNodesX[0] = x1; openNodesY[0] = y1-1;
			openNodesG[0]++; openNodesH[0] = manhattanDist(x1, y1-1, x2, y2); 
			openNodesF[0] = openNodesG[0] + openNodesH[0];
		}
		if(ShadowMap[x1][y1+1] > 2 || closedDir != 1)
		{
			openNodesX[1] = x1; openNodesY[1] = y1+1; 
			openNodesG[1]++; openNodesH[1] = manhattanDist(x1, y1+1, x2, y2); 
			openNodesF[1] = openNodesG[1] + openNodesH[1];
		}
		if(ShadowMap[x1+1][y1] > 2 || closedDir != 2)
		{
			openNodesX[2] = x1+1; openNodesY[2] = y1; 
			openNodesG[2]++; openNodesH[2] = manhattanDist(x1+1, y1, x2, y2); 
			openNodesF[2] = openNodesG[2] + openNodesH[2];
		}
		if(ShadowMap[x1-1][y1] > 2 || closedDir != 3)
		{
			openNodesX[3] = x1-1; openNodesY[3] = y1; 
			openNodesG[3]++; openNodesH[3] = manhattanDist(x1-1, y1, x2, y2); 
			openNodesF[3] = openNodesG[3] + openNodesH[3];
		}

		//Pick the dir with smallest F value - i.e. shortest path
		int dir = 3; closedDir = 2;
		if(openNodesF[0] < openNodesF[1]){
			if(openNodesF[0] < openNodesF[2]){
				if(openNodesF[0] < openNodesF[3])
				{
					y1--;
					dir = 0; closedDir = 1;
				}
			}else
				if(openNodesF[2] < openNodesF[3])
				{
					x1++;
					dir = 2; closedDir = 3;
				}
		}else if(openNodesF[1] < openNodesF[2]){
			if(openNodesF[1] < openNodesF[3])
			{
				y1++;
				dir = 1; closedDir = 0;
			}
		}else if(openNodesF[2] < openNodesF[3])
		{
			x1++;
			dir = 2; closedDir = 3;
		}
		if(dir == 3) x1--;

		path[iter] = dir;
		if(x1 == trg_x2 && y1 == trg_y2)
			break;
		iter++;
	}

	int i;
	for(i = 0; i < 20; i++)
	{
		if(path[i] = -1) break;
	}
	return i;
}

int manhattanDist(int x1, int y1, int x2, int y2)
{
	return abs(x1 - x2) + abs(y1 - y2);
}

int localDots(int xCentre, int yCentre, int radius)
{
	int dots = 0;
	int xPlus = 20, yPlus = 20;
	if(xCentre+radius < 20)
		xPlus = xCentre+radius;
	if(yCentre+radius < 20)
		yPlus = yCentre+radius;
	for(int x = xCentre-radius; x <= xPlus; x++)
	{
		for(int y = yCentre-radius; y <= yPlus; y++)
		{
			if(ShadowMap[x][y] == 4)
				dots++;
		}
	}
	return dots;
}

int localGhosts(int x, int y, int radius)
{
	int ghosts = 0;
	for(int i = 0; i < 4; i++)
	{
		if(manhattanDist(ghostIndicesX[i], ghostIndicesY[i], x, y) <= radius)
			ghosts++;
	}
	return ghosts;
}

void getSpritePos(D3DXVECTOR3 spritePos)
{
	//Obtain x and y pos values from the PacMan sprite pointer
	FLOAT xPosInitial = spritePos.x;
	FLOAT yPosInitial = spritePos.y;

	//Adjust position values to obtain PacMap index values
	xIndex = (int)((xPosInitial - GRIDTLX)/SPRITE_WIDTH);
	yIndex = (int)((yPosInitial - GRIDTLY)/SPRITE_HEIGHT);
}

void fillIndices()
{
	getSpritePos(g_ShadowSpritePos);
	ghostIndicesX[0] = xIndex; ghostIndicesY[0] = yIndex;
	getSpritePos(g_SpeedySpritePos);
	ghostIndicesX[1] = xIndex; ghostIndicesY[1] = yIndex;
	getSpritePos(g_PokeySpritePos);
	ghostIndicesX[2] = xIndex; ghostIndicesY[2] = yIndex;
	getSpritePos(g_BashfulSpritePos);
	ghostIndicesX[3] = xIndex; ghostIndicesY[3] = yIndex;
}

double lineDist(int x1, int y1, int x2, int y2)
{
	double dist = 0.0;
	dist = sqrt(double((x2 - x1)^2 + (y2 - y1)^2));
	return dist;
}

/**
 *	Logging functions and fields for outputting debug info (not exactly Stealth!)
 */
FILE *fileout = fopen("evalData/printout.txt", "wt");	//Open an output File object

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

//Flush and close the output File
void printToFile()
{
	fflush(fileout);
	fclose(fileout);
}

//Puts its string argument to a global output stream.
void print(char* string)
{
	fputs(string, fileout);
}

#endif