/**
 *	Author: Ben Cowley, 2008.
 *	Functions for CLASSIFYING players based on features from game log
 */
#include "universals.h"
#include "Classifier.h"
#include "Deriver.h"

deque <float> res_Avg;
deque <float> res_Wdw;
deque <float> res_Nve;
deque <char*> feature;
deque <int> usage_Wdw;
deque <int> usage_Avg;
deque <int> usage_Nve;
deque <int> fired;
deque <int> weight( 50, 0 );
deque <char*> avrge_rules;
deque <char*> naive_rules;

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
 *	Description: Append all the res_Nves of feature derivation to the universal string: logData
 */
void GetFeatures( deque<PacState> arg )
{
/*	Log the individual res_Nves of each time a Pill is eaten - needs to be matched to code in Aggression4_ * /
	temp = Aggression4_HuntAfterPill( arg );
	res_Nve.push_back( temp[0] );
	feature.push_back( "Pill 1" );
	res_Nve.push_back( temp[1] );
	feature.push_back( "Pill 2" );
	res_Nve.push_back( temp[2] );
	feature.push_back( "Pill 3" );
	res_Nve.push_back( temp[3] );
	feature.push_back( "Pill 4" );
/**/
	int s = arg.size();
	if( s < 2 ) return;

	int l = Level;

	deque <float> caution1 = Caution1_ThreatPrcptn( arg );	
cout << "C1 ok\t";
	deque <float> plan1 = Planning1_LureToPill( arg );		
cout << "P1 ok\t";
	deque <float> plan4 = Planning4_SpeedOfHunt(arg);		
cout << "P4 ok\t";
	deque <float> resource = Resource_TimeAndNumChry( arg );
cout << "R1 & R2 ok\t";
//	deque <float> simple = Simple1_Levels( arg );	//Not needed, required info (avg States & Cycles) is last entry in log
	deque <float> simple2 = Simple2_LivesChng( arg );		
cout << "S2 ok\t";

	//Need to list non-alphabetically - it is used later						// #	Frq	Actual		| Average		| Normals	|
	S5_GhostHunts.push_back( 4.0 - arg[s-1].Pills );							// 32	3	# hunts = # Pills used					|

	A1_AtkGstHse.push_back( Aggression1_AtkGstHse( arg ) );						// 0	2	0 - 35		| 0 - 3.2		| 0 - 1		|
cout << "A1 ok\t";
	A3_GhostKills.push_back( Aggression3_GhostKills( arg ) );					// 1	2	0.021-1.217	|				| 0.017 - 1	|
cout << "A3 ok\t";
	A4_HuntAfterPill.push_back( Aggression4_HuntAfterPill( arg ) );				// 2	25	0 - 4		| 0.125 - 1.5	| 0.083 - 1	|
cout << "A4 ok\t";
	A6_Chase¬Dots.push_back( Aggression6_Chase¬Dots( arg ) );					// 3	5	1 - 118		| 0.25 - 11.67	| 0.021 - 1	|
cout << "A6 ok\t";
	C1b_ThreatPerception_NumKilled.push_back( caution1[0] );					// 4	4	0 - 6		| 				| 0 - 1		|
cout << "C1b ok\t";
	C2b_AvgDstPacAtkCntrd.push_back( Caution2_AvgDstPacAtkGstCtrd( arg ) );		// 5	1	7.02 - 14.09|				| 0.498 - 1	|
cout << "C2b ok\t";
	C3_CloseCalls.push_back( Caution3_CloseCalls( arg ) );						// 6	6	1 - 9		| 0.071 - 1.5	| 0.048 - 1	|
cout << "C3 ok\t";
	C4_CaughtAfterHunt.push_back( Caution4_CaughtAfterHunt( arg ) );			// 7	5	0 - 9		| 0 - 1.25		| 0 - 1		|
cout << "C4 ok\t";
	C5_MvsWoutGain.push_back( Caution5_MovesWithoutGain( arg ) );				// 8	7	118 - 2404	| 57.5 - 256.5	| 0.224 - 1	|
cout << "C5 ok\t";
	C6_PntsOvrLives.push_back( arg[s-1].Points/max(simple2[0]+simple2[1], 1) );	// 9	2	142 - 3750	|				| 0.038 - 1	|
	//Alternative (check Clem): abs( arg[s-1].Lives - arg[0].Lives ) + 1;
	C7_KilledAtHouse.push_back( Caution7_KilledAtHouse( arg ) );				// 10	1	0 - 5		| 0 - 1.5		| 0 - 1 	|
cout << "C7 ok\t";
	ChryBool_Sum.push_back( Aggregate2_ChryBoolSum( arg ) );					// 11	9	25 - 2048	| 7.1 - 165.2	| 0.043 - 1	|
cout << "Agg2 ok\t";
	CycleCountPerState_SDev.push_back(Aggregate3_CycleCountPerStateSDev(arg));	// 12	2	6.9 - 57.3	|				| 0.12 - 1	|
cout << "Agg3 ok\t";
	D1_CyclesPerMove.push_back( Decisive1_CyclesPrMv( arg ) );					// 13	1	7.2 - 42.4	|				| 0.17 - 1	|
cout << "D1 ok\t";
	D2_Osclltng.push_back( Decisive2_Osclltng( arg ) );							// 14	2	124 - 2241	| 62 - 181.5	| 0.342 - 1	|
cout << "D2 ok\t";
	Lives_SDev.push_back( Aggregate1_LivesSDev( arg ) );						// 15	11	0.38 - 2.54	|				| 0.148 - 1	|
cout << "Agg1 ok\t";
	P1_LureToPill.push_back( plan1[0] );										// 16	1	0 - 24		| 0 - 2.67		| 0 - 1 	|

	P1b_2GstsLrd2Pll.push_back( plan1[2] );										// 17	1	0 - 9		| 0 - 1			| 0 - 1		|

	P1c_GstsEtnOnPll.push_back( plan1[3] );										// 18	3	0 - 37		| 0 - 3.6		| 0 - 1		|

	P1d_PacEtnB4GetPll.push_back( plan1[4] );									// 19	2	0 - 1		| 0 - 0.333		| 0 - 1		|

	P3_CntDtsB4Pll1.push_back( Planning3_CntDtsB4Pll1( arg ) );					// 20	1	6.3 - 97.6	| 0.9 - 22		| 0.041 - 1	|
cout << "P3 ok\t";
	P4a_HuntGst1_AvgSpd.push_back( plan4[0] /
							max(1, min(S5_GhostHunts[l], A3_GhostKills[l])) );	// 21	2	62 - 349	|				| 0.178 - 1	|

	P4b_HuntGst2_AvgSpd.push_back( plan4[2] / max(1, A3_GhostKills[l]-4) );		// 22	2	0 - 2307	|				| 0 - 1		|

	//This becomes binary since you can only put off the ghost house once per level.
	P7_PutOffGstHse.push_back( Planning7_PutOffGstHse( arg ) );					// 23	8	0 - 5		| 0 - 0.571		| 0 - 1		|
cout << "P7 ok\t";

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
cout << "S4 ok\n";
//	res_Nve.push_back( Thorough2_Hoover( arg ) );								// .	.	1 - 47		| 0.07 - 5.25	| 0.013 - 1	|
//	feature.push_back( "T2_HooversDots" );										// NOT AMONG FEATS FROM 50 RULES

/**
 *	SUM THE SAVED DERIVED FEATURE SCORES FROM PREVIOUS LEVELS, ASSIGN TO deque[0] (except PointsMax - that total is in the raw data).
 */
	res_Avg.assign( 33, 0 );
	res_Avg[0] = A1_AtkGstHse[0] = recAvg( A1_AtkGstHse );
	res_Avg[1] = A3_GhostKills[0] = recAvg( A3_GhostKills );
	res_Avg[2] = A4_HuntAfterPill[0] = recAvg( A4_HuntAfterPill );
	res_Avg[3] = A6_Chase¬Dots[0] = recAvg( A6_Chase¬Dots );
	res_Avg[4] = C1b_ThreatPerception_NumKilled[0] = recAvg( C1b_ThreatPerception_NumKilled );
	res_Avg[5] = C2b_AvgDstPacAtkCntrd[0] = recAvg( C2b_AvgDstPacAtkCntrd );
	res_Avg[6] = C3_CloseCalls[0] = recAvg( C3_CloseCalls );
	res_Avg[7] = C4_CaughtAfterHunt[0] = recAvg( C4_CaughtAfterHunt );
	res_Avg[8] = C5_MvsWoutGain[0] = recAvg( C5_MvsWoutGain );
	res_Avg[9] = C6_PntsOvrLives[0] = recAvg( C6_PntsOvrLives );
	res_Avg[10] = C7_KilledAtHouse[0] = recAvg( C7_KilledAtHouse );
	res_Avg[11] = ChryBool_Sum[0] = recAvg( ChryBool_Sum );
	res_Avg[12] = CycleCountPerState_SDev[0] = recAvg( CycleCountPerState_SDev );
	res_Avg[13] = D1_CyclesPerMove[0] = recAvg( D1_CyclesPerMove );
	res_Avg[14] = D2_Osclltng[0] = recAvg( D2_Osclltng );
	res_Avg[15] = Lives_SDev[0] = recAvg( Lives_SDev );
	res_Avg[16] = P1_LureToPill[0] = recAvg( P1_LureToPill );
	res_Avg[17] = P1b_2GstsLrd2Pll[0] = recAvg( P1b_2GstsLrd2Pll );
	res_Avg[18] = P1c_GstsEtnOnPll[0] = recAvg( P1c_GstsEtnOnPll );
	res_Avg[19] = P1d_PacEtnB4GetPll[0] = recAvg( P1d_PacEtnB4GetPll );
	res_Avg[20] = P3_CntDtsB4Pll1[0] = recAvg( P3_CntDtsB4Pll1 );
	res_Avg[21] = P4a_HuntGst1_AvgSpd[0] = recAvg( P4a_HuntGst1_AvgSpd );
	res_Avg[22] = P4b_HuntGst2_AvgSpd[0] = recAvg( P4b_HuntGst2_AvgSpd );
	res_Avg[23] = P7_PutOffGstHse[0] = recAvg( P7_PutOffGstHse );
	res_Avg[24] = Points_Max[l-1];
	res_Avg[25] = R1_AvgTimeToEatChrry[0] = recAvg( R1_AvgTimeToEatChrry );
	res_Avg[26] = R2_AvgChrryPerLvl[0] = recAvg( R2_AvgChrryPerLvl );
	res_Avg[27] = S1b_AvgStates[0] = recAvg( S1b_AvgStates );
	res_Avg[28] = S1c_AvgCycles[0] = recAvg( S1c_AvgCycles );
	res_Avg[29] = S2a_LivesGained[0] = recAvg( S2a_LivesGained );
	res_Avg[30] = S2b_LivesLost[0] = recAvg( S2b_LivesLost );
	res_Avg[31] = S4_TeleportUse[0] = recAvg( S4_TeleportUse );
	res_Avg[32] = S5_GhostHunts[0] = recAvg( S5_GhostHunts );

/**
 *	CODE TO LOG ALL DERIVED FEATURES TO A .csv FILE - SEE CLASSIFY FUNCTION ABOVE.
 */
	res_Nve.push_back( A1_AtkGstHse[l] );
	res_Nve.push_back( A3_GhostKills[l] );
	res_Nve.push_back( A4_HuntAfterPill[l] );
	res_Nve.push_back( A6_Chase¬Dots[l] );
	res_Nve.push_back( C1b_ThreatPerception_NumKilled[l] );
	res_Nve.push_back( C2b_AvgDstPacAtkCntrd[l] );
	res_Nve.push_back( C3_CloseCalls[l] );
	res_Nve.push_back( C4_CaughtAfterHunt[l] );
	res_Nve.push_back( C5_MvsWoutGain[l] );
	res_Nve.push_back( C6_PntsOvrLives[l] );
	res_Nve.push_back( C7_KilledAtHouse[l] );
	res_Nve.push_back( ChryBool_Sum[l] );
	res_Nve.push_back( CycleCountPerState_SDev[l] );
	res_Nve.push_back( D1_CyclesPerMove[l] );
	res_Nve.push_back( D2_Osclltng[l] );
	res_Nve.push_back( Lives_SDev[l] );
	res_Nve.push_back( P1_LureToPill[l] );
	res_Nve.push_back( P1b_2GstsLrd2Pll[l] );
	res_Nve.push_back( P1c_GstsEtnOnPll[l] );
	res_Nve.push_back( P1d_PacEtnB4GetPll[l] );
	res_Nve.push_back( P3_CntDtsB4Pll1[l] );
	res_Nve.push_back( P4a_HuntGst1_AvgSpd[l] );
	res_Nve.push_back( P4b_HuntGst2_AvgSpd[l] );
	res_Nve.push_back( P7_PutOffGstHse[l] );
	res_Nve.push_back( Points_Max[l-1] );
	res_Nve.push_back( R1_AvgTimeToEatChrry[l] );
	res_Nve.push_back( R2_AvgChrryPerLvl[l] );
	res_Nve.push_back( S1b_AvgStates[l] );
	res_Nve.push_back( S1c_AvgCycles[l] );
	res_Nve.push_back( S2a_LivesGained[l] );
	res_Nve.push_back( S2b_LivesLost[l] );
	res_Nve.push_back( S4_TeleportUse[l] );
	res_Nve.push_back( S5_GhostHunts[l] );
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
 *	SUM THE SAVED DERIVED FEATURE SCORES FROM PREVIOUS LEVELS, ASSIGN TO deque[0] (except PointsMax - that total is in the raw data).
 */
void Record_Averages( int w )
{
	int l = Level;

	res_Wdw[0] = A1_AtkGstHse[0] = recAvgWindow( A1_AtkGstHse, w );
	res_Wdw[1] = A3_GhostKills[0] = recAvgWindow( A3_GhostKills, w );
	res_Wdw[2] = A4_HuntAfterPill[0] = recAvgWindow( A4_HuntAfterPill, w );
	res_Wdw[3] = A6_Chase¬Dots[0] = recAvgWindow( A6_Chase¬Dots, w );
	res_Wdw[4] = C1b_ThreatPerception_NumKilled[0] = recAvgWindow( C1b_ThreatPerception_NumKilled, w );
	res_Wdw[5] = C2b_AvgDstPacAtkCntrd[0] = recAvgWindow( C2b_AvgDstPacAtkCntrd, w );
	res_Wdw[6] = C3_CloseCalls[0] = recAvgWindow( C3_CloseCalls, w );
	res_Wdw[7] = C4_CaughtAfterHunt[0] = recAvgWindow( C4_CaughtAfterHunt, w );
	res_Wdw[8] = C5_MvsWoutGain[0] = recAvgWindow( C5_MvsWoutGain, w );
	res_Wdw[9] = C6_PntsOvrLives[0] = recAvgWindow( C6_PntsOvrLives, w );
	res_Wdw[10] = C7_KilledAtHouse[0] = recAvgWindow( C7_KilledAtHouse, w );
	res_Wdw[11] = ChryBool_Sum[0] = recAvgWindow( ChryBool_Sum, w );
	res_Wdw[12] = CycleCountPerState_SDev[0] = recAvgWindow( CycleCountPerState_SDev, w );
	res_Wdw[13] = D1_CyclesPerMove[0] = recAvgWindow( D1_CyclesPerMove, w );
	res_Wdw[14] = D2_Osclltng[0] = recAvgWindow( D2_Osclltng, w );
	res_Wdw[15] = Lives_SDev[0] = recAvgWindow( Lives_SDev, w );
	res_Wdw[16] = P1_LureToPill[0] = recAvgWindow( P1_LureToPill, w );
	res_Wdw[17] = P1b_2GstsLrd2Pll[0] = recAvgWindow( P1b_2GstsLrd2Pll, w );
	res_Wdw[18] = P1c_GstsEtnOnPll[0] = recAvgWindow( P1c_GstsEtnOnPll, w );
	res_Wdw[19] = P1d_PacEtnB4GetPll[0] = recAvgWindow( P1d_PacEtnB4GetPll, w );
	res_Wdw[20] = P3_CntDtsB4Pll1[0] = recAvgWindow( P3_CntDtsB4Pll1, w );
	res_Wdw[21] = P4a_HuntGst1_AvgSpd[0] = recAvgWindow( P4a_HuntGst1_AvgSpd, w );
	res_Wdw[22] = P4b_HuntGst2_AvgSpd[0] = recAvgWindow( P4b_HuntGst2_AvgSpd, w );
	res_Wdw[23] = P7_PutOffGstHse[0] = recAvgWindow( P7_PutOffGstHse, w );
	res_Wdw[24] = Points_Max[l-1];	
	res_Wdw[25] = R1_AvgTimeToEatChrry[0] = recAvgWindow( R1_AvgTimeToEatChrry, w );
	res_Wdw[26] = R2_AvgChrryPerLvl[0] = recAvgWindow( R2_AvgChrryPerLvl, w );
	res_Wdw[27] = S1b_AvgStates[0] = recAvgWindow( S1b_AvgStates, w );
	res_Wdw[28] = S1c_AvgCycles[0] = recAvgWindow( S1c_AvgCycles, w );
	res_Wdw[29] = S2a_LivesGained[0] = recAvgWindow( S2a_LivesGained, w );
	res_Wdw[30] = S2b_LivesLost[0] = recAvgWindow( S2b_LivesLost, w );
	res_Wdw[31] = S4_TeleportUse[0] = recAvgWindow( S4_TeleportUse, w );
	res_Wdw[32] = S5_GhostHunts[0] = recAvgWindow( S5_GhostHunts, w );
}

/**
 *	Description: Log results of feature derivers and classifiers to a log file
 */
void Classify()
{
/**/
	int notcq = 0, conq = 0;
	usage_Avg.assign( 33, 0 );	//Prepare holder for feature usage stats 
	usage_Wdw.assign( 33, 0 );	//Prepare holder for feature usage stats 
	usage_Nve.assign( 33, 0 );	//Prepare holder for feature usage stats 
	fired.assign( 50, 0 );	// +1 is Conq rule firing, -1 is Not_Conq rule firing, 0 is no fire

	float classification = 0.0;
/**/
	classification = ClassifyConqueror_Averages();

	for( int i = 0; i < 50; i++ )
	{
		if( fired[i] == 1 ){
			conq++;
			logData += avrge_rules[i];
		}
		if( fired[i] == -1 ){
			notcq++;
			logData += avrge_rules[i];
		}
	}

	logData += "Score,Conqueror,Not_Conqueror\n";
	sprintf_s( out, 99, "%f", classification );
	logData += out;		logData += ",";

	sprintf_s( out, 99, "%d", conq );
	logData += out;		logData += ",";

	sprintf_s( out, 99, "%d", notcq );
	logData += out;		logData += "\n\n";

	/* NEED THESE LINES BETWEEN EVERY CALL TO A CLASSIFY FUNCTION */
	fired.clear();
	fired.assign( 50, 0 );	// +1 is Conq rule firing, -1 is Not_Conq rule firing, 0 is no fire
	notcq = 0, conq = 0;

	res_Wdw.assign( 33, 0 );
	if( Level > 3 )
	{
		Record_Averages( 3 );

		classification = ClassifyConqueror_WindowAvg();

		for( int i = 0; i < 50; i++ )
		{
			if( fired[i] == 1 ){
				conq++;
				logData += avrge_rules[i];
			}
			if( fired[i] == -1 ){
				notcq++;
				logData += avrge_rules[i];
			}
		}

		logData += "Score,Conqueror,Not_Conqueror\n";
		sprintf_s( out, 99, "%f", classification );
		logData += out;		logData += ",";

		sprintf_s( out, 99, "%d", conq );
		logData += out;		logData += ",";

		sprintf_s( out, 99, "%d", notcq );
		logData += out;		logData += "\n\n";

		/* NEED THESE LINES BETWEEN EVERY CALL TO A CLASSIFY FUNCTION */
		fired.clear();
		fired.assign( 50, 0 );	// +1 is Conq rule firing, -1 is Not_Conq rule firing, 0 is no fire
		notcq = 0, conq = 0;
	}

	classification = ClassifyConqueror_Naive();

	for( int i = 0; i < 50; i++ )
	{
		if( fired[i] == 1 ){
			conq++;
			logData += naive_rules[i];
		}
		if( fired[i] == -1 ){
			notcq++;
			logData += naive_rules[i];
		}
	}

	logData += "Score,Conqueror,Not_Conqueror\n";
	sprintf_s( out, 99, "%f", classification );
	logData += out;		logData += ",";

	sprintf_s( out, 99, "%d", conq );
	logData += out;		logData += ",";

	sprintf_s( out, 99, "%d", notcq );
	logData += out;		logData += "\n\n";

	/*	Output the feature names, values derived, and usage in rules.	*/
	logData += "Feature,Avg_Val,Wndw_Val,Naive_Val,use_Avg,use_Wdw,use_Nve\n";
	for( int i = 0; i < feature.size(); i++ )
	{
		logData += feature[i];	logData += ","; 
		sprintf_s( out, 99, "%f", res_Avg[i] );
		logData += out;			logData += ",";
		sprintf_s( out, 99, "%d", usage_Avg[i] );
		logData += out;			logData += ",";
		sprintf_s( out, 99, "%f", res_Wdw[i] );
		logData += out;			logData += ",";
		sprintf_s( out, 99, "%d", usage_Wdw[i] );
		logData += out;			logData += ",";
		sprintf_s( out, 99, "%f", res_Nve[i] );
		logData += out;			logData += ",";
		sprintf_s( out, 99, "%d", usage_Nve[i] );
		logData += out;			logData += "\n";
	}

	/* NEED THESE LINES BETWEEN EVERY LEVEL */
	usage_Avg.clear();
	usage_Wdw.clear();
	usage_Nve.clear();
	fired.clear();
	res_Avg.clear();
	res_Wdw.clear();
	res_Nve.clear();

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
 *	Description: Implement rules from 50 Rule ensemble to classify player as a Conqueror or Not over all levels played thus far.
 */
float ClassifyConqueror_WindowAvg()
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
	if( Lives_SDev[0] > 1.5 ){ conq -= 0.235264706; usage_Wdw[15]++; fired[0]--; }	//weight[0] -= 0.01; }

	if( D2_Osclltng[0] > 122 ){ conq -= 0.2133; usage_Wdw[14]++; fired[1]--; }

	if( A4_HuntAfterPill[0] <= 1.16 ){ conq -= 0.213213115; usage_Wdw[2]++; fired[2]--; }

	if( Points_Max[l-1] <= 1870 ){ conq += 0.161826824; usage_Wdw[24]++; fired[3]++; }	// Can only fire on last level

	if( A4_HuntAfterPill[0] > 3.08 ){ conq += 0.147540984; usage_Wdw[2]++; fired[4]++; }

	if( S1c_AvgCycles[0] > 5500 ){ conq -= 0.147540984; usage_Wdw[28]++; fired[5]--; }

	if( ChryBool_Sum[0] <= 23 ){ conq -= 0.1333; usage_Wdw[11]++; fired[6]--; }

	if( C5_MvsWoutGain[0] > 150 ){ conq -= 0.1067; usage_Wdw[8]++; fired[7]--; }

	if( P7_PutOffGstHse[0] > 0.375 ){ conq += 0.106509836; usage_Wdw[23]++; fired[8]++; }

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
	if( A4_HuntAfterPill[0] > 2.5185 && C4_CaughtAfterHunt[0] <= 0.69625 ){ conq += 0.226733333; usage_Wdw[2]++; usage_Wdw[7]++; fired[9]++; }

	if( C5_MvsWoutGain[0] <= 106 && S2a_LivesGained[0] > 0.775 ){ conq += 0.213196721; usage_Wdw[8]++; usage_Wdw[29]++; fired[10]++; }

	if( A4_HuntAfterPill[0] >= 0.914 && S1b_AvgStates[0] <= 304 ){ conq += 0.191264706; usage_Wdw[2]++; usage_Wdw[27]++; fired[11]++; }

	if( A4_HuntAfterPill[0] > 3.08 && S1c_AvgCycles[0] <= 5653 ){ conq += 0.161764706; usage_Wdw[2]++; usage_Wdw[28]++; fired[12]++; }

	if( A4_HuntAfterPill[0] > 1.16 && ChryBool_Sum[0] <= 23 ){ conq -= 0.147540984; usage_Wdw[2]++; usage_Wdw[11]++; fired[13]--; }

	if( Points_Max[l-1] <= 1878 && P4b_HuntGst2_AvgSpd[0] <= 39 ){ conq += 0.132352941; usage_Wdw[24]++; usage_Wdw[22]++; fired[14]++; }	// Can only fire on last level

	if( D2_Osclltng[0] <= 123 && R1_AvgTimeToEatChrry[0] <= 21 ){ conq -= 0.126575738; usage_Wdw[14]++; usage_Wdw[25]++; fired[15]--; }

	if( ChryBool_Sum[0] > 23 && C3_CloseCalls[0] <= 0.174 ){ conq += 0.12; usage_Wdw[11]++; usage_Wdw[25]++; fired[16]++; }

	if( ChryBool_Sum[0] > 22 && A6_Chase¬Dots[0] > 5.25 ){ conq -= 0.102941176; usage_Wdw[11]++; usage_Wdw[3]++; fired[17]--; }

	if( C5_MvsWoutGain[0] > 78 && C3_CloseCalls[0] > 0.1545 ){ conq -= 0.311442623; usage_Wdw[8]++; usage_Wdw[6]++; usage_Wdw[6]++; fired[18]--; }

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
	if( C5_MvsWoutGain[0] > 106 && P4b_HuntGst2_AvgSpd[0] <= 1163 && P7_PutOffGstHse[0] <= 0.2678 ){ conq -= 0.393491803; usage_Wdw[8]++; usage_Wdw[22]++; usage_Wdw[23]++; fired[19]--; }

	if( S2a_LivesGained[0] > 0.775 && S1c_AvgCycles[0] <= 5653 && Lives_SDev[0] <= 1.674 ){ conq += 0.327836066; usage_Wdw[29]++; usage_Wdw[28]++; usage_Wdw[15]++; fired[20]++; }

	if( A4_HuntAfterPill[0] <= 3.08 && S2a_LivesGained[0] <= 0.78375 && Lives_SDev[0] <= 1.468 ){ conq -= 0.327737705; usage_Wdw[2]++; usage_Wdw[29]++; usage_Wdw[15]++; fired[21]--; }

	if( S2a_LivesGained[0] > 0.775 && C4_CaughtAfterHunt[0] <= 0.69625 && S1c_AvgCycles[0] <= 6527 ){ conq += 0.323647059; usage_Wdw[29]++; usage_Wdw[7]++; usage_Wdw[28]++; fired[22]++; }

	if( S1c_AvgCycles[0] <= 4396 && A4_HuntAfterPill[0] > 0.99 && S2a_LivesGained[0] > 0.53625 ){ conq += 0.294117647; usage_Wdw[28]++; usage_Wdw[2]++; usage_Wdw[29]++; fired[23]++; }

	if( C6_PntsOvrLives[0] <= 1897 && Lives_SDev[0] <= 1.54 && S2a_LivesGained[0] > 0.775 ){ conq += 0.279529412; usage_Wdw[9]++; usage_Wdw[15]++; usage_Wdw[29]++; fired[24]++; }

	if( ChryBool_Sum[0] > 23 && S2a_LivesGained[0] > 0.775 && Lives_SDev[0] <= 1.53 ){ conq += 0.2667; usage_Wdw[11]++; usage_Wdw[29]++; usage_Wdw[15]++; fired[25]++; }

	if( Points_Max[l-1] > 1950 && P3_CntDtsB4Pll1[0] > 4 && P4a_HuntGst1_AvgSpd[0] <= 236 ){ conq -= 0.220705882; usage_Wdw[24]++; usage_Wdw[20]++; usage_Wdw[21]++; fired[26]--; }

	//This rule selects play where PutOffGstHse occurs between 1 sixth and 1 quarter times per level - can only be measured over 6 or more levels.
	if( R2_AvgChrryPerLvl[0] > 0.333 && P7_PutOffGstHse[0] <= 0.250098 && P7_PutOffGstHse[0] >= 0.166732 ){ conq -= 0.219350147; usage_Wdw[26]++; usage_Wdw[23]++; usage_Wdw[23]++; fired[27]--; }

	if( A4_HuntAfterPill[0] <= 3.08 && R2_AvgChrryPerLvl[0] <= 0.802 && A3_GhostKills[0] > 0.46 ){ conq -= 0.213114754; usage_Wdw[2]++; usage_Wdw[26]++; usage_Wdw[1]++; fired[28]--; }

	//This rule's 2nd condition can only NOT fire on the last level, depending on whether Pacman dies after 3rd or fourth Pill - but we let it fire anyway to balance available points between Conq & Not.
	if( A4_HuntAfterPill[0] > 0.947175 && S5_GhostHunts[l] > 3.764 && CycleCountPerState_SDev[0] > 13 ){ conq -= 0.2; usage_Wdw[2]++; usage_Wdw[32]++; usage_Wdw[12]++; fired[29]--; }

	if( A4_HuntAfterPill[0] > 0.7446 && S1b_AvgStates[0] <= 305 && C6_PntsOvrLives[0] <= 1912 ){ conq += 0.176455882; usage_Wdw[2]++; usage_Wdw[27]++; usage_Wdw[9]++; fired[30]++; }

	//This rule's 2nd condition can only NOT fire on the last level, depending on whether Pacman dies after 3rd or fourth Pill - so we only let it fire on the last level.
	if( GameOver && S1b_AvgStates[0] <= 394 && C4_CaughtAfterHunt[0] > 0.445 && S5_GhostHunts[l] > 3.644 ){ conq -= 0.174110588; usage_Wdw[27]++; usage_Wdw[7]++; usage_Wdw[32]++; fired[31]--; }

	if( R2_AvgChrryPerLvl[0] > 0.333 && P7_PutOffGstHse[0] > 0.250098 && Lives_SDev[0] <= 1.52 ){ conq += 0.163206441; usage_Wdw[26]++; usage_Wdw[23]++; usage_Wdw[15]++; fired[32]++; }

	if( A4_HuntAfterPill[0] <= 3.08 && S2a_LivesGained[0] > 0.78375 && Lives_SDev[0] > 1.55 ){ conq -= 0.131163934; usage_Wdw[2]++; usage_Wdw[29]++; usage_Wdw[15]++; fired[33]--; }

	if( ChryBool_Sum[0] > 23 && C3_CloseCalls[0] > 0.174 && A4_HuntAfterPill[0] > 3.104325 ){ conq += 0.119973333; usage_Wdw[11]++; usage_Wdw[6]++; usage_Wdw[2]++; fired[34]++; }

	if( S2a_LivesGained[0] <= 0.775 && A4_HuntAfterPill[0] > 1.407075 && A6_Chase¬Dots[0] <= 1.447 ){ conq += 0.117647059; usage_Wdw[29]++; usage_Wdw[2]++; usage_Wdw[3]++; fired[35]++; }

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
	if( A4_HuntAfterPill[0] > 0.914325 && S1b_AvgStates[0] > 304 && C3_CloseCalls[0] > 0.1665 && S4_TeleportUse[0] > 0.858 )
	{ conq -= 0.374970588; usage_Wdw[2]++; usage_Wdw[27]++; usage_Wdw[6]++; usage_Wdw[31]++; fired[36]--; }

	if( C5_MvsWoutGain[0] > 106 && P7_PutOffGstHse[0] <= 0.2678 && C1b_ThreatPerception_NumKilled[0] <= 2.5 && A4_HuntAfterPill[0] <= 2.9346 )
	{ conq -= 0.34442623; usage_Wdw[8]++; usage_Wdw[23]++; usage_Wdw[4]++; usage_Wdw[2]++; fired[37]--; }

	if( Lives_SDev[0] <= 1.54 && A4_HuntAfterPill[0] > 1.14975 && CycleCountPerState_SDev[0] <= 15.7575 && C4_CaughtAfterHunt[0] <= 0.45 )
	{ conq += 0.294264706; usage_Wdw[15]++; usage_Wdw[2]++; usage_Wdw[12]++; usage_Wdw[7]++; fired[38]++; }

	//This rule's 2nd condition can ONLY fire on the last level, depending on whether Pacman dies after 3rd or fourth Pill - so we only let it fire on the last level.
	if( GameOver && A4_HuntAfterPill[0] > 0.947175 && S5_GhostHunts[l] <= 3.764 && A6_Chase¬Dots[0] <= 5.3 && ChryBool_Sum[0] > 24 )
	{ conq += 0.2933; usage_Wdw[2]++; usage_Wdw[32]++; usage_Wdw[3]++; usage_Wdw[11]++; fired[39]++; }

	if( C5_MvsWoutGain[0] > 78 && ChryBool_Sum[0] > 23 && R1_AvgTimeToEatChrry[0] > 36 && A4_HuntAfterPill[0] <= 2.4966 )
	{ conq -= 0.278770492; usage_Wdw[8]++; usage_Wdw[11]++; usage_Wdw[25]++; usage_Wdw[2]++; fired[40]--; }

	if( R2_AvgChrryPerLvl[0] > 0.333 && P7_PutOffGstHse[0] <= 0.166732 && C1b_ThreatPerception_NumKilled[0] <= 1 && S2b_LivesLost[0] <= 0.737 )
	{ conq -= 0.226133647; usage_Wdw[26]++; usage_Wdw[23]++; usage_Wdw[4]++; usage_Wdw[30]++; fired[41]--; }

	if( A4_HuntAfterPill[0] <= 3.08 && R2_AvgChrryPerLvl[0] <= 0.802 && A3_GhostKills[0] <= 0.46 && P1c_GstsEtnOnPll[0] <= 0.6588 )
	{ conq -= 0.196688525; usage_Wdw[2]++; usage_Wdw[26]++; usage_Wdw[1]++; usage_Wdw[18]++; fired[42]--; }

	if( A4_HuntAfterPill[0] > 0.914325 && A1_AtkGstHse[0] <= 0.4288 && A6_Chase¬Dots[0] <= 4.843 && P1c_GstsEtnOnPll[0] > 0.5 )
	{ conq += 0.19175; usage_Wdw[2]++; usage_Wdw[0]++; usage_Wdw[3]++; usage_Wdw[18]++; fired[43]++; }

	if( A4_HuntAfterPill[0] <= 3.08 && S2a_LivesGained[0] > 0.78375 && Lives_SDev[0] <= 1.55 && P1_LureToPill[0] > 0.08277 )
	{ conq += 0.163934426; usage_Wdw[2]++; usage_Wdw[29]++; usage_Wdw[15]++; usage_Wdw[16]++; fired[44]++; }

	if( A4_HuntAfterPill[0] <= 3.08 && S2a_LivesGained[0] > 0.775 && D1_CyclesPerMove[0] <= 15.6 && Lives_SDev[0] <= 1.4 )
	{ conq += 0.161764706; usage_Wdw[2]++; usage_Wdw[29]++; usage_Wdw[13]++; usage_Wdw[15]++; fired[45]++; }

	if( R2_AvgChrryPerLvl[0] > 0.333 && P7_PutOffGstHse[0] <= 0.166732 && C1b_ThreatPerception_NumKilled[0] > 1 && S4_TeleportUse[0] <= 1.923 )
	{ conq += 0.131411765; usage_Wdw[26]++; usage_Wdw[23]++; usage_Wdw[4]++; usage_Wdw[31]++; fired[46]++; }

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
		usage_Wdw[19]++; usage_Wdw[4]++; usage_Wdw[26]++; usage_Wdw[18]++; usage_Wdw[17]++; usage_Wdw[21]++; usage_Wdw[26]++; usage_Wdw[3]++; fired[47]--;
	}

	if( A4_HuntAfterPill[0] > 0.914325 && A1_AtkGstHse[0] > 0.4288 && P1d_PacEtnB4GetPll[0] <= 0 && Lives_SDev[0] <= 1.52 && C3_CloseCalls[0] <= 0.5385 && C7_KilledAtHouse[0] <= 0.222 )
	{
		conq += 0.113455882;
		usage_Wdw[2]++; usage_Wdw[0]++; usage_Wdw[19]++; usage_Wdw[15]++; usage_Wdw[6]++; usage_Wdw[10]++; fired[48]++;
	}

	if( C5_MvsWoutGain[0] <= 150 && A4_HuntAfterPill[0] > 0.936225 && C2b_AvgDstPacAtkCntrd[0] <= 13.26 && C4_CaughtAfterHunt[0] <= 0.6925 && ChryBool_Sum[0] > 22 )
	{
		conq += 0.4536;
		usage_Wdw[8]++; usage_Wdw[2]++; usage_Wdw[5]++; usage_Wdw[7]++; usage_Wdw[11]++; fired[49]++;
	}
/**/
	return conq;
}

/**
 *	Description: Implement rules from 50 Rule ensemble to classify player as a Conqueror or Not over all levels played thus far.
 */
float ClassifyConqueror_Averages()
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
	if( Lives_SDev[0] > 1.5 ){ conq -= 0.235264706; usage_Avg[15]++; fired[0]--; }	//weight[0] -= 0.01; }

	if( D2_Osclltng[0] > 122 ){ conq -= 0.2133; usage_Avg[14]++; fired[1]--; }

	if( A4_HuntAfterPill[0] <= 1.16 ){ conq -= 0.213213115; usage_Avg[2]++; fired[2]--; }

	if( Points_Max[l-1] <= 1870 ){ conq += 0.161826824; usage_Avg[24]++; fired[3]++; }	// Can only fire on last level

	if( A4_HuntAfterPill[0] > 3.08 ){ conq += 0.147540984; usage_Avg[2]++; fired[4]++; }

	if( S1c_AvgCycles[0] > 5500 ){ conq -= 0.147540984; usage_Avg[28]++; fired[5]--; }

	if( ChryBool_Sum[0] <= 23 ){ conq -= 0.1333; usage_Avg[11]++; fired[6]--; }

	if( C5_MvsWoutGain[0] > 150 ){ conq -= 0.1067; usage_Avg[8]++; fired[7]--; }

	if( P7_PutOffGstHse[0] > 0.375 ){ conq += 0.106509836; usage_Avg[23]++; fired[8]++; }

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
	if( A4_HuntAfterPill[0] > 2.5185 && C4_CaughtAfterHunt[0] <= 0.69625 ){ conq += 0.226733333; usage_Avg[2]++; usage_Avg[7]++; fired[9]++; }

	if( C5_MvsWoutGain[0] <= 106 && S2a_LivesGained[0] > 0.775 ){ conq += 0.213196721; usage_Avg[8]++; usage_Avg[29]++; fired[10]++; }

	if( A4_HuntAfterPill[0] >= 0.914 && S1b_AvgStates[0] <= 304 ){ conq += 0.191264706; usage_Avg[2]++; usage_Avg[27]++; fired[11]++; }

	if( A4_HuntAfterPill[0] > 3.08 && S1c_AvgCycles[0] <= 5653 ){ conq += 0.161764706; usage_Avg[2]++; usage_Avg[28]++; fired[12]++; }

	if( A4_HuntAfterPill[0] > 1.16 && ChryBool_Sum[0] <= 23 ){ conq -= 0.147540984; usage_Avg[2]++; usage_Avg[11]++; fired[13]--; }

	if( Points_Max[l-1] <= 1878 && P4b_HuntGst2_AvgSpd[0] <= 39 ){ conq += 0.132352941; usage_Avg[24]++; usage_Avg[22]++; fired[14]++; }	// Can only fire on last level

	if( D2_Osclltng[0] <= 123 && R1_AvgTimeToEatChrry[0] <= 21 ){ conq -= 0.126575738; usage_Avg[14]++; usage_Avg[25]++; fired[15]--; }

	if( ChryBool_Sum[0] > 23 && C3_CloseCalls[0] <= 0.174 ){ conq += 0.12; usage_Avg[11]++; usage_Avg[25]++; fired[16]++; }

	if( ChryBool_Sum[0] > 22 && A6_Chase¬Dots[0] > 5.25 ){ conq -= 0.102941176; usage_Avg[11]++; usage_Avg[3]++; fired[17]--; }

	if( C5_MvsWoutGain[0] > 78 && C3_CloseCalls[0] > 0.1545 ){ conq -= 0.311442623; usage_Avg[8]++; usage_Avg[6]++; usage_Avg[6]++; fired[18]--; }

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
	if( C5_MvsWoutGain[0] > 106 && P4b_HuntGst2_AvgSpd[0] <= 1163 && P7_PutOffGstHse[0] <= 0.2678 ){ conq -= 0.393491803; usage_Avg[8]++; usage_Avg[22]++; usage_Avg[23]++; fired[19]--; }

	if( S2a_LivesGained[0] > 0.775 && S1c_AvgCycles[0] <= 5653 && Lives_SDev[0] <= 1.674 ){ conq += 0.327836066; usage_Avg[29]++; usage_Avg[28]++; usage_Avg[15]++; fired[20]++; }

	if( A4_HuntAfterPill[0] <= 3.08 && S2a_LivesGained[0] <= 0.78375 && Lives_SDev[0] <= 1.468 ){ conq -= 0.327737705; usage_Avg[2]++; usage_Avg[29]++; usage_Avg[15]++; fired[21]--; }

	if( S2a_LivesGained[0] > 0.775 && C4_CaughtAfterHunt[0] <= 0.69625 && S1c_AvgCycles[0] <= 6527 ){ conq += 0.323647059; usage_Avg[29]++; usage_Avg[7]++; usage_Avg[28]++; fired[22]++; }

	if( S1c_AvgCycles[0] <= 4396 && A4_HuntAfterPill[0] > 0.99 && S2a_LivesGained[0] > 0.53625 ){ conq += 0.294117647; usage_Avg[28]++; usage_Avg[2]++; usage_Avg[29]++; fired[23]++; }

	if( C6_PntsOvrLives[0] <= 1897 && Lives_SDev[0] <= 1.54 && S2a_LivesGained[0] > 0.775 ){ conq += 0.279529412; usage_Avg[9]++; usage_Avg[15]++; usage_Avg[29]++; fired[24]++; }

	if( ChryBool_Sum[0] > 23 && S2a_LivesGained[0] > 0.775 && Lives_SDev[0] <= 1.53 ){ conq += 0.2667; usage_Avg[11]++; usage_Avg[29]++; usage_Avg[15]++; fired[25]++; }

	if( Points_Max[l-1] > 1950 && P3_CntDtsB4Pll1[0] > 4 && P4a_HuntGst1_AvgSpd[0] <= 236 ){ conq -= 0.220705882; usage_Avg[24]++; usage_Avg[20]++; usage_Avg[21]++; fired[26]--; }

	//This rule selects play where PutOffGstHse occurs between 1 sixth and 1 quarter times per level - can only be measured over 6 or more levels.
	if( R2_AvgChrryPerLvl[0] > 0.333 && P7_PutOffGstHse[0] <= 0.250098 && P7_PutOffGstHse[0] >= 0.166732 ){ conq -= 0.219350147; usage_Avg[26]++; usage_Avg[23]++; usage_Avg[23]++; fired[27]--; }

	if( A4_HuntAfterPill[0] <= 3.08 && R2_AvgChrryPerLvl[0] <= 0.802 && A3_GhostKills[0] > 0.46 ){ conq -= 0.213114754; usage_Avg[2]++; usage_Avg[26]++; usage_Avg[1]++; fired[28]--; }

	//This rule's 2nd condition can only NOT fire on the last level, depending on whether Pacman dies after 3rd or fourth Pill - but we let it fire anyway to balance available points between Conq & Not.
	if( A4_HuntAfterPill[0] > 0.947175 && S5_GhostHunts[l] > 3.764 && CycleCountPerState_SDev[0] > 13 ){ conq -= 0.2; usage_Avg[2]++; usage_Avg[32]++; usage_Avg[12]++; fired[29]--; }

	if( A4_HuntAfterPill[0] > 0.7446 && S1b_AvgStates[0] <= 305 && C6_PntsOvrLives[0] <= 1912 ){ conq += 0.176455882; usage_Avg[2]++; usage_Avg[27]++; usage_Avg[9]++; fired[30]++; }

	//This rule's 2nd condition can only NOT fire on the last level, depending on whether Pacman dies after 3rd or fourth Pill - so we only let it fire on the last level.
	if( GameOver && S1b_AvgStates[0] <= 394 && C4_CaughtAfterHunt[0] > 0.445 && S5_GhostHunts[l] > 3.644 ){ conq -= 0.174110588; usage_Avg[27]++; usage_Avg[7]++; usage_Avg[32]++; fired[31]--; }

	if( R2_AvgChrryPerLvl[0] > 0.333 && P7_PutOffGstHse[0] > 0.250098 && Lives_SDev[0] <= 1.52 ){ conq += 0.163206441; usage_Avg[26]++; usage_Avg[23]++; usage_Avg[15]++; fired[32]++; }

	if( A4_HuntAfterPill[0] <= 3.08 && S2a_LivesGained[0] > 0.78375 && Lives_SDev[0] > 1.55 ){ conq -= 0.131163934; usage_Avg[2]++; usage_Avg[29]++; usage_Avg[15]++; fired[33]--; }

	if( ChryBool_Sum[0] > 23 && C3_CloseCalls[0] > 0.174 && A4_HuntAfterPill[0] > 3.104325 ){ conq += 0.119973333; usage_Avg[11]++; usage_Avg[6]++; usage_Avg[2]++; fired[34]++; }

	if( S2a_LivesGained[0] <= 0.775 && A4_HuntAfterPill[0] > 1.407075 && A6_Chase¬Dots[0] <= 1.447 ){ conq += 0.117647059; usage_Avg[29]++; usage_Avg[2]++; usage_Avg[3]++; fired[35]++; }

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
	if( A4_HuntAfterPill[0] > 0.914325 && S1b_AvgStates[0] > 304 && C3_CloseCalls[0] > 0.1665 && S4_TeleportUse[0] > 0.858 )
	{ conq -= 0.374970588; usage_Avg[2]++; usage_Avg[27]++; usage_Avg[6]++; usage_Avg[31]++; fired[36]--; }

	if( C5_MvsWoutGain[0] > 106 && P7_PutOffGstHse[0] <= 0.2678 && C1b_ThreatPerception_NumKilled[0] <= 2.5 && A4_HuntAfterPill[0] <= 2.9346 )
	{ conq -= 0.34442623; usage_Avg[8]++; usage_Avg[23]++; usage_Avg[4]++; usage_Avg[2]++; fired[37]--; }

	if( Lives_SDev[0] <= 1.54 && A4_HuntAfterPill[0] > 1.14975 && CycleCountPerState_SDev[0] <= 15.7575 && C4_CaughtAfterHunt[0] <= 0.45 )
	{ conq += 0.294264706; usage_Avg[15]++; usage_Avg[2]++; usage_Avg[12]++; usage_Avg[7]++; fired[38]++; }

	//This rule's 2nd condition can ONLY fire on the last level, depending on whether Pacman dies after 3rd or fourth Pill - so we only let it fire on the last level.
	if( GameOver && A4_HuntAfterPill[0] > 0.947175 && S5_GhostHunts[l] <= 3.764 && A6_Chase¬Dots[0] <= 5.3 && ChryBool_Sum[0] > 24 )
	{ conq += 0.2933; usage_Avg[2]++; usage_Avg[32]++; usage_Avg[3]++; usage_Avg[11]++; fired[39]++; }

	if( C5_MvsWoutGain[0] > 78 && ChryBool_Sum[0] > 23 && R1_AvgTimeToEatChrry[0] > 36 && A4_HuntAfterPill[0] <= 2.4966 )
	{ conq -= 0.278770492; usage_Avg[8]++; usage_Avg[11]++; usage_Avg[25]++; usage_Avg[2]++; fired[40]--; }

	if( R2_AvgChrryPerLvl[0] > 0.333 && P7_PutOffGstHse[0] <= 0.166732 && C1b_ThreatPerception_NumKilled[0] <= 1 && S2b_LivesLost[0] <= 0.737 )
	{ conq -= 0.226133647; usage_Avg[26]++; usage_Avg[23]++; usage_Avg[4]++; usage_Avg[30]++; fired[41]--; }

	if( A4_HuntAfterPill[0] <= 3.08 && R2_AvgChrryPerLvl[0] <= 0.802 && A3_GhostKills[0] <= 0.46 && P1c_GstsEtnOnPll[0] <= 0.6588 )
	{ conq -= 0.196688525; usage_Avg[2]++; usage_Avg[26]++; usage_Avg[1]++; usage_Avg[18]++; fired[42]--; }

	if( A4_HuntAfterPill[0] > 0.914325 && A1_AtkGstHse[0] <= 0.4288 && A6_Chase¬Dots[0] <= 4.843 && P1c_GstsEtnOnPll[0] > 0.5 )
	{ conq += 0.19175; usage_Avg[2]++; usage_Avg[0]++; usage_Avg[3]++; usage_Avg[18]++; fired[43]++; }

	if( A4_HuntAfterPill[0] <= 3.08 && S2a_LivesGained[0] > 0.78375 && Lives_SDev[0] <= 1.55 && P1_LureToPill[0] > 0.08277 )
	{ conq += 0.163934426; usage_Avg[2]++; usage_Avg[29]++; usage_Avg[15]++; usage_Avg[16]++; fired[44]++; }

	if( A4_HuntAfterPill[0] <= 3.08 && S2a_LivesGained[0] > 0.775 && D1_CyclesPerMove[0] <= 15.6 && Lives_SDev[0] <= 1.4 )
	{ conq += 0.161764706; usage_Avg[2]++; usage_Avg[29]++; usage_Avg[13]++; usage_Avg[15]++; fired[45]++; }

	if( R2_AvgChrryPerLvl[0] > 0.333 && P7_PutOffGstHse[0] <= 0.166732 && C1b_ThreatPerception_NumKilled[0] > 1 && S4_TeleportUse[0] <= 1.923 )
	{ conq += 0.131411765; usage_Avg[26]++; usage_Avg[23]++; usage_Avg[4]++; usage_Avg[31]++; fired[46]++; }

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
		usage_Avg[19]++; usage_Avg[4]++; usage_Avg[26]++; usage_Avg[18]++; usage_Avg[17]++; usage_Avg[21]++; usage_Avg[26]++; usage_Avg[3]++; fired[47]--;
	}

	if( A4_HuntAfterPill[0] > 0.914325 && A1_AtkGstHse[0] > 0.4288 && P1d_PacEtnB4GetPll[0] <= 0 && Lives_SDev[0] <= 1.52 && C3_CloseCalls[0] <= 0.5385 && C7_KilledAtHouse[0] <= 0.222 )
	{
		conq += 0.113455882;
		usage_Avg[2]++; usage_Avg[0]++; usage_Avg[19]++; usage_Avg[15]++; usage_Avg[6]++; usage_Avg[10]++; fired[48]++;
	}

	if( C5_MvsWoutGain[0] <= 150 && A4_HuntAfterPill[0] > 0.936225 && C2b_AvgDstPacAtkCntrd[0] <= 13.26 && C4_CaughtAfterHunt[0] <= 0.6925 && ChryBool_Sum[0] > 22 )
	{
		conq += 0.4536;
		usage_Avg[8]++; usage_Avg[2]++; usage_Avg[5]++; usage_Avg[7]++; usage_Avg[11]++; fired[49]++;
	}
/**/
	return conq;
}

/**
 *	Notes: All rules are being interpreted to work for a single level. Thus each threshold number 
 *		from a rule must be mapped to a sensible number to be derived from a single level...see Mapping_NrmlsToAvgs.xlsx
 */

/**
 *	Description: Implement rules from 50 Rule ensemble to classify player as a Conqueror or Not for one level.
 */
float ClassifyConqueror_Naive()
{
	float conq = 0.0;	//As a float the modifier applied by each rule can be weighted by reasonable, fractional, amounts - e.g. +0.5, +1.2, etc
	int l = Level;

	/////////	Rules with one condition		//////////
	/**
	 *	Rank			Rule
	 *	0.235264706		Rule 2 Not (19; 0.842)  if Lives_SDev > 0.597 then Not
	 *	0.2133			Rule 2 Not (14.22; 0.915) if D2_Osclltng_Avg > 0.678 then Not
	 *	0.213213115		Rule 1 Not (14; 0.929)  if HuntAfterPill_Avg <= 0.212 then Not		// direct map - 0.324
	 *	0.161826824		Rule 1 Conqr (12.992; 0.847) if Points_Max_Avg <= 0.446 then Conqr
	 *	0.147540984		Rule 3 Conqr (9; 1.0) if HuntAfterPill_Avg > 0.563 then Conqr		// direct map - 0.84
	 *	0.147540984		Rule 2 Not (10; 0.9) if S1.c_AvgCycles > 0.573 then Not
	 *	0.133333333		Rule 1 Not (10; 1.0) if ChryBool_Sum_Avg <= 0.137 then Not
	 *	0.106666667		Rule 5 Not (8; 1.0) if C5_MvsW-outGain_Avg > 0.584 then Not
	 *	0.106509836		Rule 3 Conqr (7.219; 0.9) if P7_PutOffGstHse_Avg > 0.656 then Conqr	// direct map - 0.375
	 */
	if( Lives_SDev[l] > 1.5 ){ conq -= 0.235264706; usage_Nve[15]++; fired[0]--; }	//weight[0] -= 0.01; }

	if( D2_Osclltng[l] > 122 ){ conq -= 0.2133; usage_Nve[14]++; fired[1]--; }

	if( A4_HuntAfterPill[l] <= 1 ){ conq -= 0.213213115; usage_Nve[2]++; fired[2]--; }

	if( Points_Max[l-1] <= 1895 ){ conq += 0.161826824; usage_Nve[24]++; fired[3]++; }

	if( A4_HuntAfterPill[l] > 2 ){ conq += 0.147540984; usage_Nve[2]++; fired[4]++; }

	if( S1c_AvgCycles[l] > 5500 ){ conq -= 0.147540984; usage_Nve[28]++; fired[5]--; }

	if( ChryBool_Sum[l] <= 24 ){ conq -= 0.1333; usage_Nve[11]++; fired[6]--; }

	if( C5_MvsWoutGain[l] > 150 ){ conq -= 0.1067; usage_Nve[8]++; fired[7]--; }

	if( P7_PutOffGstHse[0] > 0.656 ){ conq += 0.106509836; usage_Nve[23]++; fired[8]++; }

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
	if( A4_HuntAfterPill[l] > 2 && C4_CaughtAfterHunt[l] <= 2 ){ conq += 0.226733333; usage_Nve[2]++; usage_Nve[7]++; fired[9]++; }

	if( C5_MvsWoutGain[l] <= 106 && S2a_LivesGained[0] > 0.620 ){ conq += 0.213196721; usage_Nve[8]++; usage_Nve[29]++; fired[10]++; }

	if( A4_HuntAfterPill[l] >= 1 && S1b_AvgStates[l] <= 304 ){ conq += 0.191264706; usage_Nve[2]++; usage_Nve[27]++; fired[11]++; }

	if( A4_HuntAfterPill[l] > 2 && S1c_AvgCycles[l] <= 5650 ){ conq += 0.161764706; usage_Nve[2]++; usage_Nve[28]++; fired[12]++; }

	if( A4_HuntAfterPill[l] > 1 && ChryBool_Sum[l] <= 23 ){ conq -= 0.147540984; usage_Nve[2]++; usage_Nve[11]++; fired[13]--; }

	if( Points_Max[l-1] <= 1950 && P4b_HuntGst2_AvgSpd[l] <= 38 ){ conq += 0.132352941; usage_Nve[24]++; usage_Nve[22]++; fired[14]++; }

	if( D2_Osclltng[l] <= 123 && R1_AvgTimeToEatChrry[l] <= 21 ){ conq -= 0.126575738; usage_Nve[14]++; usage_Nve[25]++; fired[15]--; }

	if( ChryBool_Sum[l] > 23 && C3_CloseCalls[l] <= 2 ){ conq += 0.12; usage_Nve[11]++; usage_Nve[25]++; fired[16]++; }

	if( ChryBool_Sum[l] > 23 && A6_Chase¬Dots[l] > 5 ){ conq -= 0.102941176; usage_Nve[11]++; usage_Nve[3]++; fired[17]--; }

	if( C5_MvsWoutGain[l] > 79 && C3_CloseCalls[l] == 1 ){ conq -= 0.311442623; usage_Nve[8]++; usage_Nve[6]++; usage_Nve[6]++; fired[18]--; }

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
	if( C5_MvsWoutGain[l] > 106 && P4b_HuntGst2_AvgSpd[l] <= 1164 && P7_PutOffGstHse[0] <= 0.469 ){ conq -= 0.393491803; usage_Nve[8]++; usage_Nve[22]++; usage_Nve[23]++; fired[19]--; }

	if( S2a_LivesGained[0] > 0.620 && S1c_AvgCycles[l] <= 5650 && Lives_SDev[l] <= 1.654 ){ conq += 0.327836066; usage_Nve[29]++; usage_Nve[28]++; usage_Nve[15]++; fired[20]++; }

	if( A4_HuntAfterPill[l] <= 2 && S2a_LivesGained[0] <= 0.627 && Lives_SDev[l] <= 1.469 ){ conq -= 0.327737705; usage_Nve[2]++; usage_Nve[29]++; usage_Nve[15]++; fired[21]--; }

	if( S2a_LivesGained[0] > 0.620 && C4_CaughtAfterHunt[l] <= 2 && S1c_AvgCycles[l] <= 6530 ){ conq += 0.323647059; usage_Nve[29]++; usage_Nve[7]++; usage_Nve[28]++; fired[22]++; }

	if( S1c_AvgCycles[l] <= 4410 && A4_HuntAfterPill[l] > 1 && S2a_LivesGained[0] > 0.429 ){ conq += 0.294117647; usage_Nve[28]++; usage_Nve[2]++; usage_Nve[29]++; fired[23]++; }

	if( C6_PntsOvrLives[l] <= 1900 && Lives_SDev[l] <= 1.54 && S2a_LivesGained[0] > 0.620 ){ conq += 0.279529412; usage_Nve[9]++; usage_Nve[15]++; usage_Nve[29]++; fired[24]++; }

	if( ChryBool_Sum[l] > 23 && S2a_LivesGained[0] > 0.620 && Lives_SDev[l] <= 1.54 ){ conq += 0.2667; usage_Nve[11]++; usage_Nve[29]++; usage_Nve[15]++; fired[25]++; }

	if( Points_Max[l-1] > 1950 && P3_CntDtsB4Pll1[l] > 19 && P4a_HuntGst1_AvgSpd[l] <= 230 ){ conq -= 0.220705882; usage_Nve[24]++; usage_Nve[20]++; usage_Nve[21]++; fired[26]--; }

	//This rule selects play where PutOffGstHse occurs between 1 sixth and 1 quarter times per level - can only be measured over 6 or more levels.
	if( R2_AvgChrryPerLvl[0] > 0.333 && P7_PutOffGstHse[0] <= 0.438 && P7_PutOffGstHse[0] >= 0.292 ){ conq -= 0.219350147; usage_Nve[26]++; usage_Nve[23]++; usage_Nve[23]++; fired[27]--; }

	if( A4_HuntAfterPill[l] <= 2 && R2_AvgChrryPerLvl[0] <= 0.802 && A3_GhostKills[l] > 0 ){ conq -= 0.213114754; usage_Nve[2]++; usage_Nve[26]++; usage_Nve[1]++; fired[28]--; }

	//This rule's 2nd condition can only NOT fire on the last level, depending on whether Pacman dies after 3rd or fourth Pill - so we only let it fire on the last level.
	if( GameOver && A4_HuntAfterPill[l] > 1 && S5_GhostHunts[l] > 3.75 && CycleCountPerState_SDev[l] > 13 ){ conq -= 0.2; usage_Nve[2]++; usage_Nve[32]++; usage_Nve[12]++; fired[29]--; }

	if( A4_HuntAfterPill[l] > 0 && S1b_AvgStates[l] <= 305 && C6_PntsOvrLives[l] <= 1900 ){ conq += 0.176455882; usage_Nve[2]++; usage_Nve[27]++; usage_Nve[9]++; fired[30]++; }

	if( S1b_AvgStates[l] <= 393 && C4_CaughtAfterHunt[l] > 2 && S5_GhostHunts[l] > 3.75 ){ conq -= 0.174110588; usage_Nve[27]++; usage_Nve[7]++; usage_Nve[32]++; fired[31]--; }

	if( R2_AvgChrryPerLvl[0] > 0.333 && P7_PutOffGstHse[0] > 0.438 && Lives_SDev[l] <= 1.52 ){ conq += 0.163206441; usage_Nve[26]++; usage_Nve[23]++; usage_Nve[15]++; fired[32]++; }

	if( A4_HuntAfterPill[l] <= 2 && S2a_LivesGained[0] > 0.627 && Lives_SDev[l] > 1.55 ){ conq -= 0.131163934; usage_Nve[2]++; usage_Nve[29]++; usage_Nve[15]++; fired[33]--; }

	if( ChryBool_Sum[l] > 23 && C3_CloseCalls[l] > 0 && A4_HuntAfterPill[l] > 2 ){ conq += 0.119973333; usage_Nve[11]++; usage_Nve[6]++; usage_Nve[2]++; fired[34]++; }

	if( S2a_LivesGained[0] <= 0.620 && A4_HuntAfterPill[l] > 1 && A6_Chase¬Dots[l] <= 1 ){ conq += 0.117647059; usage_Nve[29]++; usage_Nve[2]++; usage_Nve[3]++; fired[35]++; }

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
	if( A4_HuntAfterPill[l] > 0 && S1b_AvgStates[l] > 304 && C3_CloseCalls[l] > 0 && S4_TeleportUse[l] > 1 )
	{ conq -= 0.374970588; usage_Nve[2]++; usage_Nve[27]++; usage_Nve[6]++; usage_Nve[31]++; fired[36]--; }

	if( C5_MvsWoutGain[l] > 106 && P7_PutOffGstHse[0] <= 0.469 && C1b_ThreatPerception_NumKilled[l] <= 2 && A4_HuntAfterPill[l] <= 2 )
	{ conq -= 0.34442623; usage_Nve[8]++; usage_Nve[23]++; usage_Nve[4]++; usage_Nve[2]++; fired[37]--; }

	if( Lives_SDev[l] <= 1.54 && A4_HuntAfterPill[l] > 1 && CycleCountPerState_SDev[l] <= 15.77 && C4_CaughtAfterHunt[l] <= 1 )
	{ conq += 0.294264706; usage_Nve[15]++; usage_Nve[2]++; usage_Nve[12]++; usage_Nve[7]++; fired[38]++; }

	if( A4_HuntAfterPill[l] > 1 && S5_GhostHunts[l] <= 3.75 && A6_Chase¬Dots[l] <= 5 && ChryBool_Sum[l] > 25 )
	{ conq += 0.2933; usage_Nve[2]++; usage_Nve[32]++; usage_Nve[3]++; usage_Nve[11]++; fired[39]++; }

	if( C5_MvsWoutGain[l] > 79 && ChryBool_Sum[l] > 23 && R1_AvgTimeToEatChrry[l] > 36 && A4_HuntAfterPill[l] <= 2 )
	{ conq -= 0.278770492; usage_Nve[8]++; usage_Nve[11]++; usage_Nve[25]++; usage_Nve[2]++; fired[40]--; }

	if( R2_AvgChrryPerLvl[0] > 0.333 && P7_PutOffGstHse[0] <= 0.292 && C1b_ThreatPerception_NumKilled[l] <= 1 && S2b_LivesLost[0] <= 0.737 )
	{ conq -= 0.226133647; usage_Nve[26]++; usage_Nve[23]++; usage_Nve[4]++; usage_Nve[30]++; fired[41]--; }

	if( A4_HuntAfterPill[l] <= 2 && R2_AvgChrryPerLvl[0] <= 0.802 && A3_GhostKills[l] <= 0 && P1c_GstsEtnOnPll[l] <= 1 )
	{ conq -= 0.196688525; usage_Nve[2]++; usage_Nve[26]++; usage_Nve[1]++; usage_Nve[18]++; fired[42]--; }

	if( A4_HuntAfterPill[l] > 0 && A1_AtkGstHse[l] <= 0 && A6_Chase¬Dots[l] <= 5 && P1c_GstsEtnOnPll[l] > 0 )
	{ conq += 0.19175; usage_Nve[2]++; usage_Nve[0]++; usage_Nve[3]++; usage_Nve[18]++; fired[43]++; }

	if( A4_HuntAfterPill[l] <= 2 && S2a_LivesGained[0] > 0.627 && Lives_SDev[l] <= 1.55 && P1_LureToPill[l] > 0 )
	{ conq += 0.163934426; usage_Nve[2]++; usage_Nve[29]++; usage_Nve[15]++; usage_Nve[16]++; fired[44]++; }

	if( A4_HuntAfterPill[l] <= 2 && S2a_LivesGained[0] > 0.620 && D1_CyclesPerMove[l] <= 15.45 && Lives_SDev[l] <= 1.4 )
	{ conq += 0.161764706; usage_Nve[2]++; usage_Nve[29]++; usage_Nve[13]++; usage_Nve[15]++; fired[45]++; }

	if( R2_AvgChrryPerLvl[0] > 0.333 && P7_PutOffGstHse[0] <= 0.292 && C1b_ThreatPerception_NumKilled[l] > 1 && S4_TeleportUse[l] <= 2 )
	{ conq += 0.131411765; usage_Nve[26]++; usage_Nve[23]++; usage_Nve[4]++; usage_Nve[31]++; fired[46]++; }

	/////////	Rules with more than four conditions		//////////
	/**
	 *	Rank		Rule
	 *	0.161764706	Rule 2 Not (11; 1.0)  if P1.e_PacEtnB4GetPll_Avg <= 0 and C1.b_ThreatPrcptn-#Killed <= 0.333 and R2_AvgChrryPerLvl > 0.333 and P1.c_GstsEtnOnPll_Avg <= 0.598 and P1.b_2GstsLrd2Pll_Avg <= 0.167 and HuntGst1_AvgSpd <= 0.640 and R2_AvgChrryPerLvl <= 0.750 and Chase¬Dots_Avg > 0.120 then Not
	 *	0.113455882	Rule 3 Conqr (7.715; 1.0)  if HuntAfterPill_Avg > 0.167 and A1_AtkGstHse_Avg > 0.134 and P1.e_PacEtnB4GetPll_Avg <= 0 and Lives_SDev <= 0.597 and CloseCalls_Avg <= 0.359 and C7_KilledAtHouse_Avg <= 0.148 then Conqr
	 *	0.4536		Rule 1 Conqr (45; 0.756)  if C5_MvsW-outGain_Avg <= 0.584 and HuntAfterPill_Avg > 0.171 and C2.b_AvgDstPacAtk-Cntrd <= 0.941 and C4_CaughtAfterHunt_Avg <= 0.554 and ChryBool_Sum_Avg > 0.136 then Conqr
	 */
	if( P1d_PacEtnB4GetPll[l] <= 0 && C1b_ThreatPerception_NumKilled[l] <= 2 && R2_AvgChrryPerLvl[0] > 0.333 && P1c_GstsEtnOnPll[l] <= 2 && P1b_2GstsLrd2Pll[l] <= 1 && P4a_HuntGst1_AvgSpd[l] <= 224 && R2_AvgChrryPerLvl[0] <= 0.750 && A6_Chase¬Dots[l] > 1 )
	{
		conq -= 0.161764706; 
		usage_Nve[19]++; usage_Nve[4]++; usage_Nve[26]++; usage_Nve[18]++; usage_Nve[17]++; usage_Nve[21]++; usage_Nve[26]++; usage_Nve[3]++; fired[47]--;
	}

	if( A4_HuntAfterPill[l] > 0 && A1_AtkGstHse[l] > 0 && P1d_PacEtnB4GetPll[l] <= 0 && Lives_SDev[l] <= 1.52 && C3_CloseCalls[l] <= 3 && C7_KilledAtHouse[l] <= 2 )
	{
		conq += 0.113455882;
		usage_Nve[2]++; usage_Nve[0]++; usage_Nve[19]++; usage_Nve[15]++; usage_Nve[6]++; usage_Nve[10]++; fired[48]++;
	}

	if( C5_MvsWoutGain[l] <= 150 && A4_HuntAfterPill[l] > 0 && C2b_AvgDstPacAtkCntrd[l] <= 13.26 && C4_CaughtAfterHunt[l] <= 2 && ChryBool_Sum[l] > 23 )
	{
		conq += 0.4536;
		usage_Nve[8]++; usage_Nve[2]++; usage_Nve[5]++; usage_Nve[7]++; usage_Nve[11]++; fired[49]++;
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

avrge_rules.push_back( "if( Lives_SDev[0] > 1.5 )then NotCq\n\n" );
avrge_rules.push_back( "if( D2_Osclltng[0] > 122 ) then NotCq\n\n" );
avrge_rules.push_back( "if( A4_HuntAfterPill[0] <= 1.16 ) then NotCq\n\n" );
avrge_rules.push_back( "if( Points_Max[l-1] <= 1870 ) then Conq\n\n" );
avrge_rules.push_back( "if( A4_HuntAfterPill[0] > 3.08 ) then Conq\n\n" );
avrge_rules.push_back( "if( S1c_AvgCycles[0] > 5500 ) then NotCq\n\n" );
avrge_rules.push_back( "if( ChryBool_Sum[0] <= 23 ) then NotCq\n\n" );
avrge_rules.push_back( "if( C5_MvsWoutGain[0] > 150 ) then NotCq\n\n" );
avrge_rules.push_back( "if( P7_PutOffGstHse[0] > 0.375 ) then Conq\n\n" );
avrge_rules.push_back( "if( A4_HuntAfterPill[0] > 2.5185 and C4_CaughtAfterHunt[0] <= 0.69625 ) then Conq\n\n" );
avrge_rules.push_back( "if( C5_MvsWoutGain[0] <= 106 and S2a_LivesGained[0] > 0.775 ) then Conq\n\n" );
avrge_rules.push_back( "if( A4_HuntAfterPill[0] >= 0.914 and S1b_AvgStates[0] <= 304 ) then Conq\n\n" );
avrge_rules.push_back( "if( A4_HuntAfterPill[0] > 3.08 and S1c_AvgCycles[0] <= 5653 ) then Conq\n\n" );
avrge_rules.push_back( "if( A4_HuntAfterPill[0] > 1.16 and ChryBool_Sum[0] <= 23 ) then NotCq\n\n" );
avrge_rules.push_back( "if( Points_Max[l-1] <= 1878 and P4b_HuntGst2_AvgSpd[0] <= 39 ) then Conq\n\n" );
avrge_rules.push_back( "if( D2_Osclltng[0] <= 123 and R1_AvgTimeToEatChrry[0] <= 21 ) then NotCq\n\n" );
avrge_rules.push_back( "if( ChryBool_Sum[0] > 23 and C3_CloseCalls[0] <= 0.174 ) then Conq\n\n" );
avrge_rules.push_back( "if( ChryBool_Sum[0] > 22 and A6_Chase¬Dots[0] > 5.25 ) then NotCq\n\n" );
avrge_rules.push_back( "if( C5_MvsWoutGain[0] > 78 and C3_CloseCalls[0] > 0.1545 ) then NotCq\n\n" );
avrge_rules.push_back( "if( C5_MvsWoutGain[0] > 106 and P4b_HuntGst2_AvgSpd[0] <= 1163 and P7_PutOffGstHse[0] <= 0.2678 ) then NotCq\n\n" );
avrge_rules.push_back( "if( S2a_LivesGained[0] > 0.775 and S1c_AvgCycles[0] <= 5653 and Lives_SDev[0] <= 1.674 ) then Conq\n\n" );
avrge_rules.push_back( "if( A4_HuntAfterPill[0] <= 3.08 and S2a_LivesGained[0] <= 0.78375 and Lives_SDev[0] <= 1.468 ) then NotCq\n\n" );
avrge_rules.push_back( "if( S2a_LivesGained[0] > 0.775 and C4_CaughtAfterHunt[0] <= 0.69625 and S1c_AvgCycles[0] <= 6527 ) then Conq\n\n" );
avrge_rules.push_back( "if( S1c_AvgCycles[0] <= 4396 and A4_HuntAfterPill[0] > 0.99 and S2a_LivesGained[0] > 0.53625 ) then Conq\n\n" );
avrge_rules.push_back( "if( C6_PntsOvrLives[0] <= 1897 and Lives_SDev[0] <= 1.54 and S2a_LivesGained[0] > 0.775 ) then Conq\n\n" );
avrge_rules.push_back( "if( ChryBool_Sum[0] > 23 and S2a_LivesGained[0] > 0.775 and Lives_SDev[0] <= 1.53 ) then Conq\n\n" );
avrge_rules.push_back( "if( Points_Max[l-1] > 1950 and P3_CntDtsB4Pll1[0] > 4 and P4a_HuntGst1_AvgSpd[0] <= 236 ) then NotCq\n\n" );
avrge_rules.push_back( "if( R2_AvgChrryPerLvl[0] > 0.333 and P7_PutOffGstHse[0] <= 0.250098 and P7_PutOffGstHse[0] >= 0.166732 ) then NotCq\n\n" );
avrge_rules.push_back( "if( A4_HuntAfterPill[0] <= 3.08 and R2_AvgChrryPerLvl[0] <= 0.802 and A3_GhostKills[0] > 0.46 ) then NotCq\n\n" );
avrge_rules.push_back( "if( A4_HuntAfterPill[0] > 0.947175 and S5_GhostHunts[l] > 3.764 and CycleCountPerState_SDev[0] > 13 ) then NotCq\n\n" );
avrge_rules.push_back( "if( A4_HuntAfterPill[0] > 0.7446 and S1b_AvgStates[0] <= 305 and C6_PntsOvrLives[0] <= 1912 ) then Conq\n\n" );
avrge_rules.push_back( "if( GameOver and S1b_AvgStates[0] <= 394 and C4_CaughtAfterHunt[0] > 0.445 and S5_GhostHunts[l] > 3.644 ) then NotCq\n\n" );
avrge_rules.push_back( "if( R2_AvgChrryPerLvl[0] > 0.333 and P7_PutOffGstHse[0] > 0.250098 and Lives_SDev[0] <= 1.52 ) then Conq\n\n" );
avrge_rules.push_back( "if( A4_HuntAfterPill[0] <= 3.08 and S2a_LivesGained[0] > 0.78375 and Lives_SDev[0] > 1.55 ) then NotCq\n\n" );
avrge_rules.push_back( "if( ChryBool_Sum[0] > 23 and C3_CloseCalls[0] > 0.174 and A4_HuntAfterPill[0] > 3.104325 ) then Conq\n\n" );
avrge_rules.push_back( "if( S2a_LivesGained[0] <= 0.775 and A4_HuntAfterPill[0] > 1.407075 and A6_Chase¬Dots[0] <= 1.447 ) then Conq\n\n" );
avrge_rules.push_back( "if( A4_HuntAfterPill[0] > 0.914325 and S1b_AvgStates[0] > 304 and C3_CloseCalls[0] > 0.1665 and S4_TeleportUse[0] > 0.858 ) then NotCq\n\n" );
avrge_rules.push_back( "if( C5_MvsWoutGain[0] > 106 and P7_PutOffGstHse[0] <= 0.2678 and C1b_ThreatPerception_NumKilled[0] <= 2.5 and A4_HuntAfterPill[0] <= 2.9346 ) then NotCq\n\n" );
avrge_rules.push_back( "if( Lives_SDev[0] <= 1.54 and A4_HuntAfterPill[0] > 1.14975 and CycleCountPerState_SDev[0] <= 15.7575 and C4_CaughtAfterHunt[0] <= 0.45 ) then Conq\n\n" );
avrge_rules.push_back( "if( GameOver and A4_HuntAfterPill[0] > 0.947175 and S5_GhostHunts[l] <= 3.764 and A6_Chase¬Dots[0] <= 5.3 and ChryBool_Sum[0] > 24 ) then Conq\n\n" );
avrge_rules.push_back( "if( C5_MvsWoutGain[0] > 78 and ChryBool_Sum[0] > 23 and R1_AvgTimeToEatChrry[0] > 36 and A4_HuntAfterPill[0] <= 2.4966 ) then NotCq\n\n" );
avrge_rules.push_back( "if( R2_AvgChrryPerLvl[0] > 0.333 and P7_PutOffGstHse[0] <= 0.166732 and C1b_ThreatPerception_NumKilled[0] <= 1 and S2b_LivesLost[0] <= 0.737 ) then NotCq\n\n" );
avrge_rules.push_back( "if( A4_HuntAfterPill[0] <= 3.08 and R2_AvgChrryPerLvl[0] <= 0.802 and A3_GhostKills[0] <= 0.46 and P1c_GstsEtnOnPll[0] <= 0.6588 ) then NotCq\n\n" );
avrge_rules.push_back( "if( A4_HuntAfterPill[0] > 0.914325 and A1_AtkGstHse[0] <= 0.4288 and A6_Chase¬Dots[0] <= 4.843 and P1c_GstsEtnOnPll[0] > 0.5 ) then Conq\n\n" );
avrge_rules.push_back( "if( A4_HuntAfterPill[0] <= 3.08 and S2a_LivesGained[0] > 0.78375 and Lives_SDev[0] <= 1.55 and P1_LureToPill[0] > 0.08277 ) then Conq\n\n" );
avrge_rules.push_back( "if( A4_HuntAfterPill[0] <= 3.08 and S2a_LivesGained[0] > 0.775 and D1_CyclesPerMove[0] <= 15.6 and Lives_SDev[0] <= 1.4 ) then Conq\n\n" );
avrge_rules.push_back( "if( R2_AvgChrryPerLvl[0] > 0.333 and P7_PutOffGstHse[0] <= 0.166732 and C1b_ThreatPerception_NumKilled[0] > 1 and S4_TeleportUse[0] <= 1.923 ) then Conq\n\n" );
avrge_rules.push_back( "if( P1d_PacEtnB4GetPll[0] <= 0 and C1b_ThreatPerception_NumKilled[0] <= 2 and R2_AvgChrryPerLvl[0] > 0.333 and P1c_GstsEtnOnPll[0] <= 2.1528 and P1b_2GstsLrd2Pll[0] <= 0.167 and P4a_HuntGst1_AvgSpd[0] <= 223 and R2_AvgChrryPerLvl[0] <= 0.750 and A6_Chase¬Dots[0] > 1.4 ) then NotCq\n\n" );
avrge_rules.push_back( "if( A4_HuntAfterPill[0] > 0.914325 and A1_AtkGstHse[0] > 0.4288 and P1d_PacEtnB4GetPll[0] <= 0 and Lives_SDev[0] <= 1.52 and C3_CloseCalls[0] <= 0.5385 and C7_KilledAtHouse[0] <= 0.222 ) then Conq\n\n" );
avrge_rules.push_back( "if( C5_MvsWoutGain[0] <= 150 and A4_HuntAfterPill[0] > 0.936225 and C2b_AvgDstPacAtkCntrd[0] <= 13.26 and C4_CaughtAfterHunt[0] <= 0.6925 and ChryBool_Sum[0] > 22 ) then Conq\n\n" );

naive_rules.push_back( "if( Lives_SDev > 1.5 )then NotCq\n\n" );
naive_rules.push_back( "if( D2_Osclltng > 122 )then NotCq\n\n" );
naive_rules.push_back( "if( A4_HuntAfterPill <= 1 )then NotCq\n\n" );
naive_rules.push_back( "if( Points_Max <= 1895 )then Conqr\n\n" );
naive_rules.push_back( "if( A4_HuntAfterPill > 2 )then Conqr\n\n" );
naive_rules.push_back( "if( S1c_AvgCycles > 5500 )then NotCq\n\n" );
naive_rules.push_back( "if( ChryBool_Sum <= 24 )then NotCq\n\n" );
naive_rules.push_back( "if( C5_MvsWoutGain > 150 )then NotCq\n\n" );
naive_rules.push_back( "if( P7_PutOffGstHse == 1 )then Conqr\n\n" );
naive_rules.push_back( "if( A4_HuntAfterPill > 2  and  C4_CaughtAfterHunt <= 2 )then Conqr\n\n" );
naive_rules.push_back( "if( C5_MvsWoutGain <= 106  and  S2a_LivesGained == 1 )then Conqr\n\n" );
naive_rules.push_back( "if( A4_HuntAfterPill >= 1  and  S1b_AvgStates <= 304 )then Conqr\n\n" );
naive_rules.push_back( "if( A4_HuntAfterPill > 2  and  S1c_AvgCycles <= 5650 )then Conqr\n\n" );
naive_rules.push_back( "if( A4_HuntAfterPill > 1  and  ChryBool_Sum <= 23 )then NotCq\n\n" );
naive_rules.push_back( "if( Points_Max <= 1950  and  P4b_HuntGst2_AvgSpd <= 38 )then Conqr\n\n" );
naive_rules.push_back( "if( D2_Osclltng <= 123  and  R1_AvgTimeToEatChrry <= 21 )then NotCq\n\n" );
naive_rules.push_back( "if( ChryBool_Sum > 23  and  C3_CloseCalls <= 2 )then Conqr\n\n" );
naive_rules.push_back( "if( ChryBool_Sum > 23  and  A6_Chase¬Dots > 5 )then NotCq\n\n" );
naive_rules.push_back( "if( C5_MvsWoutGain > 79  and  C3_CloseCalls == 1 )then NotCq\n\n" );
naive_rules.push_back( "if( C5_MvsWoutGain > 106  and  P4b_HuntGst2_AvgSpd <= 1164  and  P7_PutOffGstHse == 0 )then NotCq\n\n" );
naive_rules.push_back( "if( S2a_LivesGained == 1  and  S1c_AvgCycles <= 5650  and  Lives_SDev <= 1.654 )then Conqr\n\n" );
naive_rules.push_back( "if( A4_HuntAfterPill <= 2  and  S2a_LivesGained == 0  and  Lives_SDev <= 1.469 )then NotCq\n\n" );
naive_rules.push_back( "if( S2a_LivesGained == 1  and  C4_CaughtAfterHunt <= 2  and  S1c_AvgCycles <= 6530 )then Conqr\n\n" );
naive_rules.push_back( "if( S1c_AvgCycles <= 4410  and  A4_HuntAfterPill > 1  and  S2a_LivesGained == 1 )then Conqr\n\n" );
naive_rules.push_back( "if( C6_PntsOvrLives <= 1900  and  Lives_SDev <= 1.54  and  S2a_LivesGained == 1 )then Conqr\n\n" );
naive_rules.push_back( "if( ChryBool_Sum > 23  and  S2a_LivesGained == 1  and  Lives_SDev <= 1.54 )then Conqr\n\n" );
naive_rules.push_back( "if( Points_Max > 1950  and  P3_CntDtsB4Pll1 > 19  and  P4a_HuntGst1_AvgSpd <= 230 )then NotCq\n\n" );
naive_rules.push_back( "if( R2_AvgChrryPerLvl == 1  and  P7_PutOffGstHse == 0  and  P7_PutOffGstHse == 1 )then NotCq\n\n" );
naive_rules.push_back( "if( A4_HuntAfterPill <= 2  and  R2_AvgChrryPerLvl == 0  and  A3_GhostKills > 0 )then NotCq\n\n" );
naive_rules.push_back( "if( A4_HuntAfterPill > 1  and  S5_GhostHunts > 3.75  and  CycleCountPerState_SDev > 13 )then NotCq\n\n" );
naive_rules.push_back( "if( A4_HuntAfterPill > 0  and  S1b_AvgStates <= 305  and  C6_PntsOvrLives <= 1900 )then Conqr\n\n" );
naive_rules.push_back( "if( S1b_AvgStates <= 393  and  C4_CaughtAfterHunt > 2  and  S5_GhostHunts > 3.75 )then NotCq\n\n" );
naive_rules.push_back( "if( R2_AvgChrryPerLvl == 1  and  P7_PutOffGstHse == 1  and  Lives_SDev <= 1.52 )then Conqr\n\n" );
naive_rules.push_back( "if( A4_HuntAfterPill <= 2  and  S2a_LivesGained == 1  and  Lives_SDev > 1.55 )then NotCq\n\n" );
naive_rules.push_back( "if( ChryBool_Sum > 23  and  C3_CloseCalls > 0  and  A4_HuntAfterPill > 2 )then Conqr\n\n" );
naive_rules.push_back( "if( S2a_LivesGained == 0  and  A4_HuntAfterPill > 1  and  A6_Chase¬Dots <= 1 )then Conqr\n\n" );
naive_rules.push_back( "if( A4_HuntAfterPill > 0  and  S1b_AvgStates > 304  and  C3_CloseCalls > 0  and  S4_TeleportUse > 1 )then NotCq\n\n" );
naive_rules.push_back( "if( C5_MvsWoutGain > 106  and  P7_PutOffGstHse == 0  and  C1b_ThreatPerception_NumKilled <= 2  and  A4_HuntAfterPill <= 2 )then NotCq\n\n" );
naive_rules.push_back( "if( Lives_SDev <= 1.54  and  A4_HuntAfterPill > 1  and  CycleCountPerState_SDev <= 15.77  and  C4_CaughtAfterHunt <= 1 )then Conqr\n\n" );
naive_rules.push_back( "if( A4_HuntAfterPill > 1  and  S5_GhostHunts <= 3.75  and  A6_Chase¬Dots <= 5  and  ChryBool_Sum > 25 )then Conqr\n\n" );
naive_rules.push_back( "if( C5_MvsWoutGain > 79  and  ChryBool_Sum > 23  and  R1_AvgTimeToEatChrry > 36  and  A4_HuntAfterPill <= 2 )then NotCq\n\n" );
naive_rules.push_back( "if( R2_AvgChrryPerLvl == 1  and  P7_PutOffGstHse == 0  and  C1b_ThreatPerception_NumKilled <= 1  and  S2b_LivesLost <= 14 )then NotCq\n\n" );
naive_rules.push_back( "if( A4_HuntAfterPill <= 2  and  R2_AvgChrryPerLvl == 1  and  A3_GhostKills <= 0  and  P1c_GstsEtnOnPll <= 1 )then NotCq\n\n" );
naive_rules.push_back( "if( A4_HuntAfterPill > 0  and  A1_AtkGstHse <= 0  and  A6_Chase¬Dots <= 5  and  P1c_GstsEtnOnPll > 0 )then Conqr\n\n" );
naive_rules.push_back( "if( A4_HuntAfterPill <= 2  and  S2a_LivesGained == 1  and  Lives_SDev <= 1.55  and  P1_LureToPill > 0 )then Conqr\n\n" );
naive_rules.push_back( "if( A4_HuntAfterPill <= 2  and  S2a_LivesGained == 1  and  D1_CyclesPerMove <= 15.45  and  Lives_SDev <= 1.4 )then Conqr\n\n" );
naive_rules.push_back( "if( R2_AvgChrryPerLvl == 1  and  P7_PutOffGstHse == 0  and  C1b_ThreatPerception_NumKilled > 1  and  S4_TeleportUse <= 2 )then Conqr\n\n" );
naive_rules.push_back( "if( P1d_PacEtnB4GetPll <= 0  and  C1b_ThreatPerception_NumKilled <= 2  and  R2_AvgChrryPerLvl > 0.333  and  P1c_GstsEtnOnPll <= 2  and  P1b_2GstsLrd2Pll <= 1  and  P4a_HuntGst1_AvgSpd <= 224  and  R2_AvgChrryPerLvl <= 0.750  and  A6_Chase¬Dots > 1 )then NotCq\n\n" );
naive_rules.push_back( "if( A4_HuntAfterPill > 0  and  A1_AtkGstHse > 0  and  P1d_PacEtnB4GetPll <= 0  and  Lives_SDev <= 1.52  and  C3_CloseCalls <= 3  and  C7_KilledAtHouse <= 2 )then Conqr\n\n" );
naive_rules.push_back( "if( C5_MvsWoutGain <= 150  and  A4_HuntAfterPill > 0  and  C2b_AvgDstPacAtkCntrd <= 13.26  and  C4_CaughtAfterHunt <= 2  and  ChryBool_Sum > 23 )then Conqr\n\n" );

}

//End of file
