/**
 *	Author: Ben Cowley, 2008.
 *	Functions for CLASSIFYING players based on features from game log
 */
#include "Classifier.h"
#include "Deriver.h"

bool ClassifyConqueror( deque<PacState> arg )
{
	bool conq = false;
	deque <float> result;
	deque<char*> feature;
	deque <float> temp;
	
	result.push_back( Aggression4_HuntAfterPill( arg ) );
	feature.push_back( "A4_HuntAfterPill = " );

	result.push_back( Aggregate1_LivesSDev( arg ) );
	feature.push_back( "Lives_SDev = " );

	result.push_back( Simple2_LivesChng( arg )[0] );
	feature.push_back( "S2.a_LivesGained = " );

	result.push_back( Simple1_Levels( arg )[1] );
	feature.push_back( "S1.c_AvgCycles = " );

	result.push_back( Aggregate2_ChryBoolSum( arg ) );
	feature.push_back( "ChryBool_Sum = " );
	
	result.push_back( Caution4_CaughtAfterHunt( arg ) );
	feature.push_back( "C4_CaughtAfterHunt = " );

//	result.push_back( Resource_TimeAndNumChry( arg )[0] );
//	feature.push_back( "R1_AvgTimeToEatChry" );

	result.push_back( Decisive2_Osclltng( arg ) );
	feature.push_back( "D2_Osclltng = " );

	result.push_back( Caution5_MovesWithoutGain( arg ) );
	feature.push_back( "C5_MvsWoutGain = " );

	result.push_back( Simple4_TeleportUse( arg ) );
	feature.push_back( "S4_TeleportUse = " );

	result.push_back( Aggregate3_CycleCountPerStateSDev( arg ) );
	feature.push_back( "CycleCountPerState_SDev = " );

	for( int i = 0; i < feature.size(); i++ )
	{
		sprintf_s( out, 99, "%f", result[i] );
		logData += feature[i]; logData += ","; logData += out; logData += "\n";
	}

	for( int i = 1; i < result.size(); i++ )
	{
		result[0] += result[i];
	}
	//Set a threshold value over which summed features need to climb to classify Conqueror.
	if( result[0] > 0 )
		conq = true;

	return conq;
}

//End of file
