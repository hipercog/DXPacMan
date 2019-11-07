/**
 *	Author: Ben Cowley, 2008.
 *	Functions for GENERATING features from game log
 */
#include "stdafx.h"
#include "Deriver.h"

/**
 *	Unavoidable Globals
 */
deque <int> aPathX;
deque <int> aPathY;
int g_Levels = 1;

/*****************************************************************************
 *	Function for generating AGGREGATE features
 *****************************************************************************/

/**
 *	Description: Standard deviation of the player’s Lives over the period of arg
 */
float Aggregate1_LivesSDev( deque<PacState> arg )
{
	float output = 0.0;
	float s = arg.size();
	float avgLives = 0.0;

	for( int i = 0; i < s; i++ )
		avgLives += (float)arg[i].Lives;
	avgLives /= s;

	for( int i = 0; i < s; i++ )
		output += (arg[i].Lives - avgLives)*(arg[i].Lives - avgLives);
	if( output > 0 )
		output = sqrtf(output / (s-1));

	return output;
}

/**
 *	Description: Sum of Cherry Boolean flag over length of game in arg – gives total time onscreen
 */
float Aggregate2_ChryBoolSum( deque<PacState> arg )
{
	float output = 0.0;
	int s = arg.size();

	for( int i = 0; i < s; i++ )
	{
		if( arg[i].CherryThere )
			output++;
	}

	return output;
}

/**
 *	Description: Standard Deviation of Cycles per Pacman move, over length of game in arg
 */
float Aggregate3_CycleCountPerStateSDev( deque<PacState> arg )
{	
	float output = 0.0;
	int s = arg.size();
	deque <float> perState;
	float avgCycles = arg[s-1].Cycle / s;
	
	//Obtain each 'cycle count per state'
	perState.push_back( arg[0].Cycle );
	for( int i = 1; i < s; i++ )
	{
		perState.push_back( arg[i].Cycle - arg[i-1].Cycle );
	}

	for( int i = 0; i < s; i++ )	//Summing the square of each item minus the average
	{
		output += (perState[i] - avgCycles) * (perState[i] - avgCycles);
	}
	if( output > 0 )
		output = sqrtf(output / (s-1));
	//Uncomment this and return it to log out list of cycles-per-state counts (set return value to:deque<float>)
//	perState.push_front(output);

	return output;
}

/*****************************************************************************
 *	Function for generating SIMPLE features
 *****************************************************************************/

/**
 *	Description: Counts the number of levels played, and the number of states per level
 *	Returns: a deque - [0] number of states in level S1.b_AvgStates; [1] number of cycles per level S1.c_AvgCycles
 */
deque<float> Simple1_Levels( deque<PacState> arg )
{
	deque<float> output;
	int s = arg.size();

	output.push_back( arg[s-1].State );
	output.push_back( arg[s-1].Cycle );

	return output;
}

/**
 *	Description: Counts the number of lives gained (by cherry eating), and lost (by ghost collision)
 *	Returns: a deque with two elements, [0] : gained lives S2.a_LivesGained, [1] : lost lives S2.b_LivesLost
 */
deque<float> Simple2_LivesChng( deque<PacState> arg )
{
	deque<float> output;
	output.assign(2, 0);
	int s = arg.size();

	for( int i = 3; i < s; i++ )
	{
		if( arg[i].Lives > arg[i-1].Lives )
			output[0]++;
		else if( arg[i].Lives < arg[i-1].Lives )
			output[1]++;
	}

	return output;
}

/**
 *	Description: Count Pacman traversal of teleporters
 */
float Simple4_TeleportUse( deque<PacState> arg )
{
	float output = 0.0;
	int s = arg.size();
	PacState pS;

	for( int i = 1; i < s; i++ )
	{
		pS = arg[i];
//		if( pS.dtpMap[ pS.xPac ][ pS.yPac ] == 3 )
		if( pS.yPac == 9 && ((pS.xPac < 2 && arg[i-1].xPac > 17) || pS.xPac > 17 && arg[i-1].xPac < 2) )
			output++;
	}

	return output;
}

/*****************************************************************************
 *	Functions for generating AGGRESSION features
 *****************************************************************************/

/**
 *	Description: Count times when Pacman attacks the ghosts and follows them right into their house
 */
float Aggression1_AtkGstHse( deque<PacState> arg )
{
	float output = 0.0;
	PacState pS;
	int s = arg.size(), yHse = 9, yP = 7;
	bool odd = false;

	for( int i = 1; i < s; i++ )
	{
		pS = arg[i];
/*
		if( odd )
		{
			yHse = 9;
			yP = 7;
		}
		else
		{
			yP = 10;
			yHse = 8;
		}
*/
		if( pS.PacAttack && 
			( myHeuristicDist( pS.xGhost[0], pS.yGhost[0], 9, yHse, pS ) < 3 || myHeuristicDist( pS.xGhost[1], pS.yGhost[1], 9, yHse, pS ) < 3 ))
		{
			while( pS.PacAttack && i < s-1 )
			{
				pS = arg[++i];
				if( myHeuristicDist( pS.xPac, pS.yPac, 9, yP, pS ) < 3 )
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
 *	Description : Implementation of Clementine code: While Pacman is attacking, count the ghosts eaten by Pacman.
 *	Returns		: while(PacAtkBool){ if( Points(t) >= Points(t-1) + 100 ) then Output++ }
 */
float Aggression3_GhostKills( deque<PacState> arg )
{
	float output = 0.0;
	PacState pS;
	int s = arg.size();

	for( int i = 1; i < s; i++ )
	{
		pS = arg[i];

		if( pS.PacAttack && pS.Points > arg[i-1].Points + 99 )
			output++;
	}

	return output;
}

/**
 *	Description	: Implementation of Clementine code which counts the number of times this condition is met:
 *	((@DIFF1(dst_PacG1) <= 0 and dst_PacG1 < 5) or 
 *	(@DIFF1(dst_PacG2) <= 0 and dst_PacG2 < 5)) and 
 *	(@SINCE0(PacAtkBool = 1,20) < 10) and 
 *	PacAtkBool = 0
 *	Returns		: [0-3] a value at each Pill - number of states in which Pacman is getting closer to a Ghost AFTER Pill; [4] number of previous values != 0
 */
float Aggression4_HuntAfterPill( deque<PacState> arg )
{
	deque <float> output;
	output.assign(4, 0);
	PacState pS;
	int s = arg.size(), pill = 0;
	bool pilling = false;
	
	for( int i = 10; i < s; i++ )
	{
		pS = arg[i];
		while( pS.PacAttack == false && arg[i-10].PacAttack == true && i < s-1)
		{				
			int d0 = manhattanDist( pS.xPac, pS.yPac, pS.xGhost[0], pS.yGhost[0] );
			int d1 = manhattanDist( pS.xPac, pS.yPac, pS.xGhost[1], pS.yGhost[1] );
		
			pS = arg[++i];

			int d0next = manhattanDist( pS.xPac, pS.yPac, pS.xGhost[0], pS.yGhost[0] );
			int d1next = manhattanDist( pS.xPac, pS.yPac, pS.xGhost[1], pS.yGhost[1] );			
			
			if( (d0next < 5 &&  d0next < d0) || (d1next < 5 &&  d1next < d1) )
			{
				output[pill]++;
			}
			pilling = true;
		}
		if( pilling )
			pill++;
		pilling = false;
	}
	pill = 0;
	for( int i = 0; i < 4; i++ )
		if( output[i] > 0 )
			pill++;
	output.push_back( pill );

	return output[4];
}

/**
 *	Description:Whether the player uses the Pills to chase ghosts, or just continues collecting Dots
 *	Pseudocode:	while(PacAtkBool){ if( dist(PacXY, Gst{1,2}XY):decrease ) then Output++ }
 *	Clementine:	((@DIFF1(dst_PacG1) <= 0 and dst_PacG1 < 5) or (@DIFF1(dst_PacG2) <= 0 and dst_PacG2 < 5)) and 
 *				(@SINCE0(PacAtkBool = 0,20) < 10) and PacAtkBool = 1
 *	Returns		: [0-3] a value at each Pill - number of states in which Pacman is getting closer to a Ghost DURING Pill; [4] number of previous values != 0
 */
float Aggression6_Chase¬Dots( deque<PacState> arg )
{
	deque <float> output;
	output.assign(4, 0);
	PacState pS = arg[0];
	int s = arg.size(), pill = 0;
	bool pilling = false;

	for( int i = 1; i < s; i++ )
	{
		pS = arg[i];
		while( pS.PacAttack == true && arg[i-10].PacAttack == false && i < s-1)
		{
			int d0 = manhattanDist( pS.xPac, pS.yPac, pS.xGhost[0], pS.yGhost[0] );
			int d1 = manhattanDist( pS.xPac, pS.yPac, pS.xGhost[1], pS.yGhost[1] );
		
			pS = arg[++i];

			int d0next = manhattanDist( pS.xPac, pS.yPac, pS.xGhost[0], pS.yGhost[0] );
			int d1next = manhattanDist( pS.xPac, pS.yPac, pS.xGhost[1], pS.yGhost[1] );	
			
			if( (d0next < d0 && d0next < 5) || (d1next < d1 && d1next < 5) )
			{
				output[pill]++;
			}
			pilling = true;
		}
		if( pilling )
			pill++;
		pilling = false;
	}
	pill = 0;
	for( int i = 0; i < 4; i++ )
		if( output[i] > 0 )
			pill++;
	output.push_back( pill );

	return output[4];
}


/*****************************************************************************
 *	Function for generating PLANNING features
 *****************************************************************************/

/**
 *	Description: does player wait near Pill to lure ghosts before eating them?
 */
deque<float> Planning1_LureToPill( deque<PacState> arg )
{
	deque<float> features;	//0 - Counts the number of ghosts lured to Pills, when the pills were then eaten
							//1 - Counts how much Pacman moved from when he got close to the Pill while the ghosts were still far away
							//2 - Counts the number of times both ghosts were lured in one go
							//3 - Counts how many Ghosts Pacman actually eats after doing a lure
							//4 - Measures whether Pacman didn't do the lure & get the Pill because he was caught
	features.assign(5, 0);
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
				if( myHeuristicDist(pS.xPac, pS.yPac, xy[0], xy[1], pS) < 8 && 
					manhattanDist(pS.xGhost[0], pS.yGhost[0], pS.xPac, pS.yPac) > 8 && 
					manhattanDist(pS.xGhost[1], pS.yGhost[1], pS.xPac, pS.yPac) > 8 )
				{
					int pxt, pyt;
					//Cycle until the Pill is eaten or Pacman strays away from the Pill again 
					while( !pS.PacAttack && myHeuristicDist(pS.xPac, pS.yPac, xy[0], xy[1], pS) < 8 && i < s-1 )
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
						{
							features[0]++;
							features[2] += 0.5;
						}
						if( manhattanDist(pS.xGhost[1], pS.yGhost[1], pS.xPac, pS.yPac) < 8 )
						{
							features[0]++;
							features[2] += 0.5;
						}
						else if( fmod( features[2], (float)1.0 ) > 0 )
							features[2] -= 0.5;
					
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
					else if( pS.Lives < arg[i-2].Lives )
							features[4]++;
				}
			}
		}
	}
	return features;
}

/**
 *	Description: Counts the number of Dots eaten before the first Pill of every level.
 */
float Planning3_CntDtsB4Pll1( deque<PacState> arg )
{
	float output = 0.0;
	PacState pS;
	int s = arg.size(), lastDotCnt = 0;

	for( int i = 1; i < s; i++ )
	{
		pS = arg[i];
		while( !pS.PacAttack && pS.Pills == 4 && i < s-1 )
		{
			lastDotCnt = pS.Dots;
			pS = arg[++i];
			if( pS.Dots == lastDotCnt - 1 )
				output++;
		}
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
deque<float> Planning4_SpeedOfHunt( deque<PacState> arg )
{
	deque<float> output;
	output.assign(4, 0);
	PacState pS;
	int s = arg.size(), cy, st, pts;

	for( int i = 1; i < s; i++ )
	{
		pS = arg[i];
		if( pS.PacAttack )
		{
			cy = pS.Cycle;
			st = pS.State;

			while( pS.PacAttack && i < s-1 )
			{
				pts = pS.Points;
				pS = arg[++i];
				if( pS.Points > pts + 190 )
				{
					output[2] += pS.Cycle - cy;
					output[3] += pS.State - st;
					break;
				}
				else if( pS.Points > pts + 90 )
				{
					output[0] += pS.Cycle - cy;
					cy = pS.Cycle;
					output[1] += pS.State - st;
					st = pS.State;
				}
			}
		}
	}

	return output;
}

/**
 *	Description: Count whether the player leaves off collecting Dots around the ghost house until end of the level – as the hardest area it might be put off by novice players.
 */

//Won't work until refactored for single level use

float Planning7_PutOffGstHse( deque<PacState> arg )
{
	float output = 0.0;
	PacState pS;
	int s = arg.size(), outsideDots, houseDots, tail;

	tail = s-2;
	while( arg[tail].Pills == 0 )
		tail--;

	while( tail < s-1 )
	{
		pS = arg[tail++];
		outsideDots = 0;
		houseDots = 0;
		for( int x = 1; x < 19; x++ )
		for( int y = 1; y < 19; y++ )
		{
			if( pS.dtpMap[x][y] == 4 )
					outsideDots++;
		}

		for( int x = 7; x < 12; x++ )
		for( int y = 5; y < 10; y++ )
		{
			if( pS.dtpMap[x][y] == 4 )
					houseDots++;
		}

		if( houseDots == 0 )
			break;

		outsideDots -= houseDots;
		if( houseDots > outsideDots )
		{
			output++;				
			break;
		}
	}

	return output;
}

/*****************************************************************************
 *	Functions for generating CAUTION features
 *****************************************************************************/

/**
 *	Description: Count if Pacman gets trapped in corridor by Ghosts – i.e. one ghost on each possible direction of movement (vector). 
 *	Return: a deque of length 2: [0] Count times Pacman gets trapped; [1] Whether he gets killed as a result
 */
deque<float> Caution1_ThreatPrcptn( deque<PacState> arg )
{
	deque<float> output;
	output.assign(2, 0);
	PacState pS;
	int s = arg.size(), len;

	for( int i = 1; i < s; i++ )
	{
		pS = arg[i];
		if( !pS.PacAttack && 
			manhattanDist( pS.xPac, pS.yPac, pS.xGhost[0], pS.yGhost[0] ) < 5 && 
			manhattanDist( pS.xPac, pS.yPac, pS.xGhost[1], pS.yGhost[1] ) < 5 )
		{
			//find path to for (pS.xPac, pS.yPac, 20-pS.xPac, 20-pS.yPac)
			//If ghost[0] x,y is on path, block in by setting to 1, then recurse
			for( int c = 0; c < NUM_GS; c++ )
			{
				len = a_Star(pS.xPac, pS.yPac, 20-pS.xPac, 20-pS.yPac, true, pS);
				
				for( int g = 0; g < NUM_GS; g++ )
				for( int p = 0; p < len; p += 2 )
				{
					if( pS.xGhost[g] == aPathX[p] && pS.yGhost[g] == aPathY[p] )
						pS.dtpMap[ pS.xGhost[g] ][ pS.yGhost[g] ] = 1;
				}
			}
			if( a_Star(pS.xPac, pS.yPac, 20-pS.xPac, 20-pS.yPac, true, pS) < 6 )
			{
				output[0]++;
			
				if( pS.Lives > arg[ min( i+10, s-1 ) ].Lives )
					output[1]++;
			}
		}
	}

	return output;
}

/**
 *	Description: Average distance Pacman keeps from the Ghosts 
 *	Return: While under PacAttack = Avg:manhattan(Pac, Gst Centroid)
 */
float Caution2_AvgDstPacAtkGstCtrd( deque<PacState> arg )
{
	float output = 0.0;
	PacState pS;
	int s = arg.size(), xCentroid, yCentroid, cntAtk = 0;

	for( int i = 1; i < s; i++ )
	{
		pS = arg[i];
		xCentroid = yCentroid = 0;
		if( pS.PacAttack )
		{
			cntAtk++;
			for(int j = 0; j < NUM_GS; j++)
			{
				xCentroid += pS.xGhost[j];
				yCentroid += pS.yGhost[j];
			}
			xCentroid = xCentroid / NUM_GS;
			yCentroid = yCentroid / NUM_GS;
			output += manhattanDist( pS.xPac, pS.yPac, xCentroid, yCentroid );
		}
	}
	output /= cntAtk;

	return output;
}

/**
 *	Description : How often player comes very close to a ghost, when not on a Pill, and doesn’t die afterwards!
 *	Returns		: if( dist(PacXY, Gst{1,2}XY)@State(t-5) == 1 & Lives@State(t-5) == Lives) then Output++
 */
float Caution3_CloseCalls( deque<PacState> arg )
{
	float output = 0.0;
	int s = arg.size();
	PacState pS;

	for( int i = 0; i < s-6; i++ )
	{
		pS = arg[i];

		if( (manhattanDist( pS.xPac, pS.yPac, pS.xGhost[0], pS.yGhost[0] ) == 1 || manhattanDist( pS.xPac, pS.yPac, pS.xGhost[1], pS.yGhost[1] ) == 1) && pS.Lives <= arg[i+5].Lives )
			output++;
	}

	return output;
}

/**
 *	Description: Counts how often Pacman loses a life just as the Pill wears off (within 15 states) 
 */
float Caution4_CaughtAfterHunt( deque<PacState> arg )
{
	float output = 0.0;
	int s = arg.size(), count;
	deque<int> dists;
	PacState pS;

	for( int i = 2; i < s; i++ )
	{
		pS = arg[i];
		if( arg[i - 1].PacAttack && !pS.PacAttack )
		{
			for(int k = 0; k < NUM_GS; k++)
			{
				dists.push_back( manhattanDist( pS.xPac, pS.yPac, pS.xGhost[k], pS.yGhost[k] ) );
			}

			count = 0;
			int j = i;
			while( arg[j].Lives == arg[j - 1].Lives )
			{
				count++;
				j++;
				if( j >= s-1 )
					break;
				if( arg[j].PacAttack )
				{
					count = -100;
					break;
				}
			}
			if( count > 0 && count < lowest( dists )*2 && arg[ min( j, s-1 ) ].Lives < pS.Lives )
				output++;
			
			i = j;
		}
	}
	return output;
}

/**
 *	Description: Counts how often Pacman moves to an empty square 
 */
float Caution5_MovesWithoutGain( deque<PacState> arg )
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
 *	Description: Counts player's score per life per level 
 */
float Caution6_PntsOvrLives( deque<PacState> arg )
{
	float output = 0.0;
	int s = arg.size();
	
	//Final points divided by difference between starting and ending Lives
	if( s > 0 )
		output = arg[s-1].Points / ( abs( arg[s-1].Lives - arg[0].Lives ) + 1 );

	return output;
}

/**
 *	Description: Count if the player dies collecting Dots around the ghost house
 */
float Caution7_KilledAtHouse( deque<PacState> arg )
{
	float output = 0;
	int s = arg.size();
	PacState pS;

	for( int i = 1; i < s; i++ )
	{
		pS = arg[i-1];
		if( !pS.PacAttack && myHeuristicDist( pS.xPac, pS.yPac, 9, 8, pS ) < 5 && arg[i].Lives < pS.Lives )
		{
			output++;
		}
	}

	return output;
}

/*****************************************************************************
 *	Function for generating DECISIVENESS features
 *****************************************************************************/

/**
 *	Description: Average number of cycles it takes the player to make their moves
 */
float Decisive1_CyclesPrMv( deque<PacState> arg )
{
	float output = 0.0;
	int s = arg.size();

	//divide number of cycles/number of states
	output = arg[s-1].Cycle / max(1, arg[s-1].State);

	return output;
}

/**
 *	Description: Counts instances where the player made oscillating movements with no ghost around
 */
float Decisive2_Osclltng( deque<PacState> arg )
{
	float output = 0.0;
	PacState pS;
	int s = arg.size(), x, y, t, limit = 5;

	for( int i = 1; i < s-10; i++ )
	{
		pS = arg[i];
		x = pS.xPac;
		y = pS.yPac;
		t = 0;
		while( manhattanDist( pS.xPac, pS.yPac, pS.xGhost[0], pS.yGhost[0] ) > 9 && 
			manhattanDist( pS.xPac, pS.yPac, pS.xGhost[1], pS.yGhost[1] ) > 9 )
		{
			pS = arg[ i+t ];
			t++;
			if( t == limit )
				break;

			if( x == pS.xPac && y == pS.yPac )
			{
				if( limit == 5 )
					limit = 10;
				else{
					output++;
					break;
				}
			}
		}
	}

	return output;
}
/** /
//Old method only has distance from ghosts condition at start.
for( int t = 2; t < 5; t++ )
	if( pS.xPac == arg[ i+t ].xPac && pS.yPac == arg[ i+t ].yPac )
	{
		for( int t2 = t+2; t2 < t+5; t2++ )
			if( pS.xPac == arg[ i+t2 ].xPac && pS.yPac == arg[ i+t2 ].yPac )
			{
				output++;
				break;
			}
		break;
	}
/**/

/*****************************************************************************
 *	Function for generating RESOURCE features
 *****************************************************************************/

/**
 *	Description: How long after a Cherry appears does it take Pacman to eat it? The same number as ChryBool_Sum, but only returns if it wasn't the Ghost who ate it.
 *	Returns:	 [0] How quickly the player collects the newly appeared Cherry; [1] Whether the player increases their lives: 0/1
 */
deque<float> Resource_TimeAndNumChry( deque<PacState> arg )
{
	deque<float> output;
	output.assign(2, 0);
	float temp = 0.0, lvls = 0.0;
	PacState pS;
	int s = arg.size(), yChry = 11;
	bool odd = false;

	for( int i = 1; i < s; i++ )
	{
		pS = arg[i];

		if( pS.CherryThere )
		{
			while( pS.CherryThere && i < s-1 )
			{
				i++;
				pS = arg[i];
				temp++;
			}
			if( i == s-1 )
				break;	
			
			if( manhattanDist( arg[i-1].xPac, arg[i-1].yPac, 10, yChry ) < manhattanDist( arg[i-1].xGhost[0], arg[i-1].yGhost[0], 10, yChry ) 
				&& pS.State != 0 )
			{
				output[0] += temp;
				output[1] = 1;
			}
			temp = 0.0;	
		}
/*		if( pS.State == 0 )
		{
			odd = !odd;
			lvls++;
			if( odd )
				yChry = 11;
			else
				yChry = 6;
		}
*/	}

	return output;
}

/*****************************************************************************
 *	Functions for reading and manipulating records from a Pacman game play log
 *****************************************************************************/

/**
 *	Description: Function to build a PacState deque out of all the PacStates inherent in a raw Pacman game log.
 */
deque<PacState> BuildStatesDq( const char* filename )
{
	deque<int> csv = readFromCSVFile( filename ); //"stateData/stateData-1.csv" );
	PacState pState;
	deque<PacState> allStates;
	if( csv.size() < 100 )
		return allStates;
	
	int tempMap[20][20], xGst[NUM_GS], yGst[NUM_GS];
	int addend = 2 + (NUM_GS * 2);
	int i = 0, pred = 0, temp, count;

	while( i < csv.size() )
	{
		count = 0;
		temp = i;
		while( csv[temp] != INT_MIN && csv[temp] != INT_MAX )
			temp++;

		if( csv[temp] == INT_MAX )
			break;

		for( int n = 0; n < NUM_GS; n++ )
		{
			xGst[n] = csv[i+3+(n*2)];
			yGst[n] = csv[i+4+(n*2)];
		}	
		for( int x = 0; x < 20; x++ )
			for( int y = 0; y < 20; y++ )
			{
				tempMap[x][y] = csv[i+addend+9+count];
				count++;
			}
		pState = PacState(csv[i], csv[i+1], csv[i+2], csv[i+addend+2], csv[i+addend+4], csv[i+addend+3], csv[i+addend+1], csv[i+addend+7], csv[i+addend+8], xGst, yGst, csv[i+addend+5] == 1, csv[i+addend+6] == 1, tempMap );

		i = ++temp;
		allStates.push_back(pState);
	}

	return allStates;
}

/**
 *	Description: Reads in PacStates from a .csv file, named in argument.
 *	Returns:	deque of ints holding values from the .csv, need to be interpreted as PacState fields by caller
 */
deque<int> readFromCSVFile( const char * filename )
{
	int c;
	FILE *states;
	deque <int> csv;
	string state("");

	// Open for read (will fail if 'filename'.csv does not exist)
	fopen_s( &states, filename, "r" );

	while( true )
	{
		if( (c = fgetc(states)) == EOF )
		{
			csv.push_back( INT_MAX );
			break;
		}
		state += c;
		if( c == ',' )
		{
			csv.push_back( atoi(state.c_str()) );
			state.clear();
		}
		if( c == '\n' )
		{
			csv.push_back( atoi(state.c_str()) );
			state.clear();
			csv.push_back( INT_MIN );
		}
	}

	fclose( states );

	return csv;
}

/********************************************************************
 *	Distance measure functions
 ********************************************************************/

/**
 * Description:	Implement the A* pathfinding algorithm - optimised for Pacman map. Stores path in global deques aPathX and aPathY
 * Arguments:	Ints give the source x,y and the target x,y; 
 *				bool sets whether it is a path for Pacman - i.e. 2's not navigable;
 *				PacState is the state to pathfind on.
 * Return Val:	number of moves in path.
 */
int a_Star(int src_x1, int src_y1, int trg_x2, int trg_y2, bool pacTravelling, PacState pS)
{
	if( src_x1 == trg_x2 && src_y1 == trg_y2 )
		return 0;

	if(src_x1 < 0) src_x1 = 1;
	if(src_y1 < 1) src_y1 = 1;
	if(trg_x2 < 0) trg_x2 = 1;
	if(trg_y2 < 1) trg_y2 = 1;
	if(src_x1 > 19) src_x1 = 18;
	if(src_y1 > 18) src_y1 = 18;
	if(trg_x2 > 19) trg_x2 = 18;
	if(trg_y2 > 18) trg_y2 = 18;

	int limit = 1;
	if( pacTravelling )
		limit++;

	while( pS.dtpMap[src_x1][src_y1] <= limit )
	{
		if( trg_x2 < src_x1 )
			src_x1--;
		else if( trg_x2 > src_x1 )
			src_x1++;
		else if( trg_y2 < src_y1 )
			src_y1--;
		else
			src_y1++;
	}

	while( pS.dtpMap[trg_x2][trg_y2] <= limit )
	{
		if( src_x1 < trg_x2 )
			trg_x2--;
		else if( src_x1 > trg_x2 )
			trg_x2++;
		else if( src_y1 < trg_y2 )
			trg_y2--;
		else
			trg_y2++;
	}

	if( src_x1 == trg_x2 && src_y1 == trg_y2 )
		return 0;

	//Initialisations
	int x1 = src_x1; 
	int y1 = src_y1;
	aPathX.clear();
	aPathY.clear();

	//List of directions, starting from x1, y1
	int path[ASTAR_BOUND];
	int closedNodesX[ASTAR_BOUND] = {0};
	int closedNodesY[ASTAR_BOUND] = {0};
	int openNodesX[ASTAR_BOUND][4] = {0};	//four directions X
	int openNodesY[ASTAR_BOUND][4] = {0};	//four directions Y
	int openNodesF[4];	//four Final scores = G + H
	int fScoresHist[ASTAR_BOUND][4] = {0};

	for(int i = 0; i < ASTAR_BOUND; i++)
		for(int j = 0; j < 4; j++)
			fScoresHist[i][j] = INT_MAX;
	
	int iter = 0;
	int dir = -1, closedDir = -1;
	int G = 1;//, F = 0, H = 0, currNode = 0;
	int tempTrgX = -1;

	deque<int> indexArr;
	deque<int> valueArr;
	int temp = 0;
/**/
	//Some functionality to handle the teleporters
	if(src_y1 == 9)
		if(src_x1 < 5)
			if(trg_x2 > 9+src_x1)
			{
				for(int i = src_x1; i > -1; i--)
				{
					closedNodesX[iter] = i;
					closedNodesY[iter] = 9;
					iter++; G++;
				}
				closedDir = 2;
				x1 = 19;
			}
		else if(src_x1 > 14)
			if(trg_x2 < 10-(20-src_x1))
			{
				for(int i = src_x1; i < 20; i++)
				{
					closedNodesX[iter] = i;
					closedNodesY[iter] = 9;
					iter++; G++;
				}
				closedDir = 3;
				x1 = 0;
			}
/**/
	//And for the target being near teleporter
	if(trg_y2 == 9)
		if(trg_x2 < 5 && src_x1 > 12)
		{
			tempTrgX = trg_x2;
			trg_x2 = 19;
		}else if(trg_x2 > 14 && src_x1 < 7)
		{
			tempTrgX = trg_x2;
			trg_x2 = 0;
		}
/**/
	//Check our path and update path[]
	while(iter < ASTAR_BOUND)
	{
		bool deadEnd = true;
		openNodesF[0] = INT_MAX, openNodesF[1] = INT_MAX, openNodesF[2] = INT_MAX, openNodesF[3] = INT_MAX;
		//Seal off the way we came from investigation.
		closedNodesX[iter] = x1;
		closedNodesY[iter] = y1;

		if( manhattanDist( x1, y1, trg_x2, trg_y2 ) == 1 )
			break;

		//Find available directions and calculate pathfinding indices for each dir
		if(pS.dtpMap[x1][y1-1] > limit && closedDir != 0)
		{
			deadEnd = false;
			openNodesX[iter][0] = x1; 
			openNodesY[iter][0] = y1-1;
			fScoresHist[iter][0] = openNodesF[0] = G + myHeuristicDist(x1, y1-1, trg_x2, trg_y2, pS);
		}
		if(pS.dtpMap[x1][y1+1] > limit && closedDir != 1)
		{
			deadEnd = false;
			openNodesX[iter][1] = x1; 
			openNodesY[iter][1] = y1+1;  
			fScoresHist[iter][1] = openNodesF[1] = G + myHeuristicDist(x1, y1+1, trg_x2, trg_y2, pS); 
		}
		if(pS.dtpMap[x1+1][y1] > limit && closedDir != 2)
		{
			deadEnd = false;
			openNodesX[iter][2] = x1+1; 
			openNodesY[iter][2] = y1;  
			fScoresHist[iter][2] = openNodesF[2] = G + myHeuristicDist(x1+1, y1, trg_x2, trg_y2, pS);
		}
		if(pS.dtpMap[x1-1][y1] > limit && closedDir != 3)
		{
			deadEnd = false;
			openNodesX[iter][3] = x1-1; 
			openNodesY[iter][3] = y1; 
			fScoresHist[iter][3] = openNodesF[3] = G + myHeuristicDist(x1-1, y1, trg_x2, trg_y2, pS);
		}


		/*Need to add parents array:
		@ iter, parent of X,Y is closedDir or something
		at end, have aPathX[end] = trg_x, aPathX[end-1] = trg_x.parent, ...
		In other words aPathX[end] = closedNodesX[end].parent, aPathX[end-1] = closedNodesX[end].parent, ...
		That should give a seamless path...
		*/
/* PRINT OUTS * /
sprintf_s(out, 99, "%d", lowest(fScoresHist[iter], 4)); print("\ndir of F score @ iter: "); print(out);
sprintf_s(out, 99, "%d", fScoresHist[iter][lowest(fScoresHist[iter], 4)]); print("\nlowest F score @ iter: "); print(out);
/**/		
		//Now look at all the F scores for the smallest one...going back through all previously tested nodes : maybe we shud just go thru nodes on path?
		//If indices are (i, j) then node to pick is: x1 = openNodesX[i][j], y1 = openNodesY[i][j]
		//dir will be equal to j, closedDir its opposite
		for(int i = 0; i <= iter; i++)
		{
			temp = lowest( fScoresHist[i], 4 );
			indexArr.push_back( temp );
			valueArr.push_back( fScoresHist[i][temp] );
		}

		if( deadEnd )
		{}
		
		temp = lowest(valueArr);
		dir = indexArr[temp];
		if(dir%2 == 0)
			closedDir = dir + 1;
		else
			closedDir = dir - 1;
		x1 = openNodesX[temp][dir];
		y1 = openNodesY[temp][dir];

/*
if(fScoresHist[temp][dir] < openNodesF[lowest(openNodesF, 4)])
{	sprintf_s(out, 99, "%d", fScoresHist[temp][dir]); print("\nlowest f scores hist: "); print(out);
}else if(fScoresHist[temp][dir] == openNodesF[lowest(openNodesF, 4)])
{	sprintf_s(out, 99, "%d", fScoresHist[temp][dir]); print("\nequal f scores: "); print(out);
}else{
	sprintf_s(out, 99, "%d", openNodesF[lowest(openNodesF, 4)]); print("\nlowest F from openNodes: "); print(out);
}
*/
		//Put the chosen node on the closed lists
		fScoresHist[temp][dir] = INT_MAX;
		openNodesX[temp][dir] = -1;
		openNodesY[temp][dir] = -1;
/** /
		//Pick the dir with smallest F value - i.e. shortest path
		//Set the next testable node to this, and close the direction we came from
		dir = lowest(openNodesF, 4);
		switch(dir)
		{
		case 0:
			{
			y1--;
			closedDir = 1;
			break;
			}
		case 1:
			{
			closedDir = 0;
			y1++;
			break;
			}
		case 2:
			{
			closedDir = 3;
			x1++;
			break;
			}
		case 3:
			{
			closedDir = 2;
			x1--;
			break;
			}
		}

/** /		//Implement backtracking capability
		for(int i = 0; i < iter-1; i++) //Test up until current - 1 : to prevent oscillation
		{
			if(fScoresHist[i] < openNodesF[dir])
			{
				//Set the current node back to the 1st one with lower F score
				//Test if any nodes are available - not blocked by walls, not on closedList
				if(pS.dtpMap[closedNodesX[i]][closedNodesY[i] - 1] > 2 && ((closedNodesX[i] != closedNodesX[i-1] && closedNodesY[i] - 1 != closedNodesY[i-1]) || (closedNodesX[i] != closedNodesX[i+1] && closedNodesY[i] - 1 != closedNodesY[i+1])))
				{
					x1 = closedNodesX[i];
					y1 = closedNodesY[i] - 1;
					G = iter = i; G++;
					closedDir = 1;
					break;
				}
				if(pS.dtpMap[closedNodesX[i]][closedNodesY[i] + 1] > 2 && ((closedNodesX[i] != closedNodesX[i-1] && closedNodesY[i] + 1 != closedNodesY[i-1]) || (closedNodesX[i] != closedNodesX[i+1] && closedNodesY[i] + 1 != closedNodesY[i+1])))
				{
					x1 = closedNodesX[i];
					y1 = closedNodesY[i] + 1;
					G = iter = i; G++;
					closedDir = 0;
					break;
				}
				if(pS.dtpMap[closedNodesX[i] + 1][closedNodesY[i]] > 2 && ((closedNodesX[i] + 1 != closedNodesX[i-1] && closedNodesY[i] != closedNodesY[i-1]) || (closedNodesX[i] + 1 != closedNodesX[i+1] && closedNodesY[i] != closedNodesY[i+1])))
				{
					x1 = closedNodesX[i] + 1;
					y1 = closedNodesY[i];
					G = iter = i; G++;
					closedDir = 3;
					break;
				}
				if(pS.dtpMap[closedNodesX[i] - 1][closedNodesY[i]] > 2 && ((closedNodesX[i] - 1 != closedNodesX[i-1] && closedNodesY[i] != closedNodesY[i-1]) || (closedNodesX[i] - 1 != closedNodesX[i+1] && closedNodesY[i] != closedNodesY[i+1])))
				{
					x1 = closedNodesX[i] - 1;
					y1 = closedNodesY[i];
					G = iter = i; G++;
					closedDir = 2;
					break;
				}
				//Else -> Continue looking for lower F scores until i = iter+1
			}
		}
/**/
		path[iter] = dir;
		iter++;
		G++;
		indexArr.clear();
		valueArr.clear();
	}//End While Loop

	//Cut out any loops that have appeared in the closedNodes arrays.
/** /	for(int pt = 0; pt < iter-1; pt++)
		{
			for(int luk = pt+1; luk < iter; luk++)
			{
				if(closedNodesX[pt] == closedNodesX[luk] && closedNodesY[pt] == closedNodesY[luk])
				{
					int t = pt;
					int l = luk;
					while( l < iter )
					{
						closedNodesX[t] = closedNodesX[l];
						closedNodesY[t] = closedNodesY[l];
						t++; l++;
					}
					iter -= (luk - pt);
					break;
				}
			}
		}
/**/
	//If the target was in one of the teleport deadends, we may have teleported there, need to finish off.
	if(tempTrgX != -1)
	{
		trg_x2 = tempTrgX;
		if(tempTrgX < 5)
			for(int i = 0; i < tempTrgX; i++)
			{
				closedNodesX[iter] = i;
				closedNodesY[iter] = 9;
				iter++;
			}
		else if(tempTrgX > 14)
			for(int i = 19; i > tempTrgX; i--)
			{
				closedNodesX[iter] = i;
				closedNodesY[iter] = 9;
				iter++;
			}
	}

	//When we have our path, iterate back from the target and push down the parent of each path node.
	aPathX.push_front(trg_x2);
	aPathY.push_front(trg_y2);

	for(int i = iter; i > -1; i--)
		if(manhattanDist(aPathX.front(), aPathY.front(), closedNodesX[i], closedNodesY[i]) == 1)
		{
			aPathX.push_front(closedNodesX[i]);
			aPathY.push_front(closedNodesY[i]);
		}

//sprintf_s(out, 99, "%d", iter); print("\nWhile Loop iter's: "); println(out);
	iter = (int)aPathX.size();
/** /
	print("Source: ");
	sprintf_s(out, 99, "%d", src_x1); print(out); print(", ");
	sprintf_s(out, 99, "%d", src_y1); print(out); print("\n");
/** /
	sprintf_s(out, 99, "%d", iter); print("path length: "); println(out); 
	for(int i = 0; i < iter; i++)
	{
		print("Iter: ");
		sprintf_s(out, 99, "%d", i); print(out); print(";\t");
		sprintf_s(out, 99, "%d", aPathX[i]); print(out); print(", ");
		sprintf_s(out, 99, "%d", aPathY[i]); print(out); print(";\t");
		if(i > 0){
			sprintf_s(out, 99, "%d", path[i-1]); print(out); } print("\n");
	}
/** /
	print("Target: ");
	sprintf_s(out, 99, "%d", trg_x2); print(out); print(", ");
	sprintf_s(out, 99, "%d", trg_y2); println(out);
/**/
	return iter;
}

/**
 * Distance measures. manhattanDist and lineDist are self explanatory.
 */
int manhattanDist(int x1, int y1, int x2, int y2)
{
	return abs(x1 - x2) + abs(y1 - y2);
}

double lineDist(int x1, int y1, int x2, int y2)
{
	double dist = 0.0;
	dist = sqrt(double((x2 - x1)^2 + (y2 - y1)^2));
	return dist;
}

/**
 *	Description: Return the x,y of the closest Pill to Pacman in the state passed as argument
 */
deque<int> GetClosestPill( PacState pS )
{
	deque<int> xy;
	xy.assign(2, 0);

	deque <int> mDist; //Deque of distances between things
	deque <int> xPill;
	deque <int> yPill;

	//Get the closest pill's x and y
	if(pS.dtpMap[1][3] == 5){
		mDist.push_back( myHeuristicDist(pS.xPac, pS.yPac, 1, 3, pS) );
		xPill.push_back(1);
		yPill.push_back(3);
	}
	if(pS.dtpMap[18][3] == 5){
		mDist.push_back( myHeuristicDist(pS.xPac, pS.yPac, 18, 3, pS) );
		xPill.push_back(18);
		yPill.push_back(3);
	}
	if(pS.dtpMap[1][15] == 5){
		mDist.push_back( myHeuristicDist(pS.xPac, pS.yPac, 1, 15, pS) );
		xPill.push_back(1);
		yPill.push_back(15);
	}
	if(pS.dtpMap[18][15] == 5){
		mDist.push_back( myHeuristicDist(pS.xPac, pS.yPac, 18, 15, pS) );
		xPill.push_back(18);
		yPill.push_back(15);
	}
	if( mDist.size() > 0 ){
		mDist.push_front(lowest(mDist));
		xy[0] = xPill[mDist[0]];
		xy[1] = yPill[mDist[0]];
	}

	return xy;
}

/**
 * Description: function that counts the number of dots within a given radius of an x,y coord
 */
int localDots(int xCentre, int yCentre, int radius, PacState pS )
{
	int dots = 0;
	
	int x = 0, y = 0, x2 = 19, y2 = 19;
	if(xCentre - radius > 0)
		x = xCentre - radius;
	if(yCentre - radius > 0)
		y = yCentre - radius;

	if(xCentre + radius < 19)
		x2 = xCentre + radius;
	if(yCentre + radius < 19)
		y2 = yCentre + radius;
	
	while(x <= x2)
	{
		while(y <= y2)
		{
			if(pS.dtpMap[x][y] == 4)
				dots++;
			y++;
		}
		x++;
	}
	return dots;
}

/**
 * Description: Heuristic distance counts the number of open and closed squares along four paths - left & right L's, and left & right biased diagonals
	Test loop for myHeuristicDist;
	for(int i = 0; i < 20; i++)
	{
		for(int j = 0; j < 20; j++)
		{
			sprintf_s(out, 99, "%d", myHeuristicDist(i, j, 10, 11)); print("\nHeuristic dist: "); print(out);
		}
	}
*/
int myHeuristicDist(int x1, int y1, int x2, int y2, PacState pS)
{
	if(x1 == x2 && y1 == y2)
		return 0;
	if(x1 < 0) x1 = 0;
	if(y1 < 0) y1 = 0;
	if(x2 < 0) x2 = 0;
	if(y2 < 0) y2 = 0;
	if(x1 > 19) x1 = 19;
	if(y1 > 19) y1 = 19;
	if(x2 > 19) x2 = 19;
	if(y2 > 19) y2 = 19;

	int count[4] = {0};
	int opx = 1, opy = 1;
	if(x1 > x2)	opx = -1;
	if(y1 > y2)	opy = -1;
	int x = 0, y = 0;
	
	for(x = x1; x != x2; x+=opx)
	{
		if(pS.dtpMap[x][y1] < 3)
			count[0]++;
	}
	for(y = y1; y != y2; y+=opy)
	{
		if(pS.dtpMap[x][y] < 3)
			count[0]++;
	}

	for(y = y1; y != y2; y+=opy)
	{
		if(pS.dtpMap[x1][y] < 3)
			count[1]++;
	}
	for(x = x1; x != x2; x+=opx)
	{
		if(pS.dtpMap[x][y] < 3)
			count[1]++;
	}
/**/
	if(abs(x1 - x2) > 1 && abs(y1 - y2) > 1)
	{
		x = x1;
		y = y1;
		//Step once in x, then in y, etc. Count PacMap 1's and alternate
		while(x != x2 && y != y2)
		{
			if(pS.dtpMap[x][y] < 3)
				count[2]++;
			x += opx;
			if(pS.dtpMap[x][y] < 3)
				count[2]++;
			y += opy;
		}
		if(x == x2)
		{
			while(y != y2)
			{
				if(pS.dtpMap[x][y] < 3)
					count[2]++;
				y += opy;
			}
		}else if(y == y2)
		{
			while(x != x2)
			{
				if(pS.dtpMap[x][y] < 3)
					count[2]++;
				x += opx;
			}
		}
		//Alternating
		x = x1;
		y = y1;
		while(x != x2 && y != y2)
		{
			if(pS.dtpMap[x][y] < 3)
				count[3]++;
			y += opy;
			if(pS.dtpMap[x][y] < 3)
				count[3]++;
			x += opx;
		}
		if(x == x2)
		{
			while(y != y2)
			{
				if(pS.dtpMap[x][y] < 3)
					count[3]++;
				y += opy;
			}
		}else if(y == y2)
		{
			while(x != x2)
			{
				if(pS.dtpMap[x][y] < 3)
					count[3]++;
				x += opx;
			}
		}
	}else{
		count[2] = count[3] = 99;
	}
/**/
	sort(count, count+4);

	return abs(x1 - x2) + abs(y1 - y2) + count[lowest(count, 4)];
}

//End of File
