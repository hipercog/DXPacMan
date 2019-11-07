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
int localDots(int xCentre, int yCentre, int radius, int Map[20][20]);
int localGhosts(int x, int y, int radius, PacState pS);
void printGhostMoves();

//The A* function
int a_Star(int src_x1, int src_y1, int trg_x2, int trg_y2, bool pacTravelling, PacState pS);

/**
 *	Global fields.
 */
PacState S;
deque <int> aPathX;
deque <int> aPathY;
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
		S = PacState(csv[i+1], csv[i+2], csv[i+addend+2], csv[i+addend+4], csv[i+addend+3], csv[i+addend+1], csv[temp+addend+8], csv[i+addend+9], xGst, yGst, csv[i+addend+5] == 1, csv[i+addend+6] == 1, tempMap );
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
	xCentroid = ((xCentroid / 4) + pS.xPac) / 2;
	yCentroid = ((yCentroid / 4) + pS.yPac) / 2;
	
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

////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////             FUNCTIONS FOR STATE ATTRIBUTE CALCULATION                      /////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Description: function that counts the number of dots within a given radius of an x,y coord
 */
int localDots(int xCentre, int yCentre, int radius, int Map[20][20])
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
	
	while(x <= xCentre)
	{
		while(y <= yCentre)
		{
			if(Map[x][y] == 4)
				dots++;
			y++;
		}
		x++;
	}
	return dots;
}

/**
 * Description: function that counts the number of ghosts within a given radius of an x,y coord
 */
int localGhosts(int x, int y, int radius, PacState pS)
{
	int ghosts = 0;
	if(x < 0)
		x = 0;
	else if(x > 19)
		x = 19;
	if(y < 0)
		y = 0;
	else if(y > 19)
		y = 19;
	for(int i = 0; i < NUM_GS; i++)
		if(myHeuristicDist(pS.xGhost[i], pS.yGhost[i], x, y) <= radius)
			ghosts++;
	return ghosts;
}

/**
 * Description:	Implement the A* pathfinding algorithm - optimised for Pacman map. Stores path in global deques aPathX and aPathY
 * Arguments:	The source x,y and the target x,y; whether it is a path for pacman; and the state to pathfind on.
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
			fScoresHist[iter][0] = openNodesF[0] = G + myHeuristicDist(x1, y1-1, trg_x2, trg_y2);
		}
		if(pS.dtpMap[x1][y1+1] > limit && closedDir != 1)
		{
			deadEnd = false;
			openNodesX[iter][1] = x1; 
			openNodesY[iter][1] = y1+1;  
			fScoresHist[iter][1] = openNodesF[1] = G + myHeuristicDist(x1, y1+1, trg_x2, trg_y2); 
		}
		if(pS.dtpMap[x1+1][y1] > limit && closedDir != 2)
		{
			deadEnd = false;
			openNodesX[iter][2] = x1+1; 
			openNodesY[iter][2] = y1;  
			fScoresHist[iter][2] = openNodesF[2] = G + myHeuristicDist(x1+1, y1, trg_x2, trg_y2);
		}
		if(pS.dtpMap[x1-1][y1] > limit && closedDir != 3)
		{
			deadEnd = false;
			openNodesX[iter][3] = x1-1; 
			openNodesY[iter][3] = y1; 
			fScoresHist[iter][3] = openNodesF[3] = G + myHeuristicDist(x1-1, y1, trg_x2, trg_y2);
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