/**
 *	Author: Ben Cowley, 2008.
 *	Decision Theory Player Modelling functions for predicting player utility
 */
#include "main.h"
#include "dtpm.h"

//New Utility functions based on most important derived features from rule ensemble
float util_A1_AtkGstHse( deque<PacState> arg );
float util_A4_HuntAfterPill( deque<PacState> arg );
float util_A6_Chase¬Dots( deque<PacState> arg );
float util_Agg1_LivesSDev( deque<PacState> arg );
float util_Agg2_ChryBoolSum( deque<PacState> arg );
float util_C1_ThreatPrcptn( deque<PacState> arg );
float util_C2_AvgDstGstCtrd( deque<PacState> arg );
float util_C2a_AvgDstPac( deque<PacState> arg );
float util_C3_CloseCalls( deque<PacState> arg );
float util_C4_CaughtAfterHunt( deque<PacState> arg );
float util_C5_MovesWithoutGain( deque<PacState> arg );
float util_C7_KilledAtHouse( deque<PacState> arg );
float util_D2_Vacillate( deque<PacState> arg );
deque<float> util_P1_LureToPill( deque<PacState> arg );
float util_P4_SpeedOfHunt( deque<PacState> arg );
float util_S2_LivesChng( deque<PacState> arg );
float util_S4_TeleportUse( deque<PacState> arg );

/*	Functions for doing handy things */
deque<PacState> prependReality( int lookback, deque<PacState> pre );

//Names for Features
string feats[] = {
	",AtkGstHse:,",				//0
	",HuntAfter:,",				//1
	",Chase¬Dots:,",			//2
	",CaughtAfterHunt:,",		//3
	",AvgDst_PacA:,",			//4
	",SpeedHunt:,",				//5
	",ThreatPrcptn:,",			//6
	",CloseCalls:,",			//7
	",AvgDst_GstA:,",			//8
	",Points:,",				//9
	",CountUntilCherryEaten:,", //10
	",LivesChange:,",			//11
	",Teleport:,",				//12
	",MvsWOutGain:,",			//13
	",LureToPill 0:,",			//14
	",LureToPill 1:,",			//15
	",LureToPill 2:,",			//16
	",LureToPill 3:,",			//17
	",LureToPill 4:,",			//18
	",KilledAtHouse:,",			//19
	",D2_Vacillate:,"			//20
};

//Weights for the features			3	4				7	8					12	13				16	17				20
float weights[] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
float holdwts[] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};

#define FTS 21

////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////             UTILITY FUNCTIONS                                              /////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 *	Description: Capture the values of weights[] in the array holdwts[]
 */
void hold()
{
	for( int i = 0; i < FTS; i++ )
	{
		holdwts[i] = weights[i];
	}
}

/**
 *	Description: put the holdwts[] back into weights[]
 */
void rollback()
{
	for( int i = 0; i < FTS; i++ )
	{
		weights[i] = holdwts[i];
	}
}

/**
 *	Description: Alter the weights of the features
 */
void learn( int m, float t )
{
	for( int i = 0; i < FTS; i++ )
	{
//		if( i == m )
			weights[i] = abs(weights[i] + (float)(rand()%m / t));
	}
}

/**
 * Description: Utility of a state is a measure of the best possible end result that can be obtained from that state - i.e. we assume an optimally played game
 * Arguments:	parameters of the utility calculation formulae - when the formulae are ready, this will supply parameters for each in order of occurence of use.
 */
deque<float> utility( deque <PacState> States )
{
	//Weight a feature set polynomial or something similar
	deque<float> utilitay( 2, 0 );
	float util = 0.0;
	deque<float> utils( FTS, 0 );
	int s = States.size(), S = actual_States.size()-1;
	PacState pS;

	if( s > 0 )
	{
		/*	Here we call the various functions to determine utility - depending on the state we're in some may not be called	*/
		for( int i = S; i > max( 0, S-lookahead ); i-- )
		{
			if( actual_States[i].PacAttack )
			{
/**/
				utils[0] = util_A1_AtkGstHse( States );
/**/
				utils[1] = util_A4_HuntAfterPill( States );
/**/
				utils[2] = util_A6_Chase¬Dots( States );
/**/
				utils[3] = util_C4_CaughtAfterHunt( States );
/**/
				utils[4] = (20 - util_C2a_AvgDstPac( States ));
/**/
				utils[5] = util_P4_SpeedOfHunt( States );
/**/
				util++;
				break;
			}
		}

		//If no PacAttack states have occured in the last few actual states, trigger our !PacAttack only states
		if( util == 0 )
		{
/** /
			utils[6] = util_C1_ThreatPrcptn( States );
/**/
			utils[7] = util_C3_CloseCalls( States );
/**/
			utils[8] = util_C2_AvgDstGstCtrd( States );
/**/
		}
/**/
		utils[9] = (States[s-1].Points - actual_States[S].Points);
/** /
		if( actual_States[S].CherryThere )
		{
			utils[10] = util_Agg2_ChryBoolSum( States );
		}
/**/
		utils[11] = util_S2_LivesChng( States );
/** /
		utils[12] = util_S4_TeleportUse( States );
/**/
		utils[13] = util_C5_MovesWithoutGain( States );
/**/
		if( util == 0 )
		{
/** /
			deque<float> p1utils = util_P1_LureToPill( States );
			utils[14] == p1utils[0];
			utils[15] == p1utils[1] / 10;
			utils[16] == p1utils[2] * Lvl;
			utils[17] == p1utils[3] * 100;
			utils[18] == p1utils[4];
/**/
			utils[19] == util_C7_KilledAtHouse( States );
/**/
		}
/**/
		utils[20] == util_D2_Vacillate( States );
/**/
		for( int f = 0; f < FTS; f++ )
		{
			utilitay[0] += (utils[f] * weights[f]);
			utilitay[1] += (utils[f] * holdwts[f]);

			if( utils[f] != 0 )
			{
				sprintf_s( out, 99, "%f", utils[f] );
				logData += feats[f]; logData += out;
			}else{
				logData += feats[f];
			}
		}
/**/
		sprintf_s( out, 99, "%f", utilitay[0] );
		logData += ",Utility:,"; logData += out;
		sprintf_s( out, 99, "%f", utilitay[1] );
		logData += ","; logData += out;
/** /
		for( int i = 0; i < s; i++ )
		{
			pS = States[i];

			sprintf_s(out, 99, "%d", pS.State);
			print( out ); print( "," );
			sprintf_s(out, 99, "%d", pS.getProb());
			print( out ); print( "," );
		}
		print( "\n" );
/**/
	}else
		logData += "Bad Branch, No utility";

	return utilitay;
}

/********************************************************************************************/
/*	Functions for calculation of features from short runs of states (prediction branches)	*/	
/********************************************************************************************/

/**
 *	Description: Count times when Pacman attacks the ghosts and follows them right into their house
 */
float util_A1_AtkGstHse( deque<PacState> arg )
{
	float output = 0.0;
	PacState pS;
	int s = arg.size(), yHse, yP;

	for( int i = 0; i < s; i++ )
	{
		pS = arg[i];

		if( PacMap[9][10] == 1 )
		{
			yHse = 9;
			yP = 7;
		}
		else
		{
			yP = 10;
			yHse = 8;
		}

		if( pS.PacAttack && 
			( myHeuristicDist( pS.xGhost[0], pS.yGhost[0], 9, yHse ) < 3 || myHeuristicDist( pS.xGhost[1], pS.yGhost[1], 9, yHse ) < 3 ))
		{
			while( pS.PacAttack && i < s-1 )
			{
				pS = arg[++i];
				if( myHeuristicDist( pS.xPac, pS.yPac, 9, yP ) < 5 )
				{
					output++;
					break;
				}
			}
		}
	}

	return output;
}

/**
 *	Description	: Derivation of Clementine code which counts the number of times this condition is met:
 *	((@DIFF1(dst_PacG1) <= 0 and dst_PacG1 < 5) or 
 *	(@DIFF1(dst_PacG2) <= 0 and dst_PacG2 < 5)) and 
 *	(@SINCE0(PacAtkBool = 1,20) < 10) and 
 *	PacAtkBool = 0
 */
float util_A4_HuntAfterPill( deque<PacState> arg )
{
	float output = 0.0;
	PacState pS;
	int s = arg.size();

	for( int i = 0; i < s-1; i++ )
	{
		pS = arg[i];
		if( !pS.PacAttack )
		{		
			int d0 = manhattanDist( pS.xPac, pS.yPac, pS.xGhost[0], pS.yGhost[0] );
			int d1 = manhattanDist( pS.xPac, pS.yPac, pS.xGhost[1], pS.yGhost[1] );

			pS = arg[++i];

			int d0next = manhattanDist( pS.xPac, pS.yPac, pS.xGhost[0], pS.yGhost[0] );
			int d1next = manhattanDist( pS.xPac, pS.yPac, pS.xGhost[1], pS.yGhost[1] );			

			if( (d0next < 5 &&  d0next < d0) || (d1next < 5 &&  d1next < d1) )
			{
				output++;
			}
		}
	}

	return output;
}

/**
 *	Description:Whether the player uses the Pills to chase ghosts, or just continues collecting Dots
 *	Pseudocode:	while(PacAtkBool){ if( dist(PacXY, Gst{1,2}XY):decrease ) then Output++ }
 *	Clementine:	((@DIFF1(dst_PacG1) <= 0 and dst_PacG1 < 5) or (@DIFF1(dst_PacG2) <= 0 and dst_PacG2 < 5)) and 
 *				(@SINCE0(PacAtkBool = 0,20) < 10) and PacAtkBool = 1
 *	Returns		: [0-3] a value at each Pill - number of states in which Pacman is getting closer to a Ghost DURING Pill; [4] number of previous values != 0
 */
float util_A6_Chase¬Dots( deque<PacState> arg )
{
	float output = 0.0;
	int s = arg.size();
	PacState pS;

	for( int i = 0; i < s; i++ )
	{
		pS = arg[i];
		if( pS.PacAttack == true && i < s-1)
		{
			int d0 = manhattanDist( pS.xPac, pS.yPac, pS.xGhost[0], pS.yGhost[0] );
			int d1 = manhattanDist( pS.xPac, pS.yPac, pS.xGhost[1], pS.yGhost[1] );
		
			pS = arg[++i];

			int d0next = manhattanDist( pS.xPac, pS.yPac, pS.xGhost[0], pS.yGhost[0] );
			int d1next = manhattanDist( pS.xPac, pS.yPac, pS.xGhost[1], pS.yGhost[1] );	
			
			if( (d0next < d0 && d0next < 5) || (d1next < d1 && d1next < 5) )
			{
				output++;
			}
		}
	}

	return output;
}

/**
 *	Description: Standard deviation of the player’s Lives over the period of arg
 */
float util_Agg1_LivesSDev( deque<PacState> arg )
{
	float output = 0.0;
	float s = arg.size();
	float avgLives = 0.0;

	for( int i = 0; i < s; i++ )
	{
		avgLives += (float)arg[i].Lives;
	}
	avgLives /= s;

	for( int i = 0; i < s; i++ )
	{
		output += (float)((arg[i].Lives - avgLives) * (arg[i].Lives - avgLives));
	}
	if( output > 0 )
		output = sqrtf(output / (s-1));

	return output;
}

/**
 *	Description: Sum of Cherry Boolean flag over length of game in arg – gives total time onscreen
 */
float util_Agg2_ChryBoolSum( deque<PacState> arg )
{
	float output = 0.0;
	int s = arg.size(), i;

	for( i = 1; i < s; i++ )
	{
		if( arg[i-1].CherryThere && !arg[i].CherryThere )
		{
			for( int j = actual_States.size() - 1; j > 0; j-- )
			{
				i++;
				if( !actual_States[j].CherryThere )
					break;
			}
		}
	}
	output = (float)i;

	return output;
}

/**
 *	Description: Count if Pacman gets trapped in corridor by Ghosts – i.e. one ghost on each possible direction of movement (vector). 
 *	Return: a deque of length 2: [0] Count times Pacman gets trapped; [1] Whether he gets killed as a result
 */
float util_C1_ThreatPrcptn( deque<PacState> arg )
{
	float output = 0.0;
	PacState pS;
	int s = arg.size(), len;

	for( int i = 0; i < s; i++ )
	{
		pS = arg[i];
		if( manhattanDist( pS.xPac, pS.yPac, pS.xGhost[0], pS.yGhost[0] ) < 5 && 
			manhattanDist( pS.xPac, pS.yPac, pS.xGhost[1], pS.yGhost[1] ) < 5 )
		{
			//find path to for (pS.xPac, pS.yPac, 20-pS.xPac, 20-pS.yPac)
			//If ghost[0] x,y is on path, block in by setting to 1, then recurse
			for( int c = 0; c < NUM_GS; c++ )
			{
				len = a_Star(pS.xPac, pS.yPac, 20-pS.xPac, 20-pS.yPac, true, pS);
				
				for( int g = 0; g < NUM_GS; g++ )
				for( int p = 0; p < len; p++ )
				{
					if( pS.xGhost[g] == aPathX[p] && pS.yGhost[g] == aPathY[p] )
						pS.dtpMap[ pS.xGhost[g] ][ pS.yGhost[g] ] = 1;
				}
			}
			if( a_Star(pS.xPac, pS.yPac, 20-pS.xPac, 20-pS.yPac, true, pS) < 6 )
			{
				output--;
			
				if( pS.Lives > arg[s-1].Lives )
					output += (arg[i].Lives - (Lvl + 5));// * 100;
			}
		}
	}

	return output;
}

/**
 *	Description: Average distance Pacman keeps from the Ghosts 
 *	Return: While under PacAttack = Avg:manhattan(Pac, Gst Centroid)
 */
float util_C2_AvgDstGstCtrd( deque<PacState> arg )
{
	float output = 0.0;
	PacState pS;
	int s = arg.size(), xCentroid, yCentroid;

	for( int i = 0; i < s; i++ )
	{
		pS = arg[i];
		xCentroid = yCentroid = 0;

		for(int j = 0; j < NUM_GS; j++)
		{
			xCentroid += pS.xGhost[j];
			yCentroid += pS.yGhost[j];
		}
		xCentroid = xCentroid / NUM_GS;
		yCentroid = yCentroid / NUM_GS;
		output += manhattanDist( pS.xPac, pS.yPac, xCentroid, yCentroid );
	}
	output /= s;

	return output;
}

/**
 *	Description: Average distance between Pacman and the nearest ghost
 */
float util_C2a_AvgDstPac( deque<PacState> arg )
{
	float output = 0.0;
	PacState pS;
	int s = arg.size();

	for( int i = 0; i < s; i++ )
	{
		pS = arg[i];

		output += min( manhattanDist( pS.xPac, pS.yPac, pS.xGhost[0], pS.yGhost[0] ), manhattanDist( pS.xPac, pS.yPac, pS.xGhost[1], pS.yGhost[1] ) );
	}
	output /= s;

	return output;
}

/**
 *	Description : How often player comes very close to a ghost, when not on a Pill, and doesn’t die afterwards!
 *	Returns		: if( dist(PacXY, Gst{1,2}XY)@State(t-5) == 1 & Lives@State(t-5) == Lives) then Output++
 */
float util_C3_CloseCalls( deque<PacState> arg )
{
	float output = 0.0;
	int s = arg.size();
	PacState pS;

	for( int i = 0; i < s-1; i++ )
	{
		pS = arg[i];

		if( (manhattanDist( pS.xPac, pS.yPac, pS.xGhost[0], pS.yGhost[0] ) == 1 || 
			manhattanDist( pS.xPac, pS.yPac, pS.xGhost[1], pS.yGhost[1] ) == 1) &&
			arg[s-1].Lives >= arg[0].Lives )
			output++;
	}

	return output;
}

/**
 *	Description: Counts how often Pacman loses a life just as the Pill wears off (within twice the number of states as the distance to nearest ghost right after Pill finish) 
 */
float util_C4_CaughtAfterHunt( deque<PacState> arg )
{
	float output = 0.0;

	int s = arg.size();
	PacState pS;

	for( int i = 1; i < s; i++ )
	{
		pS = arg[i];
		if( arg[i-1].PacAttack && !pS.PacAttack )
		{
			for(int k = 0; k < NUM_GS; k++)
			{
				if( pS.xPac == pS.xGhost[k] && pS.yPac == pS.yGhost[k] )
				{
					output += (arg[i].Lives - (Lvl + 5));// * 100;
					break;
				}
			}
		}
	}

	return output;
}

/**
 *	Description: Counts how often Pacman moves to an empty square 
 */
float util_C5_MovesWithoutGain( deque<PacState> arg )
{
	float output = 0;
	int s = arg.size();

	for( int i = 1; i < s; i++ )
	{
		if( !arg[i].PacAttack && 
			arg[i].Dots == arg[i-1].Dots && 
			arg[i].Lives == arg[i-1].Lives && 
			arg[i].Pills == arg[i-1].Pills )
			output++;
	}

	return output;
}

/**
 *	Description: Count if the player dies collecting Dots around the ghost house
 */
float util_C7_KilledAtHouse( deque<PacState> arg )
{
	float output = 0;
	int s = arg.size();
	PacState pS;

	for( int i = 1; i < s; i++ )
	{
		pS = arg[i-1];
		if( !pS.PacAttack && myHeuristicDist( pS.xPac, pS.yPac, 9, 8 ) < 5 && arg[i].Lives < pS.Lives )
		{
			output++;
		}
	}

	return output;
}

/**
 *	Description: Counts instances where the player made vacillating movements with no ghost around
 */
float util_D2_Vacillate( deque<PacState> arg )
{
	float output = 0.0;
	PacState pS;
	int s = arg.size(), x, y, t;

	pS = arg[0];
	x = pS.xPac;
	y = pS.yPac;
	t = 0;

	while(	manhattanDist( pS.xPac, pS.yPac, pS.xGhost[0], pS.yGhost[0] ) > 9 && 
			manhattanDist( pS.xPac, pS.yPac, pS.xGhost[1], pS.yGhost[1] ) > 9 &&
			t < s-1 )
	{
		t++;
		pS = arg[ t ];
		
		if( x == pS.xPac && y == pS.yPac )
		{
			output--;
			break;
		}
	}

	return output;
}

/**
 *	Description: does player wait near Pill to lure ghosts before eating them?
 */
deque<float> util_P1_LureToPill( deque<PacState> arg )
{
	arg = prependReality( 20 - lookahead, arg );

	deque<float> features(5, 0);//0 - Counts the number of ghosts lured to Pills, when the pills were then eaten
								//1 - Counts how much Pacman moved from when he got close to the Pill while the ghosts were still far away
								//2 - Counts the number of times both ghosts were lured in one go
								//3 - Counts how many Ghosts Pacman actually eats after doing a lure
								//4 - Measures whether Pacman didn't do the lure & get the Pill because he was caught
	PacState pS;
	int s = arg.size();

	for( int i = 1; i < s; i++ )
	{
		pS = arg[i];
		if( !pS.PacAttack && pS.Pills > 0 )
		{
			deque<int> xy = GetClosestPill( pS );
			if( xy[0] != 0 )
			{
				//Now we have Pacman's closest Pill - how close is it, and how close are the Ghosts to him?
				if( myHeuristicDist(pS.xPac, pS.yPac, xy[0], xy[1]) < 8 && 
					manhattanDist(pS.xGhost[0], pS.yGhost[0], pS.xPac, pS.yPac) > 8 && 
					manhattanDist(pS.xGhost[1], pS.yGhost[1], pS.xPac, pS.yPac) > 8 )
				{
					int pxt, pyt;
					//Cycle until the Pill is eaten or Pacman strays away from the Pill again 
					while( !pS.PacAttack && myHeuristicDist(pS.xPac, pS.yPac, xy[0], xy[1]) < 8 && i < s-1 )
					{
						pxt = pS.xPac;
						pyt = pS.yPac;
						pS = arg[++i];
						features[1] += manhattanDist( pxt, pyt, pS.xPac, pS.yPac );
					}
					//If the Pill has been eaten (Pacman must be in proximity to it, so we don't need another dist check)
					if( pS.PacAttack ) 
					{
						//Measure the number of Ghosts now in proximity to Pacman
						if( manhattanDist(pS.xGhost[0], pS.yGhost[0], pS.xPac, pS.yPac) < 8 )
							features[0]++;

						if( manhattanDist(pS.xGhost[1], pS.yGhost[1], pS.xPac, pS.yPac) < 8 )
							features[0]++;

						if( features[0] == 2 )
							features[2] += 1;
					
						//Measure the number of Ghosts caught by Pacman 
						while( pS.PacAttack && i < s-1 )
						{
							pxt = pS.Points;	//using pxt int from above as temp holder of last state's Points, for comparison
							pS = arg[++i];
							if( pS.Points - pxt > 95 )
								features[3]++;
						}
					}
					//Measure whether Pacman didn't do the lure & get the Pill because he was caught
					else if( pS.Lives < arg[0].Lives )
							features[4] += arg[i].Lives - (Lvl + 5);
				}
			}
		}
	}
	return features;
}

/**
 *	Description: Measures the speed with which Pacman makes each of his Ghost kills after eating a Pill
 *	Returns: deque containing - [0] the speed in cycles to 1st ghost kill after Pill
 *								[1] the speed in States to 1st ghost kill after Pill
 *								[2] the speed in cycles to 2nd ghost kill after Pill
 *								[3] the speed in States to 2nd ghost kill after Pill
 */
float util_P4_SpeedOfHunt( deque<PacState> arg )
{
	float output = 0.0;
	PacState pS = arg[0];
	int s = arg.size(), i = 0, pts;

	while( pS.PacAttack && i < s-1 )
	{
		pts = pS.Points;
		pS = arg[++i];
		if( pS.Points > pts + 90 )
		{
			output += arg[s-1].Cycle - pS.Cycle;
			break;
		}
	}

	return output;
}

/**
 *	Description: Counts the number of lives gained (by cherry eating), and lost (by ghost collision)
 *	Returns: a deque with two elements, [0] : gained lives S2.a_LivesGained, [1] : lost lives S2.b_LivesLost
 */
float util_S2_LivesChng( deque<PacState> arg )
{
	float output = 0;
	int s = arg.size();

	for( int i = 1; i < s; i++ )
	{
		if( arg[i].Lives > arg[i-1].Lives )
			output -= (arg[i].Lives - (Lvl + 5));// * 100;
		else if( arg[i].Lives < arg[i-1].Lives )
			output += (arg[i].Lives - (Lvl + 5));// * 100;
	}

	return output;
}

/**
 *	Description: Count Pacman traversal of teleporters
 */
float util_S4_TeleportUse( deque<PacState> arg )
{
	float output = 0.0;
	int s = arg.size();
	PacState pS;

	for( int i = 1; i < s; i++ )
	{
		pS = arg[i];
//		if( pS.dtpMap[ pS.xPac ][ pS.yPac ] == 3 )
		if( pS.yPac == 9 && ((pS.xPac < 2 && arg[i-1].xPac > 17) || (pS.xPac > 17 && arg[i-1].xPac < 2)) )
			output += 100;
	}

	return output;
}

/**
 *	Description: Add some states from the actual_States to the beginning of the argument - i.e. those states that have already happened to those predicted
 *	Returns: Deque of states as described
 */
deque<PacState> prependReality( int lookback, deque<PacState> pre )
{
	int S = actual_States.size();

	for( int i = S-1; i >= S-lookback && i > 0; i-- )
	{
		pre.push_front( actual_States[i] );
	}

	return pre;
}
/**/
//End of file