/**
 *	Author: Ben Cowley, 2008.
 *	Functions for CLASSIFYING players based on features from game log
 */
#include "Classifier.h"
#include "Deriver.h"

bool ClassifyConqueror( deque<PacState> arg )
{
	bool conq = false;
	float huntAfterPill = Aggression4_HuntAfterPill( arg );
	if( huntAfterPill > 2 )
		conq = true;

	return conq;
}

//End of file
