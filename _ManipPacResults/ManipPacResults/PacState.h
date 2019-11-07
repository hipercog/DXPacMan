#define NUM_GS		2		//Number of Ghosts we are using
#define POT_GS		16		//Number of potential next move permutations = 4 ^ NUM_GS

class PacState;

/**
 *	Description: struct holds the state of the game, including x,y position for all sprites (!cherry - x,y is fixed).
 *	This is a potential state, all the state variables for the state rendered to screen are declared & instantiated elsewhere (mostly universals.h)
 */
class PacState
{
public:
	
	int State, xPac, yPac, Lives, Points, Pills, Dots, MovedDir, Cycle;
	int xGhost[NUM_GS], yGhost[NUM_GS];
	bool CherryThere, PacAttack;
	int dtpMap[20][20];

	/**
	 * Default Construcor
	 */
	PacState::PacState(){}

	/**
	 *	Description: Creates a new PacState class from the values passed in
	 */
	PacState::PacState(int state, int PacX, int PacY, int numLives, int numPoints, int numPills, int numDots, int moved, int cycle, int *ghostX, int *ghostY, bool cherryThere, bool pacAtk, int Map[20][20])
	{
		State = state;
		xPac = PacX;
		yPac = PacY;
		Lives = numLives;
		Points = numPoints;
		Pills = numPills;
		Dots = numDots;
		MovedDir = moved;
		Cycle = cycle;
		for(int i = 0; i < NUM_GS; i++)
		{
			xGhost[i] = ghostX[i];
			yGhost[i] = ghostY[i];
		}
		CherryThere = cherryThere;
		PacAttack = pacAtk;

		for(int i = 0; i < 20; i++)
		for(int j = 0; j < 20; j++)
			dtpMap[i][j] = Map[i][j];
	}
	/**/
	void PacState::operator=(PacState s)
	{
		State = s.State;
		xPac = s.xPac;
		yPac = s.yPac;
		Lives = s.Lives;
		Points = s.Points;
		Pills = s.Pills;
		Dots = s.Dots;
		MovedDir = s.MovedDir;
		Cycle = s.Cycle;
		for(int i = 0; i < NUM_GS; i++)
		{
			xGhost[i] = s.xGhost[i];
			yGhost[i] = s.yGhost[i];
		}
		CherryThere = s.CherryThere;
		PacAttack = s.PacAttack;

		for(int i = 0; i < 20; i++)
		for(int j = 0; j < 20; j++)
			dtpMap[i][j] = s.dtpMap[i][j];
	}
	/**/
}; // End state struct