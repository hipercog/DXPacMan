/**
 *	Author: Ben Cowley, 2008.
 *	Functions for CLASSIFYING players based on features from game log
 */
#include "universals.h"
#include "Classifier.h"
#include "Deriver.h"

deque <float> result;
deque<char*> feature;

float ChryBool_Sum = 0.0;
float CycleCountPerState_SDev = 0.0;
float Lives_SDev = 0.0;
// float Points_Max = 0.0;
// float A1_AtkGstHse = 0.0;
float A3_GhostKills = 0.0;
float A4_HuntAfterPill = 0.0;
float A6_Chase¬Dots = 0.0;
float C1b_ThreatPerception_NumKilled = 0.0;
// float C2b_AvgDstPacAtkCntrd = 0.0;
float C3_CloseCalls = 0.0;
float C4_CaughtAfterHunt = 0.0;
float C5_MvsWoutGain = 0.0;
// float C6_PntsOvrLives = 0.0; Score per Life per Level
float C7_KilledAtHouse = 0.0;
// float D1_CyclesPerMove = 0.0;
float D2_Osclltng = 0.0;
float P1_LureToPill = 0.0;
// float P1.b_2GstsLrd2Pll_Avg = 0.0;
// float P1.c_GstsEtnOnPll_Avg = 0.0;
// float P1.e_PacEtnB4GetPll_Avg = 0.0;
// float P3_CntDtsB4Pll1 = 0.0;
float P4b_HuntGst2_AvgSpd = 0.0;
// float P4a_HuntGst1_AvgSpd = 0.0;
float P7_PutOffGstHse = 0.0;
float R1_AvgTimeToEatChry = 0.0;
float R2_AvgChrryPerLvl = 0.0;
float S1b_AvgStates = 0.0;
float S1c_AvgCycles = 0.0;
float S2a_LivesGained = 0.0;
// float S2b_LivesLost = 0.0;
float S4_TeleportUse = 0.0;
// float S5_GhostHunts = 0.0;

/**
 *	Description: Log results of feature derivers and classifiers to a log file
 */
void Classify()
{
	float conq = CartClassifyConqueror_Level( GameStates );
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
	deque <float> caution1 = Caution1_ThreatPrcptn( arg );
	deque <float> plan1 = Planning1_LureToPill( arg );
	deque <float> plan5 = Planning5_SpeedOfHunt(arg);
	deque <float> resource = Resource_TimeAndNumChry( arg );
	deque <float> simple = Simple1_Levels( arg );
	deque <float> simple2 = Simple2_LivesChng( arg );
																	//	#	Actual		| Average		| Normals
	A4_HuntAfterPill = Aggression4_HuntAfterPill( arg );			// .00	0 - 4		| 0.125 - 1.5	| 0.083 - 1

	Lives_SDev = Aggregate1_LivesSDev( arg );						// .01	0.38 - 2.54	|				| 0.148 - 1

	S2a_LivesGained = simple2[0];									// .02	0 - 16		| 0 - 1			| 0 - 1
	// For a single level this should be equivalent to R2_AvgChryPerLvl

	S1c_AvgCycles = simple[1];										// .03	2174 - 9598	|				| 0.226 - 1

	ChryBool_Sum = Aggregate2_ChryBoolSum( arg );					// .04	25 - 2048	| 7.1 - 165.2	| 0.043 - 1
	
	C4_CaughtAfterHunt = Caution4_CaughtAfterHunt( arg );			// .05	0 - 9		| 0 - 1.25		| 0 - 1

	R1_AvgTimeToEatChry = resource[0];								// .06	0 - 142		|				| 0 - 1

	D2_Osclltng = Decisive2_Osclltng( arg );						// .07	124 - 2241	| 62 - 181.5	| 0.342 - 1

	C5_MvsWoutGain = Caution5_MovesWithoutGain( arg );				// .08	118 - 2404	| 57.5 - 256.5	| 0.224 - 1

	S4_TeleportUse = Simple4_TeleportUse( arg );					// .09	0 - 31		| 0 - 3			| 0 - 1

CycleCountPerState_SDev = Aggregate3_CycleCountPerStateSDev( arg );	//.10	6.9 - 57.3	|				| 0.12 - 1

	A6_Chase¬Dots = Aggression6_Chase¬Dots( arg );					// .11	1 - 118		| 0.25 - 11.67	| 0.021 - 1 

	P1_LureToPill = plan1[0];										// .12	0 - 24		| 0 - 2.67		| 0 - 1

	S1b_AvgStates = simple[0];										// .14	220 - 478	|				| 0.459 - 1

	P7_PutOffGstHse = Planning7_PutOffGstHse( arg );				// .15	0 - 5		| 0 - 0.571		| 0 - 1

	C7_KilledAtHouse = Caution7_KilledAtHouse( arg );				// .16	0 - 5		| 0 - 1.5		| 0 - 1

	C1b_ThreatPerception_NumKilled = caution1[0];						// .17	0 - 6		| 				| 0 - 1

	R2_AvgChrryPerLvl = resource[1];								// .18	0 - 1		|				| 

	A3_GhostKills = Aggression3_GhostKills(arg);					// .19	0.021-1.217	|				| 0.017 - 1

	C3_CloseCalls = Caution3_CloseCalls( arg );						// .20	1 - 9		| 0.071 - 1.5	| 0.048 - 1

	P4b_HuntGst2_AvgSpd = plan5[2] / max(1,A3_GhostKills-4);			// .21	0 - 2307	|				| 0 - 1

//	result.push_back( Thorough2_Hoover( arg ) );					// .13	1 - 47		| 0.07 - 5.25	| 0.013 - 1
//	feature.push_back( "T2_HooversDots" );							// NOT AMONG FEATS FROM 50 RULES

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
 *	Notes: All rules are being interpreted to work for a single level. Thus each threshold number 
 *		from a rule must be mapped to a sensible number to be derived from a single level...
 */

/**
 *	Description: Implement rules from C&R ConqNot Decision Trees to classify player as a Conqueror or Not.
 */
float CartClassifyConqueror_Level( deque<PacState> arg )
{
	float conq = 0.0;	//As a float the modifier applied by each rule can be weighted by reasonable, fractional, amounts - e.g. +0.5, +1.2, etc

	/////////	SPLIT 61:39		//////////
	/**
	 *	Qualifying rules for Replication 1, Tree 2:
	 *	Rule 2 for  Not 
	 *	 if HuntAfterPill_Avg <= 0.563 and R2_AvgChrryPerLvl <= 0.802 and GhostKills_Avg > 0.378	 then Not
	 */
	if( result[0] < 3 && result[18] == 0 && result[19] > 0.46 ) conq -= 0.213114754; //NOT rules

	/**
	 *	Qualifying rules for Replication 1, Tree 4:
	 *	Rule 2 for  Conqr 
	 *	 if HuntAfterPill_Avg <= 0.563 and S2.a_LivesGained_Avg > 0.627 and Lives_SDev <= 0.609 and P1_LureToPill_Avg > 0.031	 then Conqr
	 *	Rule 3 for  Conqr 
	 *	 if HuntAfterPill_Avg > 0.563		 then Conqr
	 *	Rule 1 for  Not 
	 *	 if HuntAfterPill_Avg <= 0.563 and S2.a_LivesGained_Avg <= 0.627 and Lives_SDev <= 0.578	 then Not
	 */
	if( result[0] < 3 && result[2] == 1 && result[1] < 1.5 && result[12] > 0 ) conq += 0.163934426;
	else
	if( result[0] > 2 )	conq += 0.147540984;
	else		
//NOT rules
	if( result[0] < 3 && result[2] == 0 && result[1] < 1.468 ) conq -= 0.327737705;

	/**
	 *	Qualifying rules for Replication 2, Tree 4:
	 *	Rule 1 for  Conqr 
	 *	 if S2.a_LivesGained_Avg <= 0.620 and HuntAfterPill_Avg > 0.229 and R1_AvgTimeToEatChrry <= 0.245 and S2.a_LivesGained_Avg <= 0.558	 then Conqr
	 *	Rule 2 for  Conqr 
	 *	 if S2.a_LivesGained_Avg > 0.620 and S1.c_AvgCycles <= 0.589 and Lives_SDev <= 0.659		 then Conqr
	 */
	if( result[2] == 0 && result[0] > 1 && result[6] < 40 )	conq += 0.114754098;	//Rule pruned as 1st and last terms equate
	else
	if( result[2] == 1 && result[3] < 5653 && result[1] < 1.5 ) conq += 0.327836066;
//	else		//NOT rules

	/**
	 *	Qualifying rules for Replication 3, Tree 1:
	 *	Rule 1 for  Conqr (5; 1.0) 
	 *	 if C5_MvsW-outGain_Avg <= 0.305		 then Conqr
	 *	Rule 1 for  Not (23; 0.826) 
	 *	 if C5_MvsW-outGain_Avg > 0.305 and CloseCalls_Avg > 0.103 and CloseCalls_Avg <= 0.206	 then Not
	 */
	if( result[8] < 50 ) conq += 0.081967213;
	else
//NOT rules
	if( result[8] > 50 && result[20] > 0 && result[20] < 2 ) conq -= 0.311442623;

	/**
	 *	Qualifying rules for Replication 3, Tree 4:
	 *	Rule 1 for	Conqr : Pruned - identical to rule 1, rep 3, tree 1
	 *	Rule 2 for  Conqr 
	 *	 if C5_MvsW-outGain_Avg > 0.305 and ChryBool_Sum_Avg > 0.137 and R1_AvgTimeToEatChrry <= 0.257 and S1.b_AvgStates <= 0.870	 then Conqr
	 *	Rule 3 for  Not (19; 0.895) 
	 *	 if C5_MvsW-outGain_Avg > 0.305 and ChryBool_Sum_Avg > 0.137 and R1_AvgTimeToEatChrry > 0.257 and HuntAfterPill_Avg <= 0.456 then Not
	 */
	if( result[8] > 50 && result[4] > 25 && result[6] < 37 && result[14] < 415 ) conq += 0.196721311;
	else		
//NOT rules
	if( result[8] > 50 && result[4] > 25 && result[6] > 37 && result[0] < 3 ) conq -= 0.278770492;

	/**
	 *	Qualifying rules for Replication 4, Tree 3:
	 *	Rule 1 for  Conqr	: Pruned as it is the same as Replication 4 Tree 4 below
	 *	Rule 2 for  Conqr (5; 1.0) 
	 *	 if C5_MvsW-outGain_Avg > 0.413 and HuntGst2_AvgSpd <= 0.504 and P7_PutOffGstHse_Avg > 0.469 and Lives_SDev <= 0.611	then Conqr
	 *	Rule 1 for  Not (9; 0.667) 
	 *	 if C5_MvsW-outGain_Avg <= 0.413 and S2.a_LivesGained_Avg <= 0.620	 then Not
	 *	Rule 2 for  Not (27; 0.889) 
	 *	 if C5_MvsW-outGain_Avg > 0.413 and HuntGst2_AvgSpd <= 0.504 and P7_PutOffGstHse_Avg <= 0.469	 then Not
	 */
	if( result[8] > 62 && result[21] < 1162 && result[15] == 1 && result[1] < 1.5 ) conq += 0.081967213;
	else
//NOT rules
	if( result[8] < 62 && result[2] == 0 ) conq -= 0.098409836;
	else
	if( result[8] > 62 && result[21] < 1162 && result[15] == 0 ) conq -= 0.393491803;

	/**
	 *	Qualifying rules for Replication 4, Tree 4:
	 *	Rule 2 for  Conqr (15; 0.867)
	 *	 if C5_MvsW-outGain_Avg <= 0.413 and S2.a_LivesGained_Avg > 0.620	 then Conqr
	 *	Rule 5 for  Conqr (5; 1.0)
	 *	 if C5_MvsW-outGain_Avg > 0.413 and P7_PutOffGstHse_Avg > 0.469 and Lives_SDev <= 0.611		 then Conqr
	 *	Rule 1 for  Not (5; 1.0) 
	 *	 if C5_MvsW-outGain_Avg <= 0.413 and S2.a_LivesGained_Avg <= 0.620 and S4_TeleportUse_Avg <= 0.347		 then Not
	 *	Rule 2 for  Not (22; 0.955) 
	 *	 if C5_MvsW-outGain_Avg > 0.413 and P7_PutOffGstHse_Avg <= 0.469 and C1.b_ThreatPrcptn-#Killed <= 0.417 and HuntAfterPill_Avg <= 0.536	 then Not
	 */
	if( result[8] < 62 && result[2] == 1 ) conq += 0.213196721;
	else
	if( result[8] > 62 && result[15] == 1 && result[1] < 1.5 ) conq += 0.081967213;
	else
//NOT rules
	if( result[8] < 62 && result[2] == 0 && result[9] < 2 ) conq -= 0.081967213;
	else
	if( result[8] > 62 && result[15] == 0 && result[17] < 2 && result[0] < 3 ) conq -= 0.34442623;

	/**
	 *	Qualifying rules for Replication 5, Tree 4:
	 *	Rule 1 for  Conqr (5; 1.0) 
	 *	 if S1.c_AvgCycles <= 0.573 and HuntAfterPill_Avg <= 0.536 and S1.c_AvgCycles <= 0.379		then Conqr
	 *	Rule 3 for  Conqr (9; 1.0) 
	 *	 if S1.c_AvgCycles <= 0.573 and HuntAfterPill_Avg > 0.536		 then Conqr
	 *	Rule 1 for  Not (18; 0.778) 
	 *	 if S1.c_AvgCycles <= 0.573 and HuntAfterPill_Avg <= 0.536 and S1.c_AvgCycles > 0.379 and S2.a_LivesGained_Avg <= 0.620		 then Not
	 *	Rule 2 for  Not (10; 0.9) 
	 *	 if S1.c_AvgCycles > 0.573		 then Not
	 */
	if( result[3] < 5500 && result[0] < 3 ) conq += 0.081967213;	//Pruned as terms 1 & 3 of rule equate in deployment
	else
	if( result[3] < 5500 && result[0] > 2 ) conq += 0.147540984;
	else
//NOT rules
	if( result[3] < 5500 && result[0] < 3 && result[3] > 3500 && result[2] == 0 ) conq -= 0.22957377;
	else
	if( result[3] > 5500 ) conq-= 0.147540984;

	/**
	 *	Qualifying rules for Replication 6, Tree 1:
	 *	Rule 1 for  Conqr	(29; 0.759)
	 *	 if HuntAfterPill_Avg > 0.212 and ChryBool_Sum_Avg > 0.138 and Lives_SDev <= 0.602		 then Conqr
	 *	Rule 1 for  Not		(14; 0.929)
	 *	 if HuntAfterPill_Avg <= 0.212		 then Not
	 *	Rule 2 for  Not		(10; 0.9) 
	 *	 if HuntAfterPill_Avg > 0.212 and ChryBool_Sum_Avg <= 0.138		 then Not
	 *	Rule 3 for  Not		 (8; 0.75) 
	 *	 if HuntAfterPill_Avg > 0.212 and ChryBool_Sum_Avg > 0.138 and Lives_SDev > 0.602		 then Not
	 */
	if( result[0] > 1 && result[4] > 23 && result[1] < 1.5 ) conq += 0.360836066;
	else
//NOT rules
	if( result[0] == 0 ) conq -= 0.213213115;
	else
	if( result[0] > 1 && result[4] < 23  ) conq -= 0.147540984;
	else
	if( result[0] > 1 && result[4] > 23 && result[1] > 1.5 ) conq -= 0.098360656;

	/////////	SPLIT 68:32		//////////

	return conq;
}

/**
 *	Description: Implement rules from C5.0 ConqNot Decision Trees to classify player as a Conqueror or Not.
 */
float C5ClassifyConqueror_Level( deque<PacState> arg )
{
	float conq = 0.0;	//As a float the modifier applied by each rule can be weighted by reasonable, fractional, amounts - e.g. +0.5, +1.2, etc

	/////////	SPLIT 61:39		//////////
	/**
	 *	Qualifying rules for Replication 1, Tree 2:
	 *	
	 */
	return conq;
}

//End of file
