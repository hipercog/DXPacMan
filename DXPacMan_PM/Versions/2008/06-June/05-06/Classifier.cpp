/**
 *	Author: Ben Cowley, 2008.
 *	Functions for CLASSIFYING players based on features from game log
 */
#include "universals.h"
#include "Classifier.h"
#include "Deriver.h"

bool ClassifyConqueror( deque<PacState> arg )
{
	bool conq = false;
	deque <float> result;
	deque<char*> feature;
	deque <float> temp;
	
/*	Log the individual results of each time a Pill is eaten - needs to be matched to code in Aggression4_ * /
	temp = Aggression4_HuntAfterPill( arg );
	result.push_back( temp[0] );
	feature.push_back( "Pill 1" );
	result.push_back( temp[1] );
	feature.push_back( "Pill 2" );
	result.push_back( temp[2] );
	feature.push_back( "Pill 3" );
	result.push_back( temp[3] );
	feature.push_back( "Pill 4" );
/**/
	result.push_back( Aggression4_HuntAfterPill( arg ) );
	feature.push_back( "A4_HuntAfterPill =" );

	result.push_back( Aggregate1_LivesSDev( arg ) );
	feature.push_back( "Lives_SDev =" );

	result.push_back( Simple2_LivesChng( arg )[0] );
	feature.push_back( "S2.a_LivesGained =" );

	result.push_back( Simple1_Levels( arg )[1] );
	feature.push_back( "S1.c_AvgCycles =" );

	result.push_back( Aggregate2_ChryBoolSum( arg ) );
	feature.push_back( "ChryBool_Sum =" );
	
	result.push_back( Caution4_CaughtAfterHunt( arg ) );
	feature.push_back( "C4_CaughtAfterHunt =" );

	result.push_back( Resource_TimeAndNumChry( arg )[0] );
	feature.push_back( "R1_AvgTimeToEatChry" );

	result.push_back( Decisive2_Osclltng( arg ) );
	feature.push_back( "D2_Osclltng =" );

	result.push_back( Caution5_MovesWithoutGain( arg ) );
	feature.push_back( "C5_MvsWoutGain =" );

	result.push_back( Simple4_TeleportUse( arg ) );
	feature.push_back( "S4_TeleportUse =" );

/*	This code logs out the entire list of cycles-per-state counts - needs to be matched to code in Aggregate3 * /
	logData += "Cycles = ,"
	temp = Aggregate3_CycleCountPerStateSDev( arg );
	for( int i = 1; i < temp.size(); i++ )
	{
		sprintf_s( out, 99, "%f", temp[i] );
		logData += out; logData += ",";
	}
	logData += "\n";	
/**/
	result.push_back( Aggregate3_CycleCountPerStateSDev( arg ) );
	feature.push_back( "CycleCountPerState_SDev =" );

	result.push_back( Planning1_LureToPill( arg )[0] );
	feature.push_back( "P1_LureToPill =" );

	for( int i = 0; i < feature.size(); i++ )
	{
		sprintf_s( out, 99, "%f", result[i] );
		logData += feature[i]; logData += ","; logData += out; logData += "\n";
	}

	//Build a sample rule tree from C&R
	if( result[0] > 2 )
		conq = true;
	else if( result[2] == 1 && result[1] < 0.609 )
		conq = true;

	return conq;
}

//End of file
