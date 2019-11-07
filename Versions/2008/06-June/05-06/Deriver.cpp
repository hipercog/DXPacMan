/**
 *	Author: Ben Cowley, 2008.
 *	Functions for GENERATING features from game log
 */
#include "universals.h"
#include "Deriver.h"

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
	output.assign(1, 1);
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

/*****************************************************************************
 *	Functions for generating CAUTION features
 *****************************************************************************/

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

/*****************************************************************************
 *	Function for generating DECISIVENESS features
 *****************************************************************************/

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
	float temp = 0.0;
	PacState pS;
	int s = arg.size();

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
			
			getSpritePos( g_CherrySpritePos );
			if( manhattanDist( arg[i-1].xPac, arg[i-1].yPac, xIndex, yIndex ) < manhattanDist( arg[i-1].xGhost[0], arg[i-1].yGhost[0], xIndex, yIndex ) )
			{
				output[0] += temp;
				output[1] = 1;
				break;
			}
		}
	}

	return output;
}
//End of File
