// ManipPacResults.cpp : main project file.

#include "stdafx.h"
#include "Form1.h"
#include "Deriver.h"

using namespace ManipPacResults;

#using <mscorlib.dll>

void AnalyseResults();
void ConjoinLevels();
void RestructureDGDs();
void GenerateFeatures();
void EditField();
void ExtractInfo();
int maxInt( int a, int b );
string OutputState(deque<PacState> GameStates);

FILE *outf;
char out[99];

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
/** /
	RestructureDGDs();
/** /
	ConjoinLevels();
/** /
	EditField();
/** /
	GenerateFeatures();
/**/
	ExtractInfo();
/**/
	
	return 0;
}

void AnalyseResults()
{
/** /
	array <String^>^plyrCsv = Directory::GetFiles("./");
	String^ csv = Path::GetFileNameWithoutExtension(plyrCsv[0]);
	char* player = (char*)(void*)Marshal::StringToHGlobalAnsi(csv);

	string pcsv = "";
	pcsv += player;
	pcsv += ".csv";
	FILE *source;
	fopen_s(&source, pcsv.c_str(), "r");
	
	pcsv = "";
	pcsv += player;
	pcsv += "Type.csv";
	fopen_s(&outf, pcsv.c_str(), "wt");
/** /	
	fputs(player, outf);
	fputs("\n", outf);
	fputs(pcsv.c_str(), outf);
	fputs("\n", outf);	
/** /
	int c = 0;
	pcsv = "";
	string plyrHdr;
	string plyrData;
	bool hdr = true;

	while(true)
	{
		if( (c = fgetc(source)) == EOF )
		{
			if( hdr )
				//Remove the last data record's last char, the comma
				plyrData.resize(plyrData.size()-1);
			break;
		}else if( c == '\n' )
		{
			plyrData += ',';
			hdr = true;
			c = fgetc(source);
		}
		if( hdr )
		{
			if( c == ',' )
				hdr = false;
			plyrHdr += c;
		}else{
			if( c == ',' )
				c = '.';
			plyrData += c;
		}
	}

	//Remove the last header's last char, the comma
	plyrHdr.resize(plyrHdr.size()-4);
	plyrHdr += "IP";
	//Put all the headers out to the file
	fputs(plyrHdr.c_str(), outf);
	fputs("\n", outf);
	//Put all the headers out to the file
	fputs(plyrData.c_str(), outf);
/** /
	fclose(source);
	fflush(outf);
	fclose(outf);
/**/
}

void EditField()
{
	//Get the name of the first file in this directory, then append 'Game.csv' and pass to BuildStatesDq
	array <String^>^plyrData = Directory::GetFiles( "./" );
//	array <String^>^plyrData = Directory::GetFiles( "C:\\Documents and Settings\\Ben\\My Documents\\Visual Studio 2005\\Projects\\ManipPacResults\\" );
	String^ plyr = Path::GetFileNameWithoutExtension(plyrData[0]);
	char* player = (char*)(void*)Marshal::StringToHGlobalAnsi(plyr);

//	string name = "C:\\Documents and Settings\\Ben\\My Documents\\Visual Studio 2005\\Projects\\ManipPacResults\\";
	string name = "./";
	name += player;
	name += "Game.csv";
	deque<PacState> dps = BuildStatesDq( name.c_str() );
/**/
	fopen_s(&outf, name.c_str(), "wt");
	
	int s = dps.size();
	int m = 0;
	for(int i = 0; i < s; i++)
	{
/**/
		if( i == 1215 )
			m += 2900;
		if( i == 1614 )
			m += 1310;
		if( i == 1628 )
			m += 1510;
		if( i == 1629 )
			m += 3110;
/**/
		dps[i].Points -= m;
	}
	dps.pop_front();
	string news = OutputState( dps );

	fputs("StateNum,PacX,PacY,Gst1X,Gst1Y,Gst2X,Gst2Y,Dots,Lives,Pills,Points,ChryBool,PacAtkBool,LastMoveDir,Cycles,", outf);

	for(int x = 0; x < 20; x++)
	{
		for(int y = 0; y < 20; y++)
		{
			fputs("x", outf);
			sprintf_s(out, 99, "%d", x);
			fputs(out, outf);
			fputs(" y", outf);
			sprintf_s(out, 99, "%d", y);
			fputs(out, outf);
			if(!(x == 19 && y == 19))
				fputs(",", outf);
		}
	}

	fputs( news.c_str(), outf );
	
	fflush(outf);
	fclose(outf);
}

void GenerateFeatures()
{
	//Get the name of the first file in this directory, then append 'Game.csv' and pass to BuildStatesDq
//	array <String^>^plyrData = Directory::GetFiles( "./" );
	array <String^>^plyrData = Directory::GetFiles( "C:\\Documents and Settings\\Ben\\My Documents\\Visual Studio 2005\\Projects\\ManipPacResults\\" );
//	array <String^>^plyrData = Directory::GetFiles( "C:\\Documents and Settings\\Administrator\\My Documents\\Visual Studio 2005\\Projects\\ManipPacResults\\" );
	String^ plyr = Path::GetFileNameWithoutExtension(plyrData[0]);
	char* player = (char*)(void*)Marshal::StringToHGlobalAnsi(plyr);

//	string name = "./";
	string name = "C:\\Documents and Settings\\Ben\\My Documents\\Visual Studio 2005\\Projects\\ManipPacResults\\";
//	string name = "C:\\Documents and Settings\\Administrator\\My Documents\\Visual Studio 2005\\Projects\\ManipPacResults\\";
	name += player;
	name += ".csv";
	deque<PacState> dps = BuildStatesDq( name.c_str() );
	int s = dps.size();
	if( s < 2 )
		return;

	name = "../x_";
	name += player;
	name += "-FeatureA2.csv";
	fopen_s(&outf, name.c_str(), "wt");

	deque<char*> field;
	deque<float> record;
	deque<float> features;
/** /
	//Generate and store the values for SIMPLE Features
	features = Simple1_Levels( dps );
	g_Levels = features[0];
	float sts = 0, cyc = 0;
	for( int i = 1; i < features.size(); i++ )
	{
		sts += features[i];
		cyc += features[++i];
	}
	sts /= features[0];
	cyc /= features[0];
	field.push_back( "S1.a_#Levels" );
	record.push_back( features[0] );
	field.push_back( "S1.b_AvgStates" );
	record.push_back( sts );
	field.push_back( "S1.c_AvgCycles" );
	record.push_back( cyc );

	features = Simple2_LivesChng( dps );
	field.push_back( "S2.a_LivesGained" );
	record.push_back( features[0] );
	field.push_back( "S2.b_LivesLost" );
	record.push_back( features[1] );

	//Not to be exported - output still in square visit-count map form
	deque<PacState> pS = Simple3_CntSqVsts( dps );

	field.push_back( "S4_TeleportUse" );
	record.push_back( Simple4_TeleportUse( dps ) );

/** /
	//Generate and store the values for AGGRESSION Features
	field.push_back( "A1_AtkGstHse" );
	record.push_back( Aggression1_AtkGstHse( dps ) );

/** /
	//Generate and store the values for PLANNING Features
	features = Planning1_LureToPill( dps );
	field.push_back( "P1_LureToPill" );
	record.push_back( features[0] );
	field.push_back( "P1.a_PacMvB4Lr2Pll" );
	record.push_back( features[1] );
	field.push_back( "P1.b_2GstsLrd2Pll" );
	record.push_back( features[2] );
	field.push_back( "P1.c_GstsEtnOnPll" );
	record.push_back( features[3] );
	field.push_back( "P1.e_PacEtnB4GetPll" );
	record.push_back( features[4] );

	field.push_back( "P2_CnsrvPll4Ltr" );
	record.push_back( Planning2_SavePill( dps ) );

	field.push_back( "P4_CntDtsB4Pll1" );
	record.push_back( Planning4_CntDtsB4Pll1( dps ) );

	features = Planning5_SpeedOfHunt( dps );
	field.push_back( "P5.a_Hunt-Cycs2Gst1" );
	record.push_back( features[0] );
	field.push_back( "P5.b_Hunt-Stts2Gst1" );
	record.push_back( features[1] );
	field.push_back( "P5.c_Hunt-Cycs2Gst2" );
	record.push_back( features[2] );
	field.push_back( "P5.d_Hunt-Stts2Gst2" );
	record.push_back( features[3] );

	field.push_back( "P7_PutOffGstHse" );
	record.push_back( Planning7_PutOffGstHse( dps ) );

/** /
	//Generate and store the values for CAUTION Features
	features = Caution1_ThreatPrcptn( dps );
	field.push_back( "C1.a_ThreatPrcptn-#Trapped" );
	record.push_back( features[0] );
	field.push_back( "C1.b_ThreatPrcptn-#Killed" );
	record.push_back( features[1] );
/** /
	features = Caution2_AvgDstPacGst( dps );
	field.push_back( "C2.a_AvgDstPacGst-Avg" );
	record.push_back( features[0] );
	field.push_back( "C2.b_AvgDstPacGst-Cntrd" );
	record.push_back( features[1] );
	field.push_back( "C2.a_AvgDstPacAtk-Avg" );
	record.push_back( features[2] );
	field.push_back( "C2.b_AvgDstPacAtk-Cntrd" );
	record.push_back( features[3] );

	field.push_back( "C4_CaughtAfterHunt" );
	record.push_back( Caution4_CaughtAfterHunt( dps ) );

	field.push_back( "C5_MvsW-outGain" );
	record.push_back( Caution5_MovesWithoutGain( dps ) );

	field.push_back( "C6_PntsOvrLives" );
	record.push_back( Caution6_PntsOvrLives( dps ) );

	field.push_back( "C7_KilledAtHouse" );
	record.push_back( Caution7_KilledAtHouse( dps ) );

/** /
	//Generate and store the values for SPEED Features
	field.push_back( "S1_MvsToClearLvl" );
	record.push_back( Speed1_MvsToClrLvl( dps ) );
	
	field.push_back( "S2_CyclesToClearLvl" );
	record.push_back( Speed2_CyclsToClrLvl( dps ) );

/** /
	//Generate and store the values for DECISIVENESS Features
	field.push_back( "D1_Cycles/Move" );
	record.push_back( Decisive1_CyclesPrMv( dps ) );

	field.push_back( "D2_Osclltng" );
	record.push_back( Decisive2_Osclltng( dps ) );

/** /
	//Generate and store the values for RESOURCE_GATHERING Features
	features = Resource_TimeAndNumChry( dps );
	field.push_back( "R1_AvgTimeToEatChrry" );
	record.push_back( features[0] );
	field.push_back( "R2_AvgChrryPerLvl" );
	record.push_back( features[1] );

/** /
	//Generate and store the values for THOROUGHNESS Features
	field.push_back( "T1_Leaves1Dot" );
	record.push_back( Thorough1_Lvs1Dot( dps ) );

	field.push_back( "T2_HooversDots" );
	record.push_back( Thorough2_Hoover( dps ) );

/** /
	//Generate and store the values for CONTROL_SKILL Features
	field.push_back( "CS1_Cycs2ClrSctn" );
	record.push_back( Control1_Cycles2ClrSect( dps ) );

/** /
	//Get Maximum player type
	int i = 0;
	string p = "";
	string p2 = "";
	p2 += player[i];
	while( atoi(p2.c_str()) == 0 )
	{
		p += p2;
		i++;
		p2 = "";
		p2 += player[i];
	}
	
	/**
	 *	Test derivers used in real-time game
	 */	

	deque <float> caution1 = Caution1_ThreatPrcptn( dps );
	deque <float> plan1 = Planning1_LureToPill( dps );
	deque <float> plan4 = Planning4_SpeedOfHunt(dps);
	deque <float> resource = Resource_TimeAndNumChry( dps );
	deque <float> simple = Simple1_Levels( dps );
	deque <float> simple2 = Simple2_LivesChng( dps );

	//Need to list non-alphabetically - it is used later				// #	Frq	Actual		| Average		| Normals	|
	float S5_GhostHunts = 4.0 - dps[s-1].Pills;
	record.push_back( S5_GhostHunts );									// 32	3	# hunts = # Pills used					|

	record.push_back( Aggression1_AtkGstHse( dps ) );					// 0	2	0 - 35		| 0 - 3.2		| 0 - 1		|
	
	float A3_GhostKills = Aggression3_GhostKills( dps );
	record.push_back( A3_GhostKills );									// 1	2	0.021-1.217	|				| 0.017 - 1	|

	record.push_back( Aggression4_HuntAfterPill( dps ) );				// 2	25	0 - 4		| 0.125 - 1.5	| 0.083 - 1	|

	record.push_back( Aggression6_Chase¬Dots( dps ) );					// 3	5	1 - 118		| 0.25 - 11.67	| 0.021 - 1	|

	record.push_back( caution1[0] );									// 4	4	0 - 6		| 				| 0 - 1		|

	record.push_back( Caution2_AvgDstPacAtkGstCtrd( dps ) );			// 5	1	7.02 - 14.09|				| 0.498 - 1	|

	record.push_back( Caution3_CloseCalls( dps ) );						// 6	6	1 - 9		| 0.071 - 1.5	| 0.048 - 1	|

	record.push_back( Caution4_CaughtAfterHunt( dps ) );				// 7	5	0 - 9		| 0 - 1.25		| 0 - 1		|

	record.push_back( Caution5_MovesWithoutGain( dps ) );				// 8	7	118 - 2404	| 57.5 - 256.5	| 0.224 - 1	|

	record.push_back( dps[s-1].Points/maxInt(simple2[0]+simple2[1], 1) );	// 9	2	142 - 3750	|				| 0.038 - 1	|
	//Alternative (check Clem): abs( dps[s-1].Lives - dps[0].Lives ) + 1;
	record.push_back( Caution7_KilledAtHouse( dps ) );					// 10	1	0 - 5		| 0 - 1.5		| 0 - 1 	|

	record.push_back( Aggregate2_ChryBoolSum( dps ) );					// 11	9	25 - 2048	| 7.1 - 165.2	| 0.043 - 1	|

	record.push_back(Aggregate3_CycleCountPerStateSDev(dps));			// 12	2	6.9 - 57.3	|				| 0.12 - 1	|

	record.push_back( Decisive1_CyclesPrMv( dps ) );					// 13	1	7.2 - 42.4	|				| 0.17 - 1	|

	record.push_back( Decisive2_Osclltng( dps ) );						// 14	2	124 - 2241	| 62 - 181.5	| 0.342 - 1	|

	record.push_back( Aggregate1_LivesSDev( dps ) );					// 15	11	0.38 - 2.54	|				| 0.148 - 1	|

	record.push_back( plan1[0] );										// 16	1	0 - 24		| 0 - 2.67		| 0 - 1 	|

	record.push_back( plan1[2] );										// 17	1	0 - 9		| 0 - 1			| 0 - 1		|

	record.push_back( plan1[3] );										// 18	3	0 - 37		| 0 - 3.6		| 0 - 1		|

	record.push_back( plan1[4] );										// 19	2	0 - 1		| 0 - 0.333		| 0 - 1		|

	record.push_back( Planning3_CntDtsB4Pll1( dps ) );					// 20	1	6.3 - 97.6	| 0.9 - 22		| 0.041 - 1	|

	record.push_back( plan4[0]/min(S5_GhostHunts, A3_GhostKills) );		// 21	2	62 - 349	|				| 0.178 - 1	|

	record.push_back( plan4[2] / maxInt(1, A3_GhostKills-4) );				// 22	2	0 - 2307	|				| 0 - 1		|
	//This becomes binary since you can only put off the ghost house once per level.
	record.push_back( Planning7_PutOffGstHse( dps ) );					// 23	8	0 - 5		| 0 - 0.571		| 0 - 1		|
	//Points scored within level just ended - not prev levels. Default Points per level is 1950 = Dots + Pills, no Ghosts.

	record.push_back( dps[s-1].Points );								// 24	3	3200-62910	| 1303 - 4194	| 0.311 - 1	|
	
	record.push_back( resource[0] );									// 25	2	0 - 142		|				| 0 - 1		|

	record.push_back( resource[1] );									// 26	8	0 - 1		|				| 0 - 1		|	

	record.push_back( simple[0] );										// 27	4	220 - 478	|				| 0.459 - 1	|

	record.push_back( simple[1] );										// 28	5	2174 - 9598	|				| 0.226 - 1	|
	// For a single level this should be equivalent to R2_AvgChryPerLvl
	record.push_back( simple2[0] );										// 29	11	0 - 16		| 0 - 1			| 0 - 1		|

	record.push_back( simple2[1] );										// 30	1	0 - 19		|				| 0 - 1 	|

	record.push_back( Simple4_TeleportUse( dps ) );						// 31	2	0 - 31		| 0 - 3			| 0 - 1 	|

	field.push_back( "S5_GhostHunts" );
	field.push_back( "A1_AtkGstHse" );
	field.push_back( "A3_GhostKills" );
	field.push_back( "A4_HuntAfterPill" );
	field.push_back( "A6_Chase¬Dots" );
	field.push_back( "C1b_ThreatPerception_NumKilled" );
	field.push_back( "C2b_AvgDstPacAtkCntrd" );
	field.push_back( "C3_CloseCalls" );
	field.push_back( "C4_CaughtAfterHunt" );
	field.push_back( "C5_MvsWoutGain" );
	field.push_back( "C6_PntsOvrLives" );
	field.push_back( "C7_KilledAtHouse" );
	field.push_back( "ChryBool_Sum" );
	field.push_back( "CycleCountPerState_SDev" );
	field.push_back( "D1_CyclesPerMove" );
	field.push_back( "D2_Osclltng" );
	field.push_back( "Lives_SDev" );
	field.push_back( "P1_LureToPill" );
	field.push_back( "P1b_2GstsLrd2Pll" );
	field.push_back( "P1c_GstsEtnOnPll" );
	field.push_back( "P1d_PacEtnB4GetPll" );
	field.push_back( "P3_CntDtsB4Pll1" );
	field.push_back( "P4a_HuntGst1_AvgSpd" );
	field.push_back( "P4b_HuntGst2_AvgSpd" );
	field.push_back( "P7_PutOffGstHse" );
	field.push_back( "Points_Max" );
	field.push_back( "R1_AvgTimeToEatChrry" );
	field.push_back( "R2_AvgChrryPerLvl" );
	field.push_back( "S1b_AvgStates" );
	field.push_back( "S1c_AvgCycles" );
	field.push_back( "S2a_LivesGained" );
	field.push_back( "S2b_LivesLost" );
	field.push_back( "S4_TeleportUse" );
	

/*
	field.push_back( "A4_HuntAfterPill" );

	record.push_back( Aggregate1_LivesSDev( dps ) );				// .01	0.38 - 2.54	|				| 0.148 - 1
	field.push_back( "Lives_SDev" );

	record.push_back( Simple2_LivesChng( dps )[0] );				// .02	0 - 16		| 0 - 1			| 0 - 1
	field.push_back( "S2.a_LivesGained" );

	record.push_back( simple[1] );									// .03	2174 - 9598	|				| 0.226 - 1
	field.push_back( "S1.c_AvgCycles" );

	record.push_back( Aggregate2_ChryBoolSum( dps ) );				// .04	25 - 2048	| 7.1 - 165.2	| 0.043 - 1
	field.push_back( "ChryBool_Sum" );
	
	record.push_back( Caution4_CaughtAfterHunt( dps ) );			// .05	0 - 9		| 0 - 1.25		| 0 - 1
	field.push_back( "C4_CaughtAfterHunt" );

	record.push_back( resource[0] );								// .06	0 - 142		|				| 0 - 1
	field.push_back( "R1_AvgTimeToEatChry" );

	record.push_back( Decisive2_Osclltng( dps ) );					// .07	124 - 2241	| 62 - 181.5	| 0.342 - 1
	field.push_back( "D2_Osclltng" );

	record.push_back( Caution5_MovesWithoutGain( dps ) );			// .08	118 - 2404	| 57.5 - 256.5	| 0.224 - 1
	field.push_back( "C5_MvsWoutGain" );

	record.push_back( Simple4_TeleportUse( dps ) );					// .09	0 - 31		| 0 - 3			| 0 - 1
	field.push_back( "S4_TeleportUse" );

	record.push_back( Aggregate3_CycleCountPerStateSDev( dps ) );	// .10	6.9 - 57.3	|				| 0.12 - 1
	field.push_back( "CycleCountPerState_SDev" );

	record.push_back( Aggression6_Chase¬Dots( dps ) );				// .11	1 - 118		| 0.25 - 11.67	| 0.021 - 1 
	field.push_back( "A6_Chase¬Dots" );

	record.push_back( Planning1_LureToPill( dps )[0] );				// .12	0 - 24		| 0 - 2.67		| 0 - 1
	field.push_back( "P1_LureToPill" );

	record.push_back( simple[0] );									// .14	220 - 478	|				| 0.459 - 1
	field.push_back( "S1.c_AvgStates" );

	record.push_back( Planning7_PutOffGstHse( dps ) );				// .15	0 - 5		| 0 - 0.571		| 0 - 1
	field.push_back( "P7_PutOffGstHse" );

	record.push_back( Caution7_KilledAtHouse( dps ) );				// .16	0 - 5		| 0 - 1.5		| 0 - 1
	field.push_back( "C7_KilledAtHouse" );

	record.push_back( Caution1_ThreatPrcptn( dps )[0] );			// .17	0 - 6		| 				| 0 - 1
	field.push_back( "C1.b_ThreatPerception#Killed" );

	record.push_back( resource[1] );								// .18	0 - 1		|				| 
	field.push_back( "R2_AvgChrryPerLvl" );

	float a3 = Aggression3_GhostKills(dps); record.push_back( a3 );	// .19	0.021-1.217	|				| 0.017 - 1
	field.push_back( "A3_GhostKills" );

	record.push_back( Caution3_CloseCalls( dps ) );					// .20	1 - 9		| 0.071 - 1.5	| 0.048 - 1
	field.push_back( "C3_CloseCalls" );

	record.push_back(Planning4_SpeedOfHunt(dps)[2] / maxInt(1,a3-4) );	// .21	0 - 2307	|				| 0 - 1
	field.push_back( "HuntGst2_AvgSpd" );
*/
	/******************************
	 *	Output All Derived Features
	 */
	string outish = ""; //"MaxPlayerType,";
	for(int i = 0; i < field.size(); i++)
	{
		outish += field[i];
		outish += ",";
	}
	outish.resize(outish.size() - 1);
	outish += "\n";
	fputs(outish.c_str(), outf);
	outish.clear(); //= p; //MaxPlayerType : value
//	outish += ",";

	for(int j = 0; j < record.size(); j++)
	{
		sprintf_s(out, 99, "%f", record[j]);
		outish += out;
		outish += ",";
	}
	outish.resize(outish.size() - 1);
	fputs(outish.c_str(), outf);
/**/
	fflush(outf);
	fclose(outf);
}

void RestructureDGDs()
{
	array <String^>^plyrCsv = Directory::GetFiles("./");
	String^ csv = Path::GetFileNameWithoutExtension(plyrCsv[0]);
	char* player = (char*)(void*)Marshal::StringToHGlobalAnsi(csv);

	string pcsv = "";
	pcsv += player;
	pcsv += ".csv";
	FILE *source;
	fopen_s(&source, pcsv.c_str(), "r");
	
	pcsv = "";
	pcsv += player;
	pcsv += "Type.csv";
	fopen_s(&outf, pcsv.c_str(), "wt");
/** /	
	fputs(player, outf);
	fputs("\n", outf);
	fputs(pcsv.c_str(), outf);
	fputs("\n", outf);	
/**/
	int c = 0;
	pcsv = "";
	string plyrHdr;
	string plyrData;
	bool hdr = true;

	while(true)
	{
		if( (c = fgetc(source)) == EOF )
		{
			if( hdr )
				//Remove the last data record's last char, the comma
				plyrData.resize(plyrData.size()-1);
			break;
		}else if( c == '\n' )
		{
			plyrData += ',';
			hdr = true;
			c = fgetc(source);
		}
		if( hdr )
		{
			if( c == ',' )
				hdr = false;
			plyrHdr += c;
		}else{
			if( c == ',' )
				c = '.';
			plyrData += c;
		}
	}

	//Remove the last header's last char, the comma
	plyrHdr.resize(plyrHdr.size()-4);
	plyrHdr += "IP";
	//Put all the headers out to the file
	fputs(plyrHdr.c_str(), outf);
	fputs("\n", outf);
	//Put all the headers out to the file
	fputs(plyrData.c_str(), outf);
/**/
	fclose(source);
	fflush(outf);
	fclose(outf);
}

void ConjoinLevels()
{
/**/
	array <String^>^plyrData = Directory::GetFiles("../");
	String^ plyr = ""; //Path::GetFileNameWithoutExtension(plyrData[0]);
	plyr += "Game-.csv";
	char* player = (char*)(void*)Marshal::StringToHGlobalAnsi(plyr);
	fopen_s(&outf, player, "wt");
/** /
	printf("Enter DGD type (& ID) to assign: ");
	gets_s( out );
	string type = "";
	type += out;
	type += "Game.csv";
	fopen_s(&outf, type.c_str(), "wt");
/** /
	
	fputs("StateNum,PacX,PacY,Gst1X,Gst1Y,Gst2X,Gst2Y,Dots,Lives,Pills,Points,ChryBool,PacAtkBool,LastMoveDir,Cycles,", outf);

	for(int x = 0; x < 20; x++)
	{
		for(int y = 0; y < 20; y++)
		{
			fputs("x", outf);
			sprintf_s(out, 99, "%d", x);
			fputs(out, outf);
			fputs(" y", outf);
			sprintf_s(out, 99, "%d", y);
			fputs(out, outf);
			if(!(x == 19 && y == 19))
				fputs(",", outf);
		}
	}

	fputs("\n", outf);
/**/
	FILE *states;
	string level = "Level-1.csv";
	int count = 2;
	int a = -1, b = -1, c = -1;
	string state("");

	while( fopen_s( &states, level.c_str(), "r" ) == 0 )
	{
		while( true )
		{
			c = fgetc(states);
			if( c == EOF )
			{
				if( (a == '\n' || a == ',') && b == '\n' )
				{
					state.resize(state.size()-1);
					state += '\n';
				}
				break;
			}
			else if( a == -1 && b == -1 && c == '\n' )
			{}
			else if( b == ',' && c == '\n' )
			{
				state.resize(state.size()-1);
				state += c;
			}else
				state += c;
			a = b;
			b = c;
		}
//		state.resize(state.size()-1);
		fputs(state.c_str(), outf);
		state = "";

		level = "Level-";
		sprintf_s(out, 99, "%d", count);
		level += out;
		level += ".csv";
		count++;
	}

	fflush(outf);
	fclose(outf);
}

/**
 *	Description: 
 */
void ExtractInfo()
{
/**/
	array <String^>^plyrData = Directory::GetFiles("../");
	String^ plyr = Path::GetFileNameWithoutExtension(plyrData[0]);
	plyr = plyr->Remove( 3 );
	plyr += "_";
	plyrData[0] = Directory::GetCurrentDirectory();
	plyr += plyrData[0]->Substring( plyrData[0]->Length - 15 );
/*
	time_t curr;
	tm *local = &tm();
	time(&curr); // get current time_t value
	localtime_s(local, &curr);
	strftime( out, 32, "%y-%j-%H-%M-%S", local );
	for( int i = 0; i < 15; i++ )
	{
		plyr += out[i];
	}
*/
	plyr += "_1Rules.csv";
	char* player = (char*)(void*)Marshal::StringToHGlobalAnsi(plyr);
	fopen_s(&outf, player, "wt");

	FILE *states;
	string level = "1RuleCsfy-1.csv";
	int count = 2;
	int a = -1, b = -1, c = -1;
	string state("");
	deque <string> csv;

	while( fopen_s( &states, level.c_str(), "r" ) == 0 )
	{
		while( true )
		{
			if( (c = fgetc(states)) == EOF )
			{
				csv.push_back( "EOF" );
				break;
			}
			state += c;
			if( c == ',' || c == '\n' )
			{
				csv.push_back( state );
				state.clear();
			}
		}

		state = "";

		level = "1RuleCsfy-";
		sprintf_s(out, 99, "%d", count);
		level += out;
		level += ".csv";
		count++;
	}

	int lines = 0;
	for( int i = 0; i < csv.size(); i++ )
	{
		if( csv[i].compare(0, 7, "Conqr (") == 0 || csv[i].compare(0, 5, "Not (") == 0 )
		{
			csv[i].resize(csv[i].size()-1);
			state += csv[i];
			state += ",";
			state += csv[i+4];
			state += ",";
			if( lines < 3 )
				state += ",,,";
		}
		if( csv[i].compare(0, 7, "Feature") == 0 && csv[i+1].compare(0, 7, "Avg_Val") == 0 )
		{
			state += "\n";
			lines++;
		}
	}

	fputs(state.c_str(), outf);
	fflush(outf);
	fclose(outf);
}

/**
 *	Description: Writes all the state information from all the states in the GameStates global deque, in CSV format.
 */
string OutputState(deque<PacState> GameStates)
{
	string news("\n");

	for( int i = 0; i < GameStates.size(); i++ )
	{
		sprintf_s( out, 99, "%d", GameStates[i].State );
		news += out; news += ",";
		sprintf_s( out, 99, "%d", GameStates[i].xPac );
		news += out; news += ",";
		sprintf_s( out, 99, "%d", GameStates[i].yPac );
		news += out; news += ",";
		for( int j = 0; j < NUM_GS; j++ )
		{
			sprintf_s( out, 99, "%d", GameStates[i].xGhost[j] );
			news += out; news += ",";
			sprintf_s( out, 99, "%d", GameStates[i].yGhost[j] );
			news += out; news += ",";
		}
		sprintf_s( out, 99, "%d", GameStates[i].Dots );
		news += out; news += ",";
		sprintf_s( out, 99, "%d", GameStates[i].Lives );
		news += out; news += ",";
		sprintf_s( out, 99, "%d", GameStates[i].Pills );
		news += out; news += ",";
		sprintf_s( out, 99, "%d", GameStates[i].Points );
		news += out; news += ",";
		if( GameStates[i].CherryThere )
			news += "1,";
		else
			news += "0,";
		if( GameStates[i].PacAttack )
			news += "1,";
		else
			news += "0,";
		sprintf_s( out, 99, "%d", GameStates[i].MovedDir );
		news += out; news += ",";
		sprintf_s( out, 99, "%d", GameStates[i].Cycle );
		news += out; news += ",";
/**/	for( int x = 0; x < 20; x++ )
			for( int y = 0; y < 20; y++ )
			{
				sprintf_s( out, 99, "%d", GameStates[i].dtpMap[x][y] );
				news += out; news += ",";
			}
/**/	news += "\n";
	}

	return news;
}

int maxInt( int a, int b )
{
	if( a > b )
		return a;

	return b;
}
//end of file
