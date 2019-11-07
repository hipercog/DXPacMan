/**
 *	Author: Ben Cowley, 2008.
 *	Functions for CLASSIFYING players based on features from game log
 */
#include "universals.h"
#include "Classifier.h"
#include "Deriver.h"

deque <float> result;
deque<char*> feature;

/**
 *	Description: Log results of feature derivers and classifiers to a log file
 */
void Classify()
{
	float conq = ClassifyConqueror_Level( GameStates );
	sprintf_s(out, 99, "%f", conq);
	if( conq >= 0 )
		logData += "Conqueror,";
	else
		logData += "Not_Conqueror,";
	logData += out;

	/****************  THESE LINES NEED TO BE INSERTED AFTER ALL CLASSIFICATION, BETWEEN EVERY LEVEL *******************/
	result.clear();
	feature.clear();

	sprintf_s(out, 99, "%d", Level);
	string file = "/Classify-";
	file += out;
	file += ".csv";

	string temp = gameDir;
	temp += file;
	file = "C:/PacPlayd/Logs/";
	file += temp;
	if( fopen_s( &stateFile, file.c_str(), "wt" ) != 0 )
		cout << "The file " << gameDir.c_str() << " was not opened." << endl;
	else
		//Put the latest game state data onto the output file stream
		fputs(logData.c_str(), stateFile);

	logData.clear();
}

/**
 *	Description: Append all the results of feature derivation to the universal string: logData
 */
void GetFeatures( deque<PacState> arg )
{
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
	deque <float> temp;
																	//	#	Actual		| Average		| Normals
	result.push_back( Aggression4_HuntAfterPill( arg ) );			// .00	0 - 4		| 0.125 - 1.5	| 0.083 - 1
	feature.push_back( "A4_HuntAfterPill" );

	result.push_back( Aggregate1_LivesSDev( arg ) );				// .01	0.38 - 2.54	|				| 0.148 - 1
	feature.push_back( "Lives_SDev" );

	result.push_back( Simple2_LivesChng( arg )[0] );				// .02	0 - 16		| 0 - 1			| 0 - 1
	feature.push_back( "S2.a_LivesGained" );

	result.push_back( Simple1_Levels( arg )[1] );					// .03	2174 - 9598	|				| 0.226 - 1
	feature.push_back( "S1.c_AvgCycles" );

	result.push_back( Aggregate2_ChryBoolSum( arg ) );				// .04	25 - 2048	| 7.1 - 165.2	| 0.043 - 1
	feature.push_back( "ChryBool_Sum" );
	
	result.push_back( Caution4_CaughtAfterHunt( arg ) );			// .05	0 - 9		| 0 - 1.25		| 0 - 1
	feature.push_back( "C4_CaughtAfterHunt" );

	result.push_back( Resource_TimeAndNumChry( arg )[0] );			// .06	0 - 142		|				| 0 - 1
	feature.push_back( "R1_AvgTimeToEatChry" );

	result.push_back( Decisive2_Osclltng( arg ) );					// .07	124 - 2241	| 62 - 181.5	| 0.342 - 1
	feature.push_back( "D2_Osclltng" );

	result.push_back( Caution5_MovesWithoutGain( arg ) );			// .08	118 - 2404	| 57.5 - 256.5	| 0.224 - 1
	feature.push_back( "C5_MvsWoutGain" );

	result.push_back( Simple4_TeleportUse( arg ) );					// .09	0 - 31		| 0 - 3			| 0 - 1
	feature.push_back( "S4_TeleportUse" );

	result.push_back( Aggregate3_CycleCountPerStateSDev( arg ) );	// .10	6.9 - 57.3	|				| 0.12 - 1
	feature.push_back( "CycleCountPerState_SDev" );

	result.push_back( Aggression6_Chase¬Dots( arg ) );				// .11	1 - 118		| 0.25 - 11.67	| 0.021 - 1 
	feature.push_back( "A6_Chase¬Dots" );

	result.push_back( Planning1_LureToPill( arg )[0] );				// .12	0 - 24		| 0 - 2.67		| 0 - 1
	feature.push_back( "P1_LureToPill" );

	result.push_back( Thorough2_Hoover( arg ) );					// .13	1 - 47		| 0.07 - 5.25	| 0.013 - 1
	feature.push_back( "T2_HooversDots" );

	result.push_back( Simple1_Levels( arg )[0] );					// .14	220 - 478	|				| 0.459 - 1
	feature.push_back( "S1.c_AvgStates" );

	result.push_back( Planning7_PutOffGstHse( arg ) );				// .15	0 - 5		| 0 - 0.571		| 0 - 1
	feature.push_back( "P7_PutOffGstHse" );

	result.push_back( Caution7_KilledAtHouse( arg ) );				// .16	0 - 5		| 0 - 1.5		| 0 - 1
	feature.push_back( "C7_KilledAtHouse" );

	result.push_back( Caution1_ThreatPrcptn( arg )[0] );			// .17	0 - 6		| 				| 0 - 1
	feature.push_back( "C1.b_ThreatPerception#Killed" );
/*	This code logs out the entire list of cycles-per-state counts - needs to be matched to code in Aggregate3 * /
	logData += "Cycles = ,"
	temp = Aggregate3_CycleCountPerStateSDev( arg );
	for( int i = 1; i < temp.size(); i++ ){
		sprintf_s( out, 99, "%f", temp[i] );
		logData += out; logData += ",";
	}
	logData += "\n";	
/**/
	for( int i = 0; i < feature.size(); i++ )
	{
		sprintf_s( out, 99, "%f", result[i] );
		logData += feature[i]; logData += ","; logData += out; logData += "\n";
	}
}

/**
 *	Description: Implement rules from Decision Trees to classify player as a Conqueror or not.
 */
float ClassifyConqueror_Level( deque<PacState> arg )
{
	float conq = 0.0;	//As a float the modifier applied by each rule can be weighted by reasonable, fractional, amounts - e.g. +0.5, +1.2, etc

	/***Build a sample rule tree from C&R 7x on ConqNot, 61:39, d4, i0.4								********
	 *	Notes: These rules are being interpreted to work for a single level. Thus each threshold number 
	 *		from a rule must be mapped to a sensible number to be derived from a single level...
	 */
	/**
	 *	Conqueror rules for replication 1:
	 *	Rule 1 for  Conqr 
	 *	 if HuntAfterPill_Avg <= 0.563 and S2.a_LivesGained_Avg <= 0.627 and Lives_SDev > 0.578 and Lives_SDev <= 0.607		 then Conqr
	 *	Rule 2 for  Conqr 
	 *	 if HuntAfterPill_Avg <= 0.563 and S2.a_LivesGained_Avg > 0.627 and Lives_SDev <= 0.609 and P1_LureToPill_Avg > 0.031	 then Conqr
	 *	Rule 3 for  Conqr 
	 *	 if HuntAfterPill_Avg > 0.563		 then Conqr
	 */
	if( result[0] < 3 && result[2] == 0 ) conq += 3; //Pruned from full rule - this far gives 80%+ acc
	else
	if( result[0] < 3 && result[2] == 1 && result[1] < 1.5 && result[12] > 0 ) conq += 10;
	else
	if( result[0] > 2 )	conq += 9;
	else
		conq -= 7;	//Average of other weights
		

	/**
	 *	Conqueror rules for replication 2:
	 *	Rule 1 for  Conqr 
	 *	 if S2.a_LivesGained_Avg <= 0.620 and HuntAfterPill_Avg > 0.229 and R1_AvgTimeToEatChrry <= 0.245 and S2.a_LivesGained_Avg <= 0.558	 then Conqr
	 *	Rule 2 for  Conqr 
	 *	 if S2.a_LivesGained_Avg > 0.620 and S1.c_AvgCycles <= 0.589 and Lives_SDev <= 0.659		 then Conqr
	 *	Rule 3 for  Conqr 
	 *	 if S2.a_LivesGained_Avg > 0.620 and S1.c_AvgCycles <= 0.589 and Lives_SDev > 0.659 and T2_HooversDots_Avg <= 0.035			 then Conqr
	 */
	if( result[2] == 0 && result[0] > 1 && result[6] < 40 )	conq += 7;	//Rule pruned as 1st and last terms equate
	else
	if( result[2] == 1 && result[3] < 5653 && result[1] < 1.5 ) conq += 19.998;
	else	
	if( result[2] == 1 && result[3] < 5653 && result[1] > 1.5 && result[13] < 3 ) conq += 2;
	else
		conq -= 10.02; //Average of other weights

	/**
	 *	Conqueror rules for replication 3:
	 *	Rule 1 for  Conqr 
	 *	 if C5_MvsW-outGain_Avg <= 0.305	 then Conqr
	 *	Rule 2 for  Conqr 
	 *	 if C5_MvsW-outGain_Avg > 0.305 and ChryBool_Sum_Avg > 0.137 and R1_AvgTimeToEatChrry <= 0.257 and S1.b_AvgStates <= 0.870	 then Conqr
	 *	Rule 3 for  Conqr 
	 *	 if C5_MvsW-outGain_Avg > 0.305 and ChryBool_Sum_Avg > 0.137 and R1_AvgTimeToEatChrry > 0.257 and HuntAfterPill_Avg > 0.456	 then Conqr
	 */
	if( result[8] < 50 ) conq += 5;
	else
	if( result[8] > 50 && result[4] > 25 && result[6] < 37 && result[14] < 415 ) conq += 12;
	else
	if( result[8] > 50 && result[4] > 25 && result[6] > 37 && result[0] > 2 ) conq += 6;
	else
		conq -= 7.667; //Average of other weights

	/**
	 *	Conqueror rules for replication 4:
	 *	Rule 1 for  Conqr 
	 *	 if C5_MvsW-outGain_Avg <= 0.413 and S2.a_LivesGained_Avg <= 0.620 and S4_TeleportUse_Avg > 0.347	 then Conqr
	 *	Rule 2 for  Conqr 
	 *	 if C5_MvsW-outGain_Avg <= 0.413 and S2.a_LivesGained_Avg > 0.620	 then Conqr
	 *	Rule 3 for  Conqr 
	 *	 if C5_MvsW-outGain_Avg > 0.413 and P7_PutOffGstHse_Avg <= 0.469 and C1.b_ThreatPrcptn-#Killed <= 0.417 and HuntAfterPill_Avg > 0.536	 then Conqr
	 *	Rule 4 for  Conqr 
	 *	 if C5_MvsW-outGain_Avg > 0.413 and P7_PutOffGstHse_Avg <= 0.469 and C1.b_ThreatPrcptn-#Killed > 0.417	 then Conqr
	 *	Rule 5 for  Conqr 
	 *	 if C5_MvsW-outGain_Avg > 0.413 and P7_PutOffGstHse_Avg > 0.469 and Lives_SDev <= 0.611		 then Conqr
	 */
	if( result[8] < 62 && result[2] == 0 && result[9] > 1 ) conq += 3;
	else
	if( result[8] < 62 && result[2] == 1 ) conq += 13.005;
	else
	if( result[8] > 62 && result[15] == 0 && result[17] < 3 && result[0] > 2 ) conq += 3;
	else
	if( result[8] > 62 && result[15] == 0 && result[17] > 2 ) conq += 2;
	else
	if( result[8] > 62 && result[15] == 1 && result[1] < 1.5 ) conq += 5;
	else
		conq -= 8.668;

	/**
	 *	Conqueror rules for replication 4:
	 *	Rule 1 for  Conqr (5; 1.0) 
	 *	 if S1.c_AvgCycles <= 0.573 and HuntAfterPill_Avg <= 0.536 and S1.c_AvgCycles <= 0.379		then Conqr
	 *	Rule 2 for  Conqr (19; 0.579) 
	 *	 if S1.c_AvgCycles <= 0.573 and HuntAfterPill_Avg <= 0.536 and S1.c_AvgCycles > 0.379 and S2.a_LivesGained_Avg > 0.620		 then Conqr
	 *	Rule 3 for  Conqr (9; 1.0) 
	 *	 if S1.c_AvgCycles <= 0.573 and HuntAfterPill_Avg > 0.536		 then Conqr
	 */
	if( result[3] < 5500 && result[0] < 3 ) conq += 5;	//Pruned as terms 1 & 3 of rule equate in deployment
	else
	if( result[3] < 5500 && result[0] < 3 && result[3] > 3637 && result[2] == 1 ) conq += 11.001;
	else
	if( result[3] < 5500 && result[0] > 2 ) conq += 9;
	else
		conq -= 8.33667;

	return conq;
}
//End of file
