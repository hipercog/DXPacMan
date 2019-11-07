/**	
 *	Author: Ben Cowley, 2008.
 *	Header file for the utility-specific Decision Theory Player Modelling functions.
 */

#ifndef INC_UTILITY_H
#define INC_UTILITY_H

deque <float> utility( deque <PacState> States );
void hold();
void rollback();
void learn( int m, float t );

#endif /* INC_UTILITY_H */