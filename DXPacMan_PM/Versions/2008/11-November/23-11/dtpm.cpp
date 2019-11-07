/**
 *	Author: Ben Cowley, 2006.
 *	Decision Theory Player Modelling functions for predicting player utility
 */
#include "main.h"
#include "universals.h"
#include "dtpm.h"
#include "tree.h"
#include "utility.h"

/**
 *	Function prototypes
 */
/* Action sequence functions */
double actions( int lookahead, TreeIterator <GhostMoves> titr );
void valuateStateDQ( TreeIterator <PacState> sitr, deque<PacState> branch );

/* Generate-next-state functions */
NextMoves initNextMove_Pac( PacState pS );
GhostMoves initNextMove_Ghost( PacState pS );
PacState updateState( PacState pS );

//void buildTreeOfStates( unsigned int recurseLimit, PacState pS );
void spawnStates( Tree<PacState>* tree );

/* Utile, lower tier functions */
void printGhostMoves();

/**
 *	Global fields.
 */
int inc = 0;
Tree <PacState>* tree_States;
TreeIterator <PacState> iter_States;

//This is the current state, and is initialised from logged data and stays UNCHANGED, and so available for reference
PacState real_State;
deque <double> branchUtils;

/**
 *	Description: Entry point function for non-real-time play analysis of saved states from previous game.
 */
void postDTPM( unsigned int lookahead )
{
	deque<int> csv = readFromCSVFile( "stateData/Level-1.csv" );
/**/
	int tempMap[20][20], xGst[NUM_GS], yGst[NUM_GS];
	int addend = 2 + (NUM_GS * 2);
	int i = 0, pred = 0, temp, count;

	while( i < csv.size() )
	{
		count = 0;
		temp = i;

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
		real_State = PacState(csv[i], csv[i+1], csv[i+2], csv[i+addend+2], csv[i+addend+4], csv[i+addend+3], csv[i+addend+1], csv[i+addend+7], csv[i+addend+8], xGst, yGst, csv[i+addend+5] == 1, csv[i+addend+6] == 1, tempMap );
		i = ++temp;
/**/
		//Instantiate the global tree (from the passed PacState), and its iterator
		tree_States = new Tree<PacState>(real_State);
		iter_States = tree_States;

		//Now we must recurse - create children, go down one to their level, create children for each of them, etc
		PreorderToDepth( tree_States, spawnStates, lookahead );
		iter_States.Root();

		//Analyse and print out results in some fashion
		deque<PacState> branch;
		valuateStateDQ( iter_States, branch );
		branchUtils.clear();

		logData.copy(out, 99, 0);
		print( out );

		logData.clear();
/** /
		sprintf_s( out, 99, "%d", csv[i] );
		print(out); print(",Prediction:,");
		
		sprintf_s( out, 99, "%d", actDir ); //Print Predictions first
		print(out); print(",");

		sprintf_s( out, 99, "%d", real_State.MovedDir ); //Print Moves second
		print(out); print(",");

		if( actDir == real_State.MovedDir ) //Print match or no match
		{
			cout << "Prediction " << csv[i] << ": " << actDir << "; Match: 1" << endl;
			println( "1" );
		}else{
			cout << "Prediction " << csv[i] << ": " << actDir << "; Match: 0" << endl;
			println( "0" );
		}

		tree_GstMv->Destroy();	// DON'T FORGET TO DO THIS IN NEW FUNCTIONALITY!!
		fflush(fileout);
/**/
	}
/**/
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
///// IMPLEMENT NEW MODELLING CONCEPT - VALUATE FUTURE-STATES TREE BRANCHES/SEQUENCE OF STATES   ///////
////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 *	Description: Calculates utility and probability of all states corresponding to a single branch of the future states tree, for all branches.
 *	Arguments: The node of the tree of ghost move probabilities containing the current set of ghost moves to be considered.
 */
void valuateStateDQ( TreeIterator <PacState> sitr, deque <PacState> branch )
{
	if( sitr.ChildValid() )
	{
		branch.push_back( sitr.Item() );
		for( sitr.ChildStart(); sitr.ChildValid(); sitr.ChildForth() )
		{
			TreeIterator <PacState> temp_itr = sitr;
			temp_itr.Down();
			valuateStateDQ( temp_itr, branch );
		}
	}else{
		sitr.Up();
		DListIterator<Tree<PacState>*> chitr = sitr.m_childitr;
		for( chitr.Start(); chitr.Valid(); chitr.Forth() )
		{
			branchUtils.push_back( utility( branch ) );
			branch.pop_back();
			branch.push_back( chitr.Item()->m_data );
		}
	}

//	return branchUtils; // OR: return sum;
}

/**
 * Description: Builds the tree of the GhostMoves structs that hold all the possible moves of the ghosts and associated probabilities.
 * Arguments:	recurseLimit: depth to which to build the tree
 *				pS: PacState from which to build the tree - the state we just read from the csv.
 * Return Value: flag whether the function has executed successfully the whole way through
 * /
void buildTreeOfStates( unsigned int recurseLimit, PacState pS )
{}

/**
 * Description:		Take the PacState struct from the primary node of the passed Tree, and spawn a list of children
 * Return Value:	An increment counter i.e. if(){for(){if(){i++}}}, or 0 if iter.Valid() == false
 * Arguments:		Sub-Tree that points to the current node we want to grow
 */
void spawnStates( Tree<PacState>* tree )
{
	iter_States = tree;
	PacState curr_State = tree->m_data;

	//This is the first iteration of possible move positions and accompanying probability
	NextMoves nm = initNextMove_Pac( curr_State );
	GhostMoves gm = initNextMove_Ghost( curr_State );

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
						PacState newState = PacState(inc++, nm.xyMove[0][pac], nm.xyMove[1][pac], curr_State.Lives, curr_State.Points, 
													curr_State.Pills, curr_State.Dots, curr_State.MovedDir, curr_State.Cycle, gstX, 
													gstY, curr_State.CherryThere, curr_State.PacAttack, curr_State.dtpMap );
						newState = updateState( newState );
						newState.setProb( (int)(( (gm.ghostXYMove[0][2][pot] * 10) + (gm.ghostXYMove[1][2][pot] * 10) + (int)(100/nm.prob) ) / 3) );

						Tree <PacState>* node = new Tree<PacState>( newState );
						// New nodes are appended to tree via global iterator which has been reset via Tree argument
						iter_States.AppendChild( node );
						
					}
			}
	}
//	else print("\nEMPTY NODE - NO CHILDREN\n");
	//return tree_States->m_children.m_count;
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
			pS.Points += 5;
		}else if( pS.dtpMap[pS.xPac][pS.yPac] == 5 )
		{
			pS.dtpMap[pS.xPac][pS.yPac] = 6;
			pS.Pills--;
			pS.Points += 50;
			pS.PacAttack = true;
		}
	}
	//Now what about Ghosts ?
	if( pS.PacAttack )
	{
		//For each ghost, they are eaten if they meet Pacman
		for( int i = 0; i < NUM_GS; i++ )
			if( pS.xGhost[i] == pS.xPac && pS.yGhost[i] == pS.yPac )
				pS.Points += 100*(i+1);
	}else{
		//For each ghost, they eat Pacman if they meet him
		for( int i = 0; i < NUM_GS; i++ )
		{
			if( pS.xGhost[i] == pS.xPac && pS.yGhost[i] == pS.yPac )
			{
				pS.Lives--;
				//We should have a global bool here, to flag that Pac death - then the future moves exploration can be severed for this branch
			}

			//the Cherry for ghosts...
			if( pS.CherryThere )
				if( pS.dtpMap[10][10] == 1 )	//If it is the even numbered levels...
					if( pS.xGhost[i] == 10 && pS.yGhost[i] == 11 )
						pS.CherryThere = false;
				else							//If it is odd numbered levels...
					if( pS.xGhost[i] == 10 && pS.yGhost[i] == 6 )
						pS.CherryThere = false;
		}
	}
	//and the Cherry for pacman...
	if( pS.CherryThere )
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

	return pS;
}

/**
 * Description:	Initialise and return the struct holding Pacman's next move.
 * Arguments:	pS, the PacState that the next Pacman move will derive from.
 */
NextMoves initNextMove_Pac( PacState pS )
{
	int openDirs = 0;
	NextMoves nm = NextMoves();
	//Check all around Pacman
	if( pS.dtpMap[pS.xPac][pS.yPac - 1] > 2 )
	{
		nm.xyMove[0][0] = pS.xPac;
		nm.xyMove[1][0] = pS.yPac - 1;
		nm.prob++;
	}
	if( pS.dtpMap[pS.xPac][pS.yPac + 1] > 2 )
	{
		nm.xyMove[0][1] = pS.xPac;
		nm.xyMove[1][1] = pS.yPac + 1;
		nm.prob++;
	}
	if( pS.dtpMap[pS.xPac + 1][pS.yPac] > 2 )
	{
		nm.xyMove[0][2] = pS.xPac + 1;
		nm.xyMove[1][2] = pS.yPac;
		nm.prob++;
	}
	if( pS.dtpMap[pS.xPac - 1][pS.yPac] > 2 )
	{
		nm.xyMove[0][3] = pS.xPac - 1;
		nm.xyMove[1][3] = pS.yPac;
		nm.prob++;
	}

	return nm;
}

/**
 * Description:		Discover all possible combinations of ghost movement, and the corresponding probability
 * Arguments:		The arrays of ghost's x and y values
 * Return Value:	The number of ghost move combinations
 */
GhostMoves initNextMove_Ghost( PacState pS )	//int *baseGhostX, int *baseGhostY /*[Ghost 1-4][x-y]*/, int Map[20][20])
{
	GhostMoves gm = GhostMoves( pS );

	for( int g = 0; g < NUM_GS; g++ )
	{
		int dir = 0, dirCnt = 0, nearProb = 0, farProb = 0;
		int gDirs[4] = {0};
		int gDist[4] = {INT_MAX, INT_MAX, INT_MAX, INT_MAX};
		int pFor1Gst[4] = {0};	//All four values should sum to 1
		
		if( real_State.PacAttack )
		{
			nearProb = 5;
			farProb = 9;
		}else{
			nearProb = 9;
			farProb = 5;
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
				gm.ghostXYMove[0][2][cmbn] = NOMV;
			else{
				gm.ghostXYMove[g][2][cmbn] = pFor1Gst[dir%4];
				if( dir%4 < 2 )
					gm.ghostXYMove[g][1][cmbn] += gDirs[dir%4];
				else
					gm.ghostXYMove[g][0][cmbn] += gDirs[dir%4];
			}
		}
	}

	gm.size();

	return gm;
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