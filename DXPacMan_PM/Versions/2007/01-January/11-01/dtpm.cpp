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

void actions();

double utility();
double utilityDots();
double utilityPills();
double utilityGhosts(int xPac, int yPac, int *xGhost, int *yGhost /*[Ghost 1-4][x-y]*/);
int utilityOfState();

double probability();

double probabilityOfPac(int xPacIndex, int yPacIndex);

bool buildTreeOfGhostMoves(unsigned int recurseLimit, int trg_x, int trg_y);
short countSpawn(Tree<GhostMoves>* tree);
double probabilityOfGhost(int *baseGhostX, int *baseGhostY);
void checkAroundGhost(int dir, int gstNum, int gstNumX, int gstNumY, int count);

/* Utile, 2nd tier functions */
int localDots(int x, int y, int radius);
int localGhosts(int x, int y, int radius);

int a_Star(int src_x1, int src_y1, int trg_x2, int trg_y2);
double lineDist(int x1, int y1, int x2, int y2);
int manhattanDist(int x1, int y1, int x2, int y2);
int myHeuristicDist(int x1, int y1, int x2, int y2);
double avgDistGhosts(int *xGhost, int *yGhost);

void printGhostMoves();

/**
 *	Global fields.
 */
int dtpMap[20][20];
deque <int> aPathX;
deque <int> aPathY;

GhostMoves GhostMove;
Tree <GhostMoves>* tree_GstMv;
TreeIterator <GhostMoves> iter_GstMv;

deque <GhostMoves> dq_GhostMove;

//Ghost 1-4 Positions, X[i] == i.x, Y[i] == i.y
int *ghostIndicesX = new int[4];
int *ghostIndicesY = new int[4];

/**
 * Entry point function.
 * Initialise all the local data structures to hold the real (ie current state) values
 */
void DTPM()
{
//	print("\n\n");
/** /
	logPos(g_ShadowSpritePos, "ShadowXY: ", ";\t");
	logPos(g_SpeedySpritePos, "SpeedyXY: ", ";\t");
	logPos(g_PokeySpritePos, "PokeyXY: ", ";\t");
	logPos(g_BashfulSpritePos, "BashfulXY: ", ";\n");
/**/
//	logPos(g_PacmanSpritePos, "PacmanXY: ", ";");

	//Update array of Ghost position xy's
	getSpritePos(g_ShadowSpritePos);
	ghostIndicesX[0] = xIndex; ghostIndicesY[0] = yIndex;
	getSpritePos(g_SpeedySpritePos);
	ghostIndicesX[1] = xIndex; ghostIndicesY[1] = yIndex;
	getSpritePos(g_PokeySpritePos);
	ghostIndicesX[2] = xIndex; ghostIndicesY[2] = yIndex;
	getSpritePos(g_BashfulSpritePos);
	ghostIndicesX[3] = xIndex; ghostIndicesY[3] = yIndex;

	getSpritePos(g_PacmanSpritePos);

	for(int i = 0; i < 20; i++)
		for(int j = 0; j < 20; j++)
            dtpMap[i][j] = PacMap[i][j];

	itoa(manhattanDist(ghostIndicesX[0], ghostIndicesY[0], ghostIndicesX[1], ghostIndicesY[1]), out, 10);
	cout << "[0] to [1]: " << out << endl;
	itoa(manhattanDist(ghostIndicesX[1], ghostIndicesY[1], ghostIndicesX[2], ghostIndicesY[2]), out, 10);
	cout << "[1] to [2]: " << out << endl;
	itoa(manhattanDist(ghostIndicesX[2], ghostIndicesY[2], ghostIndicesX[3], ghostIndicesY[3]), out, 10);
	cout << "[2] to [3]: " << out << endl;
	itoa(manhattanDist(ghostIndicesX[3], ghostIndicesY[3], ghostIndicesX[0], ghostIndicesY[0]), out, 10);
	cout << "[3] to [0]: " << out << endl;
/*
	gcvt(lineDist(ghostIndicesX[0], ghostIndicesY[0], ghostIndicesX[1], ghostIndicesY[1]), 98, out);
	cout << "[0] to [1]: " << out << endl;
	gcvt(lineDist(ghostIndicesX[1], ghostIndicesY[1], ghostIndicesX[2], ghostIndicesY[2]), 98, out);
	cout << "[1] to [2]: " << out << endl;
	gcvt(lineDist(ghostIndicesX[2], ghostIndicesY[2], ghostIndicesX[3], ghostIndicesY[3]), 98, out);
	cout << "[2] to [3]: " << out << endl;
	gcvt(lineDist(ghostIndicesX[3], ghostIndicesY[3], ghostIndicesX[0], ghostIndicesY[0]), 98, out);
	cout << "[3] to [0]: " << out << endl;
*/
	/*gcvt(lineDist(ghostIndicesX[0], ghostIndicesY[0], ghostIndicesX[1], ghostIndicesY[1]) + 
	lineDist(ghostIndicesX[1], ghostIndicesY[1], ghostIndicesX[2], ghostIndicesY[2]) +
	lineDist(ghostIndicesX[2], ghostIndicesY[2], ghostIndicesX[3], ghostIndicesY[3]) + 
	lineDist(ghostIndicesX[3], ghostIndicesY[3], ghostIndicesX[0], ghostIndicesY[0]), 10, out);
	cout << "Order 1: " << out << endl;
	gcvt(lineDist(ghostIndicesX[0], ghostIndicesY[0], ghostIndicesX[2], ghostIndicesY[2]) + 
	lineDist(ghostIndicesX[2], ghostIndicesY[2], ghostIndicesX[3], ghostIndicesY[3]) +
	lineDist(ghostIndicesX[3], ghostIndicesY[3], ghostIndicesX[1], ghostIndicesY[1]) +
	lineDist(ghostIndicesX[1], ghostIndicesY[1], ghostIndicesX[0], ghostIndicesY[0]), 10, out);
	cout << "Order 2: " << out << endl;
	gcvt(lineDist(ghostIndicesX[0], ghostIndicesY[0], ghostIndicesX[3], ghostIndicesY[3]) + 
	lineDist(ghostIndicesX[1], ghostIndicesY[1], ghostIndicesX[3], ghostIndicesY[3]) +
	lineDist(ghostIndicesX[2], ghostIndicesY[2], ghostIndicesX[1], ghostIndicesY[1]) + 
	lineDist(ghostIndicesX[2], ghostIndicesY[2], ghostIndicesX[0], ghostIndicesY[0]), 10, out);
	cout << "Order 3: " << out << endl;*/
//TEMPORARY: Test the build tree of ghost movements function
//buildTreeOfGhostMoves(2, -1, -1);
}

//Decision Theory logic: look at available actions, possible states under each action, and utility of each state.
//Actions available are decided by the current state. Future states depend on actions and ghost movement.

/*	Utility of a state is relative to current state - what points/-+lives Pacman gets in the intervening time...
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

void actions()
{
	//Start at the start. Next available action for Pacman -
	//then recurse? 
	//...and return a value for each state possible at each move:
	//	P(Pacman.action)*Sum( P(Ghost[0..3].action)*U(state.features), .., P(Ghost[0..3].action)*U(state.features))

}

/**
 * Utility of a state is a measure of the best possible end result that can be obtained from that state - i.e. we assume an optimally played game
 */
double utility()
{
	//Weight a feature set polynomial or something similar
	double utility = 0.0;
	//Weight the state of the dots
	utility += utilityDots();
	//Weight the state of the pills
	utility += utilityPills();
	//Weight the state of the ghosts
	utility += utilityGhosts(xIndex, yIndex, ghostIndicesX, ghostIndicesY);

	return utility;
}

double utilityDots()
{
	double util = 0.0;
	//Calculate the utility of each dot left on the map.
	for(int i = 0; i < 20; i++)
		for(int j = 0; j < 20; j++)
		{
			if(PacMap[i][j] == 4)
			{
				//Inverse distribution density
				localDots(i, j, 1);
			}
		}
	util = NumDots * 5;
	return util;
}

double utilityPills()
{
	double util = 0.0;
	util = NumPills * 50;
	return util;
}

double utilityGhosts(int xPac, int yPac, int *xGhost, int *yGhost /*[Ghost 1-4][x-y]*/)
{
	double util = 0.0;
	//Calculate if Pacman is trapped by ghosts, and how many exits he has if not
	int locale = localGhosts(xPac, yPac, 4);
	if( locale > 1 )
	{
		int temp = a_Star(xPac, yPac, 20-xPac, 20-yPac);
		
		for(int i = 0; i < 4; i++)
			for(int e = 0; e < aPathX.size(); e++)
			{
				if(xGhost[i] == aPathX[e] && yGhost[i] == aPathY[e])
					dtpMap[xGhost[i]][yGhost[i]] = 1;
			}
		if( a_Star(xPac, yPac, 20-xPac, 20-yPac) < 4 )
            return 0.0; //no way out! Check this
	}else
		util += 1 / avgDistGhosts(xGhost, yGhost);

	for(int i = 0; i < 4; i++)
	{
		for(int l = 0; l < 20; l++)
		{

		}
		if(xGhost[i] == xPac)
		{	//We check if there is an opening 

		}
		else if(yGhost[i] == yPac)
		{}
	}
	return util;
}

//Calculates the utility value inherent in the elements of the current state
int utilityOfState()
{
	//Get lives left
	int lives = GameLives;
	//Get points value left
	int points = 0;
	if(PacMap[1][3] == 5)
		points ++;
	if(PacMap[18][3] == 5)
		points ++;
	if(PacMap[1][15] == 5)
		points ++;
	if(PacMap[18][15] == 5)
		points ++;

	//Risk is measured here in how many lives Pacman could lose and how many he has to spare
	//Do we want the cherry to give extra life? or stick with original game system?
	int risk = 100;
	if(randCherryInit == INT_MAX && CherryEaten)
	{
		if(GameLives == 6)
		{
			//Risk is minimal - we have the extra life
			risk = 0;
		}else if(GameLives < 6)
		{
			//Now we've started eating into our buffer of lives - we must be careful to avoid danger levels
		}
	}

	points *= 445;
	points += NumDots * 5;

	return points;
}

/**
 * Returns the probability of the next state.
 */
double probability()
{
	double POneStepPac = probabilityOfPac(xIndex, yIndex);
	//Prob of the Pacman action and all the Ghosts' actions, for the current Pacman state and one iteration.
	return POneStepPac + 1/probabilityOfGhost(ghostIndicesX, ghostIndicesY);
}

/**
 * Dscription:	Probability of Pacman's move. Returns the reciprocal of the number of moves open to Pacman
 * Arguments:	xPacIndex: x index of pacman, allows speculative calculations
 *				yPacIndex: y index of pacman, allows speculative calculations
 */
double probabilityOfPac(int xPacIndex, int yPacIndex)
{
	int openDirs = 0;
	//Check all around Pacman
	if( PacMap[xPacIndex][yPacIndex - 1] > 2 )
		openDirs++;
	if( PacMap[xPacIndex][yPacIndex + 1] > 2 )
		openDirs++;
	if( PacMap[xPacIndex + 1][yPacIndex] > 2 )
		openDirs++;
	if( PacMap[xPacIndex - 1][yPacIndex] > 2 )
		openDirs++;

	return 1/openDirs;
}
/**
 * Description: Builds the tree of the GhostMoves structs that hold all the possible moves of the ghosts and associated probabilities.
 * Arguments:	recurseLimit: depth to which to build the tree
 *				trg_x: x value of a location we want to test proximity of ghosts to - enter negative numbers when not targeting
 *				trg_y: y value of a location we want to test proximity of ghosts to
 * Return Value: flag whether the function has executed successfully the whole way through
 */
bool buildTreeOfGhostMoves(unsigned int recurseLimit, int trg_x, int trg_y)
{
	if( trg_x >= 0 && trg_y >= 0 )
	{
		int dists[4];
		for( int i = 0; i < 4; i++ )
			dists[i] = manhattanDist(trg_x, trg_y, ghostIndicesX[i], ghostIndicesY[i]);
		recurseLimit = dists[lowest( dists, 4 )];
	}

	//Instantiate the global tree and its iterator
	tree_GstMv = new Tree<GhostMoves>;
	iter_GstMv = tree_GstMv;

	//This is the first iteration of possible ghost move positions and accompanying probability:probs
	probabilityOfGhost( ghostIndicesX, ghostIndicesY );
	
	//Create the first node from the global GhostMoves struct and append to global tree
	iter_GstMv.m_node->m_data = GhostMove;

//Test run print output
print("\n\n******BEGIN TREE!!********\n\n");
print("\nPARENT NODE:\n");
printGhostMoves();
	//Now we must recurse - create children, go down one to their level, create children for each of them, etc
	if(!Preorder( tree_GstMv, countSpawn, recurseLimit ))
		return false;

	return true;
}

/**
 * Description:		Take the GhostMoves struct from the primary node of the passed Tree, and spawn a list of children
 * Return Value:	An increment counter i.e. if(){for(){if(){i++}}}, or 0 if iter.Valid() == false
 * Arguments:		Sub-Tree that points to the current node we want to grow
 */
short countSpawn(Tree<GhostMoves>* tree)
{
	short inc = 0;

	GhostMoves gM = tree->m_data;

//Print-Mark a run of generating child GhostMoves - parent first
if(gM.probs > 0)
	print("\n\nNEW CHILDREN BEING GENERATED\n");
else
	print("\nEMPTY NODE - NO CHILDREN\n");

	for( int count = 0; count < 256; count++ )
	{
		if( gM.ghostXYMove[0][0][count] > NOMV && gM.ghostXYMove[0][1][count] > NOMV )
		{

			int ghostBranchX[4] = {gM.ghostXYMove[0][0][count], gM.ghostXYMove[1][0][count], gM.ghostXYMove[2][0][count], gM.ghostXYMove[3][0][count]};
			int ghostBranchY[4] = {gM.ghostXYMove[0][1][count], gM.ghostXYMove[1][1][count], gM.ghostXYMove[2][1][count], gM.ghostXYMove[3][1][count]};
			probabilityOfGhost( ghostBranchX, ghostBranchY );
			Tree <GhostMoves>* node = new Tree<GhostMoves>;
			node->m_data = GhostMove;
			// Global Tree gets new nodes just appended to local iterator
			iter_GstMv.AppendChild( node );
			inc++;

//Print call for test run situations - can't use in normal run, would produce far too much output
itoa(inc, out, 10); print("Child #"); print(out); print("\n");
printGhostMoves();
		}
	}

	return inc; //tree_GstMv->m_children.m_count;
}

/**
 * Description:		Discover all possible combinations of ghost movement, and the corresponding probability
 * Arguments:		The arrays of ghost's x and y values
 * Return Value:	The reciprocal of the number of ghost move combinations
 */
double probabilityOfGhost(int *baseGhostX, int *baseGhostY /*[Ghost 1-4][x-y]*/)
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
		checkAroundGhost(g1, 0, baseGhostX[0], baseGhostY[0], count);
		//Check all around 2nd Ghost
		checkAroundGhost(g2, 1, baseGhostX[1], baseGhostY[1], count);
		//Check all around 3rd Ghost
		checkAroundGhost(g3, 2, baseGhostX[2], baseGhostY[2], count);
		//Check all around 4th Ghost
		checkAroundGhost(g4, 3, baseGhostX[3], baseGhostY[3], count);
	}
/* Limit the recursive tree building by filling in each ghost position on the temporary map with a wall */
	dtpMap[baseGhostX[0]][baseGhostY[0]] = 1;
	dtpMap[baseGhostX[1]][baseGhostY[1]] = 1;
	dtpMap[baseGhostX[2]][baseGhostY[2]] = 1;
	dtpMap[baseGhostX[3]][baseGhostY[3]] = 1;
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
void checkAroundGhost(int dir, int gstNum, int gstNumX, int gstNumY, int count)
{
	switch(dir)
	{
	case 0:
		if( dtpMap[gstNumX][gstNumY - 1 ] == 1 )
			GhostMove.ghostXYMove[0][0][count] = GhostMove.ghostXYMove[0][1][count] = NOMV;
		else
			GhostMove.ghostXYMove[gstNum][1][count]--;
		break;
	case 1:
		if( dtpMap[gstNumX][gstNumY + 1 ] == 1 )
			GhostMove.ghostXYMove[0][0][count] = GhostMove.ghostXYMove[0][1][count] = NOMV;
		else
			GhostMove.ghostXYMove[gstNum][1][count]++;
		break;
	case 2:
		if( dtpMap[gstNumX + 1 ][gstNumY] == 1 )
			GhostMove.ghostXYMove[0][0][count] = GhostMove.ghostXYMove[0][1][count] = NOMV;
		else
			GhostMove.ghostXYMove[gstNum][0][count]++;
		break;
	case 3:
		if( dtpMap[gstNumX - 1 ][gstNumY] == 1 )
			GhostMove.ghostXYMove[0][0][count] = GhostMove.ghostXYMove[0][1][count] = NOMV;
		else
			GhostMove.ghostXYMove[gstNum][0][count]--;
		break;
	default:
		break;
	}
}

/**
 * Description: function that counts the number of dots within a given radius of an x,y coord
 */
int localDots(int xCentre, int yCentre, int radius)
{
	int dots = 0;
	
	int x = 0, y = 0;
	if(xCentre - radius > 0)
		x = xCentre - radius;
	if(yCentre - radius > 0)
		y = yCentre - radius;

	if(xCentre + radius > 19)
		xCentre = 19;
	if(yCentre + radius > 19)
		yCentre = 19;
	
	while(x <= xCentre)
	{
		x++;
		while(y <= yCentre)
		{
			y++;
			if(PacMap[x][y] == 4)
				dots++;
		}
	}
	return dots;
}

/**
 * Description: function that counts the number of ghosts within a given radius of an x,y coord
 */
int localGhosts(int x, int y, int radius)
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
		if(myHeuristicDist(ghostIndicesX[i], ghostIndicesY[i], x, y) <= radius)
			ghosts++;
	return ghosts;
}

/**
 * Description:	Implement the A* pathfinding algorithm - returns number of moves in path. Stores path in global deques aPathX and aPathY
 * Arguments:	The source x,y and the target x,y
 */
int a_Star(int src_x1, int src_y1, int trg_x2, int trg_y2)
{
	if(src_x1 < 0) src_x1 = 1;
	if(src_y1 < 0) src_y1 = 1;
	if(trg_x2 < 0) trg_x2 = 1;
	if(trg_y2 < 0) trg_y2 = 1;
	if(src_x1 > 19) src_x1 = 18;
	if(src_y1 > 19) src_y1 = 18;
	if(trg_x2 > 19) trg_x2 = 18;
	if(trg_y2 > 19) trg_y2 = 18;
	if((src_x1 == trg_x2 && src_y1 == trg_y2) || PacMap[src_x1][src_y1] <= 2 || PacMap[trg_x2][trg_y2] <= 2)
		return 99;
/** /
	//Alter some data for local use
	int temp1[5] = {0};
	int temp2[5] = {0};
	for(int i = 0; i < 5; i++)
	{
		temp1[i] = PacMap[i][9]; PacMap[i][9] = 2;
		temp2[i] = PacMap[i+15][9]; PacMap[i+15][9] = 2;
	}
//	PacMap[9][8] = PacMap[9][9] = PacMap[10][8] = PacMap[10][9] = 4;
/**/
	//Initialisations
	int x1 = src_x1; 
	int y1 = src_y1;
	aPathX.clear();
	aPathY.clear();

	//List of directions, starting from x1, y1
	int path[87];
	int closedNodesX[87] = {0};
	int closedNodesY[87] = {0};
	int openNodesX[87][4] = {0};	//four directions X
	int openNodesY[87][4] = {0};	//four directions Y
	int openNodesF[4];	//four Final scores = G + H

	int fScoresHist[87][4] = {0};
	for(int i = 0; i < 87; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			fScoresHist[i][j] = 99;
		}
	}
	
	int iter = 0;
	int dir = -1, closedDir = -1;
	int G = 1;//, F = 0, H = 0, currNode = 0;
	int tempTrgX = -1;
/**/
	//Some functionality to handle the teleporters
	if(src_y1 == 9)
	{
		if(src_x1 < 5)
		{
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
		}else if(src_x1 > 14)
		{
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
		}
	}
/**/
	//And for the target being near teleporter
	if(trg_y2 == 9)
	{
		if(trg_x2 < 5 && src_x1 > 12)
		{
			tempTrgX = trg_x2;
			trg_x2 = 19;
		}else if(trg_x2 > 14 && src_x1 < 7)
		{
			tempTrgX = trg_x2;
			trg_x2 = 0;
		}
	}
/**/
	//Check our path and update path[]
	while(iter < 87)
	{

		openNodesF[0] = 99, openNodesF[1] = 99, openNodesF[2] = 99, openNodesF[3] = 99;
		//Seal off the way we came from investigation.
		closedNodesX[iter] = x1;
		closedNodesY[iter] = y1;

		//Find available directions and calculate pathfinding indices for each dir
		if(PacMap[x1][y1-1] > 2 && closedDir != 0)
		{
			openNodesX[iter][0] = x1; 
			openNodesY[iter][0] = y1-1;
			fScoresHist[iter][0] = openNodesF[0] = G + myHeuristicDist(x1, y1-1, trg_x2, trg_y2);
		}
		if(PacMap[x1][y1+1] > 2 && closedDir != 1)
		{
			openNodesX[iter][1] = x1; 
			openNodesY[iter][1] = y1+1;  
			fScoresHist[iter][1] = openNodesF[1] = G + myHeuristicDist(x1, y1+1, trg_x2, trg_y2); 
		}
		if(PacMap[x1+1][y1] > 2 && closedDir != 2)
		{
			openNodesX[iter][2] = x1+1; 
			openNodesY[iter][2] = y1;  
			fScoresHist[iter][2] = openNodesF[2] = G + myHeuristicDist(x1+1, y1, trg_x2, trg_y2);
		}
		if(PacMap[x1-1][y1] > 2 && closedDir != 3)
		{
			openNodesX[iter][3] = x1-1; 
			openNodesY[iter][3] = y1; 
			fScoresHist[iter][3] = openNodesF[3] = G + myHeuristicDist(x1-1, y1, trg_x2, trg_y2);
		}
/**/
        //Now look at all the F scores for the smallest one...if indices are (i, j) then node to pick is:
		//x1 = openNodesX[i][j], y1 = openNodesY[i][j]
		//dir will be equal to j, closedDir its opposite

		/*Need to add parents array:
		@ iter, parent of X,Y is closedDir or something
		at end, have aPathX[end] = trg_x, aPathX[end-1] = trg_x.parent, ...
		In other words aPathX[end] = closedNodesX[end].parent, aPathX[end-1] = closedNodesX[end].parent, ...
		That should give a seamless path...
		*/
//itoa(lowest(fScoresHist[iter], 4), out, 10); print("\ndir of F score @ iter: "); print(out);
//itoa(fScoresHist[iter][lowest(fScoresHist[iter], 4)], out, 10); print("\nlowest F score @ iter: "); print(out);
		
		deque<int> indexArr;
		deque<int> valueArr;
		int temp = 0;

		for(int i = 0; i <= iter; i++)
		{
			int t = lowest(fScoresHist[i], 4);
			indexArr.push_back(t);
			valueArr.push_back(fScoresHist[i][t]);
		}

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
{	itoa(fScoresHist[temp][dir], out, 10); print("\nlowest f scores hist: "); print(out);
}else if(fScoresHist[temp][dir] == openNodesF[lowest(openNodesF, 4)])
{	itoa(fScoresHist[temp][dir], out, 10); print("\nequal f scores: "); print(out);
}else{
	itoa(openNodesF[lowest(openNodesF, 4)], out, 10); print("\nlowest F from openNodes: "); print(out);
}
*/
		//Put the chosen node on the closed lists
		fScoresHist[temp][dir] = 99;
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
				if(PacMap[closedNodesX[i]][closedNodesY[i] - 1] > 2 && ((closedNodesX[i] != closedNodesX[i-1] && closedNodesY[i] - 1 != closedNodesY[i-1]) || (closedNodesX[i] != closedNodesX[i+1] && closedNodesY[i] - 1 != closedNodesY[i+1])))
				{
					x1 = closedNodesX[i];
					y1 = closedNodesY[i] - 1;
					G = iter = i; G++;
					closedDir = 1;
					break;
				}
				if(PacMap[closedNodesX[i]][closedNodesY[i] + 1] > 2 && ((closedNodesX[i] != closedNodesX[i-1] && closedNodesY[i] + 1 != closedNodesY[i-1]) || (closedNodesX[i] != closedNodesX[i+1] && closedNodesY[i] + 1 != closedNodesY[i+1])))
				{
					x1 = closedNodesX[i];
					y1 = closedNodesY[i] + 1;
					G = iter = i; G++;
					closedDir = 0;
					break;
				}
				if(PacMap[closedNodesX[i] + 1][closedNodesY[i]] > 2 && ((closedNodesX[i] + 1 != closedNodesX[i-1] && closedNodesY[i] != closedNodesY[i-1]) || (closedNodesX[i] + 1 != closedNodesX[i+1] && closedNodesY[i] != closedNodesY[i+1])))
				{
					x1 = closedNodesX[i] + 1;
					y1 = closedNodesY[i];
					G = iter = i; G++;
					closedDir = 3;
					break;
				}
				if(PacMap[closedNodesX[i] - 1][closedNodesY[i]] > 2 && ((closedNodesX[i] - 1 != closedNodesX[i-1] && closedNodesY[i] != closedNodesY[i-1]) || (closedNodesX[i] - 1 != closedNodesX[i+1] && closedNodesY[i] != closedNodesY[i+1])))
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

		if(x1 == trg_x2 && y1 == trg_y2)
			break;
	}

	//Cut out any loops that have appeared in the closedNodes arrays.
/**/	for(int pt = 0; pt < iter-1; pt++)
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
		{
			for(int i = 0; i < tempTrgX; i++)
			{
				closedNodesX[iter] = i;
				closedNodesY[iter] = 9;
				iter++;
			}
		}else if(tempTrgX > 14)
		{
			for(int i = 19; i > tempTrgX; i--)
			{
				closedNodesX[iter] = i;
				closedNodesY[iter] = 9;
				iter++;
			}
		}
	}
	
	//When we have our path, iterate back from the target and push down the parent of each path node.
	aPathX.push_front(trg_x2);
	aPathY.push_front(trg_y2);
	for(int i = iter; i >= 0; i--)
	{
		if(manhattanDist(aPathX.front(), aPathY.front(), closedNodesX[i], closedNodesY[i]) == 1)
		{
			aPathX.push_front(closedNodesX[i]);
			aPathY.push_front(closedNodesY[i]);
		}
	}

	iter = (int)aPathX.size();
/** /
	print("\nSource: ");
	itoa(src_x1, out, 10); print(out); print(", ");
	itoa(src_y1, out, 10); print(out); print("\n");
/** /
	for(int i = 0; i < iter; i++)
	{
		print("Iter: ");
		itoa(i, out, 10); print(out); print(";\t");
		itoa(aPathX[i], out, 10); print(out); print(", ");
		itoa(aPathY[i], out, 10); print(out); print(";\t");
		itoa(path[i], out, 10); print(out); print("\n");
	}
/** /
	print("Target: ");
	itoa(trg_x2, out, 10); print(out); print(", ");
	itoa(trg_y2, out, 10); print(out);
/** /
	//Readjust Map
//	PacMap[4][9] = temp1; PacMap[15][9] = temp2;
	for(int i = 0; i < 4; i++)
	{
		PacMap[i][9] = temp1[i];
		i += 15;
		PacMap[i][9] = temp2[i-15];
		i -= 15;
	}
//	PacMap[9][8] = PacMap[9][9] = PacMap[10][8] = PacMap[10][9] = 2;
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
 * Description: Heuristic distance counts the number of open and closed squares along four paths - left & right L's, and left & right biased diagonals
	Test loop for myHeuristicDist;
	for(int i = 0; i < 20; i++)
	{
		for(int j = 0; j < 20; j++)
		{
			itoa(myHeuristicDist(i, j, 10, 11), out, 10); print("\nHeuristic dist: "); print(out);
		}
	}
*/
int myHeuristicDist(int x1, int y1, int x2, int y2)
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
		if(PacMap[x][y1] < 3)
			count[0]++;
	}
	for(y = y1; y != y2; y+=opy)
	{
		if(PacMap[x][y] < 3)
			count[0]++;
	}

	for(y = y1; y != y2; y+=opy)
	{
		if(PacMap[x1][y] < 3)
			count[1]++;
	}
	for(x = x1; x != x2; x+=opx)
	{
		if(PacMap[x][y] < 3)
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
			if(PacMap[x][y] < 3)
				count[2]++;
			x += opx;
			if(PacMap[x][y] < 3)
				count[2]++;
			y += opy;
		}
		if(x == x2)
		{
			while(y != y2)
			{
				if(PacMap[x][y] < 3)
					count[2]++;
				y += opy;
			}
		}else if(y == y2)
		{
			while(x != x2)
			{
				if(PacMap[x][y] < 3)
					count[2]++;
				x += opx;
			}
		}
		//Alternating
		x = x1;
		y = y1;
		while(x != x2 && y != y2)
		{
			if(PacMap[x][y] < 3)
				count[3]++;
			y += opy;
			if(PacMap[x][y] < 3)
				count[3]++;
			x += opx;
		}
		if(x == x2)
		{
			while(y != y2)
			{
				if(PacMap[x][y] < 3)
					count[3]++;
				y += opy;
			}
		}else if(y == y2)
		{
			while(x != x2)
			{
				if(PacMap[x][y] < 3)
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

/**
 * Description: returns a value for the distribution of the four ghosts
 */
double avgDistGhosts(int *xGhost, int *yGhost)
{
	//centroid
	int Xes = 0, Yes = 0;
	for(int i = 0; i < 4; i++)
	{
		Xes += xGhost[i]; Yes += yGhost[i];
	}
	(int)Xes /= 4; (int)Yes /= 4;

	//Circumference using average radius = avg(r)*2*Pi
	return (manhattanDist(xGhost[0], yGhost[0], Xes, Yes) +
		manhattanDist(xGhost[1], yGhost[1], Xes, Yes) +
		manhattanDist(xGhost[2], yGhost[2], Xes, Yes) +
		manhattanDist(xGhost[3], yGhost[3], Xes, Yes) / 2) * (22 / 7);
}

/**
 * Description: Will print out the contents of the global GhostMoves struct in 16x16 matrix form, with entries as either 0 or ghost[1-4].x.y
 */
void printGhostMoves()
{
	unsigned int count = -1;
	//Can use this to log the matrix of ghost moves and associated probabilities
	print("\n#P(S): 1/"); itoa(GhostMove.probs, out, 10); print(out);
	print("\t3:0, 4:0\t\t\t\t3:0, 4:1\t\t\t\t3:0, 4:2\t\t\t\t3:0, 4:3\t\t\t\t3:1, 4:0\t\t\t\t3:1, 4:1\t\t\t\t3:1, 4:2\t\t\t\t3:1, 4:3\t\t\t\t3:2, 4:0\t\t\t\t3:2, 4:1\t\t\t\t3:2, 4:2\t\t\t\t3:2, 4:3\t\t\t\t3:3, 4:0\t\t\t\t3:3, 4:1\t\t\t\t3:3, 4:2\t\t\t\t3:3, 4:3\n");
	print("-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------");

	for(int p1 = 0; p1 < 4; p1++)
	for(int p2 = 0; p2 < 4; p2++)
	{
		print("\n0:"); itoa(p1, out, 10); print(out); print(", 1:"); itoa(p2, out, 10); print(out); print("|\t");
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
				print("G"); itoa(g2, out, 10); print(out); print(":");
				for(int g3 = 0; g3 < 2; g3++)
				{
					itoa(GhostMove.ghostXYMove[g2][g3][count], out, 10); 
					if(GhostMove.ghostXYMove[g2][g3][count] < 10) print(" "); //Adjust spacing between 1 and 2 digit numbers
					print(out); print(",");
				}
			}
			print("\t");
		}
	}}
	print("\n\n");
}