/**
 *	Author: Ben Cowley, 2006.
 *	Decision Theory functions for predicting player utility
 */
#include "main.h"
#include "universals.h"
#include "dtpm.h"
#include "tree.h"

/**
 *	Function prototypes
 */
//Action sequence functions
void actions( int lookahead );
double sumStates();

//Utility calculation functions
double utility(PacState pState);
double utilityDots(PacState pS);
double utilityPills(PacState pS, int risk);
double utilityGhostHunt(PacState pS, int risk);
double utilityGhosts(PacState pS, int risk);

//Probability of state functions
double probability(PacState pS);
double probabilityOfPac(PacState pS);
bool buildTreeOfGhostMoves(unsigned int recurseLimit, int trg_x, int trg_y, PacState pS);
int countSpawn(Tree<GhostMoves>* tree);
double probabilityOfGhost(int *baseGhostX, int *baseGhostY, int Map[20][20]);
void checkAroundGhost(int dir, int gstNum, int gstNumX, int gstNumY, int count, int Map[20][20]);

/* Utile, 2nd tier functions */
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

/**
 * Entry point function.
 * Initialise all the local data structures to hold the real (ie current state) values
 */
void DTPM()
{
	//Grab the state of the game, initialise PacState field around it.
	S = PacState();
	actions( 1 );
/** /
	logPos(g_ShadowSpritePos, "ShadowXY: ", ";\t");
	logPos(g_SpeedySpritePos, "SpeedyXY: ", ";\t");
	logPos(g_PokeySpritePos, "PokeyXY: ", ";\t");
	logPos(g_BashfulSpritePos, "BashfulXY: ", ";\n");
	logPos(g_PacmanSpritePos, "PacmanXY: ", ";");
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

void actions( int lookahead )
{
	//Start at the start. Next available action for Pacman (then recurse?) -
	//...and return a value for each state possible at each move:
	//	P(Pacman.action)*Sum( P(Ghost[0..3].action)*U(state.features), .., P(Ghost[0..3].action)*U(state.features))
	double aVal = 0.0;
	deque <double> actArr;
	double states = 0.0;

	while( lookahead > 0 )
	{
		lookahead--;
		if( PacMap[xIndex][yIndex - 1] > 2 )
		{
			S.yPac--;
			if( PacMap[xIndex][yIndex - 1] == 4 )
			{
				S.dtpMap[xIndex][yIndex - 1] = 6;
				S.Dots--;
				S.Points += 5;
			}else if( PacMap[xIndex][yIndex - 1] == 5 )
			{
				S.dtpMap[xIndex][yIndex - 1] = 6;
				S.Pills--;
				S.Points += 50;
				S.PacAttack = true;
			}
			actArr.push_back( sumStates() );
		}
		if( PacMap[xIndex][yIndex + 1] > 2 )
		{
			S.yPac++;
			if( PacMap[xIndex][yIndex + 1] == 4 )
			{
				S.dtpMap[xIndex][yIndex + 1] = 6;
				S.Dots--;
				S.Points += 5;
			}else if( PacMap[xIndex][yIndex + 1] == 5 )
			{
				S.dtpMap[xIndex][yIndex + 1] = 6;
				S.Pills--;
				S.Points += 50;
				S.PacAttack = true;
			}
			actArr.push_back( sumStates() );
		}
		if( PacMap[xIndex + 1][yIndex] > 2 )
		{
			S.xPac++;
			if( PacMap[xIndex + 1][yIndex] == 4 )
			{
				S.dtpMap[xIndex + 1][yIndex] = 6;
				S.Dots--;
				S.Points += 5;
			}else if( PacMap[xIndex + 1][yIndex] == 5 )
			{
				S.dtpMap[xIndex + 1][yIndex] = 6;
				S.Pills--;
				S.Points += 50;
				S.PacAttack = true;
			}
			actArr.push_back( sumStates() );
		}
		if( PacMap[xIndex - 1][yIndex] > 2 )
		{
			S.xPac--;
			if( PacMap[xIndex - 1][yIndex] == 4 )
			{
				S.dtpMap[xIndex - 1][yIndex] = 6;
				S.Dots--;
				S.Points += 5;
			}else if( PacMap[xIndex - 1][yIndex] == 5 )
			{
				S.dtpMap[xIndex - 1][yIndex] = 6;
				S.Pills--;
				S.Points += 50;
				S.PacAttack = true;
			}
			actArr.push_back( sumStates() );
		}
		int dir = highest(actArr);
		aVal += actArr[ dir ];
		actArr.clear();
	}
}

double sumStates()
{
	double sum = 0.0;

	// Generate the set of ghost move possibilities here, then calculate utility for each state. Probability follows from GhostMoves.probs
	// Use the code from countSpawn to do above?
	sum += utility(S) + probability(S);

	return sum;
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
println("Ghosts Attack");
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
 * Arguments:	Pacman x and y 
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
 * Arguments:	Pacman x and y 
 *				Ghost 1-4 x and y
 *				risk : a measure of how vulnerable Pacman is (# lives)
 */
double utilityGhosts(PacState pS, int risk)
{
	double util = 0.0;

	//Calculate if Pacman is trapped by ghosts, and how many exits he has if not
//	int locBound = 4;	int locale = localGhosts(xPac, yPac, locBound);		if( locale > 1 ){}

	//The average distance of the ghosts is good if large
	int dist = 0;
	for(int i = 0; i < 4; i++)
		dist += myHeuristicDist(pS.xPac, pS.yPac, pS.xGhost[i], pS.yGhost[i]);
	util += 100.0 - (dist / 4);

	//The distribution size of the ghosts is good if small.
	util += 100.0 / avgDistribution(pS.xGhost, pS.yGhost);

	int temp = a_Star(pS.xPac, pS.yPac, 20-pS.xPac, 20-pS.yPac, true, pS);
	int tempi = 0;
	for(int i = 0; i < 4; i++)
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
 * Arguments:	Pacman x and y 
 *				Ghost 1-4 x and y
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
	int xGhostCentroid = (((pS.xGhost[0] + pS.xGhost[1] + pS.xGhost[2] + pS.xGhost[3]) / 4) + pS.xPac) / 2;
	int yGhostCentroid = (((pS.yGhost[0] + pS.yGhost[1] + pS.yGhost[2] + pS.yGhost[3]) / 4) + pS.yPac) / 2;
	
	//Get the closest pill's x and y
	if(pS.dtpMap[1][3] == 5){
		mDist.push_back( myHeuristicDist(xGhostCentroid, yGhostCentroid, 1, 3) );
		xPill.push_back(1);
		yPill.push_back(3);
	}
	if(pS.dtpMap[18][3] == 5){
		mDist.push_back( myHeuristicDist(xGhostCentroid, yGhostCentroid, 18, 3) );
		xPill.push_back(18);
		yPill.push_back(3);
	}
	if(pS.dtpMap[1][15] == 5){
		mDist.push_back( myHeuristicDist(xGhostCentroid, yGhostCentroid, 1, 15) );
		xPill.push_back(1);
		yPill.push_back(15);
	}
	if(pS.dtpMap[18][15] == 5){
		mDist.push_back( myHeuristicDist(xGhostCentroid, yGhostCentroid, 18, 15) );
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
 * Arguments:	Pacman x and y 
 *				Ghost 1-4 x and y
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
	for(int i = 0; i < 4; i++)
		mDist.push_back( manhattanDist(pS.xPac, pS.yPac, pS.xGhost[i], pS.yGhost[i]) );
	//The index of the lowest ghost manhattan distance measure to Pacman
	chain.push_back(lowest(mDist));

	//...then the closest to the closest ghost (as we assume Pacman moved there to eat him), etc.
	for(int i = 0; i < 3; i++)
	{
		mDist.clear();
		for(int j = 0; j < 4; j++)
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
	for(int i = 1; i < 4; i++)
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
 */
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
 *				trg_y: y value of a location we want to test proximity of ghosts to - enter negative numbers when not targeting
 * Return Value: flag whether the function has executed successfully the whole way through
 */
bool buildTreeOfGhostMoves(unsigned int recurseLimit, int trg_x, int trg_y, PacState pS)
{
	if( trg_x >= 0 && trg_x < 20 && trg_y >= 0 && trg_y < 20 )
	{
		int dists[4];
		for( int i = 0; i < 4; i++ )
			dists[i] = manhattanDist(trg_x, trg_y, pS.xGhost[i], pS.yGhost[i]);
		recurseLimit = dists[lowest( dists, 4 )];
	}
	//This is the first iteration of possible ghost move positions and accompanying probability:probs
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
	if(!Preorder( tree_GstMv, countSpawn, recurseLimit ))
		return false;

	tree_GstMv->Destroy();
	S = tempPS;

	return true;
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
if(gM.probs > 0)
{
//print("\nNEW CHILDREN BEING GENERATED\n");
	for( int count = 0; count < 256; count++ )
	{
		if( gM.ghostXYMove[0][0][count] > NOMV )//&& gM.ghostXYMove[0][1][count] > NOMV )
		{
			int ghostBranchX[4] = {gM.ghostXYMove[0][0][count], gM.ghostXYMove[1][0][count], gM.ghostXYMove[2][0][count], gM.ghostXYMove[3][0][count]};
			int ghostBranchY[4] = {gM.ghostXYMove[0][1][count], gM.ghostXYMove[1][1][count], gM.ghostXYMove[2][1][count], gM.ghostXYMove[3][1][count]};
			probabilityOfGhost( ghostBranchX, ghostBranchY, S.dtpMap );
			Tree <GhostMoves>* node = new Tree<GhostMoves>( GhostMove );
			// New nodes are appended to tree via global iterator which has been reset via Tree argument
			iter_GstMv.AppendChild( node );
			inc++;

/* Print call for test run situations - can't use in normal run, would produce far too much output */
sprintf_s(out, 99, "%d", inc); print("Child #"); print(out); print("\tProbs#: "); sprintf_s(out, 99, "%d", GhostMove.probs); println(out);
//printGhostMoves();
/**/
		}
	}
}
else
	print("\nEMPTY NODE - NO CHILDREN\n");

	return inc; //tree_GstMv->m_children.m_count;
}

/**
 * Description:		Discover all possible combinations of ghost movement, and the corresponding probability
 * Arguments:		The arrays of ghost's x and y values
 * Return Value:	The reciprocal of the number of ghost move combinations
 */
double probabilityOfGhost(int *baseGhostX, int *baseGhostY /*[Ghost 1-4][x-y]*/, int Map[20][20])
{
	//256 possible states given any distinct PacAction (since we have 4 ghosts with 4 poss move dirs each = 4^4)

	for(int p1 = 0; p1 < 4; p1++)
		for(int p2 = 0; p2 < 256; p2++)
		{
			GhostMove.ghostXYMove[p1][0][p2] = baseGhostX[p1];
			GhostMove.ghostXYMove[p1][1][p2] = baseGhostY[p1];
		}
	
	int count = -1;
	for(int g1 = 0; g1 < 4; g1++)
	for(int g2 = 0; g2 < 4; g2++)
	for(int g3 = 0; g3 < 4; g3++)
	for(int g4 = 0; g4 < 4; g4++)
	{
		count++;
		//Check around ghost 0
		checkAroundGhost(g1, 0, baseGhostX[0], baseGhostY[0], count, Map);
		//Check all around 2nd Ghost
		checkAroundGhost(g2, 1, baseGhostX[1], baseGhostY[1], count, Map);
		//Check all around 3rd Ghost
		checkAroundGhost(g3, 2, baseGhostX[2], baseGhostY[2], count, Map);
		//Check all around 4th Ghost
		checkAroundGhost(g4, 3, baseGhostX[3], baseGhostY[3], count, Map);
	}
/* Limit the recursive tree building by filling in each ghost position on the temporary map with a wall * /
	pS.dtpMap[baseGhostX[0]][baseGhostY[0]] = 1;
	pS.dtpMap[baseGhostX[1]][baseGhostY[1]] = 1;
	pS.dtpMap[baseGhostX[2]][baseGhostY[2]] = 1;
	pS.dtpMap[baseGhostX[3]][baseGhostY[3]] = 1;
/**/
	//GhostMove.probs = 
	GhostMove.size();

	return GhostMove.probs;
}

/**
 * Description:	Checks vicinity of each ghost and sets the ghost move matrix details accordingly.
 * Arguments:	Direction to check, number of ghost to check, ghosts x and y index (allows speculative calculations)...
 *				count: the number of the ghost move possibility being filled in.
 */
void checkAroundGhost(int dir, int gstNum, int gstNumX, int gstNumY, int count, int Map[20][20])
{
	switch(dir)
	{
	case 0:
		if( Map[gstNumX][gstNumY - 1 ] == 1 )
			GhostMove.ghostXYMove[0][0][count] = GhostMove.ghostXYMove[0][1][count] = NOMV;
		else
			GhostMove.ghostXYMove[gstNum][1][count]--;
		break;
	case 1:
		if( Map[gstNumX][gstNumY + 1 ] == 1 )
			GhostMove.ghostXYMove[0][0][count] = GhostMove.ghostXYMove[0][1][count] = NOMV;
		else
			GhostMove.ghostXYMove[gstNum][1][count]++;
		break;
	case 2:
		if( Map[gstNumX + 1 ][gstNumY] == 1 )
			GhostMove.ghostXYMove[0][0][count] = GhostMove.ghostXYMove[0][1][count] = NOMV;
		else
			GhostMove.ghostXYMove[gstNum][0][count]++;
		break;
	case 3:
		if( Map[gstNumX - 1 ][gstNumY] == 1 )
			GhostMove.ghostXYMove[0][0][count] = GhostMove.ghostXYMove[0][1][count] = NOMV;
		else
			GhostMove.ghostXYMove[gstNum][0][count]--;
		break;
	default:
		break;
	}
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
	for(int i = 0; i < 4; i++)
		if(myHeuristicDist(pS.xGhost[i], pS.yGhost[i], x, y) <= radius)
			ghosts++;
	return ghosts;
}

/**
 * Description:	Implement the A* pathfinding algorithm - returns number of moves in path. Stores path in global deques aPathX and aPathY
 * Arguments:	The source x,y and the target x,y
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

sprintf_s(out, 99, "%d", iter); print("\nWhile Loop iter's: "); println(out);
	iter = (int)aPathX.size();
/**/
	print("Source: ");
	sprintf_s(out, 99, "%d", src_x1); print(out); print(", ");
	sprintf_s(out, 99, "%d", src_y1); print(out); print("\n");
/**/
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
/**/
	print("Target: ");
	sprintf_s(out, 99, "%d", trg_x2); print(out); print(", ");
	sprintf_s(out, 99, "%d", trg_y2); println(out);
/**/
	return iter;
}

/**
 * Description: Will print out the contents of the global GhostMoves struct in 16x16 matrix form, with entries as either 0 or ghost[1-4].x.y
 */
void printGhostMoves()
{
	unsigned int count = -1;
	//Can use this to log the matrix of ghost moves and associated probabilities
	print("\n#P(S): 1/"); sprintf_s(out, 99, "%d", GhostMove.probs); print(out);
	print("\t3:0, 4:0\t\t\t\t3:0, 4:1\t\t\t\t3:0, 4:2\t\t\t\t3:0, 4:3\t\t\t\t3:1, 4:0\t\t\t\t3:1, 4:1\t\t\t\t3:1, 4:2\t\t\t\t3:1, 4:3\t\t\t\t3:2, 4:0\t\t\t\t3:2, 4:1\t\t\t\t3:2, 4:2\t\t\t\t3:2, 4:3\t\t\t\t3:3, 4:0\t\t\t\t3:3, 4:1\t\t\t\t3:3, 4:2\t\t\t\t3:3, 4:3\n");
	print("-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------");

	for(int p1 = 0; p1 < 4; p1++)
	for(int p2 = 0; p2 < 4; p2++)
	{
		print("\n0:"); sprintf_s(out, 99, "%d", p1); print(out); print(", 1:"); sprintf_s(out, 99, "%d", p2); print(out); print("|\t");
	for(int p3 = 0; p3 < 4; p3++)
	for(int p4 = 0; p4 < 4; p4++)
	{
		count++;
		if(GhostMove.ghostXYMove[0][0][count] <= NOMV && GhostMove.ghostXYMove[0][1][count] <= NOMV)
			print("-\t\t\t\t\t");
		else
		{
			for(int g2 = 0; g2 < 4; g2++)
			{
				print("G"); sprintf_s(out, 99, "%d", g2); print(out); print(":");
				for(int g3 = 0; g3 < 2; g3++)
				{
					sprintf_s(out, 99, "%d", GhostMove.ghostXYMove[g2][g3][count]); 
					if(GhostMove.ghostXYMove[g2][g3][count] < 10) print(" "); //Adjust spacing between 1 and 2 digit numbers
					print(out); print(",");
				}
			}
			print("\t");
		}
	}}
	print("\n\n");
}