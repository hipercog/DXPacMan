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
float Points_Max = 0.0;
float A1_AtkGstHse = 0.0;
float A3_GhostKills = 0.0;
float A4_HuntAfterPill = 0.0;
float A6_Chase¬Dots = 0.0;
float C1b_ThreatPerception_NumKilled = 0.0;
float C2b_AvgDstPacAtkCntrd = 0.0;
float C3_CloseCalls = 0.0;
float C4_CaughtAfterHunt = 0.0;
float C5_MvsWoutGain = 0.0;
float C6_PntsOvrLives = 0.0; // Score per Life per Level
float C7_KilledAtHouse = 0.0;
float D1_CyclesPerMove = 0.0;
float D2_Osclltng = 0.0;
float P1_LureToPill = 0.0;
float P1b_2GstsLrd2Pll = 0.0;
float P1c_GstsEtnOnPll = 0.0;
float P1d_PacEtnB4GetPll = 0.0;
float P3_CntDtsB4Pll1 = 0.0;
float P4a_HuntGst1_AvgSpd = 0.0;
float P4b_HuntGst2_AvgSpd = 0.0;
float P7_PutOffGstHse = 0.0;
float R1_AvgTimeToEatChrry = 0.0;
float R2_AvgChrryPerLvl = 0.0;
float S1b_AvgStates = 0.0;
float S1c_AvgCycles = 0.0;
float S2a_LivesGained = 0.0;
float S2b_LivesLost = 0.0;
float S4_TeleportUse = 0.0;
float S5_GhostHunts = 0.0;

/**
 *	Description: Log results of feature derivers and classifiers to a log file
 */
void Classify()
{
/** /
	float conq = ClassifyConqueror_Level( GameStates );
	sprintf_s(out, 99, "%f", conq);
	if( conq >= 0 )
		logData += "Conqueror,";
	else
		logData += "Not_Conqueror,";
	logData += out;

	/****************  THESE LINES NEED TO BE INSERTED AFTER ALL CLASSIFICATION FUNCTIONS, BETWEEN EVERY LEVEL *******************/
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
	int s = arg.size();

	deque <float> caution1 = Caution1_ThreatPrcptn( arg );
	deque <float> plan1 = Planning1_LureToPill( arg );
	deque <float> plan4 = Planning4_SpeedOfHunt(arg);
	deque <float> resource = Resource_TimeAndNumChry( arg );
	deque <float> simple = Simple1_Levels( arg );
	deque <float> simple2 = Simple2_LivesChng( arg );
																	// Freq	Actual		| Average		| Normals
	S5_GhostHunts = 4.0 - arg[s-1].Pills;							// 3	# hunts = # Pills used - This needs to be non-alphabetically called - it is used later

	A1_AtkGstHse = Aggression1_AtkGstHse( arg );					// 2	0 - 35		| 0 - 3.2		| 0 - 1

	A3_GhostKills = Aggression3_GhostKills( arg );					// 2	0.021-1.217	|				| 0.017 - 1

	A4_HuntAfterPill = Aggression4_HuntAfterPill( arg );			// 25	0 - 4		| 0.125 - 1.5	| 0.083 - 1

	A6_Chase¬Dots = Aggression6_Chase¬Dots( arg );					// 5	1 - 118		| 0.25 - 11.67	| 0.021 - 1

	C1b_ThreatPerception_NumKilled = caution1[0];					// 4	0 - 6		| 				| 0 - 1	

	C2b_AvgDstPacAtkCntrd = Caution2_AvgDstPacAtkGstCtrd( arg );	// 1	7.02 - 14.09|				| 0.498 - 1

	C3_CloseCalls = Caution3_CloseCalls( arg );						// 6	9-Jan		| 0.071 - 1.5	| 0.048 - 1	

	C4_CaughtAfterHunt = Caution4_CaughtAfterHunt( arg );			// 5	0 - 9		| 0 - 1.25		| 0 - 1

	C5_MvsWoutGain = Caution5_MovesWithoutGain( arg );				// 7	118 - 2404	| 57.5 - 256.5	| 0.224 - 1

	C6_PntsOvrLives = Caution6_PntsOvrLives( arg );					// 2	142 - 3750	|				| 0.038 - 1

	C7_KilledAtHouse = Caution7_KilledAtHouse( arg );				// 1	0 - 5		| 0 - 1.5		| 0 - 1

	ChryBool_Sum = Aggregate2_ChryBoolSum( arg );					// 9	25 - 2048	| 7.1 - 165.2	| 0.043 - 1	

	CycleCountPerState_SDev=Aggregate3_CycleCountPerStateSDev(arg);	// 2	6.9 - 57.3	|				| 0.12 - 1

	D1_CyclesPerMove = Decisive1_CyclesPrMv( arg );					// 1	7.2 - 42.4	|				| 0.17 - 1

	D2_Osclltng = Decisive2_Osclltng( arg );						// 2	124 - 2241	| 62 - 181.5	| 0.342 - 1	

	Lives_SDev = Aggregate1_LivesSDev( arg );						// 11	0.38 - 2.54	|				| 0.148 - 1	

	P1_LureToPill = plan1[0];										// 1	0 - 24		| 0 - 2.67		| 0 - 1

	P1b_2GstsLrd2Pll = plan1[2];									// 3	0 - 9		| 0 - 1			| 0 - 1	

	P1c_GstsEtnOnPll = plan1[3];									// 3	0 - 37		| 0 - 3.6		| 0 - 1	

	P1d_PacEtnB4GetPll = plan1[4];									// 2	0 - 1		| 0 - 0.333		| 0 - 1	

	P3_CntDtsB4Pll1 = Planning3_CntDtsB4Pll1( arg );				// 1	6.3 - 97.6	| 0.9 - 22		| 0.041 - 1	

	P4a_HuntGst1_AvgSpd =plan4[0]/min(S5_GhostHunts,A3_GhostKills);	// 2	62 - 349	|				| 0.178 - 1	

	P4b_HuntGst2_AvgSpd = plan4[2] / max(1,A3_GhostKills-4);		// 2	0 - 2307	|				| 0 - 1	

	P7_PutOffGstHse = Planning7_PutOffGstHse( arg );				// 8	0 - 5		| 0 - 0.571		| 0 - 1	

	Points_Max = arg[s-1].Points;									// 3	Points scored by level end.	

	R1_AvgTimeToEatChrry = resource[0];								// 2	0 - 142		|				| 0 - 1	

	R2_AvgChrryPerLvl = resource[1];								// 8	0 - 1		|				| 		

	S1b_AvgStates = simple[0];										// 4	220 - 478	|				| 0.459 - 1	

	S1c_AvgCycles = simple[1];										// 5	2174 - 9598	|				| 0.226 - 1	
	// For a single level this should be equivalent to R2_AvgChryPerLvl
	S2a_LivesGained = simple2[0];									// 11	0 - 16		| 0 - 1			| 0 - 1	

	S2b_LivesLost = simple2[1];										// 1	0 - 19		|				| 0 - 1

	S4_TeleportUse = Simple4_TeleportUse( arg );					// 2	0 - 31		| 0 - 3			| 0 - 1

//	result.push_back( Thorough2_Hoover( arg ) );					// .	1 - 47		| 0.07 - 5.25	| 0.013 - 1
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

	result.push_back( A1_AtkGstHse );			feature.push_back( "A1_AtkGstHse" );
	result.push_back( A3_GhostKills );			feature.push_back( "A3_GhostKills" );
	result.push_back( A4_HuntAfterPill );		feature.push_back( "A4_HuntAfterPill" );
	result.push_back( A6_Chase¬Dots );			feature.push_back( "A6_Chase¬Dots" );
	result.push_back( C1b_ThreatPerception_NumKilled );		feature.push_back( "C1b_ThreatPerception_NumKilled" );
	result.push_back( C2b_AvgDstPacAtkCntrd );	feature.push_back( "C2.b_AvgDstPacAtkCntrd" );	
	result.push_back( C3_CloseCalls );			feature.push_back( "C3_CloseCalls" );
	result.push_back( C4_CaughtAfterHunt );		feature.push_back( "C4_CaughtAfterHunt" );
	result.push_back( C5_MvsWoutGain );			feature.push_back( "C5_MvsWoutGain" );
	result.push_back( C6_PntsOvrLives );		feature.push_back( "C6_PntsOvrLives" );	
	result.push_back( C7_KilledAtHouse );		feature.push_back( "C7_KilledAtHouse" );
	result.push_back( ChryBool_Sum );			feature.push_back( "ChryBool_Sum" );
	result.push_back( CycleCountPerState_SDev);	feature.push_back( "CycleCountPerState_SDev" );
	result.push_back( D1_CyclesPerMove );		feature.push_back( "D1_CyclesPerMove" );	
	result.push_back( D2_Osclltng );			feature.push_back( "D2_Osclltng" );
	result.push_back( Lives_SDev );				feature.push_back( "Lives_SDev" );
	result.push_back( P1_LureToPill );			feature.push_back( "P1_LureToPill" );
	result.push_back( P1b_2GstsLrd2Pll );		feature.push_back( "P1b_2GstsLrd2Pll" );
	result.push_back( P1c_GstsEtnOnPll );		feature.push_back( "P1c_GstsEtnOnPll" );
	result.push_back( P1d_PacEtnB4GetPll );		feature.push_back( "P1d_PacEtnB4GetPll" );
	result.push_back( P3_CntDtsB4Pll1 );		feature.push_back( "P3_CntDtsB4Pll1" );	
	result.push_back( P4a_HuntGst1_AvgSpd );	feature.push_back( "P4a_HuntGst1_AvgSpd" );
	result.push_back( P4b_HuntGst2_AvgSpd );	feature.push_back( "P4b_HuntGst2_AvgSpd" );
	result.push_back( P7_PutOffGstHse );		feature.push_back( "P7_PutOffGstHse" );
	result.push_back( Points_Max );				feature.push_back( "Points_Max" );
	result.push_back( R1_AvgTimeToEatChrry );	feature.push_back( "R1_AvgTimeToEatChrry" );
	result.push_back( R2_AvgChrryPerLvl );		feature.push_back( "R2_AvgChrryPerLvl" );
	result.push_back( S1b_AvgStates );			feature.push_back( "S1b_AvgStates" );
	result.push_back( S1c_AvgCycles );			feature.push_back( "S1c_AvgCycles" );
	result.push_back( S2a_LivesGained );		feature.push_back( "S2a_LivesGained" );
	result.push_back( S2b_LivesLost );			feature.push_back( "S2b_LivesLost" );
	result.push_back( S4_TeleportUse );			feature.push_back( "S4_TeleportUse" );
	result.push_back( S5_GhostHunts );			feature.push_back( "S5_GhostHunts" );

	for( int i = 0; i < feature.size(); i++ )
	{
		sprintf_s( out, 99, "%f", result[i] );
		logData += feature[i]; logData += ","; logData += out; logData += "\n";
	}
/**/
}

/**
 *	Notes: All rules are being interpreted to work for a single level. Thus each threshold number 
 *		from a rule must be mapped to a sensible number to be derived from a single level...
 */


/**
 *	Description: Implement rules from 50 Rule ensemble to classify player as a Conqueror or Not for one level.
 */
float ClassifyConqueror_Level( deque<PacState> arg )
{
	float conq = 0.0;	//As a float the modifier applied by each rule can be weighted by reasonable, fractional, amounts - e.g. +0.5, +1.2, etc

	/////////	Rules with one condition		//////////
	/**
	 *	Rank			Rule
	 *	0.235264706		Rule 2 Not (19; 0.842)  if Lives_SDev > 0.597 then Not
	 *	0.2133			Rule 2 Not (14.22; 0.915) if D2_Osclltng_Avg > 0.678 then Not
	 *	0.213213115		Rule 1 Not (14; 0.929)  if HuntAfterPill_Avg <= 0.212 then Not
	 *	0.161826824		Rule 1 Conqr (12.992; 0.847) if Points_Max_Avg <= 0.446 then Conqr
	 *	0.147540984		Rule 3 Conqr (9; 1.0) if HuntAfterPill_Avg > 0.563 then Conqr
	 *	0.147540984		Rule 2 Not (10; 0.9) if S1.c_AvgCycles > 0.573 then Not
	 *	0.133333333		Rule 1 Not (10; 1.0) if ChryBool_Sum_Avg <= 0.137 then Not
	 *	0.106666667		Rule 5 Not (8; 1.0) if C5_MvsW-outGain_Avg > 0.584 then Not
	 *	0.106509836		Rule 3 Conqr (7.219; 0.9) if P7_PutOffGstHse_Avg > 0.656 then Conqr
	 */
	if( Lives_SDev > 0.597 ) conq -= 0.235264706;

	if( D2_Osclltng > 0.678 ) conq -= 0.2133;

	if( A4_HuntAfterPill <= 0.212 ) conq -= 0.213213115;

	if( Points_Max <= 0.446 ) conq += 0.161826824;

	if( A4_HuntAfterPill > 0.563 ) conq += 0.147540984;

	if( S1c_AvgCycles > 0.573 ) conq -= 0.147540984;

	if( ChryBool_Sum <= 0.137 ) conq -= 0.1333;

	if( C5_MvsWoutGain > 0.584 ) conq -= 0.1067;

	if( P7_PutOffGstHse > 0.656 ) conq += 0.106509836;

	/////////	Rules with two conditions		//////////
	/**
	 *	Rank			Rule
	 *	0.226733333		Rule 2 Conqr (19; 0.895) if HuntAfterPill_Avg > 0.460 and C4_CaughtAfterHunt_Avg <= 0.557 then Conqr
	 *	0.213196721		Rule 1 Conqr (15; 0.867)  if C5_MvsW-outGain_Avg <= 0.413 and S2.a_LivesGained_Avg > 0.620 then Conqr
	 *	0.191264706		Rule 1 Conqr (14; 0.929)  if HuntAfterPill_Avg > 0.167 and S1.b_AvgStates <= 0.636 then Conqr
	 *	0.161764706		Rule 3 Conqr (11; 1.0) if HuntAfterPill_Avg > 0.563 and S1.c_AvgCycles <= 0.589 then Conqr
	 *	0.147540984		Rule 2 Not (10; 0.9) if HuntAfterPill_Avg > 0.212 and ChryBool_Sum_Avg <= 0.138 then Not
	 *	0.132352941		Rule 1 Conqr (9; 1.0) if Points_Max_Avg <= 0.448 and HuntGst2_AvgSpd <= 0.017 then Conqr
	 *	0.126575738		Rule 1 Not (8.56; 0.902)  if D2_Osclltng_Avg <= 0.678 and R1_AvgTimeToEatChrry <= 0.145 then Not
	 *	0.12			Rule 1 Conqr (10; 0.9)  if ChryBool_Sum_Avg > 0.137 and CloseCalls_Avg <= 0.116 then Conqr
	 *	0.102941176		Rule 3 Not (7; 1.0) if ChryBool_Sum_Avg > 0.136 and Chase¬Dots_Avg > 0.450 then Not
	 */
	if( A4_HuntAfterPill > 0.460 && C4_CaughtAfterHunt <= 0.557 ) conq += 0.226733333;

	if( C5_MvsWoutGain <= 0.413 && S2a_LivesGained > 0.620 ) conq += 0.213196721;

	if( A4_HuntAfterPill > 0.167 && S1b_AvgStates <= 0.636 ) conq += 0.191264706;

	if( A4_HuntAfterPill > 0.563 && S1c_AvgCycles <= 0.589 ) conq += 0.161764706;

	if( A4_HuntAfterPill > 0.212 && ChryBool_Sum <= 0.138 ) conq -= 0.147540984;

	if( Points_Max <= 0.448 && P4b_HuntGst2_AvgSpd <= 0.017 ) conq += 0.132352941;

	if( D2_Osclltng <= 0.678 && R1_AvgTimeToEatChrry <= 0.145 ) conq -= 0.126575738;

	if( ChryBool_Sum > 0.137 && C3_CloseCalls <= 0.116 ) conq += 0.12;

	if( ChryBool_Sum > 0.136 && A6_Chase¬Dots > 0.450 ) conq -= 0.102941176;

	/////////	Rules with three conditions		//////////
	/**
	 *	Rank		Rule
	 *	0.393491803	Rule 2 Not (27; 0.889) if C5_MvsW-outGain_Avg > 0.413 and HuntGst2_AvgSpd <= 0.504 and P7_PutOffGstHse_Avg <= 0.469 then Not
	 *	0.327836066	Rule 2 Conqr (22; 0.909)  if S2.a_LivesGained_Avg > 0.620 and S1.c_AvgCycles <= 0.589 and Lives_SDev <= 0.659 then Conqr
	 *	0.327737705	Rule 1 Not (21; 0.952) if HuntAfterPill_Avg <= 0.563 and S2.a_LivesGained_Avg <= 0.627 and Lives_SDev <= 0.578 then Not
	 *	0.323647059	Rule 2 Conqr (28; 0.786) if S2.a_LivesGained_Avg > 0.620 and C4_CaughtAfterHunt_Avg <= 0.557 and S1.c_AvgCycles <= 0.680 then Conqr
	 *	0.311442623	Rule 1 Not (23; 0.826) if C5_MvsW-outGain_Avg > 0.305 and CloseCalls_Avg > 0.103 and CloseCalls_Avg <= 0.206 then Not
	 *	0.294117647	Rule 1 Conqr (25; 0.8)  if S1.c_AvgCycles <= 0.458 and HuntAfterPill_Avg > 0.181 and S2.a_LivesGained_Avg > 0.429 then Conqr
	 *	0.279529412	Rule 2 Conqr (22; 0.864) if C6_PntsOvrLives <= 0.506 and Lives_SDev <= 0.603 and S2.a_LivesGained_Avg > 0.620 then Conqr
	 *	0.266666667	Rule 2 Conqr (25; 0.8)  if ChryBool_Sum_Avg > 0.137 and S2.a_LivesGained_Avg > 0.620 and Lives_SDev <= 0.602 then Conqr
	 *	0.220705882	Rule 3 Not (16; 0.938)  if Points_Max_Avg > 0.448 and P4_CntDtsb4Pll1_Avg > 0.191 and HuntGst1_AvgSpd <= 0.676 then Not
	 *	0.219350147	Rule 3 Not (16.391; 0.91) if R2_AvgChrryPerLvl > 0.333 and P7_PutOffGstHse_Avg <= 0.438 and P7_PutOffGstHse_Avg > 0.292 then Not
	 *	0.213114754	Rule 2 Not (13; 1.0) if HuntAfterPill_Avg <= 0.563 and R2_AvgChrryPerLvl <= 0.802 and GhostKills_Avg > 0.378 then Not
	 *	0.2			Rule 5 Not (15; 1.0) if HuntAfterPill_Avg > 0.173 and GhostHunts_Avg > 0.941 and CycleCountPerState_SDev > 0.229 then Not
	 *	0.176455882	Rule 1 Conqr (13; 0.923) if HuntAfterPill_Avg > 0.136 and S1.b_AvgStates <= 0.639 and C6_PntsOvrLives <= 0.510 then Conqr
	 *	0.174110588	Rule 2 Not (13.64; 0.868)  if S1.b_AvgStates <= 0.824 and C4_CaughtAfterHunt_Avg > 0.356 and GhostHunts_Avg > 0.911 then Not
	 *	0.163206441	Rule 4 Conqr (13.307; 0.834)  if R2_AvgChrryPerLvl > 0.333 and P7_PutOffGstHse_Avg > 0.438 and Lives_SDev <= 0.597 then Conqr
	 *	0.131163934	Rule 4 Not (9; 0.889)  if HuntAfterPill_Avg <= 0.563 and S2.a_LivesGained_Avg > 0.627 and Lives_SDev > 0.609 then Not
	 *	0.119973333	Rule 2 Conqr (11; 0.818)  if ChryBool_Sum_Avg > 0.137 and CloseCalls_Avg > 0.116 and HuntAfterPill_Avg > 0.567 then Conqr
	 *	0.117647059	Rule 1 Conqr (10; 0.8) if S2.a_LivesGained_Avg <= 0.620 and HuntAfterPill_Avg > 0.257 and Chase¬Dots_Avg <= 0.124 then Conqr
	 */
	if( C5_MvsWoutGain > 0.413 && P4b_HuntGst2_AvgSpd <= 0.504 && P7_PutOffGstHse <= 0.469 ) conq -= 0.393491803;

	if( S2a_LivesGained > 0.620 && S1c_AvgCycles <= 0.589 && Lives_SDev <= 0.659 ) conq += 0.327836066;

	if( A4_HuntAfterPill <= 0.563 && S2a_LivesGained <= 0.627 && Lives_SDev <= 0.578 ) conq -= 0.327737705;

	if( S2a_LivesGained > 0.620 && C4_CaughtAfterHunt <= 0.557 && S1c_AvgCycles <= 0.680 ) conq += 0.323647059;

	if( C5_MvsWoutGain > 0.305 && C3_CloseCalls > 0.103 && C3_CloseCalls <= 0.206 ) conq -= 0.311442623;

	if( S1c_AvgCycles <= 0.458 && A4_HuntAfterPill > 0.181 && S2a_LivesGained > 0.429 ) conq += 0.294117647;

	if( C6_PntsOvrLives <= 0.506 && Lives_SDev <= 0.603 && S2a_LivesGained > 0.620 ) conq += 0.279529412;

	if( ChryBool_Sum > 0.137 && S2a_LivesGained > 0.620 && Lives_SDev <= 0.602 ) conq += 0.2667;

	if( Points_Max > 0.448 && P3_CntDtsB4Pll1 > 0.191 && P4a_HuntGst1_AvgSpd <= 0.676 ) conq -= 0.220705882;
	
	if( R2_AvgChrryPerLvl > 0.333 && P7_PutOffGstHse <= 0.438 && P7_PutOffGstHse > 0.292 ) conq -= 0.219350147;

	if( A4_HuntAfterPill <= 0.563 && R2_AvgChrryPerLvl <= 0.802 && A3_GhostKills > 0.378 ) conq -= 0.213114754;

	if( A4_HuntAfterPill > 0.173 && S5_GhostHunts > 0.941 && CycleCountPerState_SDev > 0.229 ) conq -= 0.2;

	if( A4_HuntAfterPill > 0.136 && S1b_AvgStates <= 0.639 && C6_PntsOvrLives <= 0.510 ) conq += 0.176455882;

	if( S1b_AvgStates <= 0.824 && C4_CaughtAfterHunt > 0.356 && S5_GhostHunts > 0.911 ) conq -= 0.174110588;
	
	if( R2_AvgChrryPerLvl > 0.333 && P7_PutOffGstHse > 0.438 && Lives_SDev <= 0.597 ) conq += 0.163206441;

	if( A4_HuntAfterPill <= 0.563 && S2a_LivesGained > 0.627 && Lives_SDev > 0.609 ) conq -= 0.131163934;

	if( ChryBool_Sum > 0.137 && C3_CloseCalls > 0.116 && A4_HuntAfterPill > 0.567 ) conq += 0.119973333;

	if( S2a_LivesGained <= 0.620 && A4_HuntAfterPill > 0.257 && A6_Chase¬Dots <= 0.124 ) conq += 0.117647059;

	/////////	Rules with four conditions		//////////
	/**
	 *	Rank		Rule
	 *	0.374970588	Rule 2 Not (30.5; 0.836)  if HuntAfterPill_Avg > 0.167 and S1.b_AvgStates > 0.636 and CloseCalls_Avg > 0.111 and S4_TeleportUse_Avg > 0.286 then Not
	 *	0.34442623	Rule 2 Not (22; 0.955) if C5_MvsW-outGain_Avg > 0.413 and P7_PutOffGstHse_Avg <= 0.469 and C1.b_ThreatPrcptn-#Killed <= 0.417 and HuntAfterPill_Avg <= 0.536 then Not
	 *	0.294264706	Rule 1 Conqr (23; 0.87)  if Lives_SDev <= 0.602 and HuntAfterPill_Avg > 0.210 and CycleCountPerState_SDev <= 0.275 and C4_CaughtAfterHunt_Avg <= 0.360 then Conqr
	 *	0.293333333	Rule 1 Conqr (25; 0.88)  if HuntAfterPill_Avg > 0.173 and GhostHunts_Avg <= 0.941 and Chase¬Dots_Avg <= 0.454 and ChryBool_Sum_Avg > 0.144 then Conqr
	 *	0.278770492	Rule 3 Not (19; 0.895)  if C5_MvsW-outGain_Avg > 0.305 and ChryBool_Sum_Avg > 0.137 and R1_AvgTimeToEatChrry > 0.257 and HuntAfterPill_Avg <= 0.456 then Not
	 *	0.226133647	Rule 1 Not (18.112; 0.849)  if R2_AvgChrryPerLvl > 0.333 and P7_PutOffGstHse_Avg <= 0.292 and C1.b_ThreatPrcptn-#Killed <= 0.167 and S2.b_LivesLost <= 0.737 then Not
	 *	0.196688525	Rule 1 Not (14; 0.857)  if HuntAfterPill_Avg <= 0.563 and R2_AvgChrryPerLvl <= 0.802 and GhostKills_Avg <= 0.378 and P1.c_GstsEtnOnPll_Avg <= 0.183 then Not
	 *	0.19175		Rule 2 Conqr (13.039; 1.0)  if HuntAfterPill_Avg > 0.167 and A1_AtkGstHse_Avg <= 0.134 and Chase¬Dots_Avg <= 0.415 and P1.c_GstsEtnOnPll_Avg > 0.139 then Conqr
	 *	0.163934426	Rule 2 Conqr (10; 1.0) if HuntAfterPill_Avg <= 0.563 and S2.a_LivesGained_Avg > 0.627 and Lives_SDev <= 0.609 and P1_LureToPill_Avg > 0.031 then Conqr
	 *	0.161764706	Rule 2 Conqr (11; 1.0) if HuntAfterPill_Avg <= 0.563 and S2.a_LivesGained_Avg > 0.620 and D1_Cycles/Move <= 0.368 and Lives_SDev <= 0.551 then Conqr
	 *	0.131411765	Rule 3 Conqr (8.936; 1.0)  if R2_AvgChrryPerLvl > 0.333 and P7_PutOffGstHse_Avg <= 0.292 and C1.b_ThreatPrcptn-#Killed > 0.167 and S4_TeleportUse_Avg <= 0.641 then Conqr
	 */
	if( A4_HuntAfterPill > 0.167 && S1b_AvgStates > 0.636 && C3_CloseCalls > 0.111 && S4_TeleportUse > 0.286 ) conq -= 0.374970588;
	
	if( C5_MvsWoutGain > 0.413 && P7_PutOffGstHse <= 0.469 && C1b_ThreatPerception_NumKilled <= 0.417 && A4_HuntAfterPill <= 0.536 ) conq -= 0.34442623;
	
	if( Lives_SDev <= 0.602 && A4_HuntAfterPill > 0.210 && CycleCountPerState_SDev <= 0.275 && C4_CaughtAfterHunt <= 0.360 ) conq += 0.294264706;
	
	if( A4_HuntAfterPill > 0.173 && S5_GhostHunts <= 0.941 && A6_Chase¬Dots <= 0.454 && ChryBool_Sum > 0.144 ) conq += 0.2933;
	
	if( C5_MvsWoutGain > 0.305 && ChryBool_Sum > 0.137 && R1_AvgTimeToEatChrry > 0.257 && A4_HuntAfterPill <= 0.456 ) conq -= 0.278770492;
	
	if( R2_AvgChrryPerLvl > 0.333 && P7_PutOffGstHse <= 0.292 && C1b_ThreatPerception_NumKilled <= 0.167 && S2b_LivesLost <= 0.737 ) conq -= 0.226133647;
	
	if( A4_HuntAfterPill <= 0.563 && R2_AvgChrryPerLvl <= 0.802 && A3_GhostKills <= 0.378 && P1c_GstsEtnOnPll <= 0.183 ) conq -= 0.196688525;
	
	if( A4_HuntAfterPill > 0.167 && A1_AtkGstHse <= 0.134 && A6_Chase¬Dots <= 0.415 && P1c_GstsEtnOnPll > 0.139 ) conq += 0.19175;
	
	if( A4_HuntAfterPill <= 0.563 && S2a_LivesGained > 0.627 && Lives_SDev <= 0.609 && P1_LureToPill > 0.031 ) conq += 0.163934426;
	
	if( A4_HuntAfterPill <= 0.563 && S2a_LivesGained > 0.620 && D1_CyclesPerMove <= 0.368 && Lives_SDev <= 0.551 ) conq += 0.161764706;
	
	if( R2_AvgChrryPerLvl > 0.333 && P7_PutOffGstHse <= 0.292 && C1b_ThreatPerception_NumKilled > 0.167 && S4_TeleportUse <= 0.641 ) conq += 0.131411765;

	/////////	Rules with more than four conditions		//////////
	/**
	 *	Rank		Rule
	 *	0.161764706	Rule 2 Not (11; 1.0)  if P1.e_PacEtnB4GetPll_Avg <= 0 and C1.b_ThreatPrcptn-#Killed <= 0.333 and R2_AvgChrryPerLvl > 0.333 and P1.c_GstsEtnOnPll_Avg <= 0.598 and P1.b_2GstsLrd2Pll_Avg <= 0.167 and HuntGst1_AvgSpd <= 0.640 and R2_AvgChrryPerLvl <= 0.750 and Chase¬Dots_Avg > 0.120 then Not
	 *	0.113455882	Rule 3 Conqr (7.715; 1.0)  if HuntAfterPill_Avg > 0.167 and A1_AtkGstHse_Avg > 0.134 and P1.e_PacEtnB4GetPll_Avg <= 0 and Lives_SDev <= 0.597 and CloseCalls_Avg <= 0.359 and C7_KilledAtHouse_Avg <= 0.148 then Conqr
	 *	0.4536		Rule 1 Conqr (45; 0.756)  if C5_MvsW-outGain_Avg <= 0.584 and HuntAfterPill_Avg > 0.171 and C2.b_AvgDstPacAtk-Cntrd <= 0.941 and C4_CaughtAfterHunt_Avg <= 0.554 and ChryBool_Sum_Avg > 0.136 then Conqr
	 */
	if( P1d_PacEtnB4GetPll <= 0 && C1b_ThreatPerception_NumKilled <= 0.333 && R2_AvgChrryPerLvl > 0.333 && P1c_GstsEtnOnPll <= 0.598 && P1b_2GstsLrd2Pll <= 0.167 && P4a_HuntGst1_AvgSpd <= 0.640 && R2_AvgChrryPerLvl <= 0.750 && A6_Chase¬Dots > 0.120 ) conq -= 0.161764706;
	
	if( A4_HuntAfterPill > 0.167 && A1_AtkGstHse > 0.134 && P1d_PacEtnB4GetPll <= 0 && Lives_SDev <= 0.597 && C3_CloseCalls <= 0.359 && C7_KilledAtHouse <= 0.148 ) conq += 0.113455882;
	
	if( C5_MvsWoutGain <= 0.584 && A4_HuntAfterPill > 0.171 && C2b_AvgDstPacAtkCntrd <= 0.941 && C4_CaughtAfterHunt <= 0.554 && ChryBool_Sum > 0.136 ) conq += 0.4536;

	return conq;
}

//End of file
