// ManipPacResults.cpp : main project file.

#include "stdafx.h"
#include "Form1.h"

using namespace ManipPacResults;
using namespace std;

#using <mscorlib.dll>

void ConjoinLevels();

FILE *outf;
char out[99];

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
/** /
	// Enabling Windows XP visual effects before any controls are created
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	// Create the main window and run it
	Application::Run(gcnew Form1());
/**/
	ConjoinLevels();

	return 0;
}
void ConjoinLevels()
{
/**/
	array <String^>^plyrData = Directory::GetFiles("../");
	String^ plyr = Path::GetFileNameWithoutExtension(plyrData[0]);
	plyr += "Game.csv";
	char* player = (char*)(void*)Marshal::StringToHGlobalAnsi(plyr);
	fopen_s(&outf, player, "wt");
/** /
	printf("Enter DGD type (& ID) to assign: ");
	gets_s( out );
	string type = "";
	type += out;
	type += "Game.csv";
	fopen_s(&outf, type.c_str(), "wt");
/**/
	
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
			if(!(x == 19 && y == 19))
				fputs(",", outf);
		}
	}
/** /
	int i = 0;
	string p;
	p += player;
	string p2;
	p2 += p[i];
	while( atoi(p2.c_str()) == 0 )
	{
		fputs(p2.c_str(), outf);
		i++;
		p2 = "";
		p2 += p[i];
	}
/**/
	fputs("\n", outf);
	FILE *states;
	string level = "Level-1.csv";
	int count = 2;
	int c;
	string state("");
	bool End = false;

	while( fopen_s( &states, level.c_str(), "r" ) == 0 )
	{
		while( true )
		{
			if( (c = fgetc(states)) == EOF )
				break;
			else if( c == '\n' && !End && state.size() > 1)
			{
				End = true;
				state.resize(state.size()-1);
			}
			else
				End = false;
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
