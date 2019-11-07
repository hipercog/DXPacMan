/**	
 *	Author: Ben Cowley, 2008.
 *	Header file for the utility-specific Decision Theory Player Modelling functions.
 */

#ifndef INC_UTILITY_H
#define INC_UTILITY_H

float utility( deque <PacState> States );
void learn( int t, float x );

#endif /* INC_UTILITY_H */