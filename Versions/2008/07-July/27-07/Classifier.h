/**
 *	Author: Ben Cowley, 2008.
 *	Header file for CLASSIFIER.cpp
 */

#ifndef INC_CLASSIFIER_H
#define INC_CLASSIFIER_H

void Classify();
float ClassifyConqueror_Level( deque<PacState> arg );
void GetFeatures( deque<PacState> arg );

#endif /* INC_CLASSIFIER_H */