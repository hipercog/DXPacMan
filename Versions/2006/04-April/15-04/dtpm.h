/**	
 *	Author: Ben Cowley, 2006.
 *	Header file for the specific Decision Theory Player Modelling functions.
 */

#ifndef _DTPM_H
#define _DTPM_H

#include <stdio.h>
#include <stdlib.h>

/**
 *	Function prototypes
 */
double planOfActions(int timeHorizon);

void printToFile();
void print(char* string);
void logPos(D3DXVECTOR3 spritePos, char *name, char *endChar);

/**
 *	Decision Theory functions and fields for predicting player utility
 */
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

/**
 *	Logging functions and fields for outputting debug info (not exactly Stealth!)
 */
FILE *fileout = fopen("evalData/printout.txt", "wt");	//Open an output File object

void logPos(D3DXVECTOR3 spritePos, char *name, char *endChar)
{
		//Obtain x and y pos values from the PacMan sprite pointer
		FLOAT xPosInitial = spritePos.x;
		FLOAT yPosInitial = spritePos.y;

		//Adjust position values to obtain PacMap index values
		int xPosIndex = (int)((xPosInitial - GRIDTLX)/SPRITE_WIDTH);
		int yPosIndex = (int)((yPosInitial - GRIDTLY)/SPRITE_HEIGHT);

		char indexStr[199];
		char numBin[3];
		strcpy(indexStr, name);
		strcat(indexStr, itoa(xPosIndex, numBin, 10));
		strcat(indexStr, ", ");
		strcat(indexStr, itoa(yPosIndex, numBin, 10));
		strcat(indexStr, endChar);

		print(indexStr);
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