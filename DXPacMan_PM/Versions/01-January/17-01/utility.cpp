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
float util_P4_SpeedOfHunt( deque<PacState> arg );
float util_S2_LivesChng( deque<PacState> arg );
float util_S4_TeleportUse( deque<PacState> arg );

//Legacy utility calculation functions - must replace with scaled down versions of the feature derivation functions.
double utilityDots(PacState pS);
double utilityPills(PacState pS, int risk);
double utilityGhostHunt(PacState pS, int risk);
double utilityGhosts(PacState pS, int risk);

//Functions for doing handy things
deque<PacState> prependReality( int lookback, deque<PacState> pre );

//Names for Features
string feats[] = {
	",AtkGstHse:,", 
	",HuntAfter:,", 
	",Chase¬Dots:,",	 
	",CaughtAfterHunt:,", 
	",AvgDst_PacA:,", 
	",SpeedHunt:,", 
	",ThreatPrcptn:,", 
	",CloseCalls:,", 
	",AvgDst_GstA:,", 
	",Points:,", 
	",CountUntilCherryEaten:,", 
	",LivesChange:,", 
	",Teleport:,",
	",MvsWOutGain:," 
};

//Weights for the features
float weights[] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
float holdwts[] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};

#define FTS 14

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
			weights[i] = abs(weights[i] + (rand()%m / t));
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
/**/
		utils[6] = util_C1_ThreatPrcptn( States );
/**/
		utils[7] = util_C3_CloseCalls( States );
/**/
		utils[8] = util_C2_AvgDstGstCtrd( States );
/**/
	}
/**/
	utils[9] = (States[s-1].Points - actual_States[S].Points);
/**/
	if( actual_States[S].CherryThere )
	{
		utils[10] = util_Agg2_ChryBoolSum( States );
	}
/**/
	utils[11] = util_S2_LivesChng( States );
/**/
	utils[12] = util_S4_TeleportUse( States );
/**/
	utils[13] = util_C5_MovesWithoutGain( States );
/**/
	for( int f = 0; f < FTS; f++ )
	{
		utilitay[0] += (utils[f] * weights[f]);
		utilitay[1] += (utils[f] * holdwts[f]);

		if( utils[f] != 0 )
		{
			sprintf_s( out, 99, "%f", utils[f] );
			logData += feats[f]; logData += out;
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

/*****************************************************************************************/
/*	OLDER UTILITY FUNCTIONS WHICH CONSIDER A COMPLETE SINGLE STATE, NOT MULTIPLE STATES	 */
/*****************************************************************************************/

/**
 * Description: Utility of Dots is number and density of clusters, plus proximity to Pacman.
 * Arguments:	PacState to evaluate
 */
double utilityDots( PacState pS )
{
	double util = 0.0;
	//Calculate the utility of each dot left on the map.
	for(int i = 0; i < 20; i++)
		for(int j = 0; j < 20; j++)
			if(pS.dtpMap[i][j] == 4)
			{
				util += 9.0;
				//Distribution density gives utility - singular dots are less utile
				double inc = (double)localDots(i, j, 1, pS.dtpMap);
				util += inc;
				if(inc > 1.0)
					util += 0.33 * (localDots(i, j, 2, pS.dtpMap) - inc);
				//Distance from Pacman reduces utility - close dots are easier to eat, less long-term risk.
				util -= 1.0 / myHeuristicDist(pS.xPac, pS.yPac, i, j);
			}

	return util;
}

/**
 * Description: Utility of Ghosts is proximity of Ghosts to Pacman, and how threatening their deployment is
 *				Here we have to account to a degree for the ability of humans to lookahead, to predict. Utility is not just the current state, but the relationship of current state to future states.
 * Arguments:	PacState to evaluate
 *				risk : a measure of how vulnerable Pacman is (# lives)
 */
double utilityGhosts(PacState pS, int risk)
{
	double util = 0.0;

	//Calculate if Pacman is trapped by ghosts, and how many exits he has if not
//	int locBound = 4;	int locale = localGhosts(xPac, yPac, locBound);		if( locale > 1 ){}

	//The average distance of the ghosts is good if large
	int dist = 0;
	for(int i = 0; i < NUM_GS; i++)
		dist += myHeuristicDist(pS.xPac, pS.yPac, pS.xGhost[i], pS.yGhost[i]);
	util += 100.0 - (dist / NUM_GS);

	//The distribution size of the ghosts is good if small.
	util += 100.0 / avgDistribution(pS.xGhost, pS.yGhost);

	// The distance by which ghosts increase an A* path from Pacman to his inverse x,y indicates bad utility
	int temp = a_Star(pS.xPac, pS.yPac, 20-pS.xPac, 20-pS.yPac, true, pS);
	int tempi = 0;
	for(int i = 0; i < NUM_GS; i++)
	{
		pS.dtpMap[pS.xGhost[i]][pS.yGhost[i]] = 1;
		tempi = a_Star(pS.xPac, pS.yPac, 20-pS.xPac, 20-pS.yPac, true, pS);
		util += (temp - tempi) * 10.0;
		temp = tempi;
	}

//	if( a_Star(xPac, yPac, 20-xPac, 20-yPac, true) < locBound )		return -100.0; //no way out! Check this
	
	return util;
}

/**
 * Description: Utility of Pills is proximity of one Pill to Pacman, then utilityGhostHunt(). This function only called if !PacAttack.
 * Arguments:	PacState to evaluate
 *				risk : a measure of how vulnerable Pacman is (# lives)
 */
double utilityPills(PacState pS, int risk)
{
	double util = 0.0;

	if(pS.Pills == 0)
		return 0;
		
	util = pS.Pills * 50;
	deque <int> mDist; //Deque of distances between things
	int aS = 0;

	deque <int> xPill;
	deque <int> yPill;
	//Midpoint between Pacman and the centroid of all the Ghosts
	int xCentroid = 0, yCentroid = 0;
	for(int i = 0; i < NUM_GS; i++)
	{
		xCentroid += pS.xGhost[i];
		yCentroid += pS.yGhost[i];
	}
	xCentroid = ((xCentroid / NUM_GS) + pS.xPac) / 2;
	yCentroid = ((yCentroid / NUM_GS) + pS.yPac) / 2;
	
	//Get the closest pill's x and y
	if(pS.dtpMap[1][3] == 5){
		mDist.push_back( myHeuristicDist(xCentroid, yCentroid, 1, 3) );
		xPill.push_back(1);
		yPill.push_back(3);
	}
	if(pS.dtpMap[18][3] == 5){
		mDist.push_back( myHeuristicDist(xCentroid, yCentroid, 18, 3) );
		xPill.push_back(18);
		yPill.push_back(3);
	}
	if(pS.dtpMap[1][15] == 5){
		mDist.push_back( myHeuristicDist(xCentroid, yCentroid, 1, 15) );
		xPill.push_back(1);
		yPill.push_back(15);
	}
	if(pS.dtpMap[18][15] == 5){
		mDist.push_back( myHeuristicDist(xCentroid, yCentroid, 18, 15) );
		xPill.push_back(18);
		yPill.push_back(15);
	}
	
	//Note the index of the closest Pill and derive utility for it by distance
	mDist.push_front(lowest(mDist));

	aS = a_Star(pS.xPac, pS.yPac, xPill[mDist[0]], yPill[mDist[0]], false, pS);
	if( aS == 0 )
		aS++;

	//Utility is heavily influenced by initial proximity of Pacman to Pill - this is a counter-weight
	util -= 100.0 * (double)aS;

	// Reset xPac yPac vars to the value's of the closest Pill, from whence Pacman will launch his attack.
	pS.xPac = xPill[mDist[0]];
	pS.yPac = yPill[mDist[0]];

	util += utilityGhostHunt(pS, risk);

	return util;
}

/**
 * Description: Utility of Ghost Hunt is the chained proximity of Ghosts to Pacman.
 * Arguments:	PacState to evaluate
 *				risk : a measure of how vulnerable Pacman is (# lives)
 */
double utilityGhostHunt(PacState pS, int risk)
{
	double util = 0.0;
	deque <int> mDist; //Deque of distances between things
	deque <int> chain; //Deque of indexes to the ghost array
	double inc = 0.0;
	int aS = 0;

	//Start with closest ghost to Pacman...
	for(int i = 0; i < NUM_GS; i++)
		mDist.push_back( manhattanDist(pS.xPac, pS.yPac, pS.xGhost[i], pS.yGhost[i]) );
	//The index of the lowest ghost manhattan distance measure to Pacman
	chain.push_back(lowest(mDist));

	//...then the closest to the closest ghost (as we assume Pacman moved there to eat him), etc.
	for(int i = 0; i < NUM_GS-1; i++)
	{
		mDist.clear();
		for(int j = 0; j < NUM_GS; j++)
		{
			bool jump = false;
			for(unsigned int k = 0; k < chain.size(); k++)
				if(j == chain[k])
					jump = true;
			if(!jump)
				mDist.push_back( manhattanDist(pS.xGhost[chain[i]], pS.yGhost[chain[i]], pS.xGhost[j], pS.yGhost[j]) );
			else
				mDist.push_back( 99 );
		}
		chain.push_back(lowest(mDist));
	}

	//Now get the distance to the closest ghost
	aS = a_Star(pS.xPac, pS.yPac, pS.xGhost[chain[0]], pS.yGhost[chain[0]], false, pS);
	if( aS == 0 )
		aS++;
	//Utility of catching first ghost is relative to the points obtained
	inc = 100 / (double)aS;
	util += inc;

	//Utility of catching later ghosts is more heavily penalised by distance and risk, but grows expontentially, as with the points scored
	for(int i = 1; i < chain.size(); i++)
	{
		aS = a_Star(pS.xGhost[chain[i-1]], pS.yGhost[chain[i-1]], pS.xGhost[chain[i]], pS.yGhost[chain[i]], false, pS);
		if( aS == 0 )
			aS++;
		inc = (pow(2.0, i) * 100.0) / ((double)aS * (i + risk));
		util += inc;
	}
	return util;
}

//End of file