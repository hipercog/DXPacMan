/**	
 *	Author: Ben Cowley, 2006.
 *	Header file for the specific Decision Theory Player Modelling functions.
 */

#ifndef INC_DTPM_H
#define INC_DTPM_H

#include "universals.h"

#define NOMV	-1	//Value for entries in GhostMoves struct objects that are not valid moves

void forceDTPM( unsigned int lookahead );
void postDTPM( unsigned int lookahead );

/**
 *	Description: Holds 256 permutations of ghost movement, thus details of location with four ghosts and an x,y axis
 *	probs = num of viable permutations in this struct. Probability of any one permutation, given that all are equally likely due to ghosts random movement, is 1/probs
 */
struct GhostMoves
{
public:
	GhostMoves() {}

	int ghostXYMove[NUM_GS][2][POT_GS];
	short probs;

	bool operator==(GhostMoves gM)
	{
		if(probs != gM.probs)
			return false;
		else{
			for(int i0 = 0; i0 < NUM_GS; i0++)
			{
			for(int i1 = 0; i1 < 2; i1++)
			{
			for(int i2 = 0; i2 < POT_GS; i2++)
			{
				if(ghostXYMove[i0][i1][i2] != gM.ghostXYMove[i0][i1][i2])
					return false;
			}}}
		}
		return true;
	}

	void operator=(GhostMoves gM)
	{
		probs = gM.probs;
		for(int i0 = 0; i0 < NUM_GS; i0++)
		{
		for(int i1 = 0; i1 < 2; i1++)
		{
		for(int i2 = 0; i2 < POT_GS; i2++)
		{
			ghostXYMove[i0][i1][i2] = gM.ghostXYMove[i0][i1][i2];
		}}}
	}

	void size()
	{
		probs = 0;
		for(int i = 0; i < POT_GS; i++)
			if(ghostXYMove[0][0][i] > NOMV)
				probs++;
	}
};
/* End GhostMoves struct */

#endif /* INC_DTPM_H */