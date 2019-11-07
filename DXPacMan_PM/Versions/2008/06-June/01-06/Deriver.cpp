/**
 *	Author: Ben Cowley, 2008.
 *	Functions for GENERATING features from game log
 */

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
	int s = arg.size();
	PacState pS;
	int avgLives = 0;

	for( int i = 0; i < s; i++ )
	{
		pS = arg[i];
		avgLives += pS.Lives;
	}
	avgLives = avgLives / s;

	for( int i = 0; i < s; i++ )
	{
		pS = arg[i];
		output += (pS.Lives - avgLives)^2;
	}
	output = sqrt(output / s);

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
	PacState pS;
	int avgCycles = arg[s].Cycle / s, count = 0;

	for( int i = 0; i < s; i++ )
	{
		pS = arg[i+1];
		count = pS.Cycle - arg[i].Cycle;
		output += (count - avgCycles)^2;
	}
	output = sqrt(output / s);

	return output;
}

/*****************************************************************************
 *	Function for generating SIMPLE features
 *****************************************************************************/

/**
 *	Description: Counts the number of levels played, and the number of states per level
 *	Returns: a deque - [0] number of levels; [i](odd) number of states per level; [i](even) number of cycles per level S1.c_AvgCycles
 */
deque<float> Simple1_Levels( deque<PacState> arg )
{
	deque<float> output;
	output.assign(1, 1);
	int s = arg.size();

	for( int i = 2; i < s; i++ )
		if( arg[i].State == 0 )
		{
			output[0]++;
			output.push_back( arg[i-1].State );
			output.push_back( arg[i-1].Cycle );
		}

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
 *	((@DIFF1(dst_PacG1) <= 0 and dst_PacG1 < 5) or 
 *	(@DIFF1(dst_PacG2) <= 0 and dst_PacG2 < 5)) and 
 *	(@SINCE0(PacAtkBool = 1,20) < 10) and 
 *	PacAtkBool = 0
 */
float Aggression4_HuntAfterPill( deque<PacState> arg )
{
	float output = 0.0;
	PacState pS;
	int s = arg.size();
	
	for( int i = 1; i < s-10; i++ )
	{
		pS = arg[i];
		while( pS.PacAttack == true && arg[i+10].PacAttack == false )
		{				
			int d0 = manhattanDist( pS.xPac, pS.yPac, pS.xGhost[0], pS.yGhost[0] );
			int d1 = manhattanDist( pS.xPac, pS.yPac, pS.xGhost[1], pS.yGhost[1] );
		
			pS = arg[++i];

			int d0next = manhattanDist( pS.xPac, pS.yPac, pS.xGhost[0], pS.yGhost[0] );
			int d1next = manhattanDist( pS.xPac, pS.yPac, pS.xGhost[1], pS.yGhost[1] );			
			
			if( (d0next < 5 &&  d0next < d0) || (d1next < 5 &&  d1next < d1) )
			{
				output++;
				break;
			}
		}
	}

	return output;
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
 *	Description: How long after a Cherry appears does it take Pacman to eat it?
 */
deque<float> Resource_TimeAndNumChry( deque<PacState> arg )
{
	deque<float> output;
	output.assign(2, 0);
	float temp = 0.0, lvls = 0.0;
	PacState pS;
	int s = arg.size(), yChry;
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
				output[1]++;
			}
			temp = 0.0;	
		}
		if( pS.State == 0 )
		{
			odd = !odd;
			lvls++;
			if( odd )
				yChry = 11;
			else
				yChry = 6;
		}
	}
	output[0] /= lvls;
	output[1] /= lvls;

	return output;
}
//End of File
