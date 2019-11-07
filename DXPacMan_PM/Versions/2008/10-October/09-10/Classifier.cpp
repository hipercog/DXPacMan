/**
 *	Author: Ben Cowley, 2008.
 *	Functions for CLASSIFYING players based on features from game log
 */
#include "universals.h"
#include "Classifier.h"
#include "Deriver.h"

deque <float> result;
deque <char*> feature;
deque <int> usage;
deque <int> fired;
deque <int> weight( 50, 0 );
deque <char*> rules;

deque <float> A1_AtkGstHse( 1, 0 );
deque <float> A3_GhostKills( 1, 0 );
deque <float> A4_HuntAfterPill( 1, 0 );
deque <float> A6_Chase¬Dots( 1, 0 );
deque <float> C1b_ThreatPerception_NumKilled( 1, 0 );
deque <float> C2b_AvgDstPacAtkCntrd( 1, 0 );
deque <float> C3_CloseCalls( 1, 0 );
deque <float> C4_CaughtAfterHunt( 1, 0 );
deque <float> C5_MvsWoutGain( 1, 0 );
deque <float> C6_PntsOvrLives( 1, 0 ); // Score per Life per Level
deque <float> C7_KilledAtHouse( 1, 0 );
deque <float> ChryBool_Sum( 1, 0 );
deque <float> CycleCountPerState_SDev( 1, 0 );
deque <float> D1_CyclesPerMove( 1, 0 );
deque <float> D2_Osclltng( 1, 0 );
deque <float> Lives_SDev( 1, 0 );
deque <float> P1_LureToPill( 1, 0 );
deque <float> P1b_2GstsLrd2Pll( 1, 0 );
deque <float> P1c_GstsEtnOnPll( 1, 0 );
deque <float> P1d_PacEtnB4GetPll( 1, 0 );
deque <float> P3_CntDtsB4Pll1( 1, 0 );
deque <float> P4a_HuntGst1_AvgSpd( 1, 0 );
deque <float> P4b_HuntGst2_AvgSpd( 1, 0 );
deque <float> P7_PutOffGstHse( 1, 0 );
deque <float> Points_Max(1, 0);		//This doesn't act like the other deques - total score over levels is already receorded in logs.
deque <float> R1_AvgTimeToEatChrry( 1, 0 );
deque <float> R2_AvgChrryPerLvl( 1, 0 );
deque <float> S1b_AvgStates( 1, 0 );
deque <float> S1c_AvgCycles( 1, 0 );
deque <float> S2a_LivesGained( 1, 0 );
deque <float> S2b_LivesLost( 1, 0 );
deque <float> S4_TeleportUse( 1, 0 );
deque <float> S5_GhostHunts( 1, 0 );


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
	if( s < 2 ) return;

	int l = Level;

	deque <float> caution1 = Caution1_ThreatPrcptn( arg );
	deque <float> plan1 = Planning1_LureToPill( arg );
	deque <float> plan4 = Planning4_SpeedOfHunt(arg);
	deque <float> resource = Resource_TimeAndNumChry( arg );
//	deque <float> simple = Simple1_Levels( arg );
	deque <float> simple2 = Simple2_LivesChng( arg );

	//Need to list non-alphabetically - it is used later						// #	Frq	Actual		| Average		| Normals	|
	S5_GhostHunts.push_back( 4.0 - arg[s-1].Pills );							// 32	3	# hunts = # Pills used					|

	A1_AtkGstHse.push_back( Aggression1_AtkGstHse( arg ) );						// 0	2	0 - 35		| 0 - 3.2		| 0 - 1		|
//cout << "A1 ok";
	A3_GhostKills.push_back( Aggression3_GhostKills( arg ) );					// 1	2	0.021-1.217	|				| 0.017 - 1	|
//cout << "A3 ok";
	A4_HuntAfterPill.push_back( Aggression4_HuntAfterPill( arg ) );				// 2	25	0 - 4		| 0.125 - 1.5	| 0.083 - 1	|
//cout << "A4 ok";
	A6_Chase¬Dots.push_back( Aggression6_Chase¬Dots( arg ) );					// 3	5	1 - 118		| 0.25 - 11.67	| 0.021 - 1	|
//cout << "A6 ok";
	C1b_ThreatPerception_NumKilled.push_back( caution1[0] );					// 4	4	0 - 6		| 				| 0 - 1		|
//cout << "C1b ok";
	C2b_AvgDstPacAtkCntrd.push_back( Caution2_AvgDstPacAtkGstCtrd( arg ) );		// 5	1	7.02 - 14.09|				| 0.498 - 1	|
//cout << "C2b ok";
	C3_CloseCalls.push_back( Caution3_CloseCalls( arg ) );						// 6	6	1 - 9		| 0.071 - 1.5	| 0.048 - 1	|

	C4_CaughtAfterHunt.push_back( Caution4_CaughtAfterHunt( arg ) );			// 7	5	0 - 9		| 0 - 1.25		| 0 - 1		|

	C5_MvsWoutGain.push_back( Caution5_MovesWithoutGain( arg ) );				// 8	7	118 - 2404	| 57.5 - 256.5	| 0.224 - 1	|

	C6_PntsOvrLives.push_back( arg[s-1].Points/max(simple2[0]+simple2[1], 1) );	// 9	2	142 - 3750	|				| 0.038 - 1	|
	//Alternative (check Clem): abs( arg[s-1].Lives - arg[0].Lives ) + 1;
	C7_KilledAtHouse.push_back( Caution7_KilledAtHouse( arg ) );				// 10	1	0 - 5		| 0 - 1.5		| 0 - 1 	|

	ChryBool_Sum.push_back( Aggregate2_ChryBoolSum( arg ) );					// 11	9	25 - 2048	| 7.1 - 165.2	| 0.043 - 1	|

	CycleCountPerState_SDev.push_back(Aggregate3_CycleCountPerStateSDev(arg));	// 12	2	6.9 - 57.3	|				| 0.12 - 1	|

	D1_CyclesPerMove.push_back( Decisive1_CyclesPrMv( arg ) );					// 13	1	7.2 - 42.4	|				| 0.17 - 1	|

	D2_Osclltng.push_back( Decisive2_Osclltng( arg ) );							// 14	2	124 - 2241	| 62 - 181.5	| 0.342 - 1	|

	Lives_SDev.push_back( Aggregate1_LivesSDev( arg ) );						// 15	11	0.38 - 2.54	|				| 0.148 - 1	|

	P1_LureToPill.push_back( plan1[0] );										// 16	1	0 - 24		| 0 - 2.67		| 0 - 1 	|

	P1b_2GstsLrd2Pll.push_back( plan1[2] );										// 17	1	0 - 9		| 0 - 1			| 0 - 1		|

	P1c_GstsEtnOnPll.push_back( plan1[3] );										// 18	3	0 - 37		| 0 - 3.6		| 0 - 1		|

	P1d_PacEtnB4GetPll.push_back( plan1[4] );									// 19	2	0 - 1		| 0 - 0.333		| 0 - 1		|

	P3_CntDtsB4Pll1.push_back( Planning3_CntDtsB4Pll1( arg ) );					// 20	1	6.3 - 97.6	| 0.9 - 22		| 0.041 - 1	|

P4a_HuntGst1_AvgSpd.push_back(plan4[0]/min(S5_GhostHunts[l],A3_GhostKills[l]));	// 21	2	62 - 349	|				| 0.178 - 1	|

	P4b_HuntGst2_AvgSpd.push_back( plan4[2] / max(1, A3_GhostKills[l]-4) );		// 22	2	0 - 2307	|				| 0 - 1		|
	//This becomes binary since you can only put off the ghost house once per level.
	P7_PutOffGstHse.push_back( Planning7_PutOffGstHse( arg ) );					// 23	8	0 - 5		| 0 - 0.571		| 0 - 1		|
	//Points scored within level just ended - not prev levels. Default Points per level is 1950 = Dots + Pills, no Ghosts.
	Points_Max[l-1] = arg[s-1].Points - sum( Points_Max );						// 24	3	3200-62910	| 1303 - 4194	| 0.311 - 1	|
	Points_Max.push_back( 0.0 );
	
	R1_AvgTimeToEatChrry.push_back( resource[0] );								// 25	2	0 - 142		|				| 0 - 1		|

	R2_AvgChrryPerLvl.push_back( resource[1] );									// 26	8	0 - 1		|				| 0 - 1		|	

	S1b_AvgStates.push_back( arg[s-1].State );									// 27	4	220 - 478	|				| 0.459 - 1	|

	S1c_AvgCycles.push_back( arg[s-1].Cycle );									// 28	5	2174 - 9598	|				| 0.226 - 1	|
	// For a single level this should be equivalent to R2_AvgChryPerLvl
	S2a_LivesGained.push_back( simple2[0] );									// 29	11	0 - 16		| 0 - 1			| 0 - 1		|

	S2b_LivesLost.push_back( simple2[1] );										// 30	1	0 - 19		|				| 0 - 1 	|

	S4_TeleportUse.push_back( Simple4_TeleportUse( arg ) );						// 31	2	0 - 31		| 0 - 3			| 0 - 1 	|

//	result.push_back( Thorough2_Hoover( arg ) );								// .	.	1 - 47		| 0.07 - 5.25	| 0.013 - 1	|
//	feature.push_back( "T2_HooversDots" );										// NOT AMONG FEATS FROM 50 RULES

/**
 *	SUM THE SAVED DERIVED FEATURE SCORES FROM PREVIOUS LEVELS, ASSIGN TO deque[0] (except PointsMax - that total is in the raw data).
 */
	A1_AtkGstHse[0] = recAvg( A1_AtkGstHse );
	A3_GhostKills[0] = recAvg( A3_GhostKills );
	A4_HuntAfterPill[0] = recAvg( A4_HuntAfterPill );
	A6_Chase¬Dots[0] = recAvg( A6_Chase¬Dots );
	C1b_ThreatPerception_NumKilled[0] = recAvg( C1b_ThreatPerception_NumKilled );
	C2b_AvgDstPacAtkCntrd[0] = recAvg( C2b_AvgDstPacAtkCntrd );
	C3_CloseCalls[0] = recAvg( C3_CloseCalls );
	C4_CaughtAfterHunt[0] = recAvg( C4_CaughtAfterHunt );
	C5_MvsWoutGain[0] = recAvg( C5_MvsWoutGain );
	C6_PntsOvrLives[0] = recAvg( C6_PntsOvrLives );
	C7_KilledAtHouse[0] = recAvg( C7_KilledAtHouse );
	ChryBool_Sum[0] = recAvg( ChryBool_Sum );
	CycleCountPerState_SDev[0] = recAvg( CycleCountPerState_SDev );
	D1_CyclesPerMove[0] = recAvg( D1_CyclesPerMove );
	D2_Osclltng[0] = recAvg( D2_Osclltng );
	Lives_SDev[0] = recAvg( Lives_SDev );
	P1_LureToPill[0] = recAvg( P1_LureToPill );
	P1b_2GstsLrd2Pll[0] = recAvg( P1b_2GstsLrd2Pll );
	P1c_GstsEtnOnPll[0] = recAvg( P1c_GstsEtnOnPll );
	P1d_PacEtnB4GetPll[0] = recAvg( P1d_PacEtnB4GetPll );
	P3_CntDtsB4Pll1[0] = recAvg( P3_CntDtsB4Pll1 );
	P4a_HuntGst1_AvgSpd[0] = recAvg( P4a_HuntGst1_AvgSpd );
	P4b_HuntGst2_AvgSpd[0] = recAvg( P4b_HuntGst2_AvgSpd );
	P7_PutOffGstHse[0] = recAvg( P7_PutOffGstHse );
	R1_AvgTimeToEatChrry[0] = recAvg( R1_AvgTimeToEatChrry );
	R2_AvgChrryPerLvl[0] = recAvg( R2_AvgChrryPerLvl );
	S1b_AvgStates[0] = recAvg( S1b_AvgStates );
	S1c_AvgCycles[0] = recAvg( S1c_AvgCycles );
	S2a_LivesGained[0] = recAvg( S2a_LivesGained );
	S2b_LivesLost[0] = recAvg( S2b_LivesLost );
	S4_TeleportUse[0] = recAvg( S4_TeleportUse );
	S5_GhostHunts[0] = recAvg( S5_GhostHunts );

/**
 *	CODE TO LOG ALL DERIVED FEATURES TO A .csv FILE - SEE CLASSIFY FUNCTION ABOVE.
 */
	result.push_back( A1_AtkGstHse[l] );
	result.push_back( A3_GhostKills[l] );
	result.push_back( A4_HuntAfterPill[l] );
	result.push_back( A6_Chase¬Dots[l] );
	result.push_back( C1b_ThreatPerception_NumKilled[l] );
	result.push_back( C2b_AvgDstPacAtkCntrd[l] );
	result.push_back( C3_CloseCalls[l] );
	result.push_back( C4_CaughtAfterHunt[l] );
	result.push_back( C5_MvsWoutGain[l] );
	result.push_back( C6_PntsOvrLives[l] );
	result.push_back( C7_KilledAtHouse[l] );
	result.push_back( ChryBool_Sum[l] );
	result.push_back( CycleCountPerState_SDev[l] );
	result.push_back( D1_CyclesPerMove[l] );
	result.push_back( D2_Osclltng[l] );
	result.push_back( Lives_SDev[l] );
	result.push_back( P1_LureToPill[l] );
	result.push_back( P1b_2GstsLrd2Pll[l] );
	result.push_back( P1c_GstsEtnOnPll[l] );
	result.push_back( P1d_PacEtnB4GetPll[l] );
	result.push_back( P3_CntDtsB4Pll1[l] );
	result.push_back( P4a_HuntGst1_AvgSpd[l] );
	result.push_back( P4b_HuntGst2_AvgSpd[l] );
	result.push_back( P7_PutOffGstHse[l] );
	result.push_back( Points_Max[l-1] );
	result.push_back( R1_AvgTimeToEatChrry[l] );
	result.push_back( R2_AvgChrryPerLvl[l] );
	result.push_back( S1b_AvgStates[l] );
	result.push_back( S1c_AvgCycles[l] );
	result.push_back( S2a_LivesGained[l] );
	result.push_back( S2b_LivesLost[l] );
	result.push_back( S4_TeleportUse[l] );
	result.push_back( S5_GhostHunts[l] );
/** /
	//	This OLD! code logs out the entire list of cycles-per-state counts - needs to be matched to code in Aggregate3
	logData += "Cycles = ,"
	temp = Aggregate3_CycleCountPerStateSDev( arg );
	for( int i = 1; i < temp.size(); i++ ){
		sprintf_s( out, 99, "%f", temp[i] );
		logData += out; logData += ",";
	}
	logData += "\n";	
/**/
}

/**
 *	Description: Log results of feature derivers and classifiers to a log file
 */
void Classify()
{
/**/
	int notcq = 0, conq = 0;
	usage.assign( 33, 0 );	//Prepare holder for feature usage stats 
	fired.assign( 50, 0 );	// +1 is Conq rule firing, -1 is Not_Conq rule firing, 0 is no fire

	float classification = ClassifyConqueror_Level();

	for( int i = 0; i < 50; i++ )
	{
		if( fired[i] == 1 ){
			conq++;
			logData += rules[i];
		}
		if( fired[i] == -1 ){
			notcq++;
			logData += rules[i];
		}
	}

	logData += "\n";
	sprintf_s( out, 99, "%f", classification );
	logData += out;		logData += ",Score,Usage\n";

	logData += "Conqueror,,";
	sprintf_s( out, 99, "%d", conq );
	logData += out;		logData += "\n";

	logData += "Not_Conqueror,,";
	sprintf_s( out, 99, "%d", notcq );
	logData += out;		logData += "\n\n";

	//Output the feature names, values derived, and usage in rules.
	for( int i = 0; i < feature.size(); i++ )
	{
		logData += feature[i];	logData += ","; 
		sprintf_s( out, 99, "%f", result[i] );
		logData += out;			logData += ",";
		sprintf_s( out, 99, "%d", usage[i] );
		logData += out;			logData += "\n";
	}

	/* NEED THESE LINES BETWEEN EVERY LEVEL */
	result.clear();
	usage.clear();
	fired.clear();

	sprintf_s(out, 99, "%d", Level);
	string file = "/Classify-";
	file += out;
	file += ".csv";

	string fullpath = "C:/PacPlayd/Logs/";
	fullpath += gameDir;
	fullpath += file;
	if( fopen_s( &stateFile, fullpath.c_str(), "wt" ) != 0 )
		cout << "The file " << fullpath.c_str() << " was not opened." << endl;
	
	//Put the latest game state data onto the output file stream
	fputs( logData.c_str(), stateFile );

	logData.clear();
}

/**
 *	Notes: All rules are being interpreted to work for a single level. Thus each threshold number 
 *		from a rule must be mapped to a sensible number to be derived from a single level...see Mapping_NrmlsToAvgs.xlsx
 */

/**
 *	Description: Implement rules from 50 Rule ensemble to classify player as a Conqueror or Not for one level.
 */
float ClassifyConqueror_Level()
{
	float conq = 0.0;	//As a float the modifier applied by each rule can be weighted by reasonable, fractional, amounts - e.g. +0.5, +1.2, etc
	int l = Level;

	/////////	Rules with one condition		//////////
	/**
	 *	Rank			Rule
	 *	0.235264706		Rule 2 Not (19; 0.842)  if Lives_SDev > 0.597 then Not
	 *	0.2133			Rule 2 Not (14.22; 0.915) if D2_Osclltng_Avg > 0.678 then Not
	 *	0.213213115		Rule 1 Not (14; 0.929)  if HuntAfterPill_Avg <= 0.212 then Not		// direct map - 0.324
	 *	0.161826824		Rule 1 Conqr (12.992; 0.847) if Points_Max_Avg <= 0.446 then Conqr	// This rule implies a value less than the minimum rquired to complete a level - could only fire on last level.
	 *	0.147540984		Rule 3 Conqr (9; 1.0) if HuntAfterPill_Avg > 0.563 then Conqr		// direct map - 0.84
	 *	0.147540984		Rule 2 Not (10; 0.9) if S1.c_AvgCycles > 0.573 then Not
	 *	0.133333333		Rule 1 Not (10; 1.0) if ChryBool_Sum_Avg <= 0.137 then Not
	 *	0.106666667		Rule 5 Not (8; 1.0) if C5_MvsW-outGain_Avg > 0.584 then Not
	 *	0.106509836		Rule 3 Conqr (7.219; 0.9) if P7_PutOffGstHse_Avg > 0.656 then Conqr	// direct map - 0.375
	 */
	if( Lives_SDev[0] > 1.5 ){ conq -= 0.235264706; usage[15]++; fired[0]--; weight[0] -= 0.01; }

	if( D2_Osclltng[0] > 122 ){ conq -= 0.2133; usage[14]++; fired[1]--; }

	if( A4_HuntAfterPill[l] <= 1 ){ conq -= 0.213213115; usage[2]++; fired[2]--; }

	if( Points_Max[l] <= 1870 ){ conq += 0.161826824; usage[24]++; fired[3]++; }	// Can only fire on last level

	if( A4_HuntAfterPill[l] > 2 ){ conq += 0.147540984; usage[2]++; fired[4]++; }

	if( S1c_AvgCycles[0] > 5500 ){ conq -= 0.147540984; usage[28]++; fired[5]--; }

	if( ChryBool_Sum[0] <= 23 ){ conq -= 0.1333; usage[11]++; fired[6]--; }

	if( C5_MvsWoutGain[0] > 150 ){ conq -= 0.1067; usage[8]++; fired[7]--; }

	if( P7_PutOffGstHse[0] > 0.375 ){ conq += 0.106509836; usage[23]++; fired[8]++; }

	/////////	Rules with two conditions		//////////
	/**
	 *	Rank			Rule
	 *	0.226733333		Rule 2 Conqr (19; 0.895) if HuntAfterPill_Avg > 0.460 and C4_CaughtAfterHunt_Avg <= 0.557 then Conqr	// direct map - 0.69 & 0.695
	 *	0.213196721		Rule 1 Conqr (15; 0.867)  if C5_MvsW-outGain_Avg <= 0.413 and S2.a_LivesGained_Avg > 0.620 then Conqr	// direct map - 106 & 0.77
	 *	0.191264706		Rule 1 Conqr (14; 0.929)  if HuntAfterPill_Avg > 0.167 and S1.b_AvgStates <= 0.636 then Conqr			// direct map - 0.25 & 304
	 *	0.161764706		Rule 3 Conqr (11; 1.0) if HuntAfterPill_Avg > 0.563 and S1.c_AvgCycles <= 0.589 then Conqr				// direct map - 0.84 
	 *	0.147540984		Rule 2 Not (10; 0.9) if HuntAfterPill_Avg > 0.212 and ChryBool_Sum_Avg <= 0.138 then Not				// direct map - 0.324
	 *	0.132352941		Rule 1 Conqr (9; 1.0) if Points_Max_Avg <= 0.448 and HuntGst2_AvgSpd <= 0.017 then Conqr
	 *	0.126575738		Rule 1 Not (8.56; 0.902)  if D2_Osclltng_Avg <= 0.678 and R1_AvgTimeToEatChrry <= 0.145 then Not
	 *	0.12			Rule 1 Conqr (10; 0.9)  if ChryBool_Sum_Avg > 0.137 and CloseCalls_Avg <= 0.116 then Conqr				// direct map - 23 & 0.167
	 *	0.102941176		Rule 3 Not (7; 1.0) if ChryBool_Sum_Avg > 0.136 and Chase¬Dots_Avg > 0.450 then Not
	 *	0.311442623		Rule 1 Not (23; 0.826) if C5_MvsW-outGain_Avg > 0.305 and CloseCalls_Avg > 0.103 and CloseCalls_Avg <= 0.206 then Not
	 */
	if( A4_HuntAfterPill[l] > 2 && C4_CaughtAfterHunt[0] <= 0.69625 ){ conq += 0.226733333; usage[2]++; usage[7]++; fired[9]++; }

	if( C5_MvsWoutGain[0] <= 106 && S2a_LivesGained[0] > 0.775 ){ conq += 0.213196721; usage[8]++; usage[29]++; fired[10]++; }

	if( A4_HuntAfterPill[l] >= 1 && S1b_AvgStates[0] <= 304 ){ conq += 0.191264706; usage[2]++; usage[27]++; fired[11]++; }

	if( A4_HuntAfterPill[l] > 2 && S1c_AvgCycles[0] <= 5653 ){ conq += 0.161764706; usage[2]++; usage[28]++; fired[12]++; }

	if( A4_HuntAfterPill[l] > 1 && ChryBool_Sum[0] <= 23 ){ conq -= 0.147540984; usage[2]++; usage[11]++; fired[13]--; }

	if( Points_Max[l] <= 1878 && P4b_HuntGst2_AvgSpd[0] <= 39 ){ conq += 0.132352941; usage[24]++; usage[22]++; fired[14]++; }	// Can only fire on last level

	if( D2_Osclltng[0] <= 123 && R1_AvgTimeToEatChrry[0] <= 21 ){ conq -= 0.126575738; usage[14]++; usage[25]++; fired[15]--; }

	if( ChryBool_Sum[0] > 23 && C3_CloseCalls[0] <= 0.174 ){ conq += 0.12; usage[11]++; usage[25]++; fired[16]++; }

	if( ChryBool_Sum[0] > 22 && A6_Chase¬Dots[0] > 5.25 ){ conq -= 0.102941176; usage[11]++; usage[3]++; fired[17]--; }

	if( C5_MvsWoutGain[0] > 78 && C3_CloseCalls[0] > 0.1545 ){ conq -= 0.311442623; usage[8]++; usage[6]++; usage[6]++; fired[18]--; }

	/////////	Rules with three conditions		//////////
	/**
	 *	Rank		Rule
	 *	0.393491803	Rule 2 Not (27; 0.889) if C5_MvsW-outGain_Avg > 0.413 and HuntGst2_AvgSpd <= 0.504 and P7_PutOffGstHse_Avg <= 0.469 then Not
	 *	0.327836066	Rule 2 Conqr (22; 0.909)  if S2.a_LivesGained_Avg > 0.620 and S1.c_AvgCycles <= 0.589 and Lives_SDev <= 0.659 then Conqr
	 *	0.327737705	Rule 1 Not (21; 0.952) if HuntAfterPill_Avg <= 0.563 and S2.a_LivesGained_Avg <= 0.627 and Lives_SDev <= 0.578 then Not
	 *	0.323647059	Rule 2 Conqr (28; 0.786) if S2.a_LivesGained_Avg > 0.620 and C4_CaughtAfterHunt_Avg <= 0.557 and S1.c_AvgCycles <= 0.680 then Conqr
	 *	0.294117647	Rule 1 Conqr (25; 0.8)  if S1.c_AvgCycles <= 0.458 and HuntAfterPill_Avg > 0.181 and S2.a_LivesGained_Avg > 0.429 then Conqr
	 *	0.279529412	Rule 2 Conqr (22; 0.864) if C6_PntsOvrLives <= 0.506 and Lives_SDev <= 0.603 and S2.a_LivesGained_Avg > 0.620 then Conqr
	 *	0.266666667	Rule 2 Conqr (25; 0.8)  if ChryBool_Sum_Avg > 0.137 and S2.a_LivesGained_Avg > 0.620 and Lives_SDev <= 0.602 then Conqr
	 *	0.220705882	Rule 3 Not (16; 0.938)  if Points_Max_Avg > 0.448 and P3_CntDtsb4Pll1_Avg > 0.191 and HuntGst1_AvgSpd <= 0.676 then Not
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
	if( C5_MvsWoutGain[0] > 106 && P4b_HuntGst2_AvgSpd[0] <= 1163 && P7_PutOffGstHse[0] <= 0.2678 ){ conq -= 0.393491803; usage[8]++; usage[22]++; usage[23]++; fired[19]--; }

	if( S2a_LivesGained[0] > 0.775 && S1c_AvgCycles[0] <= 5653 && Lives_SDev[0] <= 1.674 ){ conq += 0.327836066; usage[29]++; usage[28]++; usage[15]++; fired[20]++; }

	if( A4_HuntAfterPill[l] <= 2 && S2a_LivesGained[0] <= 0.78375 && Lives_SDev[0] <= 1.468 ){ conq -= 0.327737705; usage[2]++; usage[29]++; usage[15]++; fired[21]--; }

	if( S2a_LivesGained[0] > 0.775 && C4_CaughtAfterHunt[0] <= 0.69625 && S1c_AvgCycles[0] <= 6527 ){ conq += 0.323647059; usage[29]++; usage[7]++; usage[28]++; fired[22]++; }

	if( S1c_AvgCycles[0] <= 4396 && A4_HuntAfterPill[l] > 1 && S2a_LivesGained[0] > 0.53625 ){ conq += 0.294117647; usage[28]++; usage[2]++; usage[29]++; fired[23]++; }

	if( C6_PntsOvrLives[0] <= 1897 && Lives_SDev[0] <= 1.54 && S2a_LivesGained[0] > 0.775 ){ conq += 0.279529412; usage[9]++; usage[15]++; usage[29]++; fired[24]++; }

	if( ChryBool_Sum[0] > 23 && S2a_LivesGained[0] > 0.775 && Lives_SDev[0] <= 1.53 ){ conq += 0.2667; usage[11]++; usage[29]++; usage[15]++; fired[25]++; }

	if( Points_Max[l] > 1950 && P3_CntDtsB4Pll1[0] > 4 && P4a_HuntGst1_AvgSpd[0] <= 236 ){ conq -= 0.220705882; usage[24]++; usage[20]++; usage[21]++; fired[26]--; }

	//This rule selects play where PutOffGstHse occurs between 1 sixth and 1 quarter times per level - can only be measured over 6 or more levels.
	if( R2_AvgChrryPerLvl[0] > 0.333 && P7_PutOffGstHse[0] <= 0.250098 && P7_PutOffGstHse[0] >= 0.166732 ){ conq -= 0.219350147; usage[26]++; usage[23]++; usage[23]++; fired[27]--; }

	if( A4_HuntAfterPill[l] <= 2 && R2_AvgChrryPerLvl[0] <= 0.802 && A3_GhostKills[0] > 0.46 ){ conq -= 0.213114754; usage[2]++; usage[26]++; usage[1]++; fired[28]--; }

	//This rule's 2nd condition can only NOT fire on the last level, depending on whether Pacman dies after 3rd or fourth Pill - but we let it fire anyway to balance available points between Conq & Not.
	if( A4_HuntAfterPill[l] > 1 && S5_GhostHunts[l] > 3.764 && CycleCountPerState_SDev[0] > 13 ){ conq -= 0.2; usage[2]++; usage[32]++; usage[12]++; fired[29]--; }

	if( A4_HuntAfterPill[l] > 0 && S1b_AvgStates[0] <= 305 && C6_PntsOvrLives[0] <= 1912 ){ conq += 0.176455882; usage[2]++; usage[27]++; usage[9]++; fired[30]++; }

	//This rule's 2nd condition can only NOT fire on the last level, depending on whether Pacman dies after 3rd or fourth Pill - so we only let it fire on the last level.
	if( GameOver && S1b_AvgStates[0] <= 394 && C4_CaughtAfterHunt[0] > 0.445 && S5_GhostHunts[l] > 3.644 ){ conq -= 0.174110588; usage[27]++; usage[7]++; usage[32]++; fired[31]--; }

	if( R2_AvgChrryPerLvl[0] > 0.333 && P7_PutOffGstHse[0] > 0.250098 && Lives_SDev[0] <= 1.52 ){ conq += 0.163206441; usage[26]++; usage[23]++; usage[15]++; fired[32]++; }

	if( A4_HuntAfterPill[l] <= 2 && S2a_LivesGained[0] > 0.78375 && Lives_SDev[0] > 1.55 ){ conq -= 0.131163934; usage[2]++; usage[29]++; usage[15]++; fired[33]--; }

	if( ChryBool_Sum[0] > 23 && C3_CloseCalls[0] > 0.174 && A4_HuntAfterPill[l] > 2 ){ conq += 0.119973333; usage[11]++; usage[6]++; usage[2]++; fired[34]++; }

	if( S2a_LivesGained[0] <= 0.775 && A4_HuntAfterPill[l] > 1 && A6_Chase¬Dots[0] <= 1.447 ){ conq += 0.117647059; usage[29]++; usage[2]++; usage[3]++; fired[35]++; }

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
	if( A4_HuntAfterPill[l] > 0 && S1b_AvgStates[0] > 304 && C3_CloseCalls[0] > 0.1665 && S4_TeleportUse[0] > 0.858 )
	{ conq -= 0.374970588; usage[2]++; usage[27]++; usage[6]++; usage[31]++; fired[36]--; }

	if( C5_MvsWoutGain[0] > 106 && P7_PutOffGstHse[0] <= 0.2678 && C1b_ThreatPerception_NumKilled[0] <= 2.5 && A4_HuntAfterPill[l] <= 2 )
	{ conq -= 0.34442623; usage[8]++; usage[23]++; usage[4]++; usage[2]++; fired[37]--; }

	if( Lives_SDev[0] <= 1.54 && A4_HuntAfterPill[l] > 1 && CycleCountPerState_SDev[0] <= 15.7575 && C4_CaughtAfterHunt[0] <= 0.45 )
	{ conq += 0.294264706; usage[15]++; usage[2]++; usage[12]++; usage[7]++; fired[38]++; }

	//This rule's 2nd condition can ONLY fire on the last level, depending on whether Pacman dies after 3rd or fourth Pill - so we only let it fire on the last level.
	if( GameOver && A4_HuntAfterPill[l] > 1 && S5_GhostHunts[l] <= 3.764 && A6_Chase¬Dots[0] <= 5.3 && ChryBool_Sum[0] > 24 )
	{ conq += 0.2933; usage[2]++; usage[32]++; usage[3]++; usage[11]++; fired[39]++; }

	if( C5_MvsWoutGain[0] > 78 && ChryBool_Sum[0] > 23 && R1_AvgTimeToEatChrry[0] > 36 && A4_HuntAfterPill[l] <= 2 )
	{ conq -= 0.278770492; usage[8]++; usage[11]++; usage[25]++; usage[2]++; fired[40]--; }

	if( R2_AvgChrryPerLvl[0] > 0.333 && P7_PutOffGstHse[0] <= 0.166732 && C1b_ThreatPerception_NumKilled[0] <= 1 && S2b_LivesLost[0] <= 0.737 )
	{ conq -= 0.226133647; usage[26]++; usage[23]++; usage[4]++; usage[30]++; fired[41]--; }

	if( A4_HuntAfterPill[l] <= 2 && R2_AvgChrryPerLvl[0] <= 0.802 && A3_GhostKills[0] <= 0.46 && P1c_GstsEtnOnPll[0] <= 0.6588 )
	{ conq -= 0.196688525; usage[2]++; usage[26]++; usage[1]++; usage[18]++; fired[42]--; }

	if( A4_HuntAfterPill[l] > 0 && A1_AtkGstHse[0] <= 0.4288 && A6_Chase¬Dots[0] <= 4.843 && P1c_GstsEtnOnPll[0] > 0.5 )
	{ conq += 0.19175; usage[2]++; usage[0]++; usage[3]++; usage[18]++; fired[43]++; }

	if( A4_HuntAfterPill[l] <= 2 && S2a_LivesGained[0] > 0.78375 && Lives_SDev[0] <= 1.55 && P1_LureToPill[0] > 0.08277 )
	{ conq += 0.163934426; usage[2]++; usage[29]++; usage[15]++; usage[16]++; fired[44]++; }

	if( A4_HuntAfterPill[l] <= 2 && S2a_LivesGained[0] > 0.775 && D1_CyclesPerMove[0] <= 15.6 && Lives_SDev[0] <= 1.4 )
	{ conq += 0.161764706; usage[2]++; usage[29]++; usage[13]++; usage[15]++; fired[45]++; }

	if( R2_AvgChrryPerLvl[0] > 0.333 && P7_PutOffGstHse[0] <= 0.166732 && C1b_ThreatPerception_NumKilled[0] > 1 && S4_TeleportUse[0] <= 1.923 )
	{ conq += 0.131411765; usage[26]++; usage[23]++; usage[4]++; usage[31]++; fired[46]++; }

	/////////	Rules with more than four conditions		//////////
	/**
	 *	Rank		Rule
	 *	0.161764706	Rule 2 Not (11; 1.0)  if P1.e_PacEtnB4GetPll_Avg <= 0 and C1.b_ThreatPrcptn-#Killed <= 0.333 and R2_AvgChrryPerLvl > 0.333 and P1.c_GstsEtnOnPll_Avg <= 0.598 and P1.b_2GstsLrd2Pll_Avg <= 0.167 and HuntGst1_AvgSpd <= 0.640 and R2_AvgChrryPerLvl <= 0.750 and Chase¬Dots_Avg > 0.120 then Not
	 *	0.113455882	Rule 3 Conqr (7.715; 1.0)  if HuntAfterPill_Avg > 0.167 and A1_AtkGstHse_Avg > 0.134 and P1.e_PacEtnB4GetPll_Avg <= 0 and Lives_SDev <= 0.597 and CloseCalls_Avg <= 0.359 and C7_KilledAtHouse_Avg <= 0.148 then Conqr
	 *	0.4536		Rule 1 Conqr (45; 0.756)  if C5_MvsW-outGain_Avg <= 0.584 and HuntAfterPill_Avg > 0.171 and C2.b_AvgDstPacAtk-Cntrd <= 0.941 and C4_CaughtAfterHunt_Avg <= 0.554 and ChryBool_Sum_Avg > 0.136 then Conqr
	 */
	if( P1d_PacEtnB4GetPll[0] <= 0 && C1b_ThreatPerception_NumKilled[0] <= 2 && R2_AvgChrryPerLvl[0] > 0.333 && P1c_GstsEtnOnPll[0] <= 2.1528 && P1b_2GstsLrd2Pll[0] <= 0.167 && P4a_HuntGst1_AvgSpd[0] <= 223 && R2_AvgChrryPerLvl[0] <= 0.750 && A6_Chase¬Dots[0] > 1.4 )
	{
		conq -= 0.161764706; 
		usage[19]++; usage[4]++; usage[26]++; usage[18]++; usage[17]++; usage[21]++; usage[26]++; usage[3]++; fired[47]--;
	}

	if( A4_HuntAfterPill[l] > 0 && A1_AtkGstHse[0] > 0.4288 && P1d_PacEtnB4GetPll[0] <= 0 && Lives_SDev[0] <= 1.52 && C3_CloseCalls[0] <= 0.5385 && C7_KilledAtHouse[0] <= 0.222 )
	{
		conq += 0.113455882;
		usage[2]++; usage[0]++; usage[19]++; usage[15]++; usage[6]++; usage[10]++; fired[48]++;
	}

	if( C5_MvsWoutGain[0] <= 150 && A4_HuntAfterPill[l] > 0 && C2b_AvgDstPacAtkCntrd[0] <= 13.26 && C4_CaughtAfterHunt[0] <= 0.6925 && ChryBool_Sum[0] > 22 )
	{
		conq += 0.4536;
		usage[8]++; usage[2]++; usage[5]++; usage[7]++; usage[11]++; fired[49]++;
	}
/**/
	return conq;
}

/**
 *	Add 50 rule strings to deque
 */
void InitStrings()
{
feature.push_back( "A1_AtkGstHse" );
feature.push_back( "A3_GhostKills" );
feature.push_back( "A4_HuntAfterPill" );
feature.push_back( "A6_Chase¬Dots" );
feature.push_back( "C1b_ThreatPerception_NumKilled" );
feature.push_back( "C2.b_AvgDstPacAtkCntrd" );	
feature.push_back( "C3_CloseCalls" );
feature.push_back( "C4_CaughtAfterHunt" );
feature.push_back( "C5_MvsWoutGain" );
feature.push_back( "C6_PntsOvrLives" );	
feature.push_back( "C7_KilledAtHouse" );
feature.push_back( "ChryBool_Sum" );
feature.push_back( "CycleCountPerState_SDev" );
feature.push_back( "D1_CyclesPerMove" );	
feature.push_back( "D2_Osclltng" );
feature.push_back( "Lives_SDev" );
feature.push_back( "P1_LureToPill" );
feature.push_back( "P1b_2GstsLrd2Pll" );
feature.push_back( "P1c_GstsEtnOnPll" );
feature.push_back( "P1d_PacEtnB4GetPll" );
feature.push_back( "P3_CntDtsB4Pll1" );	
feature.push_back( "P4a_HuntGst1_AvgSpd" );
feature.push_back( "P4b_HuntGst2_AvgSpd" );
feature.push_back( "P7_PutOffGstHse" );
feature.push_back( "Points_Max" );
feature.push_back( "R1_AvgTimeToEatChrry" );
feature.push_back( "R2_AvgChrryPerLvl" );
feature.push_back( "S1b_AvgStates" );
feature.push_back( "S1c_AvgCycles" );
feature.push_back( "S2a_LivesGained" );
feature.push_back( "S2b_LivesLost" );
feature.push_back( "S4_TeleportUse" );
feature.push_back( "S5_GhostHunts" );

rules.push_back( "if( Lives_SDev > 1.5 )then NotCq\n\n" );
rules.push_back( "if( D2_Osclltng > 122 )then NotCq\n\n" );
rules.push_back( "if( A4_HuntAfterPill <= 1 )then NotCq\n\n" );
rules.push_back( "if( Points_Max <= 1895 )then Conqr\n\n" );
rules.push_back( "if( A4_HuntAfterPill > 2 )then Conqr\n\n" );
rules.push_back( "if( S1c_AvgCycles > 5500 )then NotCq\n\n" );
rules.push_back( "if( ChryBool_Sum <= 24 )then NotCq\n\n" );
rules.push_back( "if( C5_MvsWoutGain > 150 )then NotCq\n\n" );
rules.push_back( "if( P7_PutOffGstHse == 1 )then Conqr\n\n" );
rules.push_back( "if( A4_HuntAfterPill > 2  and  C4_CaughtAfterHunt <= 2 )then Conqr\n\n" );
rules.push_back( "if( C5_MvsWoutGain <= 106  and  S2a_LivesGained == 1 )then Conqr\n\n" );
rules.push_back( "if( A4_HuntAfterPill >= 1  and  S1b_AvgStates <= 304 )then Conqr\n\n" );
rules.push_back( "if( A4_HuntAfterPill > 2  and  S1c_AvgCycles <= 5650 )then Conqr\n\n" );
rules.push_back( "if( A4_HuntAfterPill > 1  and  ChryBool_Sum <= 23 )then NotCq\n\n" );
rules.push_back( "if( Points_Max <= 1950  and  P4b_HuntGst2_AvgSpd <= 38 )then Conqr\n\n" );
rules.push_back( "if( D2_Osclltng <= 123  and  R1_AvgTimeToEatChrry <= 21 )then NotCq\n\n" );
rules.push_back( "if( ChryBool_Sum > 23  and  C3_CloseCalls <= 2 )then Conqr\n\n" );
rules.push_back( "if( ChryBool_Sum > 23  and  A6_Chase¬Dots > 5 )then NotCq\n\n" );
rules.push_back( "if( C5_MvsWoutGain > 79  and  C3_CloseCalls == 1 )then NotCq\n\n" );
rules.push_back( "if( C5_MvsWoutGain > 106  and  P4b_HuntGst2_AvgSpd <= 1164  and  P7_PutOffGstHse == 0 )then NotCq\n\n" );
rules.push_back( "if( S2a_LivesGained == 1  and  S1c_AvgCycles <= 5650  and  Lives_SDev <= 1.654 )then Conqr\n\n" );
rules.push_back( "if( A4_HuntAfterPill <= 2  and  S2a_LivesGained == 0  and  Lives_SDev <= 1.469 )then NotCq\n\n" );
rules.push_back( "if( S2a_LivesGained == 1  and  C4_CaughtAfterHunt <= 2  and  S1c_AvgCycles <= 6530 )then Conqr\n\n" );
rules.push_back( "if( S1c_AvgCycles <= 4410  and  A4_HuntAfterPill > 1  and  S2a_LivesGained == 1 )then Conqr\n\n" );
rules.push_back( "if( C6_PntsOvrLives <= 1900  and  Lives_SDev <= 1.54  and  S2a_LivesGained == 1 )then Conqr\n\n" );
rules.push_back( "if( ChryBool_Sum > 23  and  S2a_LivesGained == 1  and  Lives_SDev <= 1.54 )then Conqr\n\n" );
rules.push_back( "if( Points_Max > 1950  and  P3_CntDtsB4Pll1 > 19  and  P4a_HuntGst1_AvgSpd <= 230 )then NotCq\n\n" );
rules.push_back( "if( R2_AvgChrryPerLvl == 1  and  P7_PutOffGstHse == 0  and  P7_PutOffGstHse == 1 )then NotCq\n\n" );
rules.push_back( "if( A4_HuntAfterPill <= 2  and  R2_AvgChrryPerLvl == 0  and  A3_GhostKills > 0 )then NotCq\n\n" );
rules.push_back( "if( A4_HuntAfterPill > 1  and  S5_GhostHunts > 3.75  and  CycleCountPerState_SDev > 13 )then NotCq\n\n" );
rules.push_back( "if( A4_HuntAfterPill > 0  and  S1b_AvgStates <= 305  and  C6_PntsOvrLives <= 1900 )then Conqr\n\n" );
rules.push_back( "if( S1b_AvgStates <= 393  and  C4_CaughtAfterHunt > 2  and  S5_GhostHunts > 3.75 )then NotCq\n\n" );
rules.push_back( "if( R2_AvgChrryPerLvl == 1  and  P7_PutOffGstHse == 1  and  Lives_SDev <= 1.52 )then Conqr\n\n" );
rules.push_back( "if( A4_HuntAfterPill <= 2  and  S2a_LivesGained == 1  and  Lives_SDev > 1.55 )then NotCq\n\n" );
rules.push_back( "if( ChryBool_Sum > 23  and  C3_CloseCalls > 0  and  A4_HuntAfterPill > 2 )then Conqr\n\n" );
rules.push_back( "if( S2a_LivesGained == 0  and  A4_HuntAfterPill > 1  and  A6_Chase¬Dots <= 1 )then Conqr\n\n" );
rules.push_back( "if( A4_HuntAfterPill > 0  and  S1b_AvgStates > 304  and  C3_CloseCalls > 0  and  S4_TeleportUse > 1 )then NotCq\n\n" );
rules.push_back( "if( C5_MvsWoutGain > 106  and  P7_PutOffGstHse == 0  and  C1b_ThreatPerception_NumKilled <= 2  and  A4_HuntAfterPill <= 2 )then NotCq\n\n" );
rules.push_back( "if( Lives_SDev <= 1.54  and  A4_HuntAfterPill > 1  and  CycleCountPerState_SDev <= 15.77  and  C4_CaughtAfterHunt <= 1 )then Conqr\n\n" );
rules.push_back( "if( A4_HuntAfterPill > 1  and  S5_GhostHunts <= 3.75  and  A6_Chase¬Dots <= 5  and  ChryBool_Sum > 25 )then Conqr\n\n" );
rules.push_back( "if( C5_MvsWoutGain > 79  and  ChryBool_Sum > 23  and  R1_AvgTimeToEatChrry > 36  and  A4_HuntAfterPill <= 2 )then NotCq\n\n" );
rules.push_back( "if( R2_AvgChrryPerLvl == 1  and  P7_PutOffGstHse == 0  and  C1b_ThreatPerception_NumKilled <= 1  and  S2b_LivesLost <= 14 )then NotCq\n\n" );
rules.push_back( "if( A4_HuntAfterPill <= 2  and  R2_AvgChrryPerLvl == 1  and  A3_GhostKills <= 0  and  P1c_GstsEtnOnPll <= 1 )then NotCq\n\n" );
rules.push_back( "if( A4_HuntAfterPill > 0  and  A1_AtkGstHse <= 0  and  A6_Chase¬Dots <= 5  and  P1c_GstsEtnOnPll > 0 )then Conqr\n\n" );
rules.push_back( "if( A4_HuntAfterPill <= 2  and  S2a_LivesGained == 1  and  Lives_SDev <= 1.55  and  P1_LureToPill > 0 )then Conqr\n\n" );
rules.push_back( "if( A4_HuntAfterPill <= 2  and  S2a_LivesGained == 1  and  D1_CyclesPerMove <= 15.45  and  Lives_SDev <= 1.4 )then Conqr\n\n" );
rules.push_back( "if( R2_AvgChrryPerLvl == 1  and  P7_PutOffGstHse == 0  and  C1b_ThreatPerception_NumKilled > 1  and  S4_TeleportUse <= 2 )then Conqr\n\n" );
rules.push_back( "if( P1d_PacEtnB4GetPll <= 0  and  C1b_ThreatPerception_NumKilled <= 2  and  R2_AvgChrryPerLvl > 0.333  and  P1c_GstsEtnOnPll <= 2  and  P1b_2GstsLrd2Pll <= 1  and  P4a_HuntGst1_AvgSpd <= 224  and  R2_AvgChrryPerLvl <= 0.750  and  A6_Chase¬Dots > 1 )then NotCq\n\n" );
rules.push_back( "if( A4_HuntAfterPill > 0  and  A1_AtkGstHse > 0  and  P1d_PacEtnB4GetPll <= 0  and  Lives_SDev <= 1.52  and  C3_CloseCalls <= 3  and  C7_KilledAtHouse <= 2 )then Conqr\n\n" );
rules.push_back( "if( C5_MvsWoutGain <= 150  and  A4_HuntAfterPill > 0  and  C2b_AvgDstPacAtkCntrd <= 13.26  and  C4_CaughtAfterHunt <= 2  and  ChryBool_Sum > 23 )then Conqr\n\n" );

}

//End of file
