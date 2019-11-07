/**
 *	Author: Ben Cowley, 2007.
 *	Header for Deriver.cpp; defines same PacState class as Pacman implementation
 */
#ifndef INC_DERIVER_H
#define INC_DERIVER_H

#include "stdafx.h"
#include "PacState.h"

#define ASTAR_BOUND					300		//Number of iterations of the while loop for path search

extern int g_Levels;

//Feature derivation functions - AGGRESSION
float Aggression1_AtkGstHse( deque<PacState> arg );
float Aggression2_ChryOrGsts( deque<PacState> arg );

//Feature derivation functions - PLANNING
deque<float> Planning1_LureToPill( deque<PacState> arg );
float Planning2_SavePill( deque<PacState> arg );
float Planning4_CntDtsB4Pll1( deque<PacState> arg );
deque<float> Planning5_SpeedOfHunt( deque<PacState> arg );
float Planning7_PutOffGstHse( deque<PacState> arg );

//Feature derivation functions - CAUTION
deque<float> Caution1_ThreatPrcptn( deque<PacState> arg );
deque<float> Caution2_AvgDstPacGst( deque<PacState> arg );
float Caution4_CaughtAfterHunt( deque<PacState> arg );
float Caution5_MovesWithoutGain( deque<PacState> arg );
float Caution6_PntsOvrLives( deque<PacState> arg );
float Caution7_KilledAtHouse( deque<PacState> arg );

//Feature derivation functions - SPEED
float Speed1_MvsToClrLvl( deque<PacState> arg );
float Speed2_CyclsToClrLvl( deque<PacState> arg );

//Feature derivation functions - DECISIVENESS
float Decisive1_CyclesPrMv( deque<PacState> arg );
float Decisive2_Osclltng( deque<PacState> arg );

//Feature derivation functions - RESOURCE
deque<float> Resource_TimeAndNumChry( deque<PacState> arg );

//Feature derivation functions - THOROUGHNESS
float Thorough1_Lvs1Dot( deque<PacState> arg );
float Thorough2_Hoover( deque<PacState> arg );

//Feature derivation functions - CONTROL SKILL
float Control1_Cycles2ClrSect( deque<PacState> arg );

//Feature derivation functions - SIMPLE
deque<float> Simple1_Levels( deque<PacState> arg );
deque<float> Simple2_LivesChng( deque<PacState> arg );
deque<PacState> Simple3_CntSqVsts( deque<PacState> arg );
float Simple4_TeleportUse( deque<PacState> arg );

//File reading functions 
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
#endif /* INC_DERIVER_H */
