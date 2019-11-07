/**
 *	Author: Ben Cowley, 2008.
 *	Header for Deriver.cpp;
 */
//#ifndef INC_DERIVER_H
//#define INC_DERIVER_H

#include "PacState.h"

extern int g_Levels;

#define ASTAR_BOUND					300		//Number of iterations of the while loop for path search

//Feature derivation functions - AGGREGATE
float Aggregate1_LivesSDev( deque<PacState> arg );
float Aggregate2_ChryBoolSum( deque<PacState> arg );
float Aggregate3_CycleCountPerStateSDev( deque<PacState> arg );

//Feature derivation functions - SIMPLE
deque<float> Simple1_Levels( deque<PacState> arg );
deque<float> Simple2_LivesChng( deque<PacState> arg );
float Simple4_TeleportUse( deque<PacState> arg );

//Feature derivation functions - AGGRESSION
float Aggression1_AtkGstHse( deque<PacState> arg );
float Aggression3_GhostKills( deque<PacState> arg );
float Aggression4_HuntAfterPill( deque<PacState> arg );
float Aggression6_Chase¬Dots( deque<PacState> arg );

//Feature derivation functions - PLANNING
deque<float> Planning1_LureToPill( deque<PacState> arg );
float Planning3_CntDtsB4Pll1( deque<PacState> arg );
deque<float> Planning4_SpeedOfHunt( deque<PacState> arg );
float Planning7_PutOffGstHse( deque<PacState> arg );

//Feature derivation functions - CAUTION
deque<float> Caution1_ThreatPrcptn( deque<PacState> arg );
float Caution2_AvgDstPacAtkGstCtrd( deque<PacState> arg );
float Caution3_CloseCalls( deque<PacState> arg );
float Caution4_CaughtAfterHunt( deque<PacState> arg );
float Caution5_MovesWithoutGain( deque<PacState> arg );
float Caution6_PntsOvrLives( deque<PacState> arg );
float Caution7_KilledAtHouse( deque<PacState> arg );

//Feature derivation functions - SPEED


//Feature derivation functions - DECISIVENESS
float Decisive2_Osclltng( deque<PacState> arg );
float Decisive1_CyclesPrMv( deque<PacState> arg );

//Feature derivation functions - RESOURCE
deque<float> Resource_TimeAndNumChry( deque<PacState> arg );

//Feature derivation functions - THOROUGHNESS

//Feature derivation functions - CONTROL SKILL

deque<PacState> BuildStatesDq( const char* filename );
deque<int> readFromCSVFile( const char * filename );

//Distance funtions
int a_Star(int src_x1, int src_y1, int trg_x2, int trg_y2, bool pacTravelling, PacState pS);
deque<int> GetClosestPill( PacState pS );
int localDots(int xCentre, int yCentre, int radius, PacState pS );
int manhattanDist(int x1, int y1, int x2, int y2);
double lineDist(int x1, int y1, int x2, int y2);
int myHeuristicDist(int x1, int y1, int x2, int y2, PacState pS);

// Return the index to the lowest value in a deque
template <class myType>
int lowest(deque<myType> deQ)
{
	int minIndex = 0;
	myType low = deQ[minIndex];

	for(unsigned int i = 1; i < deQ.size(); i++)
		if(deQ[i] < low)
		{
			low = deQ[i];
			minIndex = i;
		}
	return minIndex;
}

//Return the index to the highest value in a deque
template <class myType>
int highest(deque <myType> deQ)
{
	int maxIndex = 0;
	myType high = deQ[maxIndex];

	for(unsigned int i = 1; i < deQ.size(); i++)
		if(deQ[i] > high)
		{
			high = deQ[i];
			maxIndex = i;
		}
	return maxIndex;
}

// Return the index to the lowest value in an array
template <class myType>
int lowest(myType *aRay, int size)
{
	int minIndex = 0;
	myType low = aRay[minIndex];

	for(int i = 1; i < size; i++)
		if(aRay[i] < low)
		{
			low = aRay[i];
			minIndex = i;
		}
	return minIndex;
}

//Return the index to the highest value in an array
template <class myType>
int highest(myType *aRay, int size)
{
	int maxIndex = 0;
	myType high = aRay[maxIndex];

	for(int i = 1; i < size; i++)
		if(aRay[i] > high)
		{
			high = aRay[i];
			maxIndex = i;
		}
	return maxIndex;
}

//#endif /* INC_DERIVER_H */