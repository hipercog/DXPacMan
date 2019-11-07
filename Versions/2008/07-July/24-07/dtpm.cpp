/**
 *	Author: Ben Cowley, 2006.
 *	Decision Theory Player Modelling functions for predicting player utility
 */
#include "main.h"
#include "universals.h"
#include "dtpm.h"
#include "tree.h"

/**
 *	Function prototypes
 */
//Action sequence functions
double actions( int lookahead, TreeIterator <GhostMoves> titr );
void updateState();
double calculateState( GhostMoves *node_state );

//Utility calculation functions
double utility(PacState pState);
double utilityDots(PacState pS);
double utilityPills(PacState pS, int risk);
double utilityGhostHunt(PacState pS, int risk);
double utilityGhosts(PacState pS, int risk);

//Probability of state functions
double probability(PacState pS);
double probabilityOfPac(PacState pS);
void buildTreeOfGhostMoves(unsigned int recurseLimit, int trg_x, int trg_y, PacState pS);
int countSpawn(Tree<GhostMoves>* tree);
void probabilityOfGhost(int *baseGhostX, int *baseGhostY, int Map[20][20]);

/* Utile, lower tier functions */
void printGhostMoves();

/**
 *	Global fields.
 */
PacState S;

GhostMoves GhostMove;
Tree <GhostMoves>* tree_GstMv;
TreeIterator <GhostMoves> iter_GstMv;
int actDir = 0;

/**
 * Description: Entry point function for use in real-time - triggered by presssing a keyboard numeric, the value of which sets depth of lookahead
 */
void forceDTPM( unsigned int lookahead )
{	
	/** /
	logPos(g_ShadowSpritePos, "ShadowXY: ", ";\t");
	if( NUM_GS > 1 )
	{
		logPos(g_SpeedySpritePos, "SpeedyXY: ", ";\t");
		if( NUM_GS > 2 )
		{
			logPos(g_PokeySpritePos, "PokeyXY: ", ";\t");
			if( NUM_GS > 3 )
				logPos(g_BashfulSpritePos, "BashfulXY: ", ";\n");
	}}
	logPos(g_PacmanSpritePos, "PacmanXY: ", ";");
	/**/
	//Grab the state of the game, initialise PacState field around it.
	S = PacState();
	buildTreeOfGhostMoves( lookahead, -1, -1, S );
	iter_GstMv.Root();
//	actions( lookahead, iter_GstMv );
	tree_GstMv->Destroy();
}

/**
 *	Description: Entry point function for non-real-time play analysis of saved states from previous game.
 */
void postDTPM( unsigned int lookahead )
{
	deque<int> csv = readFromCSVFile( "stateData/stateData-1.csv" );
/**/
	int i = 0, pred = 0;
	DWORD temptime = 0;
	while( i < csv.size() )
	{
		int temp = i;
		while( csv[temp] != INT_MAX )
			temp++;
		temptime = GetTickCount();
		int xGst[NUM_GS];
		int yGst[NUM_GS];
		for( int n = 0; n < NUM_GS; n++ )
		{
			xGst[n] = csv[i+3+(n*2)];
			yGst[n] = csv[i+4+(n*2)];
		}
		int addend = 2 + (NUM_GS * 2);
		int tempMap[20][20];
		int count = 0;
		for( int x = 0; x < 20; x++ )
			for( int y = 0; y < 20; y++ )
			{
				tempMap[x][y] = csv[i+addend+9+count];
				count++;
			}
		S = PacState(logIndex, csv[i+1], csv[i+2], csv[i+addend+2], csv[i+addend+4], csv[i+addend+3], csv[i+addend+1], csv[temp+addend+7], csv[i+addend+8], xGst, yGst, csv[i+addend+5] == 1, csv[i+addend+6] == 1, tempMap );
		i = ++temp;
/**/	
		buildTreeOfGhostMoves( lookahead, -1, -1, S );
		iter_GstMv.Root();
		actions( lookahead, iter_GstMv );

		sprintf_s( out, 99, "%d", csv[i] );
		print(out); print(",Prediction:,");
		
		sprintf_s( out, 99, "%d", actDir ); //Print Predictions first
		print(out); print(",");

		sprintf_s( out, 99, "%d", S.MovedDir ); //Print Moves second
		print(out); print(",");

		if( actDir == S.MovedDir ) //Print match or no match
		{
			cout << "Prediction " << csv[i] << ": " << actDir << "; Match: 1" << endl;
			println( "1" );
		}else{
			cout << "Prediction " << csv[i] << ": " << actDir << "; Match: 0" << endl;
			println( "0" );
		}

		tree_GstMv->Destroy();
		fflush(fileout);
/**/
	}
/**/
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
 */

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
		PacState tempPS = S;
		TreeIterator <GhostMoves> temp;
		if( S.dtpMap[S.xPac][S.yPac - 1] > 2 )
		{
			openDirs++;
			S.yPac--;
			updateState();
/**/		actArr[0] = calculateState( &titr.m_node->m_data );
			titr.ChildStart();
			while( titr.ChildValid() )
			{
				temp = titr;
				temp.Down();
/** /
				for( int i = 0; i < NUM_GS; i++ )
				{
					S.xGhost[i] = temp.Item().ghostXYMove[i][0][];
					S.yGhost[i] = temp.Item().ghostXYMove[i][1][];
				}
/**/
				actArr[0] += actions( --lookahead, temp );
				lookahead++;
				titr.ChildForth();
			}
/**/		S = tempPS;
		}
		if( S.dtpMap[S.xPac][S.yPac + 1] > 2 )
		{
			openDirs++;
			S.yPac++;
			updateState();
/**/		actArr[1] = calculateState( &titr.m_node->m_data );
			titr.ChildStart();
			while( titr.ChildValid() )
			{
				temp = titr;
				temp.Down();
				actArr[1] += actions( --lookahead, temp );
				lookahead++;
				titr.ChildForth();
			}
/**/		S = tempPS;
		}
		if( S.dtpMap[S.xPac + 1][S.yPac] > 2 )
		{
			openDirs++;
			S.xPac++;
			updateState();
/**/		actArr[2] = calculateState( &titr.m_node->m_data );
			titr.ChildStart();
			while( titr.ChildValid() )
			{
				temp = titr;
				temp.Down();
				actArr[2] += actions( --lookahead, temp );
				lookahead++;
				titr.ChildForth();
			}
/**/		S = tempPS;
		}
		if( S.dtpMap[S.xPac - 1][S.yPac] > 2 )
		{
			openDirs++;
			S.xPac--;
			updateState();
/**/		actArr[3] = calculateState( &titr.m_node->m_data );
			titr.ChildStart();
			while( titr.ChildValid() )
			{
				temp = titr;
				temp.Down();
				actArr[3] += actions( --lookahead, temp );
				lookahead++;
				titr.ChildForth();
			}
/**/		S = tempPS;
		}
		actDir = highest( actArr, 4 );
		aVal = actArr[ actDir ] / openDirs;
	}
	return aVal;
}

/**
 *	Description: Deals with updating pill and Dot eating in the global virtual state object S
 */
void updateState()
{
	if( S.dtpMap[S.xPac][S.yPac] != 6 )
		if( S.dtpMap[S.xPac][S.yPac] == 4 )
		{
			S.dtpMap[S.xPac][S.yPac] = 6;
			S.Dots--;
			S.Points += 5;
		}else if( S.dtpMap[S.xPac][S.yPac] == 5 )
		{
			S.dtpMap[S.xPac][S.yPac] = 6;
			S.Pills--;
			S.Points += 50;
			S.PacAttack = true;
		}
}

/**
 *	Description: Calculates & sums the utility and probability of all the states corresponding to a single Pacman position.
 *	Arguments: The node of the tree of ghost move probabilities containing the current set of ghost moves to be considered.
 */
double calculateState( GhostMoves *node_state )
{
	double sum[POT_GS];
	int parentPerms = 1, gstPrbs = 1;

	GhostMoves gM = *node_state;
	if( gM.perms > 0 )
	{
/** /
		iter_GstMv = node_state;
		while( iter_GstMv.Valid() )
		{
			parentPerms += iter_GstMv.m_node->m_data.perms;
			iter_GstMv.Up();
		}
/**/
		// Get the set of ghost move possibilities here, then calculate utility for each state.
		PacState tempPS;
		for( int count = 0; count < POT_GS; count++ )
		{
			tempPS = S;
			if( gM.ghostXYMove[0][0][count] > NOMV )
				for( int i = 0; i < NUM_GS; i++ )
				{
					S.xGhost[i] = gM.ghostXYMove[i][0][count];
					S.yGhost[i] = gM.ghostXYMove[i][1][count];
					gstPrbs *= gM.ghostXYMove[i][2][count];
				}

			sum[count] = ( utility(S) * gstPrbs ) / 100; // OR: sum += ( utility(S) * gstPrbs ) / 100;
			S = tempPS;
			gstPrbs = 1;
		}
	}

	return sum[ highest(sum, POT_GS) ]; // OR: return sum;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////             UTILITY FUNCTIONS                                              /////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Description: Utility of a state is a measure of the best possible end result that can be obtained from that state - i.e. we assume an optimally played game
 * Arguments:	parameters of the utility calculation formulae - when the formulae are ready, this will supply parameters for each in order of occurence of use.
 */
double utility(PacState pState)
{
	//Weight a feature set polynomial or something similar
	double utility = 0;//Score;

	//Risk is measured here in how many lives Pacman could lose and how many he has to spare
	//The risk posed by ghosts is subsumed in the utility calculation for ghost state.
	int risk = 5;
	if(randCherryInit == INT_MAX && !pState.CherryThere)
	{}
	risk -= pState.Lives;

	//Weight the state of the dots
	utility += utilityDots(pState);
	//State of PacAttack defines other utilities
	if(!pState.PacAttack)
	{
//println("Ghosts Attack");
		//Weight the state of the ghosts; derive what effect the ghost's positions will have on utilityPills()
		utility += utilityGhosts(pState, risk);
		//Weight the state of the pills - Similar to utility of PacAttack, but with large negative value to reflect getting to & eating a pill before ghost hunting
		utility += utilityPills(pState, risk);
	}else
		utility += utilityGhostHunt(pState, risk) + (pState.Pills * 50);

	return utility;
}

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

////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////             PROBABILITY FUNCTIONS                                          /////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Returns the probability of the next state.
 * /
double probability(PacState pS)
{
	double POneStepPac = probabilityOfPac( pS );
	//Prob of the Pacman action and all the Ghosts' actions, for the current Pacman state and one iteration.
	return POneStepPac + 1/probabilityOfGhost(pS.xGhost, pS.yGhost, pS.dtpMap);
}

/**
 * Dscription:	Probability of Pacman's move. Returns the reciprocal of the number of moves open to Pacman
 * Arguments:	xPacIndex: x index of pacman, allows speculative calculations
 *				yPacIndex: y index of pacman, allows speculative calculations
 */
double probabilityOfPac(PacState pS)
{
	int openDirs = 0;
	//Check all around Pacman
	if( pS.dtpMap[pS.xPac][pS.yPac - 1] > 2 )
		openDirs++;
	if( pS.dtpMap[pS.xPac][pS.yPac + 1] > 2 )
		openDirs++;
	if( pS.dtpMap[pS.xPac + 1][pS.yPac] > 2 )
		openDirs++;
	if( pS.dtpMap[pS.xPac - 1][pS.yPac] > 2 )
		openDirs++;

	return 1/openDirs;
}

/**
 * Description: Builds the tree of the GhostMoves structs that hold all the possible moves of the ghosts and associated probabilities.
 * Arguments:	recurseLimit: depth to which to build the tree
 *				trg_x: x value of a location we want to test proximity of ghosts to - enter negative numbers when not targeting
 *				trg_y: y value of location as above
 * Return Value: flag whether the function has executed successfully the whole way through
 */
void buildTreeOfGhostMoves(unsigned int recurseLimit, int trg_x, int trg_y, PacState pS)
{
	if( trg_x >= 0 && trg_x < 20 && trg_y >= 0 && trg_y < 20 )
	{
		int dists[NUM_GS];
		for( int i = 0; i < NUM_GS; i++ )
			dists[i] = manhattanDist(trg_x, trg_y, pS.xGhost[i], pS.yGhost[i]);
		recurseLimit = dists[lowest( dists, NUM_GS )];
	}
	//This is the first iteration of possible ghost move positions and accompanying probability:perms
	probabilityOfGhost( pS.xGhost, pS.yGhost, pS.dtpMap );

	//Instantiate the global tree (from the global GhostMoves struct) and its iterator
	tree_GstMv = new Tree<GhostMoves>(GhostMove);
	iter_GstMv = tree_GstMv;
	
/* Test run print output * /
print("\n\n******BEGIN TREE!!********\n\n");
print("\nPARENT NODE:\n");
printGhostMoves();
/**/
	PacState tempPS = S;
	S = pS;
	//Now we must recurse - create children, go down one to their level, create children for each of them, etc
	PreorderToDepth( tree_GstMv, countSpawn, recurseLimit );

//	tree_GstMv->Destroy();
	S = tempPS;
}

/**
 * Description:		Take the GhostMoves struct from the primary node of the passed Tree, and spawn a list of children
 * Return Value:	An increment counter i.e. if(){for(){if(){i++}}}, or 0 if iter.Valid() == false
 * Arguments:		Sub-Tree that points to the current node we want to grow
 */
int countSpawn(Tree<GhostMoves>* tree)
{
	int inc = 0;
	iter_GstMv = tree;
	GhostMoves gM = tree->m_data;

	//Print-Mark a run of generating child GhostMoves - parent first
	if(gM.perms > 0)
	{
	//print("\nNEW CHILDREN BEING GENERATED\n");
		for( int count = 0; count < POT_GS; count++ )
		{
			if( gM.ghostXYMove[0][0][count] > NOMV )
			{
				int ghostBranchX[NUM_GS];
				int ghostBranchY[NUM_GS];
				for(int i = 0; i < NUM_GS; i++)
				{
					ghostBranchX[i] = gM.ghostXYMove[i][0][count];
					ghostBranchY[i] = gM.ghostXYMove[i][1][count];
				}
				probabilityOfGhost( ghostBranchX, ghostBranchY, S.dtpMap );
				Tree <GhostMoves>* node = new Tree<GhostMoves>( GhostMove );
				// New nodes are appended to tree via global iterator which has been reset via Tree argument
				iter_GstMv.AppendChild( node );
				inc++;

/* Print call for test run situations - can't use in normal run, would produce far too much output * /
sprintf_s(out, 99, "%d", inc); print("Child #"); print(out); print("\tProbs#: "); sprintf_s(out, 99, "%d", GhostMove.perms); println(out);
printGhostMoves();
/**/
			}
		}
	}
//	else print("\nEMPTY NODE - NO CHILDREN\n");

	return inc; //tree_GstMv->m_children.m_count;
}

/**
 * Description:		Discover all possible combinations of ghost movement, and the corresponding probability
 * Arguments:		The arrays of ghost's x and y values
 * Return Value:	The number of ghost move combinations
 */
void probabilityOfGhost(int *baseGhostX, int *baseGhostY /*[Ghost 1-4][x-y]*/, int Map[20][20])
{
	//Up to POT_GS possible states given any distinct PacAction (since we have NUM_GS ghosts with 4 poss move dirs each = 4^NUM_GS)
	for(int p1 = 0; p1 < NUM_GS; p1++)
		for(int p2 = 0; p2 < POT_GS; p2++)
		{
			GhostMove.ghostXYMove[p1][0][p2] = baseGhostX[p1];
			GhostMove.ghostXYMove[p1][1][p2] = baseGhostY[p1];
			GhostMove.ghostXYMove[p1][2][p2] = 87;
		}

	for( int g = 0; g < NUM_GS; g++ )
	{
		int dir = 0, dirCnt = 0, nearProb = 0, farProb = 0;
		int gDirs[4] = {0};
		int gDist[4] = {INT_MAX, INT_MAX, INT_MAX, INT_MAX};
		int pFor1Gst[4] = {43, 43, 43, 43};
		
		if( S.PacAttack )
		{
			nearProb = 5;
			farProb = 9;
		}else{
			nearProb = 9;
			farProb = 5;
		}

		if( Map[baseGhostX[g]][baseGhostY[g] - 1 ] != 1 )
		{
			gDirs[0] = -1;
			gDist[0] = myHeuristicDist( baseGhostX[g], baseGhostY[g] - 1, S.xPac, S.yPac );
		}
		if( Map[baseGhostX[g]][baseGhostY[g] + 1 ] != 1 )
		{
			gDirs[1] = 1;
			gDist[1] = myHeuristicDist( baseGhostX[g], baseGhostY[g] + 1, S.xPac, S.yPac );
		}
		if( Map[baseGhostX[g] + 1 ][baseGhostY[g]] != 1 )
		{
			gDirs[2] = 1;
			gDist[2] = myHeuristicDist( baseGhostX[g] + 1, baseGhostY[g], S.xPac, S.yPac );
		}
		if( Map[baseGhostX[g] - 1 ][baseGhostY[g]] != 1 )
		{
			gDirs[3] = -1;
			gDist[3] = myHeuristicDist( baseGhostX[g] - 1, baseGhostY[g], S.xPac, S.yPac );
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
				GhostMove.ghostXYMove[0][0][cmbn] = NOMV;
			else{
				GhostMove.ghostXYMove[g][2][cmbn] = pFor1Gst[dir%4];
				if( dir%4 < 2 )
					GhostMove.ghostXYMove[g][1][cmbn] += gDirs[dir%4];
				else
					GhostMove.ghostXYMove[g][0][cmbn] += gDirs[dir%4];
			}
		}
	}

	GhostMove.size();
}

/**
 * Description: Will print out the contents of the global GhostMoves struct in 2D matrix form, with entries as either _ or ghost[1-4].x.y
 */
void printGhostMoves()
{
	unsigned int count = -1;
	int factor = (int)pow( 2.0, NUM_GS );

	print("\n#P(S): 1/"); sprintf_s(out, 99, "%d", GhostMove.perms); print(out); print("\n\t");
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
			if( GhostMove.ghostXYMove[0][0][count] <= NOMV )
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
						if(GhostMove.ghostXYMove[g2][g3][count] < 10)
							print(" ");
						//Print the ghost position
						sprintf_s(out, 99, "%d", GhostMove.ghostXYMove[g2][g3][count]);
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
