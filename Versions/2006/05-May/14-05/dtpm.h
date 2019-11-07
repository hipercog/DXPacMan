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
void probabilityOfGhostState(unsigned int recurseLimit, int *baseGhostX, int *baseGhostY);
double probabilityOfState(unsigned int recurseLimit, int *baseGhostX, int *baseGhostY, int trg_x, int trg_y, bool targeting/*[Ghost 1-4][x-y]*/);
double planOfActions(int timeHorizon);
void availableActions(int xPac, int yPac);
vector<double> utilityOfObjective(int src_x, int src_y, int trg_x, int trg_y);
double utilityRecursion(int src_x, int src_y, int trg_x, int trg_y, int count);
double utilityPointToPoint(int src_x, int src_y, int trg_x, int trg_y);

int a_Star(int src_x1, int src_y1, int trg_x2, int trg_y2);
void fillIndices();
void getSpritePos(D3DXVECTOR3 spritePos);
double lineDist(int x1, int y1, int x2, int y2);
int localDots(int x, int y, int radius);
int localGhosts(int x, int y, int radius);
int manhattanDist(int x1, int y1, int x2, int y2);
int myHeuristicDist(int x1, int y1, int x2, int y2);
template <class myType>
int lowest(myType *array, int size);
template <class myType>
int highest(myType *array, int size);

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

vector <int> aPathX;
vector <int> aPathY;

FILE *fileout = fopen("evalData/printout.txt", "wt");	//Open an output File object

/**
 *	Decision Theory functions and fields for predicting player utility
 */
//Going Java style! (We have an algorithm to run, need a clean place to run it)
void mainDTPM()
{
	print("\n\n");
/**/
	logPos(g_ShadowSpritePos, "ShadowXY: ", ";\t");
	logPos(g_SpeedySpritePos, "SpeedyXY: ", ";\t");
	logPos(g_PokeySpritePos, "PokeyXY: ", ";\t");
	logPos(g_BashfulSpritePos, "BashfulXY: ", ";\n");
/**/
	logPos(g_PacmanSpritePos, "PacmanXY: ", ";\n");

	init();
	availableActions(xIndex, yIndex);
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
	getSpritePos(g_PacmanSpritePos);
}
//Decision Theory logic: look at available actions, possible states under each action, and utility of each state.
//Actions available are decided by the current state. Future states depend on actions and ghost movement.

//Here we decide how many actions ahead to judge utility on, and what heuristics to use based on current state
void availableActions(int xPac, int yPac)
{
	double initVal[5] = {-1000.0, -1000.0, -1000.0, -1000.0, 0.0};
	vector<double> movUtil(initVal, initVal+5);
	int x1 = 9;
	int y1 = 9;
	int dst[4] = {0};
	
	//Here we posit and test some heuristics - start with most urgent, i.e. what would a good player prioritise? 
	//We can only look at one at a time, so this will be an if/else structure - could we use a more intricate decision logic?
	if(!GhostAttack)
	{
print("\nHUNTING GHOSTIES\n");
		//Hunt them ghosties down.
		for(int i = 0; i < 4; i++)
			dst[i] = myHeuristicDist(xPac, yPac, ghostIndicesX[i], ghostIndicesY[i]);
		int index = lowest(dst, 4);
		if(dst[index] < 8)
			movUtil = utilityOfObjective(xPac, yPac, ghostIndicesX[index], ghostIndicesY[index]);
		fflush(fileout);
	}else if(!CherryEaten)
	{
print("\nCHASING CHERRY\n");
		//We need to eat that cherry!
		getSpritePos(g_CherrySpritePos);
		x1 = xIndex;
		y1 = yIndex;
		if(myHeuristicDist(xPac, yPac, x1, y1) < 6)
			movUtil = utilityOfObjective(xPac, yPac, x1, y1);
		fflush(fileout);
	}
/**/	
	else if(localGhosts(xPac, yPac, 4) > 1)	// There are Ghosts close by - either get a PowerPill or run away.
	{
		//Where are the pills? Can we get one?
		int xPill = 18, yPill = 15;
		dst[0] = myHeuristicDist(xPac, yPac, 1, 3);
		dst[1] = myHeuristicDist(xPac, yPac, 18, 3);
		dst[2] = myHeuristicDist(xPac, yPac, 1, 15);
		dst[3] = myHeuristicDist(xPac, yPac, 18, 15);
		if(dst[0] < dst[1])
		{
			xPill = 1;
			if(dst[0] < dst[2])
				yPill = 3;
		}else
			if(dst[1] < dst[3])
				yPill = 3;
		if(myHeuristicDist(xPac, yPac, xPill, yPill) < 6)
		{
print("\nTAKING PILLS\n");
			movUtil = utilityOfObjective(xPac, yPac, xPill, yPill);
			fflush(fileout);
		}else{
print("\nRUNNING AWAY - NO PILLS\n");

		int o = 3;
		int val[8] = {0};
		int valX[8] = {xPac-o, xPac, xPac+o, xPac+o, xPac+o, xPac, xPac-o, xPac-o};
		int valY[8] = {yPac-o, yPac-o, yPac-o, yPac, yPac+o, yPac+o, yPac+o, yPac};

		for(int i = 0; i < 8; i++)
			val[i] = (localGhosts(valX[i], valY[i], 4) * 4);
		for(int j = 1; j < 9; j++)
			val[j%8] += ((val[(j-1)%8] / 4) + (val[(j+1)%8] / 4) - localDots(valX[j%8], valY[j%8], 2));

for(int k = 0; k < 8; k++)
{
	itoa(val[k], out, 10); print("\nOctant value:"); print(out);
	itoa(valX[k], out, 10); print("\t\tX:"); print(out);
	itoa(valY[k], out, 10); print("\tY:"); print(out);
}
		o = lowest(val, 8);
		while(ShadowMap[valX[o]][valY[o]] < 3)
		{
			if(valX[o] != xPac)
				valX[o] += (int)((xPac - valX[o]) / abs(xPac - valX[o]));
			if(ShadowMap[valX[o]][valY[o]] > 2)
				break;
			if(valY[o] != yPac)
				valY[o] += (int)((yPac - valY[o]) / abs(yPac - valY[o]));
			if(valX[o] == xPac && valY[o] == yPac)
			{
				print("\nIn if test XY == Pac XY");
				val[o] = 99;
				o = lowest(val, 8);
for(int k = 0; k < 8; k++)
{
	itoa(val[k], out, 10); print("\nOctant value:"); print(out);
	itoa(valX[k], out, 10); print("\t\tX:"); print(out);
	itoa(valY[k], out, 10); print("\tY:"); print(out);
}
			}
		}
		movUtil = utilityOfObjective(xPac, yPac, valX[o], valY[o]);
		fflush(fileout);

			//Get local ghost locus and run away from it?
/** /		for(int i = 0; i < 4; i++)
				dst[i] = myHeuristicDist(xPac, yPac, ghostIndicesX[i], ghostIndicesY[i]);
			x1 = 1;
			y1 = 1;
			if(ghostIndicesX[lowest(dst, 4)] > xPac)
				x1 = -1;
			if(ghostIndicesY[lowest(dst, 4)] > yPac)
				y1 = -1;
			int x = xPac, y = yPac;
			bool test = true;
			while(x != xPac+(4*x1) && test)
			{
				x += x1;
				y = yPac;
				while(y != yPac+(4*y1) && test)
				{
					y += y1;
					if(localGhosts(x, y, 4) == 0 && PacMap[x][y] > 2)
					{
						itoa(x, out, 10); print("x to test:"); print(out);
						itoa(y, out, 10); print("\ty to test:"); print(out); print("\n");
						test = false;
					}
				}
			}
			movUtil = utilityOfObjective(xPac, yPac, x, y);
			fflush(fileout);
/** /		for(int i = 0; i < 4; i++)
			{
				if(dst[i] < 4)
				{
					x1 = (int)(x1 + ghostIndicesX[i]) / 2;
					y1 = (int)(y1 + ghostIndicesY[i]) / 2;
				}
			}
/**/
		}
	}
/**/else{
print("\nDOT COLLECTOR\n");
		//With no pressing concerns, we should be eating dots.
		int o = 3;
		int val[8] = {0};
		int valX[8] = {xPac-o, xPac, xPac+o, xPac+o, xPac+o, xPac, xPac-o, xPac-o};
		int valY[8] = {yPac-o, yPac-o, yPac-o, yPac, yPac+o, yPac+o, yPac+o, yPac};
		for(int i = 0; i < 8; i++)
		{
			val[i] = localDots(valX[i], valY[i], 4) - (localGhosts(valX[i], valY[i], 4) * 10);
		}
		o = highest(val, 8);
		while(ShadowMap[valX[o]][valY[o]] < 3)
		{
			if(valX[o] != xPac)
				valX[o] += (int)((xPac - valX[o]) / abs(xPac - valX[o]));
			if(ShadowMap[valX[o]][valY[o]] > 2)
				break;
			if(valY[o] != yPac)
				valY[o] += (int)((yPac - valY[o]) / abs(yPac - valY[o]));
			if(valX[o] == xPac && valY[o] == yPac)
			{
				val[o] = 0;
				o = highest(val, 8);
			}
		}
		movUtil = utilityOfObjective(xPac, yPac, valX[o], valY[o]);
		fflush(fileout);
	}
/**/
//vector<double> result = utilityOfObjective(xPac, yPac, x1, y1);
//itoa((int)result[0], out, 10); print("\nDir@PacXY: "); print(out);
//sprintf(out, "%G", result[1]); print("\tUtil@PacmanXY:"); print(out);

sprintf(out, "%G", movUtil[0]); print("\nnorth:"); print(out);
sprintf(out, "%G", movUtil[1]); print("\tsouth:"); print(out);
sprintf(out, "%G", movUtil[2]); print("\teast:"); print(out);
sprintf(out, "%G", movUtil[3]); print("\twest:"); print(out);

	//Wherever we have final utils, put this
//	sprintf(out, "%G", movUtil[(int)movUtil[4]]); print("\nUtility:"); print(out);
	switch((int)movUtil[4])
	{
	case 0:
		print("\tPrediction: North\n");
		break;
	case 1:
		print("\tPrediction: South\n");
		break;
	case 2:
		print("\tPrediction: East\n");
		break;
	case 3:
		print("\tPrediction: West\n");
		break;
	default:
		break;
	}
}

/*
	//In what state will we be measuring Pacman's utility? 
	if(GhostAttack) //Normal Pacman behaviour - utility for eating dots, negative for losing life.
	{
		for(int i = 0; i < 4; i++)
		{
			if((xPac == ghostIndicesX[i] && (yPac == ghostIndicesY[i] + 1 || yPac == ghostIndicesY[i] - 1)) || 
				((xPac == ghostIndicesX[i] + 1 || xPac == ghostIndicesX[i] - 1) && yPac == ghostIndicesY[i]))
				//If there's a Ghost in the way of this direction...we can't go that way.
				movUtil[i] = -1000.0;
		}
	}
*/

//In calling function, create new vector and assign retval, then extract 1st element and cast to int, 2nd element == util;
vector<double> utilityOfObjective(int src_x, int src_y, int trg_x, int trg_y)
{
	double tmpArr[4] = {0.0, 0.0, 0.0, 0.0};
	vector<double> util(tmpArr, tmpArr+4);
	/** / 
	//At each stage of the path, we test if the ghosts can have reached that stage by the time Pacman has. If so we run the probs for that.
	//Ghosts being able to get Pacman is bad, unless his objective is a Pill.
	//We need a function like what it says below.
	for(int g = 0; g < 4; g++)
	{
		if(a_Star(ghostIndicesX[g], ghostIndicesY[g], aPathX[i], aPathY[i]) < i) //Ghost CAN reach
			util[0] += (appropriateWeight / probabilityOfStateOfOneGhostGettingSpecifiedLocation(ghostIndicesX[g], ghostIndicesY[g], aPathX[i], aPathY[i]));
	}

	//Test if the ghosts are further from the closest pill than Pacman - if so, don't want to go for it, area has low utility.
	if()
/**/
	//Checking: Up, Down, Right, Left / North, South, East, West / y-1, y+1, x+1, x-1 / 0, 1, 2, 3
	//We will get back (always at least)two - to four - valid directions, not blocked by walls
	if(ShadowMap[src_x][src_y - 1] > 2)
		tmpArr[0] = util[0] = utilityPointToPoint(src_x, src_y - 1, trg_x, trg_y);
	if(ShadowMap[src_x][src_y + 1] > 2)
		tmpArr[1] = util[1] = utilityPointToPoint(src_x, src_y + 1, trg_x, trg_y);
	if(ShadowMap[src_x + 1][src_y] > 2)
		tmpArr[2] = util[2] = utilityPointToPoint(src_x + 1, src_y, trg_x, trg_y);
	if(ShadowMap[src_x - 1][src_y] > 2)
		tmpArr[3] = util[3] = utilityPointToPoint(src_x - 1, src_y, trg_x, trg_y);

	util.push_back((double)highest(tmpArr, 4));
	
	return util;
}

//Utility of a path from src xy to trg xy
double utilityPointToPoint(int src_x, int src_y, int trg_x, int trg_y)
{
	double util = 0.0;
/**/
	int iter = a_Star(src_x, src_y, trg_x, trg_y);
	if(iter != 99)
	{
		vector<int> X = aPathX;
		vector<int> Y = aPathY;
		for(int i = 0; i < iter; i++)
		{
			if(ShadowMap[X[i]][Y[i]] == 4)
				util += 0.8;
			if(ShadowMap[X[i]][Y[i]] == 5)
				util += 5;
/**/
			int g = 0;
			while(g < 4)
			{
				if(myHeuristicDist(ghostIndicesX[g], ghostIndicesY[g], X[i], Y[i]) < i)
				{
print("\nLogging ghost probOfState measure @ "); itoa(i, out, 10); print(out);
					util -= probabilityOfState(i, ghostIndicesX, ghostIndicesY, X[i], Y[i], true);
					g = 4;
				}
				g++;
			}
/**/
		}
		util += 10 - X.size();
	}
/**/
	return util;
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
double utilityOfGhostState(int xPacAct, int yPacAct, int *ghostsX, int *ghostsY)
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
	double util = 0.1;
/**/
	//Util is: Sum(probabilityOf(utilityOf(allStatesPossibleAt(xPacAct, yPacAct))))
	//Random ghost move means we can probably cut out probabilityOf
	int totalPills = NumPills;

	//Extra utility for moving toward cherry if still uneaten...
	if(!CherryEaten)
	{
		getSpritePos(g_CherrySpritePos);
		int x1 = xIndex;
		int y1 = yIndex;
		getSpritePos(g_PacmanSpritePos);
		vector<double> result = utilityOfObjective(xIndex, yIndex, x1, y1);
		util = result[1];
	}

	//Big utility change if ghosts are near
	if(GhostAttack) util = -0.1;
	for(int i = 0; i < 4; i++)
	{
		if(xPacAct == ghostIndicesX[i] && yPacAct == ghostIndicesY[i])
		{		
			util *= 500;
			break;
		}
		//Ghost will be within 2 moves - 0-1x, 0-1y
		if(abs(ghostIndicesX[i] - xPacAct) < 2 && abs(ghostIndicesY[i] - yPacAct) < 2)
		{
			util *= 100;
			break;
		}
	}

	if(ShadowMap[xPacAct][yPacAct] == 4)
	{
		util += 5;
		totalPills--;
	}
	else if(ShadowMap[xPacAct][yPacAct] == 5)
	{
//		GhostAttack = false; // ghosts now run away - or at least need to be programmed to
		//The eager Pacman will get a number of points equal to number of Ghosts in vicinity * 100
		util += localGhosts(xPacAct, yPacAct, 8)*100;

		util += 50;
		totalPills--;
	}
	else if(ShadowMap[xPacAct][yPacAct] == 3)
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
/**/	
	return util;
}

//Not working for large distances...call with, for instance, this line:
//	sprintf(out, "%G", utilityRecursion(xIndex, yIndex, 11, 11, a_Star(xIndex, yIndex, 11, 11))); print("Utility from Pacman to 14, 11: "); print(out); print("\n");
double utilityRecursion(int src_x, int src_y, int trg_x, int trg_y, int count)
{
	if((src_x == trg_x && src_y == trg_y) || count == 0)
		return 0.0;

	double movUtil[4] = {0.0, 0.0, 0.0, 0.0};
	vector<double> result(2);

	//Checking: Up, Down, Right, Left / North, South, East, West / y-1, y+1, x+1, x-1 / 0, 1, 2, 3
	//We will get back (always at least)two to four valid directions, not blocked by walls
	if(ShadowMap[src_x][src_y-1] > 2)
	{
		result = utilityOfObjective(src_x, src_y - 1, trg_x, trg_y);
		movUtil[0] = result[1];
		if(result[0] != 1)
			movUtil[0] += utilityRecursion(src_x, src_y - 1, trg_x, trg_y, count--);
	}
	if(ShadowMap[src_x][src_y+1] > 2)
	{
		result = utilityOfObjective(src_x, src_y + 1, trg_x, trg_y);
		movUtil[1] = result[1];
		if(result[0] != 0)
			movUtil[1] += utilityRecursion(src_x, src_y + 1, trg_x, trg_y, count--);
	}
	if(ShadowMap[src_x+1][src_y] > 2)
	{
		result = utilityOfObjective(src_x + 1, src_y, trg_x, trg_y);
		movUtil[2] = result[1];
		if(result[0] != 3)
			movUtil[2] += utilityRecursion(src_x + 1, src_y, trg_x, trg_y, count--);
	}
	if(ShadowMap[src_x-1][src_y] > 2)
	{
		result = utilityOfObjective(src_x - 1, src_y, trg_x, trg_y);
		movUtil[3] = result[1];
		if(result[0] != 2)
			movUtil[3] += utilityRecursion(src_x - 1, src_y, trg_x, trg_y, count--);
	}

	double tmpArr[4] = {movUtil[0], movUtil[1], movUtil[2], movUtil[3]};
	sort(tmpArr, tmpArr+4);

	return tmpArr[3];
}

//Discover all possible combinations of ghost movement, and the corresponding probability of each
//Passing all data structs in to enable recursion - array sizes are set, no need to pass.
//When targeting, use recurseLimit to bound how long ghosts have to get to target.
double probabilityOfState(unsigned int recurseLimit, int *baseGhostX, int *baseGhostY, int trg_x, int trg_y, bool targeting/*[Ghost 1-4][x-y]*/)
{
	//256 possible states given any distinct PacAction
	double sttSpc = 0;
	float fProbs[4][4][4][4]; 
	int GhostMove[4][2][256];

	int targetCount = 0, targetCounter = 0;
/** ///If recursing, we have decremented recurseLimit, this code might not work. Do we need targetCount anyway?
	//Might actually work to reduce computation - cut away ghost states moving away from target...but will that lose necessary probabilities?
	if(targeting)
	{
		for(int i = 0; i < 4; i++)
		{
			if(a_Star(baseGhostX[i], baseGhostY[i], trg_x, trg_y) < recurseLimit)
				targetCount++;
		}
	}
/**/
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
/** /
	print("\n#P(S): 1/"); itoa(sttSpc, out, 10); print(out);
	print("\t3:0, 4:0\t\t\t\t3:0, 4:1\t\t\t\t3:0, 4:2\t\t\t\t3:0, 4:3\t\t\t\t3:1, 4:0\t\t\t\t3:1, 4:1\t\t\t\t3:1, 4:2\t\t\t\t3:1, 4:3\t\t\t\t3:2, 4:0\t\t\t\t3:2, 4:1\t\t\t\t3:2, 4:2\t\t\t\t3:2, 4:3\t\t\t\t3:3, 4:0\t\t\t\t3:3, 4:1\t\t\t\t3:3, 4:2\t\t\t\t3:3, 4:3\n");
	print("-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------");
/**/
	count = -1;
	for(int p1 = 0; p1 < 4; p1++)
	{
		for(int p2 = 0; p2 < 4; p2++)
		{
//			print("\n0:"); itoa(p1, out, 10); print(out);
//			print(", 1:"); itoa(p2, out, 10); print(out); print("|\t");
			for(int p3 = 0; p3 < 4; p3++)
			{
				for(int p4 = 0; p4 < 4; p4++)
				{
					count++;
					if(fProbs[p1][p2][p3][p4] != 0)
//						print("0\t\t\t\t\t");
//					else
					{
/**/					for(int g2 = 0; g2 < 4; g2++)
						{
/** /						print("G"); itoa(g2, out, 10); print(out); print(":");
							for(int g3 = 0; g3 < 2; g3++)
							{
								itoa(GhostMove[g2][g3][count], out, 10); 
								if(GhostMove[g2][g3][count] < 10) print(" ");
								print(out); print(",");
							}
/**/						if(targeting && (GhostMove[g2][0][count] == trg_x && GhostMove[g2][1][count] == trg_y))
								targeting = false;
						}
//						print("\t");
						//If we want to go through the Ghost states produced, branching for new states at each one, this is how...
/**/					if(recurseLimit > 0 && targeting)
						{
							int ghostBranchX[4] = {GhostMove[0][0][count], GhostMove[1][0][count], GhostMove[2][0][count], GhostMove[3][0][count]};
							int ghostBranchY[4] = {GhostMove[0][1][count], GhostMove[1][1][count], GhostMove[2][1][count], GhostMove[3][1][count]};
							sttSpc += probabilityOfState(--recurseLimit, ghostBranchX, ghostBranchY, trg_x, trg_y, targeting);
						}
/**/				}
				}
			}
		}
	}
	return 1/sttSpc;
}

/**
 *	Functions that do things.
 */

//Implement the A* pathfinding algorithm - returns number of moves in path. 
int a_Star(int src_x1, int src_y1, int trg_x2, int trg_y2)
{
	if((src_x1 == trg_x2 && src_y1 == trg_y2) || PacMap[src_x1][src_y1] <= 2 || PacMap[trg_x2][trg_y2] <= 2)
		return 99;

	//Alter some data for local use
	int temp1 = ShadowMap[4][9]; ShadowMap[4][9] = 2;
	int temp2 = ShadowMap[15][9]; ShadowMap[15][9] = 2;
//	ShadowMap[9][8] = ShadowMap[9][9] = ShadowMap[10][8] = ShadowMap[10][9] = 4;
	int x1 = src_x1, y1 = src_y1;
	aPathX.clear();
	aPathY.clear();

	//List of directions, starting from x1, y1
	int path[87];
	int closedNodesX[87] = {0};
	int closedNodesY[87] = {0};
	int fScoresHist[87];
	int iter = 0;
	int openNodesX[4] = {-1, -1, -1, -1};	//four directions X
	int openNodesY[4] = {-1, -1, -1, -1};	//four directions Y
	int openNodesH[4] = {0, 0, 0, 0};	//four heuristics to the target (myHeuristicDist function)
	int openNodesF[4];	//four Final scores = G + H
	int closedDir = -1;
	int G = 1, F = 0, H = 0, currNode = 0;

	//Check our path and update path[]
	while(iter < 87)
	{

		openNodesF[0] = 99, openNodesF[1] = 99, openNodesF[2] = 99, openNodesF[3] = 99;
		//Seal off the way we came from investigation.
		closedNodesX[iter] = x1;
		closedNodesY[iter] = y1;

		//Find available directions and calculate pathfinding indices for each dir
		if(ShadowMap[x1][y1-1] > 2 && closedDir != 0)
		{
			openNodesX[0] = x1; openNodesY[0] = y1-1;
			openNodesH[0] = myHeuristicDist(x1, y1-1, trg_x2, trg_y2); 
			openNodesF[0] = G + openNodesH[0];
		}
		if(ShadowMap[x1][y1+1] > 2 && closedDir != 1)
		{
			openNodesX[1] = x1; openNodesY[1] = y1+1;  
			openNodesH[1] = myHeuristicDist(x1, y1+1, trg_x2, trg_y2); 
			openNodesF[1] = G + openNodesH[1];
		}
		if(ShadowMap[x1+1][y1] > 2 && closedDir != 2)
		{
			openNodesX[2] = x1+1; openNodesY[2] = y1;  
			openNodesH[2] = myHeuristicDist(x1+1, y1, trg_x2, trg_y2); 
			openNodesF[2] = G + openNodesH[2];
		}
		if(ShadowMap[x1-1][y1] > 2 && closedDir != 3)
		{
			openNodesX[3] = x1-1; openNodesY[3] = y1; 
			openNodesH[3] = myHeuristicDist(x1-1, y1, trg_x2, trg_y2); 
			openNodesF[3] = G + openNodesH[3];
		}
/** /
		//Sort into increasing order, then compare first two F's: if the same, then need further investigation on each to pick shortest
		int tmpArrF[4] = {openNodesF[0], openNodesF[1], openNodesF[2], openNodesF[3]};
		sort(tmpArrF, tmpArrF+4);
		if(tmpArrF[0] == tmpArrF[1]){}

		//Pick the dir with smallest F value - i.e. shortest path
/**/ 	int dir = 3; closedDir = 2;
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

/**/		//Implement backtracking capability
		for(int i = 0; i < iter-1; i++) //Test up until current - 1 : to prevent oscillation
		{
			if(fScoresHist[i] < openNodesF[dir])
			{
				//Set the current node back to the 1st one with lower F score
				//Test if any nodes are available - not blocked by walls, not on closedList
				if(ShadowMap[closedNodesX[i]][closedNodesY[i] - 1] > 2 && ((closedNodesX[i] != closedNodesX[i-1] && closedNodesY[i] - 1 != closedNodesY[i-1]) || (closedNodesX[i] != closedNodesX[i+1] && closedNodesY[i] - 1 != closedNodesY[i+1])))
				{
					x1 = closedNodesX[i];
					y1 = closedNodesY[i] - 1;
					G = iter = i; G++;
					closedDir = 1;
					break;
				}
				if(ShadowMap[closedNodesX[i]][closedNodesY[i] + 1] > 2 && ((closedNodesX[i] != closedNodesX[i-1] && closedNodesY[i] + 1 != closedNodesY[i-1]) || (closedNodesX[i] != closedNodesX[i+1] && closedNodesY[i] + 1 != closedNodesY[i+1])))
				{
					x1 = closedNodesX[i];
					y1 = closedNodesY[i] + 1;
					G = iter = i; G++;
					closedDir = 0;
					break;
				}
				if(ShadowMap[closedNodesX[i] + 1][closedNodesY[i]] > 2 && ((closedNodesX[i] + 1 != closedNodesX[i-1] && closedNodesY[i] != closedNodesY[i-1]) || (closedNodesX[i] + 1 != closedNodesX[i+1] && closedNodesY[i] != closedNodesY[i+1])))
				{
					x1 = closedNodesX[i] + 1;
					y1 = closedNodesY[i];
					G = iter = i; G++;
					closedDir = 3;
					break;
				}
				if(ShadowMap[closedNodesX[i] - 1][closedNodesY[i]] > 2 && ((closedNodesX[i] - 1 != closedNodesX[i-1] && closedNodesY[i] != closedNodesY[i-1]) || (closedNodesX[i] - 1 != closedNodesX[i+1] && closedNodesY[i] != closedNodesY[i+1])))
				{
					x1 = closedNodesX[i] - 1;
					y1 = closedNodesY[i];
					G = iter = i; G++;
					closedDir = 2;
					break;
				}
				//Else -> Continue looking for lower F scores until i = iter+1
			}
		}
/**/		//Record the chosen node's F score
		fScoresHist[iter] = openNodesF[dir];
		path[iter] = dir;
		iter++;
		G++;

		if(x1 == trg_x2 && y1 == trg_y2)
			break;
	}

	//Cut out any loops that have appeared in the closedNodes arrays.
/**/	for(int pt = 0; pt < iter-1; pt++)
		{
			for(int luk = pt+1; luk < iter; luk++)
			{
				if(closedNodesX[pt] == closedNodesX[luk] && closedNodesY[pt] == closedNodesY[luk])
				{
					int t = pt;
					int l = luk;
					while( l < iter )
					{
						closedNodesX[t] = closedNodesX[l];
						closedNodesY[t] = closedNodesY[l];
						t++; l++;
					}
					iter -= (luk - pt);
					break;
				}
			}
		}

	print("\nSource: ");
	itoa(src_x1, out, 10); print(out); print(", ");
	itoa(src_y1, out, 10); print(out); print("\n");
/**/
	for(int i = 0; i < iter; i++)
	{
		print("Iter: ");
		itoa(i, out, 10); print(out); print(";\t");
		itoa(closedNodesX[i], out, 10); print(out); print(", ");
		itoa(closedNodesY[i], out, 10); print(out); print(";\t");
		itoa(path[i], out, 10); print(out); print("\n");
	}
/**/
	print("Target: ");
	itoa(trg_x2, out, 10); print(out); print(", ");
	itoa(trg_y2, out, 10); print(out);

	for(int i = 0; i < iter; i++)
	{
		aPathX.push_back(closedNodesX[i]);
		aPathY.push_back(closedNodesY[i]);
	}
	//Readjust Map
	ShadowMap[4][9] = temp1; ShadowMap[15][9] = temp2;
//	ShadowMap[9][8] = ShadowMap[9][9] = ShadowMap[10][8] = ShadowMap[10][9] = 2;
	return iter;
}

int manhattanDist(int x1, int y1, int x2, int y2)
{
	return abs(x1 - x2) + abs(y1 - y2);
}

int myHeuristicDist(int x1, int y1, int x2, int y2)
{
	int count[4] = {0};
	int opx = 1, opy = 1;
	if(x1 > x2)	opx = -1;
	if(y1 > y2)	opy = -1;
	int x = 0, y = 0;
	
	for(x = x1; x != x2; x+=opx)
	{
		if(PacMap[x][y1] < 3)
			count[0]++;
	}
	for(y = y1; y != y2; y+=opy)
	{
		if(PacMap[x1][y] < 3)
			count[0]++;
	}

	for(y = y1; y != y2; y+=opy)
	{
		if(PacMap[x1][y] < 3)
			count[1]++;
	}
	for(x = x1; x != x2; x+=opx)
	{
		if(PacMap[x][y1] < 3)
			count[1]++;
	}
/**/
	if(abs(x1 - x2) > 1 && abs(y1 - y2) > 1)
	{
		x = x1;
		y = y1;
		//Step once in x, then in y, etc. Count PacMap 1's and alternate
		while(x != x2 && y != y2)
		{
			if(PacMap[x][y] < 3)
				count[2]++;
			x += opx;
			if(PacMap[x][y] < 3)
				count[2]++;
			y += opy;
		}
		if(x == x2)
		{
			while(y != y2)
			{
				if(PacMap[x][y] < 3)
					count[2]++;
				y += opy;
			}
		}else if(y == y2)
		{
			while(x != x2)
			{
				if(PacMap[x][y] < 3)
					count[2]++;
				x += opx;
			}
		}
		//Alternating
		x = x1;
		y = y1;
		while(x != x2 && y != y2)
		{
			if(PacMap[x][y] < 3)
				count[3]++;
			y += opy;
			if(PacMap[x][y] < 3)
				count[3]++;
			x += opx;
		}
		if(x == x2)
		{
			while(y != y2)
			{
				if(PacMap[x][y] < 3)
					count[3]++;
				y += opy;
			}
		}else if(y == y2)
		{
			while(x != x2)
			{
				if(PacMap[x][y] < 3)
					count[3]++;
				x += opx;
			}
		}
	}
/**/
	sort(count, count+4);

	return abs(x1 - x2) + abs(y1 - y2) + count[0];
}

int localDots(int xCentre, int yCentre, int radius)
{
	int dots = 0;
	
	int x = 0, y = 0;
	if(xCentre - radius > 0)
		x = xCentre - radius;
	if(yCentre - radius > 0)
		y = yCentre - radius;

	if(xCentre + radius > 19)
		xCentre = 19;
	if(yCentre + radius > 19)
		yCentre = 19;
	
	while(x <= xCentre)
	{
		x++;
		while(y <= yCentre)
		{
			y++;
			if(ShadowMap[x][y] == 4)
				dots++;
		}
	}
	return dots;
}

int localGhosts(int x, int y, int radius)
{
	int ghosts = 0;
	if(x < 0)
		x = 0;
	else if(x > 19)
		x = 20;
	if(y < 0)
		y = 0;
	else if(y > 19)
		y = 19;
	for(int i = 0; i < 4; i++)
		if(myHeuristicDist(ghostIndicesX[i], ghostIndicesY[i], x, y) <= radius)
			ghosts++;
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

/** /
	//Test unit for array indexing functions
	double test[25] = {0};
	for(int i = 0; i < 25; i++)
	{
		test[i] = rand();
		print("\ntest["); itoa(i, out, 10); print(out); print("]: "); 
		sprintf(out, "%G", test[i]); print(out);
	}
	itoa(lowest(test, 25), out, 10); print("\nTesting lowest: "); print(out); print("\n");
/**/

//Return the index to the lowest value in an array
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

/**
 *	Logging functions and fields for outputting debug info (not exactly Stealth!)
 */

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