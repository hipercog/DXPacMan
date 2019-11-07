/**	
 *	Author: Ben Cowley, 2006.
 *	Header file for the Decision Theory Player Modelling functions.
 */

#ifndef INC_DTPM_H
#define INC_DTPM_H

#include "universals.h"

#define NOMV	-1	//Value for entries in GhostMoves struct objects that are not valid moves

void forceDTPM( unsigned int lookahead );
void postDTPM( unsigned int lookahead );

/**
 *	Description: One Actor's potential moves in the next step
 *	Holds x,y values for any open adjacent squares, and p is the probability of moving to that square
 */
struct NextMoves
{
public:
	NextMoves() {}

	int xypMove[3][4];

	bool operator==(NextMoves nm)
	{
		for( int xyp = 0; xyp < 3; xyp++ )
			for( int mv = 0; mv < 4; mv++ )
				if( xypMove[xyp][mv] != nm.xypMove[xyp][mv] )
					return false;
		return true;
	}

	bool operator=(NextMoves nm)
	{
		for( int xyp = 0; xyp < 3; xyp++ )
			for( int mv = 0; mv < 4; mv++ )
				xypMove[xyp][mv] = nm.xypMove[xyp][mv];
	}
};

/**
 *	Description: Holds 256 permutations of ghost movement, thus details of location with four ghosts and an x,y axis
 *	perms = num of viable permutations in this struct. 
 *	If ghosts movement is random, Probability of any one permutation is 1/perms
 */
struct GhostMoves
{
public:
	GhostMoves() {}

	//Data struct holding - Ghosts[1-N] x (x,y,p) x Permutations
	int ghostXYMove[NUM_GS][3][POT_GS];
	short perms;

	bool operator==(GhostMoves gM)
	{
		if(perms != gM.perms)
			return false;
		else{
			for(int i0 = 0; i0 < NUM_GS; i0++)
			{
			for(int i1 = 0; i1 < 3; i1++)
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
		perms = gM.perms;
		for(int i0 = 0; i0 < NUM_GS; i0++)
		{
		for(int i1 = 0; i1 < 3; i1++)
		{
		for(int i2 = 0; i2 < POT_GS; i2++)
		{
			ghostXYMove[i0][i1][i2] = gM.ghostXYMove[i0][i1][i2];
		}}}
	}

	short size()
	{
		perms = 0;
		for(int i = 0; i < POT_GS; i++)
			if(ghostXYMove[0][0][i] > NOMV)
				perms++;
		return perms;
	}
};
/* End GhostMoves struct */

#endif /* INC_DTPM_H */