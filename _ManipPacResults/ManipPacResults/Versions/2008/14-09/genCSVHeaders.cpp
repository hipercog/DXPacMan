// genCSVHeaders.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

using namespace std;
/** /
using <mscorlib.dll>
using namespace system;
using namespace system::IO;
using namespace system::IO::Collections;
/**/
void ConjoinLevels();

FILE *outf;
char out[99];

int _tmain(int argc, _TCHAR* argv[])
{
/** /
	FILE *vars;
	FILE *maps;
	fopen_s(&outf, "Game.csv", "r");
	fopen_s(&vars, "Vars.csv", "wt");
	fopen_s(&maps, "Maps.csv", "wt");
/**/
	ConjoinLevels();

	return 0;
}

void ConjoinLevels()
{
/** /
	string plyrData[1];
	plyrData = GetFiles("../");
	string plyr = Path::GetFileNameWithoutExtension(plyrData[0]);
	plyr += "Game.csv";
/**/
	printf("Enter DGD type (& ID) to assign: ");
	gets_s( out );
	string type = "";
	type += out;
	type += "Game.csv";

	fopen_s(&outf, type.c_str(), "wt");
	
	fputs("StateNum,PacX,PacY,Gst1X,Gst1Y,Gst2X,Gst2Y,Dots,Lives,Pills,Points,ChryBool,PacAtkBool,LastMoveDir,Cycles,", outf);

	for(int x = 0; x < 20; x++)
	{
		for(int y = 0; y < 20; y++)
		{
			fputs("x", outf);
			sprintf_s(out, 99, "%d", x);
			fputs(out, outf);
			fputs(" y", outf);
			sprintf_s(out, 99, "%d", y);
			fputs(out, outf);
			fputs(",", outf);
		}
	}

	FILE *states;
	string level = "Level-1.csv";
	int count = 2;
	int c;
	string state("");

	while( fopen_s( &states, level.c_str(), "r" ) == 0 )
	{
		while( true )
		{
			if( (c = fgetc(states)) == EOF )
				break;
			state += c;
		}
		state.resize(state.size()-1);
		fputs(state.c_str(), outf);
		state = "";

		level = "Level-";
		sprintf_s(out, 99, "%d", count);
		level += out;
		level += ".csv";
		count++;
	}

	fflush(outf);
	fclose(outf);
}
//end of file