/**
 *	Author: Ben Cowley, 2008.
 *	Header file for CLASSIFIER.cpp
 */

#ifndef INC_CLASSIFIER_H
#define INC_CLASSIFIER_H

void Classify();
float ClassifyConqueror_Level();
void GetFeatures( deque<PacState> arg );
void InitStrings();

#endif /* INC_CLASSIFIER_H */