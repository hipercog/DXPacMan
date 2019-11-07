/**
 *	Author: Ben Cowley, 2006.
 *	Decision Theory Player Modelling functions for predicting player utility
 */
#include "main.h"
#include "universals.h"
#include "dtpm.h"
#include "tree.h"
#include "utility.h"
#include "Classifier.h"

/**
 *	Function prototypes
 */
/* Action sequence functions */
double actions( int lookahead, TreeIterator <GhostMoves> titr );
deque <PacState> valuateStateDQ( TreeIterator <PacState> sitr, deque<PacState> branch );

/* Generate-next-state functions */
NextMoves initNextMove_Pac( PacState pS );
GhostMoves initNextMove_Ghost( PacState pS );
PacState updateState( PacState pS );

/* Generate tree-of-next-states functions */
void spawnStates( Tree<PacState>* tree );
void spawnAllStates( Tree<PacState>* tree );

/* Utile, lower tier functions */
void printGhostMoves();

/**
 *	Global fields.
 */
bool PredictDeath = false;
int inc = 1, Lvl = 1, lastCycle = 0, lookahead, stateCount = 0;
Tree <PacState>* tree_States;
TreeIterator <PacState> iter_States;
GhostMoves gm;
NextMoves nm;
int moved[4] = {0};
NextMoves base_nm;

//This is the current state, and is initialised from logged data and stays UNCHANGED, and so available for reference
PacState real_State;
PacState curr_State;
PacState new_State;
deque <PacState> actual_States;
deque <float> branchUtils;
deque <float> branchUtil2;
deque <int> branchDirs;
deque <string> branchLogs;

/**
 *	Description: Entry point function for non-real-time play analysis of saved states from previous game.
 */
void postDTPM( unsigned int look_AHead, int btLag, int alpha, int ID )
{
	lookahead = look_AHead;
	string infile = "Game-";
	sprintf_s( out, 99, "%d", ID );
	infile += out;
	infile += ".csv";
	deque<int> csv = readFromCSVFile( infile.c_str() );
/**/
	int tempMap[20][20], xGst[NUM_GS], yGst[NUM_GS];
	int addend = 2 + (NUM_GS * 2);
	int i = 0, predir = -1, prdir2 = -1, compdir = -1, cmpdir2 = -1, temp, count, magnitude = 4;
	float matches = 0.0, matchs2 = 0.0, accuracy = 1.0, acc2 = 1.0, old_acc = 0.5, tendency = 16.0;

	while( i < csv.size() )
	{
		count = 0;
		temp = i;
		inc = 0;

		while( csv[temp] != INT_MIN && csv[temp] != INT_MAX )
			temp++;

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

		if( real_State.State > csv[i] )
		{
			GetFeatures( actual_States, Lvl );
			OneRuleClassify( Lvl );
			Classify( Lvl );
			
			stateCount += real_State.State - 1;
			Lvl++;
			if( Lvl == 2 || Lvl == 4 || Lvl == 7 || Lvl == 11 || Lvl == 16 )
			{
				gakProb++;
				pacProb--;
			}			
		}
		if( real_State.Lives > csv[i+addend+2] )
			stateCount--;

		lastCycle = real_State.Cycle;
		real_State = PacState(csv[i], csv[i+1], csv[i+2], csv[i+addend+2], csv[i+addend+4], csv[i+addend+3], csv[i+addend+1], csv[i+addend+7], csv[i+addend+8], xGst, yGst, csv[i+addend+5] == 1, csv[i+addend+6] == 1, tempMap );
		actual_States.push_back( real_State );
		lastCycle = real_State.Cycle - lastCycle;
		i = ++temp;
		inc++;
/**/
		//Instantiate the global tree (from the passed PacState), and its iterator
		tree_States = new Tree<PacState>(real_State);
		iter_States = tree_States;
/**/
		//Now we must recurse - create children, go down one to their level, create children for each of them, etc
		PreorderToDepth( tree_States, spawnStates, lookahead );
		iter_States.Root();

		/*	Analyse and print out results in some fashion */
		deque<PacState> branch;
		valuateStateDQ( iter_States, branch );
		int index = highest( branchUtils );
		predir = branchDirs[ index ];
/**/
		int indx2 = highest( branchUtil2 );
		prdir2 = branchDirs[ indx2 ];

		/*	Prevent backtracking when in a corridor (for a sufficient number of moves)	*/
		base_nm = initNextMove_Pac( real_State );
		if( base_nm.prob == 2 )
		{
			//Can use this moved array value to see how long we've been in a corridor, and thus decide how likely it is we keep going - need a cohesive utility scoring mechanism though
			moved[ real_State.MovedDir ]++;
			
			/*	This number = btLag = #moves before we start doing 'no-backtracking'; its very important to accuracy	*/
			if( moved[ real_State.MovedDir ] > btLag )
			{	
/**/
				deque <int> branchDir2 = branchDirs;
				while( prdir2 == real_State.MovedDir && branchUtil2.size() > 0 )
				{
					branchUtil2.erase( branchUtil2.begin() + indx2 );
					branchDir2.erase( branchDir2.begin() + indx2 );
					indx2 = highest( branchUtil2 );
					prdir2 = branchDir2[ indx2 ];
				}
/**/
				while( predir == real_State.MovedDir && branchUtils.size() > 0 )
				{
					branchUtils.erase( branchUtils.begin() + index );
					branchDirs.erase( branchDirs.begin() + index );
					branchLogs.erase( branchLogs.begin() + index );
					index = highest( branchUtils );
					predir = branchDirs[ index ];
				}				
/**/
			}
		}else{
			moved[0] = 0;
			moved[1] = 0;
			moved[2] = 0;
			moved[3] = 0;
		}

		/*	Do comparison of predicted direction (from last state?) with actual direction	*/
		sprintf_s( out, 99, "%d", compdir );
		print( "Pred:," ); print( out );
		sprintf_s( out, 99, "%d", real_State.MovedDir );
		print( ",Moved:," ); print( out );

		int match = (int)(compdir == real_State.MovedDir);
		sprintf_s( out, 99, "%d", match );
		print( ",Match:," ); print( out );

		matches += (float)match;
		accuracy = matches / (float)(stateCount + real_State.State);
		sprintf_s( out, 99, "%f", accuracy );
		print( ",%Correct:," ); print( out ); 
/**/	
		//	Do comparisons for held weights
		sprintf_s( out, 99, "%d", cmpdir2 );
		print( ",Held_Weights:,Pred2:," ); print( out );

		int match2 = (int)(cmpdir2 == real_State.MovedDir);
		sprintf_s( out, 99, "%d", match2 );
		print( ",Match2:," ); print( out );

		matchs2 += (float)match2;
		acc2 = matchs2 / (float)(stateCount + real_State.State);
		sprintf_s( out, 99, "%f", acc2 );
		print( ",%Correct2:," ); print( out ); 

		//	Determine whether we should keep new weights or rollback: mod value is like learning rate alpha;
		if( (stateCount + real_State.State)%alpha == 0 )
		{
			if( accuracy >= acc2 )
				hold();
			else{
				rollback();
				accuracy = acc2;
			}

			if( accuracy < old_acc )
			{
				tendency *= -1.0;	//Change signs	
			}
			learn( magnitude, tendency );
			old_acc = accuracy;
		}

		//	Do clean up calls and update the comparison vars to predicted values
		branchUtil2.clear();
		cmpdir2 = prdir2;
/**/
		print( branchLogs[ index ] );
		print( "\n" );

		/*	Do clean up calls and update the comparison vars to predicted values	*/
		branchDirs.clear();
		branchUtils.clear();
		branchLogs.clear();
		compdir = predir;

		tree_States->~Tree();
		fflush(fileout);
	}
/**/
	GetFeatures( actual_States, Lvl );
	OneRuleClassify( Lvl );
	Classify( Lvl );
	actual_States.~deque();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
///// IMPLEMENT NEW MODELLING CONCEPT - VALUATE FUTURE-STATES TREE BRANCHES/SEQUENCE OF STATES   ///////
////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 *	Description: Calculates utility and probability of all states corresponding to a single branch of the future states tree, for all branches.
 *	Arguments: The node of the tree of ghost move probabilities containing the current set of ghost moves to be considered.
 */
deque <PacState> valuateStateDQ( TreeIterator <PacState> sitr, deque <PacState> branch )
{
	branch.push_back( sitr.Item() );
	
	if( sitr.ChildValid() )
	{
		for( sitr.ChildStart(); sitr.ChildValid(); sitr.ChildForth() )
		{
			TreeIterator <PacState> temp_itr = sitr;
			temp_itr.Down();
			valuateStateDQ( temp_itr, branch );
		}
	}else{
		branch.pop_front();
		deque <float> utemp = utility( branch );
		branchUtils.push_back( utemp[0] );
		branchUtil2.push_back( utemp[1] );
		branchDirs.push_back( branch[0].MovedDir );
		branchLogs.push_back( logData );
		logData.clear();
	}

	return branch; // OR: return sum;
}

/**
 * Description:		Take the PacState struct from the primary node of the passed Tree, and spawn a list of children.
 *					Just works of Pacman's possible moves and saves an associated GhostMoves structwith each state so the
 * Return Value:	An increment counter i.e. if(){for(){if(){i++}}}, or 0 if iter.Valid() == false
 * Arguments:		Sub-Tree that points to the current node we want to grow
 */
void spawnStates( Tree<PacState>* tree )
{
	int counter = 0;
	iter_States = tree;
	curr_State = tree->m_data;

	//Pacman's possible move positions and accompanying probability
	nm = initNextMove_Pac( curr_State );
	//Only init the  possible move positions and accompanying probability for Ghosts at the next level of recursion
	gm = initNextMove_Ghost( curr_State );

	//Now gen all children of current state by combining Pacman and Ghost moves, with accompanying probs - store probs as % at dtpMap[0][0].
	if( gm.perms > 0 && nm.prob > 0 )
	{
		int gstX [NUM_GS];
		int gstY [NUM_GS];
		deque <int> gmProbs;

		for( int pot = 0; pot < POT_GS; pot++ )
		{
			gmProbs.push_back( gm.ghostXYMove[0][2][pot] + gm.ghostXYMove[1][2][pot] );
		}
		int highProb = highest( gmProbs );

		for( int i = 0; i < NUM_GS; i++ )
		{
			gstX[i] = gm.ghostXYMove[i][0][highProb];
			gstY[i] = gm.ghostXYMove[i][1][highProb];
		}

		for( int pac = 0; pac < 4; pac++ )
			if( nm.xyMove[0][pac] > 0 )
			{
				new_State = PacState(inc++, nm.xyMove[0][pac], nm.xyMove[1][pac], curr_State.Lives, curr_State.Points, 
											curr_State.Pills, curr_State.Dots, pac, curr_State.Cycle, gstX, 
											gstY, curr_State.CherryThere, curr_State.PacAttack, curr_State.dtpMap );
				new_State = updateState( new_State );
				new_State.setProb( (int)(( (gm.ghostXYMove[0][2][highProb] * 10) + (gm.ghostXYMove[1][2][highProb] * 10) + (int)(100/nm.prob) ) / 3) );

				Tree <PacState>* node = new Tree<PacState>( new_State );
				// New nodes are appended to tree via global iterator which has been reset via Tree argument
				iter_States.AppendChild( node );
				counter++;
/** /
sprintf_s( out, 99, "%d", inc );
print( out ); print( "," );
/**/
			}
	}else print("EMPTY NODE - NO CHILDREN\n");
/** /
sprintf_s( out, 99, "%d", counter );
print( "Children: " ); print( out ); print( "\n" );
/**/
}

/**
 * Description:		Take the PacState struct from the primary node of the passed Tree, and spawn a list of children
 * Return Value:	An increment counter i.e. if(){for(){if(){i++}}}, or 0 if iter.Valid() == false
 * Arguments:		Sub-Tree that points to the current node we want to grow
 */
void spawnAllStates( Tree<PacState>* tree )
{
int counter = 0;
	iter_States = tree;
	curr_State = tree->m_data;

	//Pacman's possible move positions and accompanying probability
	nm = initNextMove_Pac( curr_State );
	//Only init the  possible move positions and accompanying probability for Ghosts at the next level of recursion
	gm = initNextMove_Ghost( curr_State );

	//Now gen all children of current state by combining Pacman and Ghost moves, with accompanying probs - store probs as % at dtpMap[0][0].
	if( gm.perms > 0 && nm.prob > 0 )
	{
		int gstX [NUM_GS];
		int gstY [NUM_GS];
		for( int pot = 0; pot < POT_GS; pot++ )
			if( gm.ghostXYMove[0][2][pot] > NOMV )
			{
				for( int i = 0; i < NUM_GS; i++ )
				{
					gstX[i] = gm.ghostXYMove[i][0][pot];
					gstY[i] = gm.ghostXYMove[i][1][pot];
				}
				for( int pac = 0; pac < 4; pac++ )
					if( nm.xyMove[0][pac] > 0 )
					{
						new_State = PacState(inc++, nm.xyMove[0][pac], nm.xyMove[1][pac], curr_State.Lives, curr_State.Points, 
													curr_State.Pills, curr_State.Dots, pac, curr_State.Cycle, gstX, 
													gstY, curr_State.CherryThere, curr_State.PacAttack, curr_State.dtpMap );
						new_State = updateState( new_State );
						new_State.setProb( (int)(( (gm.ghostXYMove[0][2][pot] * 10) + (gm.ghostXYMove[1][2][pot] * 10) + (int)(100/nm.prob) ) / 3) );

						Tree <PacState>* node = new Tree<PacState>( new_State );
						// New nodes are appended to tree via global iterator which has been reset via Tree argument
						iter_States.AppendChild( node );
						counter++;
/** /
sprintf_s( out, 99, "%d", inc );
print( out ); print( "," );
/**/
					}
			}
	}else print("EMPTY NODE - NO CHILDREN\n");
/** /
sprintf_s( out, 99, "%d", counter );
print( "Children: " ); print( out ); print( "\n" );
/**/
}

/**
 *	Description: Updates the map in a passed State, where Pills or Dots have been eaten.
 */
PacState updateState( PacState pS )
{
	if( pS.dtpMap[pS.xPac][pS.yPac] != 6 )
	{
		if( pS.dtpMap[pS.xPac][pS.yPac] == 4 )
		{
			pS.dtpMap[pS.xPac][pS.yPac] = 6;
			pS.Dots--;
			pS.Points += 10;
		}else if( pS.dtpMap[pS.xPac][pS.yPac] == 5 )
		{
			pS.dtpMap[pS.xPac][pS.yPac] = 6;
			pS.Pills--;
			pS.Points += 50;
			pS.PacAttack = true;
		}
	}
	//Do something about Cycles - add an average amount
	pS.Cycle += lastCycle;
	
	//Check the Cherry for Pacman...
	if( pS.CherryThere )
	{
		if( pS.dtpMap[10][10] == 1 )	//If it is the even numbered levels...
			if( pS.xPac == 10 && pS.yPac == 11 )
			{
				pS.Lives++;
				pS.CherryThere = false;
			}
		else							//If it is odd numbered levels...
			if( pS.CherryThere && pS.xPac == 10 && pS.yPac == 6 )
			{
				pS.Lives++;
				pS.CherryThere = false;
			}
	}

	//Now what about Ghosts ?
	if( pS.PacAttack )
	{
		//For each ghost, they are eaten if they meet Pacman
		for( int i = 0; i < NUM_GS; i++ )
			if( pS.xGhost[i] == pS.xPac && pS.yGhost[i] == pS.yPac )
				pS.Points += 100*(i+1);

		//Test GhostFleeTimer - see if PacAttack should be set back.
		//Count the number of states where PacAttack = true
		float cyc = 0.0;
		int S = actual_States.size()-1, back = S;
		while( actual_States[back].PacAttack )
		{
			back--;
			cyc++;
		}
		//Get the average cycles per state over this period.
		cyc = (float)(actual_States[S].Cycle - actual_States[back].Cycle) / max( 1, cyc );

		//Get the time in cycles that should be left on the GhostFleeTimer
		int timeleft = 0;
		if( Lvl < 12 )
			timeleft = 340 - (Lvl*(11 - pS.Pills)) - (actual_States[S].Cycle - actual_States[back].Cycle);
		else
			timeleft = 150 + (Lvl*(11 - pS.Pills)) - (actual_States[S].Cycle - actual_States[back].Cycle);
		
		//Then estimate how many turns are left before the timer runs out.
		if( (int)(timeleft / max( 1, cyc )) == 0 )
			pS.PacAttack = false;

	}else{
		//For Shadow, they eat the Cherry if collide...
		if( pS.CherryThere )
		{
			if( pS.dtpMap[10][10] == 1 )	//If it is the even numbered levels...
				if( pS.xGhost[0] == 10 && pS.yGhost[0] == 11 )
					pS.CherryThere = false;
			else							//If it is odd numbered levels...
				if( pS.xGhost[0] == 10 && pS.yGhost[0] == 6 )
					pS.CherryThere = false;
		}

		//For each ghost, they eat Pacman if they meet him
		for( int i = 0; i < NUM_GS; i++ )
		{
			if( pS.xGhost[i] == pS.xPac && pS.yGhost[i] == pS.yPac )
			{
				pS.Lives--;
				//We have a global bool here, to flag that Pac death
				PredictDeath = true;
				pS.xPac = PACMANSPRITE_START_POS_X;
				if( pS.dtpMap[9][10] == 1 )
					pS.yPac = PACMANSPRITE_START_POS_Y;
				else
					pS.yPac = 4;

				pS.xGhost[0] = SHADOWSPRITE_START_POS_X;
				pS.yGhost[0] = SHADOWSPRITE_START_POS_Y;
				pS.xGhost[1] = SPEEDYSPRITE_START_POS_X;
				pS.yGhost[1] = SPEEDYSPRITE_START_POS_Y;
				break;
			}
		}		
	}

	return pS;
}

/**
 * Description:	Initialise and return the struct holding Pacman's next move.
 * Arguments:	pS, the PacState that the next Pacman move will derive from.
 */
NextMoves initNextMove_Pac( PacState pS )
{
	int openDirs = 0;
	NextMoves n = NextMoves();
	//Check all around Pacman
	if( pS.dtpMap[pS.xPac][pS.yPac - 1] > 3 )
	{
		n.xyMove[0][0] = pS.xPac;
		n.xyMove[1][0] = pS.yPac - 1;
		n.prob++;
	}
	if( pS.dtpMap[pS.xPac][pS.yPac + 1] > 3 )
	{
		n.xyMove[0][1] = pS.xPac;
		n.xyMove[1][1] = pS.yPac + 1;
		n.prob++;
	}
	if( pS.dtpMap[pS.xPac + 1][pS.yPac] > 3 )
	{
		n.xyMove[0][2] = pS.xPac + 1;
		n.xyMove[1][2] = pS.yPac;
		n.prob++;
	}
	if( pS.dtpMap[pS.xPac - 1][pS.yPac] > 3 )
	{
		n.xyMove[0][3] = pS.xPac - 1;
		n.xyMove[1][3] = pS.yPac;
		n.prob++;
	}

	return n;
}

/**
 * Description:		Discover all possible combinations of ghost movement, and the corresponding probability
 * Arguments:		The arrays of ghost's x and y values
 * Return Value:	The number of ghost move combinations
 */
GhostMoves initNextMove_Ghost( PacState pS )	//int *baseGhostX, int *baseGhostY /*[Ghost 1-4][x-y]*/, int Map[20][20])
{
	GhostMoves gms = GhostMoves( pS.xGhost, pS.yGhost );
	int dir = 0, dirCnt = 0, nearProb = 0, farProb = 0;
	int gDirs[4] = {0};
	int gDist[4] = {INT_MAX, INT_MAX, INT_MAX, INT_MAX};
	int pFor1Gst[4] = {0};	//All four values should sum to 1

	for( int g = 0; g < NUM_GS; g++ )
	{
		
		if( real_State.PacAttack )
		{
			nearProb = pacProb;
			farProb = gakProb;
		}else{
			nearProb = gakProb;
			farProb = pacProb;
		}

		if( pS.dtpMap[pS.xGhost[g]][pS.yGhost[g] - 1 ] != 1 )
		{
			gDirs[0] = -1;
			gDist[0] = myHeuristicDist( pS.xGhost[g], pS.yGhost[g] - 1, real_State.xPac, real_State.yPac );
		}
		if( pS.dtpMap[pS.xGhost[g]][pS.yGhost[g] + 1 ] != 1 )
		{
			gDirs[1] = 1;
			gDist[1] = myHeuristicDist( pS.xGhost[g], pS.yGhost[g] + 1, real_State.xPac, real_State.yPac );
		}
		if( pS.dtpMap[pS.xGhost[g] + 1 ][pS.yGhost[g]] != 1 )
		{
			gDirs[2] = 1;
			gDist[2] = myHeuristicDist( pS.xGhost[g] + 1, pS.yGhost[g], real_State.xPac, real_State.yPac );
		}
		if( pS.dtpMap[pS.xGhost[g] - 1 ][pS.yGhost[g]] != 1 )
		{
			gDirs[3] = -1;
			gDist[3] = myHeuristicDist( pS.xGhost[g] - 1, pS.yGhost[g], real_State.xPac, real_State.yPac );
		}
	
		for( int k = 0; k < 4; k++ )
			dirCnt += abs( gDirs[k] );

		dir = lowest( gDist, 4 );
		pFor1Gst[ dir ] = nearProb - dirCnt;
		gDist[ dir ] = INT_MIN;

		for( int f = 0; f < 4; f++ )
			if( gDist[f] == INT_MAX )
				gDist[f] = INT_MIN;

		dir = highest( gDist, 4 );
		pFor1Gst[ dir ] = farProb - dirCnt;
		gDist[ dir ] = INT_MIN;

		for( int i = 0; i < 4; i++ )
			if( gDist[i] != INT_MIN )
				pFor1Gst[i] = 2;

		dir = -1;
		for( int cmbn = 0; cmbn < POT_GS; cmbn++ )
		{
			if( cmbn%(int)pow( 4.0, g ) == 0 )
				dir++;
			//Check around ghosts
			if( gDirs[dir%4] == 0 )
				gms.ghostXYMove[0][2][cmbn] = NOMV;
			else{
				gms.ghostXYMove[g][2][cmbn] = pFor1Gst[dir%4];
				if( dir%4 < 2 )
					gms.ghostXYMove[g][1][cmbn] += gDirs[dir%4];
				else
					gms.ghostXYMove[g][0][cmbn] += gDirs[dir%4];
			}
		}
	}

	gms.size();

	return gms;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////             IMPLEMENT D.T. EQUATION                                        /////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

/*	Decision Theory logic is :- look at available actions, possible states under each action, and utility of each state.
 *	Actions available are decided by the current state. Future states depend on actions and ghost movement.
 *	Fitness function of utility - maximum points acheivable with minimal loss of life.
 *	Utility of a state is relative to current state - what points/-+lives Pacman gets in the intervening time...
 *	Putting together all consecutive utilities up to the conclusion point is the only way to accurately estimate utility
 *	Proposing a definite goal is assuming we know what the player should want to do. This is currently too sophisticated to be realistic
 *	Without a goal state, we have to iterate all potential action sets to a fixed limit.
 *	How to define the limit : this is our difficult question.
 *	Not goal state, but current state is important...
 *	So current code is ok...but must calculate all action sets out from current state, not set artificial goal state and look for best path to it
 *	Following all action sets from current state, context matters in each action in each set
 *	When following an action set, need to keep track of 'current' state context, i.e. recursion on availableActions maybe
 *	Action sets must be rational and goal-oriented, no oscillation or looping
 * /

double actions( int lookahead, TreeIterator <GhostMoves> titr )
{
	//Start at the start. Next available action for Pacman (then recurse?) -
	//...and return a value for each state possible at each move:
	//	P(Pacman.action)*Sum( P(Ghost[0..3].action)*U(state.features), .., P(Ghost[0..3].action)*U(state.features))
	double aVal = 0.0;

	if( lookahead >= 0 )
	{
		double actArr[4] = {0.0};
		int openDirs = 0;
		PacState tempPS = real_State;
		TreeIterator <GhostMoves> temp;
		if( real_State.dtpMap[real_State.xPac][real_State.yPac - 1] > 2 )
		{
			openDirs++;
			real_State.yPac--;
			updateState(real_State);
			actArr[0] = valuateStateDQ( &titr.m_node->m_data );
			titr.ChildStart();
			while( titr.ChildValid() )
			{
				temp = titr;
				temp.Down();
/** /
				for( int i = 0; i < NUM_GS; i++ )
				{
					real_State.xGhost[i] = temp.Item().ghostXYMove[i][0][];
					real_State.yGhost[i] = temp.Item().ghostXYMove[i][1][];
				}
/** /
				actArr[0] += actions( --lookahead, temp );
				lookahead++;
				titr.ChildForth();
			}
			real_State = tempPS;
		}
		if( real_State.dtpMap[real_State.xPac][real_State.yPac + 1] > 2 )
		{
			openDirs++;
			real_State.yPac++;
			updateState(real_State);
			actArr[1] = valuateStateDQ( &titr.m_node->m_data );
			titr.ChildStart();
			while( titr.ChildValid() )
			{
				temp = titr;
				temp.Down();
				actArr[1] += actions( --lookahead, temp );
				lookahead++;
				titr.ChildForth();
			}
			real_State = tempPS;
		}
		if( real_State.dtpMap[real_State.xPac + 1][real_State.yPac] > 2 )
		{
			openDirs++;
			real_State.xPac++;
			updateState(real_State);
			actArr[2] = valuateStateDQ( &titr.m_node->m_data );
			titr.ChildStart();
			while( titr.ChildValid() )
			{
				temp = titr;
				temp.Down();
				actArr[2] += actions( --lookahead, temp );
				lookahead++;
				titr.ChildForth();
			}
			real_State = tempPS;
		}
		if( real_State.dtpMap[real_State.xPac - 1][real_State.yPac] > 2 )
		{
			openDirs++;
			real_State.xPac--;
			updateState(real_State);
			actArr[3] = valuateStateDQ( &titr.m_node->m_data );
			titr.ChildStart();
			while( titr.ChildValid() )
			{
				temp = titr;
				temp.Down();
				actArr[3] += actions( --lookahead, temp );
				lookahead++;
				titr.ChildForth();
			}
			real_State = tempPS;
		}
		actDir = highest( actArr, 4 );
		aVal = actArr[ actDir ] / openDirs;
	}
	return aVal;
}

/**
 * Description: Will print out the contents of the global GhostMoves struct in 2D matrix form, with entries as either _ or ghost[1-4].x.y.p
 */
void printGhostMoves( GhostMoves gM )
{
	unsigned int count = -1;
	int factor = (int)pow( 2.0, NUM_GS );

	print("\n#P(real_State): 1/"); sprintf_s(out, 99, "%d", gM.perms); print(out); print("\n\t");
	for( int i = 0; i < factor; i++ )
	{
		print("| Column "); sprintf_s(out, 99, "%d", i); print(out);
		for( int j = 0; j < NUM_GS; j++ )
			print("\t");
	}

	for( int i = 0; i < factor; i++ )
	{
		print("\nRow "); sprintf_s(out, 99, "%d", i); print(out); print("\t");
		for( int j = 0; j < factor; j++ )
		{
			print("|");
			count++;
			if( gM.ghostXYMove[0][0][count] <= NOMV )
			{
				print(" _,_,_,_");
				for(int g = 0; g < NUM_GS; g++)
					print("\t");
			}else
			{
				for(int g2 = 0; g2 < NUM_GS; g2++)
				{
					for(int g3 = 0; g3 < 3; g3++)
					{
						//Adjust spacing between 1 and 2 digit numbers
						if( gM.ghostXYMove[g2][g3][count] < 10 )
							print(" ");
						//Print the ghost position
						sprintf_s( out, 99, "%d", gM.ghostXYMove[g2][g3][count] );
						print(out); 
						if( g3 < 2 ) print(",");
						else print("|");
					}
				}
				print("\t");
			}
		}
	}
	print("\n\n");
}
//End of file