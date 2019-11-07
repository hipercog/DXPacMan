/**
 *	Author: Ben Cowley, 2008.
 *	Header file for CLASSIFIER.cpp
 */

#ifndef INC_CLASSIFIER_H
#define INC_CLASSIFIER_H

void Classify( int lvl, char* path );
void OneRuleClassify( int lvl );
float ClassifyConqueror_WindowAvg( int lvl );
float OneRuleClassify_WindowAvg( int lvl );
float ClassifyConqueror_Averages( int lvl );
float OneRuleClassify_Averages( int lvl );
float ClassifyConqueror_Naive( int lvl );
float OneRuleClassify_Naive( int lvl );
void GetFeatures( deque<PacState> arg, int lvl );
void Record_Wndw_Avgs( int w, int lvl  );
void InitStrings();

#endif /* INC_CLASSIFIER_H */