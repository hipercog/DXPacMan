/**
 *	Author: Ben Cowley, 2008.
 *	Header file for CLASSIFIER.cpp
 */

#ifndef INC_CLASSIFIER_H
#define INC_CLASSIFIER_H

void Classify();
float ClassifyConqueror_WindowAvg();
float ClassifyConqueror_Averages();
float ClassifyConqueror_Naive();
void GetFeatures( deque<PacState> arg );
void Record_Averages( int w );
void InitStrings();

#endif /* INC_CLASSIFIER_H */