/**	
 *	Author: Ben Cowley, 2006.
 *	Header file for the specific Decision Theory Player Modelling functions.
 */

#ifndef INC_DTPM_H
#define INC_DTPM_H

#define NOMV	-1	//Value for entries in GhostMoves struct objects that are not valid moves

void DTPM();
bool buildTreeOfGhostMoves(unsigned int recurseLimit, int trg_x, int trg_y);

//256 permutations of ghost movement, and then details of location with four ghosts and an x,y axis
struct GhostMoves
{
public:
	GhostMoves() {}

	int ghostXYMove[4][2][256];
	short probs;

	bool operator==(GhostMoves gM)
	{
		if(probs != gM.probs)
			return false;
		else{
			for(int i0 = 0; i0 < 4; i0++)
			{
			for(int i1 = 0; i1 < 2; i1++)
			{
			for(int i2 = 0; i2 < 256; i2++)
			{
				if(ghostXYMove[i0][i1][i2] != gM.ghostXYMove[i0][i1][i2])
					return false;
			}}}
		}
		return true;
	}

	void operator=(GhostMoves gM)
	{
		probs = gM.probs;
		for(int i0 = 0; i0 < 4; i0++)
		{
		for(int i1 = 0; i1 < 2; i1++)
		{
		for(int i2 = 0; i2 < 256; i2++)
		{
			ghostXYMove[i0][i1][i2] = gM.ghostXYMove[i0][i1][i2];
		}}}
	}

	void size()
	{
		probs = 0;
		for(int i = 0; i < 256; i++)
			if(ghostXYMove[0][0][i] > NOMV && ghostXYMove[0][1][i] > NOMV)
				probs++;
	}
};
/**/
#endif /* INC_DTPM_H */