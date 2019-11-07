/**	
 *	Author: Ben Cowley, 2006.
 *	Header file for the Decision Theory Player Modelling functions.
 */

#ifndef INC_DTPM_H
#define INC_DTPM_H

#include "universals.h"

void postDTPM( unsigned int lookahead );

/**
 *	Description: One Actor's potential moves in the next step
 *	Holds x,y values for any open adjacent squares, and p is the probability of moving to that square
 */
struct NextMoves
{
public:
	NextMoves() 
	{
		for( int xy = 0; xy < 2; xy++ )
			for( int mv = 0; mv < 4; mv++ )
				xyMove[xy][mv] = 0;
		prob = 0;
	}

	int xyMove[2][4];
	short prob;

	bool operator==(NextMoves nm)
	{
		if( prob != nm.prob )
			return false;

		for( int xy = 0; xy < 2; xy++ )
			for( int mv = 0; mv < 4; mv++ )
				if( xyMove[xy][mv] != nm.xyMove[xy][mv] )
					return false;
		return true;
	}

	void operator=(NextMoves nm)
	{
		prob = nm.prob;
		for( int xy = 0; xy < 2; xy++ )
			for( int mv = 0; mv < 4; mv++ )
				xyMove[xy][mv] = nm.xyMove[xy][mv];
	}
};

#endif /* INC_DTPM_H */