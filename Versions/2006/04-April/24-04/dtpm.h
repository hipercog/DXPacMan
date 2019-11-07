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
double utilityNormal(int xPacAct, int yPacAct/*var gameState*/);
double utilityAttack(int xPacAct, int yPacAct/*var gameState*/);
void probabilityOfState();//int xPacLoc, int yPacLoc);
double planOfActions(int timeHorizon);
void availableActions();
void a_Star(int x1, int y1, int x2, int y2);
void fillIndices();
void getSpritePos(D3DXVECTOR3 spritePos);
double lineDist(int x1, int y1, int x2, int y2);

void printToFile();
void print(char* string);
void logPos(D3DXVECTOR3 spritePos, char *name, char *endChar);

/**
 *	Global fields.
 */
int xIndex = 0;
int yIndex = 0;
int GhostMove[4][2][256];
float fProbs[4][4][4][4];
//Ghost 1-4 Positions, [i][0] == i.x, [i][1] == i.y
int ghostIndices[4][2];
char out[25];

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
	//Call this first to update array of Ghost position xy's
	fillIndices();
	availableActions();
}

void availableActions()
{
	getSpritePos(g_PacmanSpritePos);

	//Checking: Up, Down, Right, Left / North, South, East, West / y-1, y+1, x+1, x-1 / 0, 1, 2, 3
	double moves[5] = {0.0, 0.0, 0.0, 0.0, 0.0};
	if(PacMap[xIndex][yIndex-1] > 2)
		moves[0] = 1.0;
	if(PacMap[xIndex][yIndex+1] > 2)
		moves[1] = 1.0;
	if(PacMap[xIndex+1][yIndex] > 2)
		moves[2] = 1.0;
	if(PacMap[xIndex-1][yIndex] > 2)
        moves[3] = 1.0;

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
			if((xIndex == ghostIndices[i][0] && (yIndex == ghostIndices[i][1] + 1 || yIndex == ghostIndices[i][1] - 1)) || 
				((xIndex == ghostIndices[i][0] + 1 || xIndex == ghostIndices[i][0] - 1) && yIndex == ghostIndices[i][1]))
				//If there's a Ghost in the way of this direction...we can't go that way.
				moves[i] = 0.0;
		}
		if(moves[0] == 1.0)
			moves[0] = utilityNormal(xIndex, yIndex - 1);
		if(moves[1] == 1.0)
			moves[1] = utilityNormal(xIndex, yIndex + 1);
		if(moves[2] == 1.0)
			moves[2] = utilityNormal(xIndex + 1, yIndex);
		if(moves[3] == 1.0)
			moves[3] = utilityNormal(xIndex - 1, yIndex);

for(int i = 0; i < 4; i++)
{
	sprintf(out, "%G", moves[i]); print("\tmoves[i]:"); print(out);
}

		int dir = 0;
		moves[4] = moves[0];
		for(int i = 1; i < 4; i++)
		{
			if(moves[4] <= moves[i])
			{
				moves[4] = moves[i];
				dir = i;
			}
		}
		sprintf(out, "%G", moves[4]); print("\nUtility:"); print(out);
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
	}else{
		//Hunting Pacman behaviour - run over as many Ghosts as possible
		//Could pay attention to CherryEaten to determine how much risk to take
		if(moves[0] == 1.0)
			moves[0] = utilityAttack(xIndex, yIndex - 1);
		if(moves[1] == 1.0)
			moves[1] = utilityAttack(xIndex, yIndex + 1);
		if(moves[2] == 1.0)
			moves[2] = utilityAttack(xIndex + 1, yIndex);
		if(moves[3] == 1.0)
			moves[3] = utilityAttack(xIndex - 1, yIndex);
		int dir = 0;
		moves[4] = moves[0];
		for(int i = 1; i < 4; i++)
		{
			if(moves[4] <= moves[i])
			{
				moves[4] = moves[i];
				dir = i;
			}
		}
		switch(dir)
		{
		case 0:
			print("Prediction: North\n");
			break;
		case 1:
			print("Prediction: South\n");
			break;
		case 2:
			print("Prediction: East\n");
			break;
		case 3:
			print("Prediction: West\n");
			break;
		default:
			break;
		}
	}
}

//Utility of one move under Pacman eating Ghosts condition.
double utilityAttack(int xPacAct, int yPacAct/*var gameState*/)
{
	double util = 0.0;
	//Util is: Sum(probabilityOf(utilityOf(allStatesPossibleAt(xPacAct, yPacAct))))
	//Random ghost move means we can probably cut out probabilityOf
	return util;
}

//Utility of one move under normal conditions.
double utilityNormal(int xPacAct, int yPacAct/*var gameState*/)
{
	double util = 0.0;
	//Util is: Sum(probabilityOf(utilityOf(allStatesPossibleAt(xPacAct, yPacAct))))
	//Random ghost move means we can probably cut out probabilityOf
	int totalPills = NumPills;

	for(int i = 0; i < 4; i++)
	{
		if(xPacAct == ghostIndices[i][0] && yPacAct == ghostIndices[i][1])
		{		
			util -= 500;
			break;
		}
		//Ghost will be within 2 moves - 0-1x, 0-1y
		if(abs(ghostIndices[i][0] - xPacAct) < 2 && abs(ghostIndices[i][1] - yPacAct) < 2)
		{
			util -= 100;
			break;
		}
	}

	if(PacMap[xPacAct][yPacAct] > 2) //If we're not moving into a wall
	{
		if (PacMap[xPacAct][yPacAct] == 4)
		{
			util += 5;
			totalPills--;
		}
		else if (PacMap[xPacAct][yPacAct] == 5)
		{
			GhostAttack = false; // ghosts now run away - or at least need to be programmed to

			util += 50;
			totalPills--;
		}
		else if (PacMap[xPacAct][yPacAct] == 3) 	
			//No immediate utility but teleporting may be the precursor to some plan
			util += 0;
	}else{
		util -= 2; //negative utility for pointlessly running into walls
	}

	//Extra utility for moving toward cherry if still uneaten...
/*	if(!CherryEaten)
	{
		getSpritePos(g_CherrySpritePos);
		int x1 = xIndex;
		int y1 = yIndex;
		getSpritePos(g_PacmanSpritePos);
		if(lineDist(x1, y1, xIndex, yIndex) > lineDist(xPacAct, yPacAct, xIndex, yIndex))
			//Closer to cherry
			util += 50;
		else
			util -= 50;
	}
*/
	return util;
}

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

	//Apply the Decision Theory formula
//	utilityOfPlan = Sum of : (the utilities of states possible after this action)x(the probabilities of those states occuring)

	return utilityOfPlan;
}

void probabilityOfState()
{
	//256 possible states given any distinct PacAction
	int sttSpc = 0;

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
		for(int p2 = 0; p2 < 2; p2++)
		{
			for(int p3 = 0; p3 < 256; p3++)
			{
				GhostMove[p1][p2][p3] = ghostIndices[p1][p2];
			}
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
			if( PacMap[ghostIndices[0][0]][ghostIndices[0][1] - 1 ] == 1 )
				fProbs[g1][g2][g3][g4] = 0;
			else
				GhostMove[0][1][count]--;
			break;
		case 1:
			if( PacMap[ghostIndices[0][0]][ghostIndices[0][1] + 1 ] == 1 )
				fProbs[g1][g2][g3][g4] = 0;
			else
				GhostMove[0][1][count]++;
			break;
		case 2:
			if( PacMap[ghostIndices[0][0] + 1 ][ghostIndices[0][1]] == 1 )
				fProbs[g1][g2][g3][g4] = 0;
			else
				GhostMove[0][0][count]++;
			break;
		case 3:
			if( PacMap[ghostIndices[0][0] - 1 ][ghostIndices[0][1]] == 1 )
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
			if( PacMap[ghostIndices[1][0]][ghostIndices[1][1] - 1 ] == 1 )
				fProbs[g1][g2][g3][g4] = 0;
			else
				GhostMove[1][1][count]--;
			break;
		case 1:
			if( PacMap[ghostIndices[1][0]][ghostIndices[1][1] + 1 ] == 1 )
				fProbs[g1][g2][g3][g4] = 0;
			else
				GhostMove[1][1][count]++;
			break;
		case 2:
			if( PacMap[ghostIndices[1][0] + 1 ][ghostIndices[1][1]] == 1 )
				fProbs[g1][g2][g3][g4] = 0;
			else
				GhostMove[1][0][count]++;
			break;
		case 3:
			if( PacMap[ghostIndices[1][0] - 1 ][ghostIndices[1][1]] == 1 )
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
			if( PacMap[ghostIndices[2][0]][ghostIndices[2][1] - 1 ] == 1 )
				fProbs[g1][g2][g3][g4] = 0;
			else
				GhostMove[2][1][count]--;
			break;
		case 1:
			if( PacMap[ghostIndices[2][0]][ghostIndices[2][1] + 1 ] == 1 )
				fProbs[g1][g2][g3][g4] = 0;
			else
				GhostMove[2][1][count]++;
			break;
		case 2:
			if( PacMap[ghostIndices[2][0] + 1 ][ghostIndices[2][1]] == 1 )
				fProbs[g1][g2][g3][g4] = 0;
			else
				GhostMove[2][0][count]++;
			break;
		case 3:
			if( PacMap[ghostIndices[2][0] - 1 ][ghostIndices[2][1]] == 1 )
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
			if( PacMap[ghostIndices[3][0]][ghostIndices[3][1] - 1 ] == 1 )
				fProbs[g1][g2][g3][g4] = 0;
			else
				GhostMove[3][1][count]--;
			break;
		case 1:
			if( PacMap[ghostIndices[3][0]][ghostIndices[3][1] + 1 ] == 1 )
				fProbs[g1][g2][g3][g4] = 0;
			else
				GhostMove[3][1][count]++;
			break;
		case 2:
			if( PacMap[ghostIndices[3][0] + 1 ][ghostIndices[3][1]] == 1 )
				fProbs[g1][g2][g3][g4] = 0;
			else
				GhostMove[3][0][count]++;
			break;
		case 3:
			if( PacMap[ghostIndices[3][0] - 1 ][ghostIndices[3][1]] == 1 )
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
						for(int g2 = 0; g2 < 4; g2++)
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
					}
				}
			}
		}
	}
}

//Functions that do things.
void a_Star(int x1, int y1, int x2, int y2)
{
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
	ghostIndices[0][0] = xIndex; ghostIndices[0][1] = yIndex;
	getSpritePos(g_SpeedySpritePos);
	ghostIndices[1][0] = xIndex; ghostIndices[1][1] = yIndex;
	getSpritePos(g_PokeySpritePos);
	ghostIndices[2][0] = xIndex; ghostIndices[2][1] = yIndex;
	getSpritePos(g_BashfulSpritePos);
	ghostIndices[3][0] = xIndex; ghostIndices[3][1] = yIndex;
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