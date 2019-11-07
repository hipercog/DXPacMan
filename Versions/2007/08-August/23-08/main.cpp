
////////////////////////////////////////////////////////////////////////////////////////////
//Preprocessor settings
////////////////////////////////////////////////////////////////////////////////////////////
#define STRICT
#define WIN32_LEAN_AND_MEAN			//Exclude rarely used stuff from windows header file
#define DIRECTINPUT_VERSION 0x0800 
#ifdef _DEBUG
#define D3D_DEBUG_INFO				//Allows more debug info from Direct3D
#endif
//Used as a quick fix around getting the input filestream to work (ifstream)

////////////////////////////////////////////////////////////////////////////////////////////
//Include files
////////////////////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <mmsystem.h>
#include <mmreg.h>
#include <ctime>
#include <dinput.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <dsound.h>
#include <direct.h>

#include "dtpm.h"
#include "main.h"
#include "universals.h"
#include "guicon.h"

//Diables deprecated warning messege
#pragma warning( disable : 4996 ) 
#include <strsafe.h>
#pragma warning( default : 4996 )		

////////////////////////////////////////////////////////////////////////////////////////////
//Function prototypes
////////////////////////////////////////////////////////////////////////////////////////////
HRESULT GameInit(HWND);
HRESULT InitD3D(HWND);
HRESULT InitDirectInput(HWND);
HRESULT InitDirectSound(HWND);
HRESULT InitDXFont();
HRESULT GameLoop(bool animate);
HRESULT GameRender();
HRESULT LoadBitmapToSurface(const char*, LPDIRECT3DSURFACE9*, LPDIRECT3DDEVICE9);
HRESULT CopySurfaceToSurface(RECT*, LPDIRECT3DSURFACE9, POINT*, LPDIRECT3DSURFACE9, BOOL, D3DCOLOR);
HRESULT ValidateDevice();
HRESULT DrawEnd();
HRESULT LoadMapFromFile(const char *mapName);
HRESULT SelectActiveSprite(int, int, int, RECT*);
HRESULT ChangeLevel();

void MovePacMan();
void MoveGhosts();
void PacLoop();
void GhostLoop();
void CollisionCheck();
void DrawDots();
void InitGameAgents();
int GhostDriver( int xPos, int yPos, int nGhost );
void SetPositionVector(LPD3DXVECTOR3, int, int);
int SetSpritePosition(LPD3DXVECTOR3, int nGhost );
void ResetAttackSwitch();

BOOL CheckForCollision(LPD3DXVECTOR3, LPD3DXVECTOR3); 

int GameShutdown();

UCHAR keyboard_state[256]; //Contains keyboard state table

HINSTANCE	main_instance   = NULL; //Global handle to application instance

static char strAppName[] = "DX PacMan";	//The name of the window class and also the title of the app window

LPDIRECT3D9				g_pD3D					= NULL;	//Declare pointer to IDirect3D9 interface
LPDIRECT3DDEVICE9		g_pDevice				= NULL;	//Declare pointer to IDirect3DDevice9 interface
LPDIRECT3DSURFACE9		g_pBackSurface			= NULL;	//Declare pointer to back buffer surface interface
LPDIRECT3DSURFACE9		g_pScreenSurface		= NULL;	//Declare pointer to screen surface interface
LPDIRECT3DSURFACE9		g_pGameOverSurface		= NULL; //Declare pointer to game over screen surface
LPDIRECTINPUT8			lpdi					= NULL; //Pointer for DirectInput object
LPDIRECTINPUTDEVICE8	lpdikey					= NULL; //Pointer for keyboard device object
LPDIRECTSOUND8			g_pSound				= NULL; //Pointer to IDirectSound8 interface
LPDIRECTSOUNDBUFFER		g_pWavBuffer			= NULL; //Pointer to IDirectSoundBuffer8 interface
LPD3DXFONT				g_pFont					= NULL; //Pointer to ID3DXFont interface
D3DPRESENT_PARAMETERS	g_OrigD3DParams;				//Declare pointer to presentation parameters struct

//Pointers to D3DXSprite interfaces
LPD3DXSPRITE			g_pPacManSprite			= NULL;
LPD3DXSPRITE			g_pShadowSprite			= NULL;
LPD3DXSPRITE			g_pSpeedySprite			= NULL;
LPD3DXSPRITE			g_pPokeySprite			= NULL;
LPD3DXSPRITE			g_pBashfulSprite		= NULL;
LPD3DXSPRITE			g_pCherrySprite			= NULL;

//Pointers to IDirect3DTexture9 interfaces
LPDIRECT3DTEXTURE9		g_pPacManSpriteTexture		= NULL; 
LPDIRECT3DTEXTURE9		g_pShadowSpriteTexture		= NULL;
LPDIRECT3DTEXTURE9		g_pSpeedySpriteTexture		= NULL;
LPDIRECT3DTEXTURE9		g_pPokeySpriteTexture		= NULL;
LPDIRECT3DTEXTURE9		g_pBashfulSpriteTexture		= NULL;
LPDIRECT3DTEXTURE9		g_pCherrySpriteTexture		= NULL; 
LPDIRECT3DTEXTURE9		g_pGhostFleeSpriteTexture	= NULL;

//Temporary texture pointers
LPDIRECT3DTEXTURE9		g_pOrigShadowTexture		= NULL;
LPDIRECT3DTEXTURE9		g_pOrigSpeedyTexture		= NULL;
LPDIRECT3DTEXTURE9		g_pOrigPokeyTexture			= NULL;
LPDIRECT3DTEXTURE9		g_pOrigBashfulTexture		= NULL;

int g_PacManSpriteWidth		= 0; //Global to store the width of the PacMan sprite
int g_PacManSpriteHeight	= 0; //Global to store the height of the PacMan sprite

//Note: Only one copy of width and height of ghost sprite is required as all ghost
//sprites are the same size
int g_GhostSpriteHeight		= 0; //Global to store the height of the ghost sprite
int g_GhostSpriteWidth		= 0; //Global to store the width of the ghost sprite

//RECT structures (contain vertices of current animation frame rectangle)
RECT g_PacManRect;	
RECT g_GhostRect;

//Random numbers that indicate direction.
int newRandom[4];

//Ghost persistent movement holders
int g_GhostMvDir[NUM_GS];
int gakProb = 7; //lowest to highest	= 7, 8, 9, 10, 11, 12
int pacProb = 7; //corresponding		= 7, 6, 5,  4,  3,  2

////////////////////////////////////////////////////////////////////////////////////////////
// The window procedure to handle events
////////////////////////////////////////////////////////////////////////////////////////////
LRESULT WINAPI  WndProc( HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam )
{

	// Switch the windows message to figure out what it is
	switch(uMessage)
	{
		case WM_CREATE:	// The CreateWindow() function was just called
		{
			// One Time Initialization
			return 0;
		}

		case WM_PAINT:	// The window needs to be redrawn
		{
			ValidateRect(hWnd, NULL);

			return 0;
		}

		
		case WM_DESTROY:	// The window is about to be closed
		{
			// Our main window is closing.  This means we want our app to exit.
			// Tell windows to put a WM_QUIT message in our message queue
			PostQuitMessage( 0 );
			return 0;
		}

		default:			// Some other message
		{
			// Let windows handle this message
			return DefWindowProc( hWnd, uMessage, wParam, lParam );
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// The windows entry point. The application will start executing here
////////////////////////////////////////////////////////////////////////////////////////////
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, INT iCmdShow)
{


	HWND hWnd;		// The handle to our main window
	MSG msg;		// The message windows is sending us
	WNDCLASSEX wc;	// The window class used to create our window
	srand(GetTickCount());  // Initialize random number generator.
/**/
	//Add a console to run alongside our main window, taking keyboard I/O
//	#ifdef _DEBUG
		RedirectIOToConsole();
//	#endif
/** /
	//Non essential functions for logging
	DWORD temptime = GetTickCount();
	getPlayerData();		// Initialise a file to write output to.
	unsigned int sTime = GetTickCount();
/**/
	//Create a directory to save game output to, based on the time of this code call - Year, day, hour, minute, second
	time_t curr;
	tm *local = &tm();
	time(&curr); // get current time_t value
	localtime_s(local, &curr);
	strftime ( out, 32, "%y-%j-%H-%M-%S", local );
	/* Make a directory to store game output, if results are going local */
	if(LOCALOUT)
	{
		gameDir += out;
		_mkdir( gameDir.c_str() );
	}
	/* Do any which way */
	gameDir = out;
	//Send the directory for this game if results not going local.
	if(!LOCALOUT)
		if( runclient("DirSend", gameDir.c_str()) == -1 )
		{
			GameOver = true;
			cout << "Due to the server connection failure, the game will be aborted.\n";
			cout << "Please check that your firewall settings do not block this game, and try to play again.\n";
			cout << "If you still cannot play, there may be unavoidable server downtime.\n";
			cout << "Please try again later - if at first you don't succeed...!\n";
		}
/** /
	char in;
	cout << "Play or Analyse? Pp/Aa:";
	cin >> in;
	if( in == 'A' || in == 'a' )
	{
		temptime = GetTickCount();
		postDTPM( 3 );
		sleep( 2000 );
	}else{
/**/
		
		main_instance = hInstance;

		// Fill in the window class with the attributes for our main window:
		wc.cbSize			= sizeof( WNDCLASSEX );						//The size of this struture in bytes
		wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;		//The style of the window.
		wc.cbClsExtra		= 0;										//Useless information. Just set to zero.
		wc.cbWndExtra		= 0;										//Useless information. Just set to zero.
		wc.lpfnWndProc		= WndProc;									//The name of our event handler
		wc.hInstance		= hInstance;								//Handle to the applications instance
		wc.hbrBackground	= (HBRUSH)GetStockObject( DKGRAY_BRUSH );	//Handle to the window's background brush
		wc.hIcon			= LoadIcon( NULL, IDI_APPLICATION );		//Handle to the icon to use for the window
		wc.hIconSm			= LoadIcon( NULL, IDI_APPLICATION );		//Handle to a smaller version of the apps icon
		wc.hCursor			= LoadCursor( NULL, IDC_CROSS );			//Handle to the cursor to use
		wc.lpszMenuName		= NULL;										//Handle to the resource to use as our menu
		wc.lpszClassName	= strAppName;								//The human readable name for this class

		// Register the class with windows
		RegisterClassEx( &wc );

		// Create the window based on the previous class
		hWnd = CreateWindowEx(WS_EX_TOPMOST,		// Advanced style settings
							  strAppName,			// The name of the class
							  strAppName,			// The window caption
							  WS_OVERLAPPEDWINDOW,	// The window style
			   				  CW_USEDEFAULT,		// The initial x position 
							  500,					//CW_USEDEFAULT,		// The initial y position
			   				  512, 512,				// The initial width / height
							  NULL,					// Handle to parent window						
							  NULL,					// Handle to the menu
							  hInstance,			// Handle to the apps instance
							  NULL);				// Advanced context



		// Display the window we just created
		ShowWindow(hWnd, iCmdShow);

		// Draw the window contents for the first time
		UpdateWindow(hWnd);

		//Initialise the game structures, including the creation of the Direct3D and 
		//DirectInput objects, and game objects. 
		if (FAILED(GameInit(hWnd)))
		{
			GameShutdown();
			return E_FAIL;
		}
		
		/*********************************************************
		 *	Show splash screen here and wait for some milliseconds
		 */
		HRESULT hr = 0x0000;
		//Test the device exists
		if(g_pDevice == NULL)
			GameOver = true;
		
		//Use the device interface to clear the back buffer (render target)	
		if (FAILED(hr = g_pDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 25), 1.0f, 0)))
			GameOver = true;

		//Call BeginScene() before any rendering
		if (FAILED(hr = g_pDevice->BeginScene()))
			GameOver = true;

		//Validate the device to ensure it is ready for rendering
		if (FAILED(hr = ValidateDevice()))
			GameOver = true;

		//Copy the background surface to the back buffer:
		if (FAILED(hr = CopySurfaceToSurface(0, g_pScreenSurface, 0, g_pBackSurface, 0,
											 D3DCOLOR_ARGB(0, 255, 0, 255))))
			GameOver = true;

		//Unlock the back buffer surface
		g_pBackSurface->UnlockRect();

		//Call EndScene because rendering to the device is finished
		if (FAILED(hr = g_pDevice->EndScene()))
			GameOver = true;

		//Present the contents of the back buffer to the frame buffer
		if (FAILED(hr = g_pDevice->Present(NULL, NULL, NULL, NULL)))
			GameOver = true;
		sleep(1000);

		//Load the 1st map screen
		if (FAILED(hr = LoadBitmapToSurface("resources/map0.bmp", &g_pScreenSurface, g_pDevice)))
			GameOver = true;	

		//Init some gameplay variables
		randCherryInit = rand()%500 + 500;
		unsigned int eTime = GetTickCount();// - sTime;
		unsigned int pacRndrTime = eTime; 
		float gstRndrTime = (float)eTime;
		bool animate = false;

	//	int lastSec = time(0);
	//	int frameTimer = 0, pacloop = 0, gstloop = 0;

		//Init Ghost movement
		for(int i = 0; i < NUM_GS; i++)
			g_GhostMvDir[i] = 0;

		// Start the message processing loop
		while( TRUE )
		{
			cycles++;
			/** /
			if(lastSec != time(0))
			{
				lastSec = time(0);
				cout << "Frames: " << frameTimer << "; Loops: " << pacloop << ", " << gstloop << endl;
				frameTimer = 0;
				pacloop = gstloop = 0;
			}
			frameTimer++;
			/**/
			// Check if a message is waiting for processing
			if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
			{
				// Check if the message is to quit the application
				if( msg.message == WM_QUIT )
					break;	// Exit the message loop
				
				// Change the format of certain messages
				TranslateMessage( &msg );

				// Pass the message to WndProc() for processing
				DispatchMessage( &msg );
			}else{
				/**/
				if( eTime - pacRndrTime >= pacTik )
				{
					pacRndrTime = eTime;
					PacLoop();
					animate = true;

					//pacloop++;
				}
				//Different loop speed makes Ghosts slower
				if( (float)eTime - gstRndrTime >= gstTik )
				{
					gstRndrTime = (float)eTime;
					GhostLoop();
					if(!animate) animate = true;

					//gstloop++;
				}
				GameLoop( animate ); //Perform another iteration of the GameLoop()
				animate = false;
				/**/
			}
			eTime = GetTickCount();// - sTime;

			//Check for GameOver
			if( GameOver )
				break;
		}
		//Export the game state data
		Level++;
		outputState(LOCALOUT);
		GameStates.clear();
		endclient();

		/* Draw game over screen */
		while( TRUE )
		{
			if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
			{
				if( msg.message == WM_QUIT )	
					break;

				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
			else
			{
				if (FAILED(DrawEnd()))
					return E_FAIL;
			}
		}
/** /
	}

	#ifdef _DEBUG
		temptime = GetTickCount() - temptime;
		sprintf_s( out, 99, "%d", temptime ); print("Time Lapse: "); print(out);
	#endif
/**/
	//Call the GameShutdow() to clean up all DirectX objects
	GameShutdown();

	//Unregister the window class
	UnregisterClass( strAppName, wc.hInstance );

	// Return control to windows with the exit code
	return (int)msg.wParam;
}


////////////////////////////////////////////////////////////////////////////////////////////
// The game object initialisation function
////////////////////////////////////////////////////////////////////////////////////////////
HRESULT GameInit(HWND hWnd)
{
	//Result handle for function calls
	HRESULT hr = 0x0000;

	//Switch off the cursor
	ShowCursor(false);

	//Initialise starting positions for PacMan, ghosts and other screen objects
	InitGameAgents();

	//Initialise the Direct3D device
	if (FAILED(hr = InitD3D(hWnd)))
		return hr;

	//Initialise code for DirectInput
	if (FAILED(hr = InitDirectInput(hWnd)))
		return hr;

	//Initialise the DirectSound objects and load "spell.wav" into the sound buffer
	//Note: In order to use more than one sound another buffer must be created and loaded
	//with the appropriate sound
	if (FAILED(hr = InitDirectSound(hWnd)))
		return hr;

	//Create a DirectX font object (the font will be Arial 20pt bold)
	if (FAILED(hr = InitDXFont()))
		return hr;


	//Obtain a pointer to a back buffer (from the device's swap chain)
	if (FAILED(hr = g_pDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &g_pBackSurface)))
		return hr;

	//Load the screen and game over bitmaps to their respective surfaces:
	if (FAILED(hr = LoadBitmapToSurface("resources/pac_man_splash.bmp", &g_pScreenSurface, g_pDevice)))
		return hr;	

	if (FAILED(hr = LoadBitmapToSurface("resources/gameover.bmp", &g_pGameOverSurface, g_pDevice)))
		return hr;

	//Load the game object bitmaps into their respective texture surfaces:
    D3DXIMAGE_INFO d3dxImageInfo;

    if (FAILED(hr = D3DXCreateTextureFromFileEx(g_pDevice,
												"resources/Pac.bmp",
												D3DX_DEFAULT,
												D3DX_DEFAULT,
												D3DX_FROM_FILE,
												0, 
												D3DFMT_FROM_FILE,
												D3DPOOL_MANAGED, 
												D3DX_FILTER_NONE,
												D3DX_FILTER_NONE,
												D3DCOLOR_ARGB(0, 255, 0, 255),
												&d3dxImageInfo,
												NULL,
												&g_pPacManSpriteTexture)))
		return hr;

	//Create the sprite for PacMan
	if (FAILED(hr = D3DXCreateSprite(g_pDevice, &g_pPacManSprite)))
		return hr;

	//Store the dimensions of the image used for the sprite
	g_PacManSpriteWidth = d3dxImageInfo.Width;
	g_PacManSpriteHeight = d3dxImageInfo.Height;


	//Create the texture for the sprite from the bitmap file
    if (FAILED(hr = D3DXCreateTextureFromFileEx(g_pDevice,
												"resources/shadow.bmp",
												D3DX_DEFAULT,
												//80,
												D3DX_DEFAULT,
												//20,
												//D3DX_FROM_FILE,
												1,
												0, 
												D3DFMT_FROM_FILE,
												D3DPOOL_MANAGED, 
												D3DX_FILTER_NONE,
												D3DX_FILTER_NONE,
												D3DCOLOR_ARGB(0, 255, 0, 255),
												&d3dxImageInfo,
												NULL,
												&g_pShadowSpriteTexture)))
		return hr;

	//Create the sprite
	if (FAILED(hr = D3DXCreateSprite(g_pDevice, &g_pShadowSprite)))
		return hr;

	//Store the dimensions of the image used for the sprite
	g_GhostSpriteWidth = d3dxImageInfo.Width;
	g_GhostSpriteHeight = d3dxImageInfo.Height;

	if( NUM_GS > 1 )
	{
	//Create the texture for the sprite from the bitmap file
    if (FAILED(hr = D3DXCreateTextureFromFileEx(g_pDevice,
												"resources/speedy.bmp",
												D3DX_DEFAULT,
												D3DX_DEFAULT,
												D3DX_FROM_FILE,
												0, 
												D3DFMT_FROM_FILE,
												D3DPOOL_MANAGED, 
												D3DX_FILTER_NONE,
												D3DX_FILTER_NONE,
												D3DCOLOR_ARGB(0, 255, 0, 255),
												&d3dxImageInfo,
												NULL,
												&g_pSpeedySpriteTexture)))
		return hr;

	//Create the sprite
	if (FAILED(hr = D3DXCreateSprite(g_pDevice, &g_pSpeedySprite)))
		return hr;

	if( NUM_GS > 2 )
	{
	//Create the texture for the sprite from the bitmap file
    if (FAILED(hr = D3DXCreateTextureFromFileEx(g_pDevice,
												"resources/pokey.bmp",
												D3DX_DEFAULT,
												D3DX_DEFAULT,
												D3DX_FROM_FILE,
												0, 
												D3DFMT_FROM_FILE,
												D3DPOOL_MANAGED, 
												D3DX_FILTER_NONE,
												D3DX_FILTER_NONE,
												D3DCOLOR_ARGB(0, 255, 0, 255),
												&d3dxImageInfo,
												NULL,
												&g_pPokeySpriteTexture)))
		return hr;

	//Create the sprite
	if (FAILED(hr = D3DXCreateSprite(g_pDevice, &g_pPokeySprite)))
		return hr;

	if( NUM_GS > 3 )
	{
	//Create the texture for the sprite from the bitmap file
    if (FAILED(hr = D3DXCreateTextureFromFileEx(g_pDevice,
												"resources/bashful.bmp",
												D3DX_DEFAULT,
												D3DX_DEFAULT,
												D3DX_FROM_FILE,
												0, 
												D3DFMT_FROM_FILE,
												D3DPOOL_MANAGED, 
												D3DX_FILTER_NONE,
												D3DX_FILTER_NONE,
												D3DCOLOR_ARGB(0, 255, 0, 255),
												&d3dxImageInfo,
												NULL,
												&g_pBashfulSpriteTexture)))
		return hr;

	//Create the sprite
	if (FAILED(hr = D3DXCreateSprite(g_pDevice, &g_pBashfulSprite)))
		return hr;
	}}}
        
	//Create the bonus cherry bitmap
    if (FAILED(hr = D3DXCreateTextureFromFileEx(g_pDevice,
												"resources/cherry.bmp",
												D3DX_DEFAULT,
												D3DX_DEFAULT,
												D3DX_FROM_FILE,
												0, 
												D3DFMT_FROM_FILE,
												D3DPOOL_MANAGED, 
												D3DX_FILTER_NONE,
												D3DX_FILTER_NONE,
												D3DCOLOR_XRGB(255, 0, 255),
												&d3dxImageInfo,
												NULL,
												&g_pCherrySpriteTexture)))
		return hr;

	if (FAILED(hr = D3DXCreateSprite(g_pDevice, &g_pCherrySprite)))
		return hr;

	//Creating one flee ghost texture
    if (FAILED(hr = D3DXCreateTextureFromFileEx(g_pDevice,
												"resources/speedyA.bmp",
												D3DX_DEFAULT,
												D3DX_DEFAULT,
												D3DX_FROM_FILE,
												0, 
												D3DFMT_FROM_FILE,
												D3DPOOL_MANAGED, 
												D3DX_FILTER_NONE,
												D3DX_FILTER_NONE,
												D3DCOLOR_ARGB(0, 255, 0, 255),
												&d3dxImageInfo,
												NULL,
												&g_pGhostFleeSpriteTexture)))
		return hr;

	//Store the texture pointers for the ghost sprites
	g_pOrigShadowTexture = g_pShadowSpriteTexture;
	if( NUM_GS > 1 )
	{
		g_pOrigSpeedyTexture = g_pSpeedySpriteTexture;
		if( NUM_GS > 2 )
		{
		g_pOrigPokeyTexture = g_pPokeySpriteTexture;
		if( NUM_GS > 3 )
			g_pOrigBashfulTexture = g_pBashfulSpriteTexture;
	}}

	//Load the map data from a text file:
	if (FAILED(hr = LoadMapFromFile("resources/map0.txt")))
		return hr;

	//Return S_OK from GameInit()
	return S_OK;
}

////////////////////////////////////////////////////////////////////////////////////////////
// The game loop
////////////////////////////////////////////////////////////////////////////////////////////
void PacLoop()
{
	MovePacMan();
	CollisionCheck();
}

void GhostLoop()
{
	MoveGhosts();
	CollisionCheck();
}

HRESULT GameLoop(bool animate)
{
	HRESULT hr = 0x0000;

	//Static variables are used so they retain their values between calls to GameLoop()

	//Initialise sprite animation variables
	static BOOL bIncrease = TRUE;
	static int CurrentSprite = 0;

	//Add the cherry after a random time has passed
	if( cycles > randCherryInit )
	{
		randCherryInit = INT_MAX;
		CherryEaten = false;
	}

	//Only animate game objects every so many iterations of GameLoop()
	if( animate )
	{
		//Decision Theory Pacman functionality
		//Obtain the last keyboard press
		lpdikey->GetDeviceState(256, (LPVOID)keyboard_state);
/** /
		if( keyboard_state[DIK_0] )
			forceDTPM( 0 );
		else if( keyboard_state[DIK_1] )
			forceDTPM( 1 );
		else if( keyboard_state[DIK_2] )
			forceDTPM( 2 );
		else if( keyboard_state[DIK_3] )
			forceDTPM( 3 );
		else if( keyboard_state[DIK_4] )
			forceDTPM( 4 );
		else if( keyboard_state[DIK_5] )
			forceDTPM( 5 );
		else if( keyboard_state[DIK_6] )
			forceDTPM( 6 );
		else if( keyboard_state[DIK_7] )
			forceDTPM( 7 );
		else if( keyboard_state[DIK_8] )
			forceDTPM( 8 );
		else if( keyboard_state[DIK_9] )
		{
			NumDots = 0;
			NumPills = 0;
		}else 
/**/		
		if( keyboard_state[DIK_P] )
		{
			while( true )
			{
				lpdikey->GetDeviceState(256, (LPVOID)keyboard_state);
				if( keyboard_state[DIK_R] )
					break;
			}
		}

		//Only 4 sprites per row (for animation): 0,1,2,3
		if(CurrentSprite == 3)
			bIncrease = false;
		
		if (CurrentSprite == 0)
			bIncrease = true;

		//Select the active sprite for PacMan (obtain the appropriate RECT)
		if (FAILED(hr = SelectActiveSprite((RowAnimation*4 + CurrentSprite), 
			                                g_PacManSpriteWidth, 
											g_PacManSpriteHeight,
											&g_PacManRect)))
			return hr;

		if (FAILED(hr = SelectActiveSprite(CurrentSprite,
										   g_GhostSpriteWidth,
										   g_GhostSpriteHeight,
										   &g_GhostRect)))
		   return hr;


		//Update the value of the current sprite
		if(bIncrease)
			CurrentSprite++;
		else
			CurrentSprite--;
	}
		
	//Call the game render function
	if (FAILED(hr = GameRender()))
		return hr;

	//Quit if Escape key has been pressed
	if(GetAsyncKeyState(VK_ESCAPE))
		PostQuitMessage(0);

	//If ghost game state has previously changed (i.e. GhostAttack = false) then update ghost flee timer
	//and check if ghost game state has returned to normal (i.e. GhostAttack = true)
	if (GhostAttack == false) 
	{
		//Decrement timer
		GhostFleeTimer--;

		//Reset the Ghost positions once they are ready to come back into the game
		if (GhostFleeTimer < 37)
		{
			if(ShadowEaten)
				//Reset the ghost's position
				SetPositionVector(&g_ShadowSpritePos, (GRIDTLX + (SHADOWSPRITE_START_POS_X * SPRITE_WIDTH)),
													  (GRIDTLY + (SHADOWSPRITE_START_POS_Y * SPRITE_HEIGHT)));
			if( NUM_GS > 1 )
			{
				if(SpeedyEaten)
					//Reset the ghost's position
					SetPositionVector(&g_SpeedySpritePos, (GRIDTLX + (SPEEDYSPRITE_START_POS_X * SPRITE_WIDTH)),
														  (GRIDTLY + (SPEEDYSPRITE_START_POS_Y * SPRITE_HEIGHT)));
				if( NUM_GS > 2 )
				{
					if(PokeyEaten)
						//Reset the ghost's position
						SetPositionVector(&g_SpeedySpritePos, (GRIDTLX + (POKEYSPRITE_START_POS_X * SPRITE_WIDTH)),
															  (GRIDTLY + (POKEYSPRITE_START_POS_Y * SPRITE_HEIGHT)));
					if( NUM_GS > 3 )
						if(BashfulEaten)
							//Reset the ghost's position
							SetPositionVector(&g_SpeedySpritePos, (GRIDTLX + (BASHFULSPRITE_START_POS_X * SPRITE_WIDTH)),
																  (GRIDTLY + (BASHFULSPRITE_START_POS_Y * SPRITE_HEIGHT)));
			}}

			if(GhostFleeTimer%6 == 0)
			{
				//Reset the texture pointers for the ghost sprite
				g_pShadowSpriteTexture = g_pOrigShadowTexture;
				if( NUM_GS > 1 )
				{
					g_pSpeedySpriteTexture = g_pOrigSpeedyTexture;
					if( NUM_GS > 2 )
					{
						g_pPokeySpriteTexture = g_pOrigPokeyTexture;
						if( NUM_GS > 3 )
							g_pBashfulSpriteTexture = g_pOrigBashfulTexture;
				}}
			}else{
				//Set the texture pointer for the ghost sprites
				g_pShadowSpriteTexture = g_pGhostFleeSpriteTexture;
				if( NUM_GS > 1 )
				{
					g_pSpeedySpriteTexture = g_pGhostFleeSpriteTexture;
					if( NUM_GS > 2 )
					{
						g_pPokeySpriteTexture = g_pGhostFleeSpriteTexture;
						if( NUM_GS > 3 )
							g_pBashfulSpriteTexture = g_pGhostFleeSpriteTexture;
				}}
			}
		}
		//Check if timer has finished
		if (GhostFleeTimer < 1)
		{
			ResetAttackSwitch();
		}
	}

	//Check if all the pills have been eaten (therefore game is over)
	if( NumDots+NumPills < 1 )
	{
		// reset all globals and start next level! 
		if ( FAILED(ChangeLevel()) )
			GameOver = true;
	}

	//Default return value
	return S_OK;
}

void ResetAttackSwitch()
{
	//Reset ghost game state and timer
	GhostAttack = true;
	gstTik -= (float)pacTik * 0.5;
	PacMap[9][8 + (Level%2)] = PacMap[10][8 + (Level%2)] = 2;
	GhostFleeTimer = 340 - (Level*10);
	RunOfGhostKills = 1;

	//Reset 'Ghost' Eaten bools
	ShadowEaten = false;
	if( NUM_GS > 1 )
	{
		SpeedyEaten = false;
		if( NUM_GS > 2 )
		{
			PokeyEaten = false;
			if( NUM_GS > 3 )
				BashfulEaten = false;
	}}

	//Reset the texture pointers for the ghost sprite
	g_pShadowSpriteTexture = g_pOrigShadowTexture;
	if( NUM_GS > 1 )
	{
		g_pSpeedySpriteTexture = g_pOrigSpeedyTexture;
		if( NUM_GS > 2 )
		{
			g_pPokeySpriteTexture = g_pOrigPokeyTexture;
			if( NUM_GS > 3 )
				g_pBashfulSpriteTexture = g_pOrigBashfulTexture;
	}}
}

////////////////////////////////////////////////////////////////////////////////////////////
// Level change function for resetting game elements and so on
////////////////////////////////////////////////////////////////////////////////////////////
HRESULT ChangeLevel()
{
	//Result handle for function calls
	HRESULT hr = 0x0000;

	Level++;
	if(Level%3 == 0)
		gstTik -= (float)pacTik * 0.1 * (Level / 3);//pacTik * (2.5 - (Level / 100.0));
	if(Level == 1 || Level == 3 || Level == 7 || Level == 13 || Level == 21)
	{
		gakProb++;
		pacProb--;
	}
	outputState(LOCALOUT);
	GameStates.clear();

	//switch Ghost movement
	for(int i = 0; i < NUM_GS; i++)
		g_GhostMvDir[i] = Level%2;

	string map("resources/map");
	sprintf_s(out, 99, "%d", Level % 2); //Level%2
	map += out;
	map += ".txt";

	//Load the map data from a text file:
	if (FAILED(hr = LoadMapFromFile(map.c_str())))
		return hr;

	map.erase(15, 3);
	map += "bmp";
	//Load a new screen bitmap to the game screen surfaces:
	if (FAILED(hr = LoadBitmapToSurface(map.c_str(), &g_pScreenSurface, g_pDevice)))
		return hr;

	//Reset PacMan & Cherry's starting position
	SetPositionVector(&g_PacmanSpritePos, (GRIDTLX + (PACMANSPRITE_START_POS_X * SPRITE_WIDTH)),
										(GRIDTLY + ((PACMANSPRITE_START_POS_Y - (9*(Level%2))) * SPRITE_HEIGHT))); // - (9*Level%2)
	SetPositionVector(&g_CherrySpritePos, (GRIDTLX + (CHERRYSPRITE_START_POS_X * SPRITE_WIDTH)),
								          (GRIDTLY + ((CHERRYSPRITE_START_POS_Y - (5*(Level%2))) * SPRITE_HEIGHT)));

	//Reset the ghost's position
	SetPositionVector(&g_ShadowSpritePos, (GRIDTLX + (SHADOWSPRITE_START_POS_X * SPRITE_WIDTH)),
										(GRIDTLY + (SHADOWSPRITE_START_POS_Y * SPRITE_HEIGHT)));
	if( NUM_GS > 1 )
	{
		SetPositionVector(&g_SpeedySpritePos, (GRIDTLX + (SPEEDYSPRITE_START_POS_X * SPRITE_WIDTH)),
											(GRIDTLY + (SPEEDYSPRITE_START_POS_Y * SPRITE_HEIGHT)));
		if( NUM_GS > 2 )
		{
			SetPositionVector(&g_PokeySpritePos, (GRIDTLX + (POKEYSPRITE_START_POS_X * SPRITE_WIDTH)),
												(GRIDTLY + (POKEYSPRITE_START_POS_Y * SPRITE_HEIGHT)));
			if( NUM_GS > 3 )
			{
				SetPositionVector(&g_BashfulSpritePos, (GRIDTLX + (BASHFULSPRITE_START_POS_X * SPRITE_WIDTH)),
													(GRIDTLY + (BASHFULSPRITE_START_POS_Y * SPRITE_HEIGHT)));
	}}}

	if(!GhostAttack)
	{
		ResetAttackSwitch();
	}
	randCherryInit = rand()%500 + 500;
	CherryEaten = true;
	NumPills = 4;
	NumDots = 175;
	cycles = 0;

	return S_OK;
}

////////////////////////////////////////////////////////////////////////////////////////////
// Game shutdown function for releasing game resources
////////////////////////////////////////////////////////////////////////////////////////////
int GameShutdown()
{
	//Flush the text output & clear up dtpm.cpp globals
	if( fileout != NULL )
	{
		fflush(fileout);
		fclose(fileout);
	}
	if( stateFile != NULL )
	{
		fflush(stateFile);	
		fclose(stateFile);
	}

	//Release the DX resources in the reverse order they were created in:

	//Release the font interface
	if (g_pFont != NULL)
	{
		g_pFont->Release();
		g_pFont = NULL;
	}

	//Release the DirectSound interface (note: the secondary buffer will be automatically
	//released by the DirectSound interface)
	if (g_pSound != NULL)
	{
		g_pSound->Release();
		g_pSound = NULL;
	}

	//Unacquire and release the keyboard and DirectInput resources
	if (lpdikey != NULL)
	{
		lpdikey->Unacquire();
		lpdikey->Release();
		lpdikey = NULL;
	}

	if (lpdi != NULL)
	{
		lpdi->Release();
		lpdi = NULL;
	}


	//If exit has occured while ghosts are not in attack mode (GhostAttack = false)
	//then the pointers for the ghost textures need to be reset before calling
	//the sprite release function
	if (GhostAttack == false)
	{
		g_pShadowSpriteTexture = g_pOrigShadowTexture;
		if( NUM_GS > 1 )
		{
			g_pSpeedySpriteTexture = g_pOrigSpeedyTexture;
			if( NUM_GS > 2 )
			{
				g_pPokeySpriteTexture = g_pOrigPokeyTexture;
				if( NUM_GS > 3 )
					g_pBashfulSpriteTexture = g_pOrigBashfulTexture;
		}}
	}

	//Release sprite resources
	if (g_pGhostFleeSpriteTexture != NULL)
	{
		g_pGhostFleeSpriteTexture->Release();
		g_pGhostFleeSpriteTexture = NULL;
	}

	if (g_pCherrySpriteTexture != NULL)
	{
		g_pCherrySpriteTexture->Release();
		g_pCherrySpriteTexture = NULL;
	}
	if (g_pCherrySprite != NULL)
	{
		g_pCherrySprite->Release();
		g_pCherrySprite = NULL;
	}	

	if (g_pShadowSpriteTexture != NULL)
	{
		g_pShadowSpriteTexture->Release();
		g_pShadowSpriteTexture = NULL;
	}
	if (g_pShadowSprite != NULL)
	{
		g_pShadowSprite->Release();
		g_pShadowSprite = NULL;
	}

	if( NUM_GS > 1 )
	{
	if (g_pSpeedySpriteTexture != NULL)
	{
		g_pSpeedySpriteTexture->Release();
		g_pSpeedySpriteTexture = NULL;
	}
	if (g_pSpeedySprite != NULL)
	{
		g_pSpeedySprite->Release();
		g_pSpeedySprite = NULL;
	}
	if( NUM_GS > 2 )
	{
	if (g_pPokeySpriteTexture != NULL)
	{
		g_pPokeySpriteTexture->Release();
		g_pPokeySpriteTexture = NULL;
	}
	if (g_pPokeySprite != NULL)
	{
		g_pPokeySprite->Release();
		g_pPokeySprite = NULL;
	}
	if( NUM_GS > 3 )
	{
	if (g_pBashfulSpriteTexture != NULL)
	{
		g_pBashfulSpriteTexture->Release();
		g_pBashfulSpriteTexture = NULL;
	}
	if (g_pBashfulSprite != NULL)
	{
		g_pBashfulSprite->Release();
		g_pBashfulSprite = NULL;
	}
	}}}

	if (g_pPacManSpriteTexture != NULL)
	{
		g_pPacManSpriteTexture->Release();
		g_pPacManSpriteTexture = NULL;
	}
	if (g_pPacManSprite != NULL)
	{
		g_pPacManSprite->Release();
		g_pPacManSprite = NULL;
	}

	//Release resources from game object surfaces
	if (g_pGameOverSurface != NULL)
	{
		g_pGameOverSurface->Release();
		g_pGameOverSurface = NULL;
	}

	if (g_pScreenSurface != NULL)
	{
		g_pScreenSurface->Release();
		g_pScreenSurface = NULL;
	}

	if (g_pBackSurface != NULL)
	{
		g_pBackSurface->Release();
		g_pBackSurface = NULL;
	}

	//Release Direct3D device interface
	if (g_pDevice != NULL)
	{
		g_pDevice->Release();
		g_pDevice = NULL;
	}

	//Release Direct3D interface
	if (g_pD3D != NULL)
	{
		g_pD3D->Release();
		g_pD3D = NULL;
	}

	return S_OK;
}


////////////////////////////////////////////////////////////////////////////////////////////
// The game's main frame rendering function
////////////////////////////////////////////////////////////////////////////////////////////
HRESULT GameRender()
{
	HRESULT hr = 0x0000;
	int nFontHeight = 0;
	RECT FontRect;			//Rectangle for specifying fonts

	//Declare a string buffer (array of 256 characters)
	char strBuffer[16];

	//Declare locked rect structure
	D3DLOCKED_RECT Locked;

	//Test the device exists
	if (g_pDevice == NULL)
		return E_FAIL;
	
	//Use the device interface to clear the back buffer (render target)	
	if (FAILED(hr = g_pDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 25), 1.0f, 0)))
		return hr;

	//Call BeginScene() before any rendering
	if (FAILED(hr = g_pDevice->BeginScene()))
		return hr;

//	//Validate the device to ensure it is ready for rendering
	if (FAILED(hr = ValidateDevice()))
		return hr;

	//Copy the background surface to the back buffer:
	if (FAILED(hr = CopySurfaceToSurface(0, g_pScreenSurface, 0, g_pBackSurface, 0,
		                                 D3DCOLOR_ARGB(0, 255, 0, 255))))
		return hr;

	//Draw the PacMan sprite to the back buffer
	g_pPacManSprite->Begin(D3DXSPRITE_ALPHABLEND);
	g_pPacManSprite->Draw(g_pPacManSpriteTexture, &g_PacManRect, NULL, &g_PacmanSpritePos, D3DCOLOR_ARGB(255, 255, 255, 255));
	g_pPacManSprite->End();

	//Draw the "Shadow" ghost sprite to the back buffer
	if(!GhostAttack && ShadowEaten && GhostFleeTimer > 35)
	{
		//draw nothing
	}else{
		g_pShadowSprite->Begin(D3DXSPRITE_ALPHABLEND);
		g_pShadowSprite->Draw(g_pShadowSpriteTexture, &g_GhostRect, NULL, &g_ShadowSpritePos, D3DCOLOR_ARGB(255, 255, 255, 255));
		g_pShadowSprite->End();
	}

	if( NUM_GS > 1 )
	{
	//Draw the "Speedy" ghost sprite to the back buffer
	if(!GhostAttack && SpeedyEaten && GhostFleeTimer > 35)
	{
		//draw nothing
	}else{
		g_pShadowSprite->Begin(D3DXSPRITE_ALPHABLEND);
		g_pShadowSprite->Draw(g_pSpeedySpriteTexture, &g_GhostRect, NULL, &g_SpeedySpritePos, D3DCOLOR_ARGB(255, 255, 255, 255));
		g_pShadowSprite->End();
	}

	if( NUM_GS > 2 )
	{
	//Draw the "Pokey" ghost sprite to the back buffer
	if(!GhostAttack && PokeyEaten && GhostFleeTimer > 35)
	{
		//draw nothing
	}else{
		g_pShadowSprite->Begin(D3DXSPRITE_ALPHABLEND);
		g_pShadowSprite->Draw(g_pPokeySpriteTexture, &g_GhostRect, NULL, &g_PokeySpritePos, D3DCOLOR_ARGB(255, 255, 255, 255));
		g_pShadowSprite->End();
	}

	if( NUM_GS > 3 )
	//Draw the "Bashful" ghost sprite to the back buffer
	if(!GhostAttack && BashfulEaten && GhostFleeTimer > 35)
	{
		//draw nothing
	}else{
		g_pShadowSprite->Begin(D3DXSPRITE_ALPHABLEND);
		g_pShadowSprite->Draw(g_pBashfulSpriteTexture, &g_GhostRect, NULL, &g_BashfulSpritePos, D3DCOLOR_ARGB(255, 255, 255, 255));
		g_pShadowSprite->End();
	}
	}}

	if (!CherryEaten)
	{
		if(g_pCherrySprite == NULL)
		{
			D3DXIMAGE_INFO d3dxImageInfo;
			D3DXCreateTextureFromFileEx(g_pDevice,
												"resources/cherry.bmp",
												D3DX_DEFAULT,
												D3DX_DEFAULT,
												D3DX_FROM_FILE,
												0, 
												D3DFMT_FROM_FILE,
												D3DPOOL_MANAGED, 
												D3DX_FILTER_NONE,
												D3DX_FILTER_NONE,
												D3DCOLOR_XRGB(255, 0, 255),
												&d3dxImageInfo,
												NULL,
												&g_pCherrySpriteTexture);
			D3DXCreateSprite(g_pDevice, &g_pCherrySprite);
		}
		g_pCherrySprite->Begin(D3DXSPRITE_ALPHABLEND);
		g_pCherrySprite->Draw(g_pCherrySpriteTexture, NULL, NULL, &g_CherrySpritePos, D3DCOLOR_XRGB(255, 0, 255));
		g_pCherrySprite->End();
	}

	//Display the "dots" and "power pills"
	DrawDots();

	//Lock the back buffer surface
	if (FAILED(hr = g_pBackSurface->LockRect(&Locked, 0, 0)))
		return hr;
	
	//Initialise the RECT structure for the "Score" text
	FontRect.left		= 10;
	FontRect.top		= 10;
	FontRect.right		= 100;
	FontRect.bottom		= 40;

	//Render the "Score" text to the back buffer
	nFontHeight = g_pFont->DrawText(NULL,
									"Score",
									-1,
									&FontRect,
									DT_LEFT | DT_WORDBREAK,
									D3DCOLOR_ARGB(255, 0, 255, 0)); 
	if (nFontHeight == 0)
		return E_FAIL;

	//Convert the integer score value to a string and display the string 
	sprintf_s( strBuffer, 16, "%d", Score );

	//Initialise the RECT structure for the actual score 
	FontRect.left		= 10;
	FontRect.top		= 30;
	FontRect.right		= 95;
	FontRect.bottom		= 100;

	//Render the "Score" text to the back buffer
	nFontHeight = g_pFont->DrawText(NULL,
									strBuffer,
									-1,
									&FontRect,
									DT_CENTER | DT_WORDBREAK,
									D3DCOLOR_ARGB(255, 0, 255, 0)); 
	if (nFontHeight == 0)
		return E_FAIL;

	//Initialise the RECT structure for the actual score 
	FontRect.left		= 10;
	FontRect.top		= 60;
	FontRect.right		= 65;
	FontRect.bottom		= 100;

	//Render the "Lives" text to the back buffer
	nFontHeight = g_pFont->DrawText(NULL,
									"Lives",
									-1,
									&FontRect,
									DT_LEFT | DT_WORDBREAK,
									D3DCOLOR_ARGB(255, 0, 255, 0)); 
	if (nFontHeight == 0)
		return E_FAIL;

	//Convert the integer number of lives value to a string and display the string
	sprintf_s( strBuffer, 16, "%d", GameLives );

	//Initialise the RECT structure for the actual score 
	FontRect.left		= 10;
	FontRect.top		= 80;
	FontRect.right		= 60;
	FontRect.bottom		= 100;

	//Render the actual number of lives to the back buffer
	nFontHeight = g_pFont->DrawText(NULL,
									strBuffer,
									-1,
									&FontRect,
									DT_CENTER | DT_WORDBREAK,
									D3DCOLOR_ARGB(255, 0, 255, 0)); 
	if (nFontHeight == 0)
		return E_FAIL;

	//Initialise the RECT structure for the "Level" text
	FontRect.left		= 10;
	FontRect.top		= 110;
	FontRect.right		= 70;
	FontRect.bottom		= 130;

	//Render the "Score" text to the back buffer
	nFontHeight = g_pFont->DrawText(NULL,
									"Level",
									-1,
									&FontRect,
									DT_LEFT | DT_WORDBREAK,
									D3DCOLOR_ARGB(255, 0, 255, 0)); 
	if (nFontHeight == 0)
		return E_FAIL;

	//Convert the integer Level value to a string and display the string
	sprintf_s( strBuffer, 16, "%d", Level+1 );

	//Initialise the RECT structure for the actual score 
	FontRect.left		= 70;
	FontRect.top		= 110;
	FontRect.right		= 100;
	FontRect.bottom		= 130;

	//Render the actual number of lives to the back buffer
	nFontHeight = g_pFont->DrawText(NULL,
									strBuffer,
									-1,
									&FontRect,
									DT_CENTER | DT_WORDBREAK,
									D3DCOLOR_ARGB(255, 0, 255, 0)); 
	if (nFontHeight == 0)
		return E_FAIL;

	//Unlock the back buffer surface
	g_pBackSurface->UnlockRect();

	//Call EndScene because rendering to the device is finished
	if (FAILED(hr = g_pDevice->EndScene()))
		return hr;

	//Present the contents of the back buffer to the frame buffer
	if (FAILED(hr = g_pDevice->Present(NULL, NULL, NULL, NULL)))
		return hr;

	//Default return value
	return S_OK;
}

////////////////////////////////////////////////////////////////////////////////////////////
// The function for checking PacMans movements
////////////////////////////////////////////////////////////////////////////////////////////
void MovePacMan()
{
	//Initialise local vars
	FLOAT xPosInitial = 0.0f;
	FLOAT yPosInitial = 0.0f;
	int	xPosIndex	= 0;
	int yPosIndex	= 0;

	bool bCheckMove = false;

	//Obtain the last keyboard press
	lpdikey->GetDeviceState(256, (LPVOID)keyboard_state);

	//Obtain x and y pos values from the PacMan sprite pointer
	xPosInitial = g_PacmanSpritePos.x;
	yPosInitial = g_PacmanSpritePos.y;

	//Adjust position values to obtain PacMap index values
	xPosIndex = (int)((xPosInitial - GRIDTLX)/SPRITE_WIDTH);
	yPosIndex = (int)((yPosInitial - GRIDTLY)/SPRITE_HEIGHT);

	if (keyboard_state[DIK_UP]) //"Up" arrow has been pressed    
	{
		// set the correct animation for up
		RowAnimation = 2; 
		if (PacMap[xPosIndex][yPosIndex-1] > 2) 
		{
			yPosIndex--;
			PacMoves = 0;
			bCheckMove = true;
//			print("\n****  MOVED NORTH  ****\n");
		}
	}
	else if (keyboard_state[DIK_DOWN]) //"Down" arrow has been pressed  
	{
		RowAnimation = 3;
		if (PacMap[xPosIndex][yPosIndex+1] > 2) 
		{
			yPosIndex++;
			PacMoves = 1;
			bCheckMove = true;
//			print("\n****  MOVED SOUTH  ****\n");
		}
	}
	else if (keyboard_state[DIK_RIGHT]) //"Right" arrow has been pressed   
	{
		RowAnimation = 0;
		if (PacMap[xPosIndex+1][yPosIndex] > 2) 
		{
			xPosIndex++;
			PacMoves = 2;
			bCheckMove = true;
//			print("\n****  MOVED EAST  ****\n");
		}
	}
	else if (keyboard_state[DIK_LEFT])  
	{
		RowAnimation = 1;
		if (PacMap[xPosIndex-1][yPosIndex] > 2) 
		{
			xPosIndex--;
			PacMoves = 3;
			bCheckMove = true;
//			print("\n****  MOVED WEST  ****\n");
		}
	}

	//If a legal move occurs, check its consequences
	if( !bCheckMove )
		PacMoves = -1;
	else
	{
		if (PacMap[xPosIndex][yPosIndex] == 4)
		{
			PacMap[xPosIndex][yPosIndex] = 6; //code for eaten dot
			Score += 10;
			--NumDots;

			//Play "Spell.wav"
			g_pWavBuffer->Play(NULL, NULL, NULL);
		}
		else if (PacMap[xPosIndex][yPosIndex] == 5)
		{
			PacMap[xPosIndex][yPosIndex] = 6; //code for eaten dot

			//Set the texture pointer for the ghost sprites
			g_pShadowSpriteTexture = g_pGhostFleeSpriteTexture;
			if( NUM_GS > 1 )
			{
				g_pSpeedySpriteTexture = g_pGhostFleeSpriteTexture;
				if( NUM_GS > 2 )
				{
					g_pPokeySpriteTexture = g_pGhostFleeSpriteTexture;
					if( NUM_GS > 3 )
						g_pBashfulSpriteTexture = g_pGhostFleeSpriteTexture;
			}}

			GhostAttack = false; // ghosts now run away - or at least need to be programmed to
			gstTik += (float)pacTik * 0.5;
			PacMap[9][8 + (Level%2)] = PacMap[10][8 + (Level%2)] = 6;

			Score+=50;
			--NumPills;

			//Play "Spell.wav"
			g_pWavBuffer->Play(NULL, NULL, NULL);
			g_pWavBuffer->Play(NULL, NULL, NULL);
		}
		else if ((PacMap[xPosIndex][yPosIndex] == 3) && (keyboard_state[DIK_LEFT]))
		{
			xPosIndex = 18;
			if (PacMap[xPosIndex][yPosIndex] == 4)
			{
				PacMap[xPosIndex][yPosIndex] = 6; //code for eaten dot
				Score += 10;
				--NumDots;

				//Play "Spell.wav"
				g_pWavBuffer->Play(NULL, NULL, NULL);
			}
		}
		else if ((PacMap[xPosIndex][yPosIndex] == 3) && (keyboard_state[DIK_RIGHT]))
		{
			xPosIndex = 1;
			if (PacMap[xPosIndex][yPosIndex] == 4)
			{
				PacMap[xPosIndex][yPosIndex] = 6; //code for eaten dot
				Score += 10;
				--NumDots;

				//Play "Spell.wav"
				g_pWavBuffer->Play(NULL, NULL, NULL);
			}
		}

		//Set the PacMan sprite's position vector
		SetPositionVector(&g_PacmanSpritePos, (GRIDTLX + (xPosIndex * SPRITE_WIDTH)), (GRIDTLY + (yPosIndex * SPRITE_HEIGHT)));

		GameStates.push_back( PacState() );

		//Toggle the check move bool
		bCheckMove = false;
	}

	return;
}

////////////////////////////////////////////////////////////////////////////////////////////
// The function for checking ghost movements
////////////////////////////////////////////////////////////////////////////////////////////
void MoveGhosts()
{
	//"Shadow" Ghost Sprite
	g_GhostMvDir[0] = SetSpritePosition(&g_ShadowSpritePos, 0);
	newRandom[0] = 0;	newRandom[1] = 1;	newRandom[2] = 2;	newRandom[3] = 3;

	if( NUM_GS > 1 )
	{
		//"Speedy" Ghost Sprite
		g_GhostMvDir[1] = SetSpritePosition(&g_SpeedySpritePos, 1);
		newRandom[0] = 0;	newRandom[1] = 1;	newRandom[2] = 2;	newRandom[3] = 3;

	if( NUM_GS > 2 )
	{
		//"Pokey" Ghost Sprite
		g_GhostMvDir[2] = SetSpritePosition(&g_PokeySpritePos, 2);
		newRandom[0] = 0;	newRandom[1] = 1;	newRandom[2] = 2;	newRandom[3] = 3;

	if( NUM_GS > 3 )
		//"Bashful" Ghost Sprite
		g_GhostMvDir[3] = SetSpritePosition(&g_BashfulSpritePos, 3);
	}}
}


////////////////////////////////////////////////////////////////////////////////////////////
// Function for displaying the 'dots'
////////////////////////////////////////////////////////////////////////////////////////////
void DrawDots()
{
	//Declare D3DRECT structure
	D3DRECT Rect;

	//Initialise loop variables
	int row, column;
	
	//Display the dots according to specified positions in PacMap array using Direct3D device Clear()
	for (row = 0; row < 20; row++)
	{	
		for (column = 0; column < 20; column++)
		{
			//If PacMap[row][column] = 4 then draw a dot
			if (PacMap[row][column] == 4)
			{
				//Set up the D3DRECT structure
				Rect.x1 = GRIDTLX+10 + row*20;
				Rect.x2 = GRIDTLX+12 + row*20;
				Rect.y1 = GRIDTLY+10 + column*20;
				Rect.y2 = GRIDTLY+11 + column*20;

				//Use the Clear function to draw the white dot
				g_pDevice->Clear(1, &Rect, D3DCLEAR_TARGET, D3DCOLOR_XRGB(255, 255, 255), 0.0f, 0);   
			}
			
			//If PacMap[row][column] = 5 then draw a large dot (i.e. a power pill)
			if (PacMap[row][column] == 5)
			{
				//Set up the D3DRECT structure
				Rect.x1 = GRIDTLX+8 + row*20;
				Rect.x2 = GRIDTLX+14 + row*20;
				Rect.y1 = GRIDTLY+8 + column*20;
				Rect.y2 = GRIDTLY+14 + column*20;

				//Use the Clear function to draw the white dot
				if( GhostAttack )
					g_pDevice->Clear(1, &Rect, D3DCLEAR_TARGET, D3DCOLOR_XRGB(255, 255, 255), 0.0f, 0);
				else
					g_pDevice->Clear(1, &Rect, D3DCLEAR_TARGET, D3DCOLOR_XRGB(128, 128, 255), 0.0f, 0);
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// Collision detection routine
////////////////////////////////////////////////////////////////////////////////////////////
void CollisionCheck()
{
	//Temporary toggle (boolean) as cherry eaten flag (local)
	bool bCherryGone = false;

	//Temporary toggle (boolean) for pacman killed by a ghost
	bool bPacManGone = false;

	//Check for PacMan or Ghosts eating the cherry
	if(g_pCherrySprite != NULL)
	{
		if (TRUE == CheckForCollision(&g_PacmanSpritePos, &g_CherrySpritePos))
		{
			bCherryGone = true;
			GameLives++;
		}
		else if (GhostAttack && CheckForCollision(&g_ShadowSpritePos, &g_CherrySpritePos))
			bCherryGone = true;
/** /	else if (TRUE == CheckForCollision(&g_SpeedySpritePos, &g_CherrySpritePos))
			bCherryGone = true;
		else if (TRUE == CheckForCollision(&g_PokeySpritePos, &g_CherrySpritePos))
			bCherryGone = true;
		else if (TRUE == CheckForCollision(&g_BashfulSpritePos, &g_CherrySpritePos))
			bCherryGone = true;
/**/
		//Get rid of the cherry if it has been eaten by someone
		if (bCherryGone)
		{
			if (g_pCherrySpriteTexture != NULL)
			{
				g_pCherrySpriteTexture->Release();
				g_pCherrySpriteTexture = NULL;
			}

			if (g_pCherrySprite != NULL)
			{
				g_pCherrySprite->Release();
				g_pCherrySprite = NULL;
			}
			//Set the global cherry variable
			CherryEaten = true;
		}
	}

	//Check for collision between PacMan and the Ghosts:
	if( TRUE == CheckForCollision(&g_PacmanSpritePos, &g_ShadowSpritePos) )
	{
		if (GhostAttack)
			bPacManGone = true;
		else if(!ShadowEaten)
		{
			//PacMan has killed a ghost:
			//Increase PacMan's score
			Score += 100 * RunOfGhostKills;
			RunOfGhostKills *= 2;
			ShadowEaten = true;
		}
	}
	else if( NUM_GS > 1 )
	{
		if( TRUE == CheckForCollision(&g_PacmanSpritePos, &g_SpeedySpritePos) )
		{
			if (GhostAttack)
				bPacManGone = true;
			else if(!SpeedyEaten)
			{
				//PacMan has killed a ghost:
				//Increase PacMan's score
				Score += 100 * RunOfGhostKills;
				RunOfGhostKills *= 2;
				SpeedyEaten = true;
			}
		}else if( NUM_GS > 2 )
		{
			if( TRUE == CheckForCollision(&g_PacmanSpritePos, &g_PokeySpritePos) )
			{
				if (GhostAttack)
					bPacManGone = true;
				else if(!PokeyEaten)
				{
					//PacMan has killed a ghost:
					//Increase PacMan's score
					Score += 100 * RunOfGhostKills;
					RunOfGhostKills *= 2;
					PokeyEaten = true;
				}
			}else if( NUM_GS > 3 )
				if( TRUE == CheckForCollision(&g_PacmanSpritePos, &g_BashfulSpritePos) )
				{
					if (GhostAttack)
						bPacManGone = true;
					else if(!BashfulEaten)
					{
						//PacMan has killed a ghost:
						//Increase PacMan's score
						Score += 100 * RunOfGhostKills;
						RunOfGhostKills *= 2;
						BashfulEaten = true;
					}
				}
	}}

	//Check if PacMan has been killed by a ghost
	if (bPacManGone)
	{
		//Decrement the number of lives
		GameLives--;

		//Reset PacMan's starting position
		SetPositionVector(&g_PacmanSpritePos, (GRIDTLX + (PACMANSPRITE_START_POS_X * SPRITE_WIDTH)),
											  (GRIDTLY + ((PACMANSPRITE_START_POS_Y - (9*(Level%2))) * SPRITE_HEIGHT)));
	}

	//Check if GameOver should be set
	if (GameLives == 0) 
		GameOver = true;
}

////////////////////////////////////////////////////////////////////////////////////////////
// Function to draw the game over screen
////////////////////////////////////////////////////////////////////////////////////////////
HRESULT DrawEnd()
{
	HRESULT hr = 0x0000;
	POINT pt = {120,40};

	//Test the device exists
	if (g_pDevice == NULL)
		return E_FAIL;
	
	//Use the device interface to clear the back buffer (render target)	
	if (FAILED(hr = g_pDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 25), 1.0f, 0)))
		return hr;

	//Call BeginScene() before any rendering
	if (FAILED(hr = g_pDevice->BeginScene()))
		return hr;

	if (FAILED(hr = CopySurfaceToSurface(0, g_pGameOverSurface, &pt, g_pBackSurface, true, D3DCOLOR_ARGB(255, 255, 0, 255))))
		return hr;

	/* @Game Release addition
		*/
	int nFontHeight = 0;
	RECT FontRect;			//Rectangle for specifying fonts
	char strBuffer[16];		//Declare a string buffer (array of 256 characters)

	//Initialise the RECT structure for the "Score" text
	FontRect.left		= 170;
	FontRect.top		= 100;
	FontRect.right		= 350;
	FontRect.bottom		= 140;

	//Render the "Score" text to the back buffer
	nFontHeight = g_pFont->DrawText(NULL,
									"Score",
									-1,
									&FontRect,
									DT_LEFT | DT_WORDBREAK,
									D3DCOLOR_ARGB(255, 0, 255, 0)); 
	if (nFontHeight == 0)
		return E_FAIL;

	//Convert the integer score value to a string and display the string 
	sprintf_s( strBuffer, 16, "%d", Score );

	//Initialise the RECT structure for the actual score 
	FontRect.left		= 170;
	FontRect.top		= 140;
	FontRect.right		= 350;
	FontRect.bottom		= 200;

	//Render the "Score" text to the back buffer
	nFontHeight = g_pFont->DrawText(NULL,
									strBuffer,
									-1,
									&FontRect,
									DT_LEFT | DT_WORDBREAK,
									D3DCOLOR_ARGB(255, 0, 255, 0)); 
	if (nFontHeight == 0)
		return E_FAIL;

	//Call EndScene because rendering to the device is finished
	if (FAILED(hr = g_pDevice->EndScene()))
		return hr;

	//Present the contents of the back buffer to the frame buffer
	if (FAILED(hr = g_pDevice->Present(NULL, NULL, NULL, NULL)))
		return hr;

	sleep(100);

	if(GetAsyncKeyState(VK_ESCAPE))
		PostQuitMessage(0);

	return S_OK;
}

////////////////////////////////////////////////////////////////////////////////////////////
// Function to load a bitmap to a surface
////////////////////////////////////////////////////////////////////////////////////////////
HRESULT LoadBitmapToSurface(const char* strPath, LPDIRECT3DSURFACE9* pSurface, LPDIRECT3DDEVICE9 pDevice)
{
	//Result handle for returning errors
	HRESULT hr = 0x0000;

	//Handle to bitmap
	HBITMAP hBmp = NULL;

	//Bitmap object structure
	BITMAP Bmp;

	//Return value
	int ret = 0;
	
	//Use the GDI to load the bitmap
	hBmp = (HBITMAP)LoadImage(NULL, strPath, IMAGE_BITMAP, 0, 0, 
		                      LR_LOADFROMFILE | LR_CREATEDIBSECTION );
	if(hBmp == NULL)
		return E_FAIL;

	//Obtain information about the bitmap object
	ret = GetObject(hBmp, sizeof(BITMAP), &Bmp);
	if (ret == 0)
		return E_FAIL;

	//Now we have the bitmap information, unload the bitmap from memory
	DeleteObject(hBmp);

	//We need to release the previous surface if one exists. This is to prevent
	//memory leaks occurring when the ghost sprites are set up (as they use the one 
	//global surface for initally loading their bitmaps, the surface is repeatedly
	//re-created by the CreateOffscreenPlainSurface call for each ghost sprite. As such,
	//before a new surface is created for the ghost sprite, the previous surface 
	//if one exists, should be released first
	if (*pSurface != NULL)
		(*pSurface)->Release();


	//Create the off-screen render surface based on loaded bitmap information
 	if (FAILED(hr = pDevice->CreateOffscreenPlainSurface(Bmp.bmWidth, Bmp.bmHeight, D3DFMT_A8R8G8B8,
		                                                 D3DPOOL_SYSTEMMEM, pSurface, NULL)))
		return hr;


	//Re-load the image directly to the newly created surface using Direct3D
	if (FAILED(hr = D3DXLoadSurfaceFromFile(*pSurface, NULL, NULL, strPath, NULL, 
		                                    D3DX_FILTER_NONE, 0, NULL)))
		return hr;

	//Default return value
	return S_OK;
}

////////////////////////////////////////////////////////////////////////////////////////////
// Function to copy a sprite from one surface to another surface
////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CopySurfaceToSurface(RECT* pSourceRect, LPDIRECT3DSURFACE9 pSourceSurface, POINT* pDestPoint, 
								LPDIRECT3DSURFACE9 pDestSurface, BOOL bTransparent, D3DCOLOR ColorKey)
{
	HRESULT hr = 0x0000;

	//Source rectangle structure
	RECT SourceRect;

	//Destination point structure
	POINT DestPoint;
	
	//Structures to hold information about locked surfaces
	D3DLOCKED_RECT LockedSourceRect;
	D3DLOCKED_RECT LockedDestRect;

	//Structure containing source surface information
	D3DSURFACE_DESC d3dsdSource;

	//Structure containing destination surface information
	D3DSURFACE_DESC d3dsdDest;

	//Initialise 32-bit pointers to the source and destination surface data
	DWORD* pSourceData = NULL;
	DWORD* pDestData = NULL;

	//Initialise offsets 
	int nSourceOffset = 0;
	int nDestOffset = 0;

	//Validate the source and destination surfaces
	if ((pSourceSurface == NULL)||(pDestSurface == NULL)) 
		return E_FAIL;

	//Obtain information about the source surface
	if (FAILED(hr = pSourceSurface->GetDesc(&d3dsdSource)))
		return hr;
	
	//Obtain information about the destination surface
	if (FAILED(hr = pDestSurface->GetDesc(&d3dsdDest)))
		return hr;

	//Copy the specified source rectangle into the RECT structure or else set the rectangle 
	//to cover the entire source surface
	if (pSourceRect  != NULL)
		SourceRect = *pSourceRect;
	else
		SetRect(&SourceRect, 0, 0, d3dsdSource.Width, d3dsdSource.Height);

	//Copy the destination point into the POINT structure or else set it to (0,0)
	if (pDestPoint != NULL)
		DestPoint = *pDestPoint;
	else
		DestPoint.x = DestPoint.y = 0;

	//Lock the source surface
	if (FAILED(hr = pSourceSurface->LockRect(&LockedSourceRect, 0, D3DLOCK_READONLY)))
		return hr;
	
	//Lock the destination surface
	if (FAILED(hr = pDestSurface->LockRect(&LockedDestRect, 0, 0)))
	{
		//Unlock the source surface before returning
		pSourceSurface->UnlockRect();
		return hr;
	}

	//Adjust pitch to ensure it is DWORD compatible (4 bytes per DWORD)
	LockedSourceRect.Pitch /= 4;
	LockedDestRect.Pitch /= 4;

	//Obtain pointers to the source and destination surface data
	pSourceData = (DWORD*)LockedSourceRect.pBits;
	pDestData = (DWORD*)LockedDestRect.pBits;

	//Obtain the offsets into the source and destination surfaces
	nSourceOffset = SourceRect.top * LockedSourceRect.Pitch + SourceRect.left;
	nDestOffset = DestPoint.y * LockedDestRect.Pitch + DestPoint.x;

	//Copy the pixels from source surface to destination surface, adjusting according to
	//specified transparency setting and colour key
	for (int nSrcRow = 0; nSrcRow < SourceRect.bottom; nSrcRow++)
	{
		for (int nSrcCol = 0; nSrcCol < SourceRect.right; nSrcCol++)
		{
			//Check for transparency 
			if (bTransparent)
			{
				//Check for colour key match with current source pixel. If a match occurs
				//then do not copy the pixel
				if (pSourceData[nSourceOffset] != ColorKey)
					pDestData[nDestOffset] = pSourceData[nSourceOffset];
			}
			else //No transparency was specified so copy source pixel to destination
				pDestData[nDestOffset] = pSourceData[nSourceOffset];

			//Increment the source and destination offsets to the next pixel
			nSourceOffset++;
			nDestOffset++;
		
		} //End inner pixel copy loop
 
		//Adjust the source and destination offsets to the next row of the image
		//(offset = offset + pitch - source image width)
		nSourceOffset += LockedSourceRect.Pitch - SourceRect.right;
		nDestOffset += LockedDestRect.Pitch - SourceRect.right;
	}

	//Unlock the source and destination surfaces once copy operation is completed
	pSourceSurface->UnlockRect();
	pDestSurface->UnlockRect();
	
	// Return success
	return S_OK;
}

////////////////////////////////////////////////////////////////////////////////////////////
// Function to validate the Direct3D device
////////////////////////////////////////////////////////////////////////////////////////////
HRESULT ValidateDevice()
{
	HRESULT hr = 0x0000;
	
	//Check the current state of the device 
	if (FAILED(hr = g_pDevice->TestCooperativeLevel()))
	{
		//Return appropriately: 
		//If the device is lost then return a failure
		if (hr == D3DERR_DEVICELOST)
			return E_FAIL;

		//Attempt to reset the device if possible
		if (hr == D3DERR_DEVICENOTRESET)
		{
			//Release the back buffer surface to allow it to be re-created
			g_pBackSurface->Release();

			//Attempt to reset the device
			if (FAILED(hr = g_pDevice->Reset(&g_OrigD3DParams)))
			{
				//Exit if a reset was not possible
				PostQuitMessage(E_FAIL);
				return E_FAIL;
			}

			//Re-acquire the pointer to the freshly reset back buffer
			if (FAILED(hr = g_pDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &g_pBackSurface)))
			{
				//Exit if unable to acquire back buffer
				PostQuitMessage(0);
				return hr;
			}

			//Use the device interface to clear the back buffer (render target)
			if (FAILED(hr = g_pDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0)))
				return hr;
		}


	}

	return S_OK;
}

void InitGameAgents()
{
	//Initialise the score
	Score = 0;

	//Initialise the PacMan sprite's starting position
	ZeroMemory(&g_PacmanSpritePos, sizeof(D3DXVECTOR3));
	SetPositionVector(&g_PacmanSpritePos, (GRIDTLX + (PACMANSPRITE_START_POS_X * SPRITE_WIDTH)),
								          (GRIDTLY + (PACMANSPRITE_START_POS_Y * SPRITE_HEIGHT)));

	//Initialise the "Shadow" sprite's starting position
	ZeroMemory(&g_ShadowSpritePos, sizeof(D3DXVECTOR3));
	SetPositionVector(&g_ShadowSpritePos, (GRIDTLX + (SHADOWSPRITE_START_POS_X * SPRITE_WIDTH)),
								          (GRIDTLY + (SHADOWSPRITE_START_POS_Y * SPRITE_HEIGHT)));	
	
	if( NUM_GS > 1 )
	{
		//Initialise the "Speedy" sprite's starting position
		ZeroMemory(&g_SpeedySpritePos, sizeof(D3DXVECTOR3));
		SetPositionVector(&g_SpeedySpritePos, (GRIDTLX + (SPEEDYSPRITE_START_POS_X * SPRITE_WIDTH)),
											  (GRIDTLY + (SPEEDYSPRITE_START_POS_Y * SPRITE_HEIGHT)));
		if( NUM_GS > 2 )
		{
			//Initialise the "Pokey" sprite's starting position
			ZeroMemory(&g_PokeySpritePos, sizeof(D3DXVECTOR3));
			SetPositionVector(&g_PokeySpritePos, (GRIDTLX + (POKEYSPRITE_START_POS_X * SPRITE_WIDTH)),
												  (GRIDTLY + (POKEYSPRITE_START_POS_Y * SPRITE_HEIGHT)));
			if( NUM_GS > 3 )
			{
				//Initialise the "Bashful" sprite's starting position
				ZeroMemory(&g_BashfulSpritePos, sizeof(D3DXVECTOR3));
				SetPositionVector(&g_BashfulSpritePos, (GRIDTLX + (BASHFULSPRITE_START_POS_X * SPRITE_WIDTH)),
													   (GRIDTLY + (BASHFULSPRITE_START_POS_Y * SPRITE_HEIGHT)));
	}}}

	//Initialise the cherry sprite's starting position
	ZeroMemory(&g_CherrySpritePos, sizeof(D3DXVECTOR3));
	SetPositionVector(&g_CherrySpritePos, (GRIDTLX + (CHERRYSPRITE_START_POS_X * SPRITE_WIDTH)),
								          (GRIDTLY + (CHERRYSPRITE_START_POS_Y * SPRITE_HEIGHT)));

}

HRESULT InitDirectSound(HWND hWnd)
{
	HRESULT hr = 0x0000;

	//Create the DirectSound interface object
	if (FAILED(hr = DirectSoundCreate8(NULL, &g_pSound, NULL)))
		return hr;

	//Set the cooperative level
	if(FAILED(hr = g_pSound->SetCooperativeLevel(hWnd, DSSCL_PRIORITY)))
		return hr;

	//Set up the structure describing the audio waveform as WAV format
	WAVEFORMATEX wfex;

	//Initialise with zeros
	ZeroMemory(&wfex, sizeof(WAVEFORMATEX));

	//Load the wav file
	HMMIO hMMIOWave;		//MM I/O handle for the wave 
	MMCKINFO chunkInfo;      //Structure for holding chunk info from a RIFF (WAV) file
	MMCKINFO infileChunkInfo;
	PCMWAVEFORMAT   waveFormat;  //PCM structure for loading the wav file

	//Open the file with read permission only
	hMMIOWave = mmioOpen("resources/Spell.wav", NULL, MMIO_READ | MMIO_ALLOCBUF);
	if (hMMIOWave == NULL)
		return E_FAIL;

	//Zero the MMCKINFO structures
	ZeroMemory(&chunkInfo, sizeof(MMCKINFO));
	ZeroMemory(&infileChunkInfo, sizeof(MMCKINFO));

	//Obtain the MMCKINFO structure
    if (FAILED(hr = mmioDescend(hMMIOWave, &chunkInfo, NULL, 0)))
	{
		mmioClose(hMMIOWave, 0);
        return hr;
	}

	//Check the wave header to ensure it is a wav file
	if (chunkInfo.ckid != FOURCC_RIFF)
	{
		mmioClose(hMMIOWave, 0);
        return hr;
	}
	
	if (chunkInfo.fccType != mmioFOURCC('W', 'A', 'V', 'E'))
	{
		mmioClose(hMMIOWave, 0);
        return E_FAIL;
	}

	//Find the format ('fmt') chunk in the input file
	infileChunkInfo.ckid = mmioFOURCC('f','m','t',' ');
	if (FAILED(hr = mmioDescend(hMMIOWave, &infileChunkInfo, &chunkInfo, MMIO_FINDCHUNK)))
	{
		mmioClose(hMMIOWave, 0);
        return hr;
	}

	//Check the size of the 'fmt' chunk is as larage as PCMWAVEFORMAT
	if (infileChunkInfo.cksize < (LONG)sizeof(PCMWAVEFORMAT))
	{
		mmioClose(hMMIOWave, 0);
        return E_FAIL;
	}

	//Read the 'fmt' chunk
	if (mmioRead(hMMIOWave, (HPSTR) &waveFormat, sizeof(waveFormat)) != sizeof(waveFormat))
	{
		mmioClose(hMMIOWave, 0);
        return E_FAIL;
	}

	//Copy the bytes from the pcm structure to the waveformatex structure
	memcpy(&wfex, &waveFormat, sizeof(WAVEFORMATEX));
	
	//Set the size to zero
	wfex.cbSize = 0;

    //Ascend the input file out of the 'fmt ' chunk.
    if (0 != mmioAscend(hMMIOWave, &infileChunkInfo, 0))
	{
		mmioClose(hMMIOWave, 0);
        return E_FAIL;
	}


	//Find the wavefile's data chunk (move file poisiton pointer)
	if (-1 == mmioSeek(hMMIOWave, chunkInfo.dwDataOffset+sizeof(FOURCC), SEEK_SET))
	{
		mmioClose(hMMIOWave, 0);
        return E_FAIL;
	}

	infileChunkInfo.ckid = mmioFOURCC('d','a','t','a');

	if (FAILED(hr = mmioDescend(hMMIOWave, &infileChunkInfo, &chunkInfo, MMIO_FINDCHUNK)))
	{
		mmioClose(hMMIOWave, 0);
        return E_FAIL;
	}

	//Store the size of the wavefile
	int waveSize = infileChunkInfo.cksize;

	//Create the DirectSound buffer: 

	//The DSBUFFERDESC structure is used in the creation of the sound buffer
    DSBUFFERDESC dsbd;

	//All fields of the structure are initially zeroed
    ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
    dsbd.dwSize        = sizeof(DSBUFFERDESC);
    dsbd.dwFlags       = DSBCAPS_CTRLVOLUME | DSBCAPS_GLOBALFOCUS; 
    dsbd.dwBufferBytes = waveSize;
    dsbd.lpwfxFormat   = &wfex;

	//Create a secondary buffer for a single static sound (spell.wav)
	//Note: DirectSound creates and manges the primary sound buffer for the 
	//application (which is used to mix/send the sound/stream to the device)
	if (FAILED(hr = g_pSound->CreateSoundBuffer(&dsbd, &g_pWavBuffer, NULL)))
		return hr;

	//Copy the wav into the buffer:
	VOID*   pLockedBuffer		= NULL;		//Pointer to the locked buffer memory
	DWORD   lockedBufferSize	= 0;		//The size of the locked buffer

	//Lock the buffer
	if (FAILED(hr = g_pWavBuffer->Lock(0, waveSize, &pLockedBuffer, &lockedBufferSize, NULL, NULL, 0L)))
		return hr;

	//Reset the pointer to the beginning of the wav file
	if (-1 == mmioSeek(hMMIOWave, chunkInfo.dwDataOffset+sizeof(FOURCC), SEEK_SET))
	{
		mmioClose(hMMIOWave, 0);
        return E_FAIL;
	}

	infileChunkInfo.ckid = mmioFOURCC('d','a','t','a');

	if (FAILED(hr = mmioDescend(hMMIOWave, &infileChunkInfo, &chunkInfo, MMIO_FINDCHUNK)))
	{
		mmioClose(hMMIOWave, 0);
        return E_FAIL;
	}

	//Read the data from the file into the buffer:
	MMIOINFO infoIn;	//Structure to hold current status of hMMIOWave
	DWORD dwReadSize = 0;

	if (pLockedBuffer == NULL || lockedBufferSize == 0)
	{
		mmioClose(hMMIOWave, 0);
        return E_FAIL;
	}

	if (0 != mmioGetInfo(hMMIOWave, &infoIn, 0))
	{
		mmioClose(hMMIOWave, 0);
        return E_FAIL;
	}

	if (lockedBufferSize > infileChunkInfo.cksize)
		lockedBufferSize = infileChunkInfo.cksize;

	infileChunkInfo.cksize -= lockedBufferSize;

	for (DWORD buffOffset = 0; buffOffset < lockedBufferSize; buffOffset++)
	{
		//Copy bytes from the wav file to the buffer
		if (infoIn.pchNext == infoIn.pchEndRead)
		{
			if (0 != mmioAdvance(hMMIOWave, &infoIn, MMIO_READ))
			{
				mmioClose(hMMIOWave, 0);
				return E_FAIL;
			}

			if (infoIn.pchNext == infoIn.pchEndRead)
			{
				mmioClose(hMMIOWave, 0);
				return E_FAIL;
			}
		}

		*((BYTE*)pLockedBuffer+buffOffset) = *((BYTE*)infoIn.pchNext);
		infoIn.pchNext++;
	}

	if (0 != mmioSetInfo(hMMIOWave, &infoIn, 0))
	{
		mmioClose(hMMIOWave, 0);
        return E_FAIL;
	}

	//Unlock the buffer, the data has been copied
	if (FAILED(hr = g_pWavBuffer->Unlock(pLockedBuffer, lockedBufferSize, NULL, 0)))
	{
		mmioClose(hMMIOWave, 0);
        return E_FAIL;
	}

	//Close the wav file
	mmioClose(hMMIOWave, 0);

	return S_OK;
}

HRESULT InitD3D(HWND hWnd)
{
	HRESULT hr = 0x0000;

	//Create the Direct3D object and return the interface to it
	g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (g_pD3D == NULL)
		return E_FAIL;

	//The D3DPRESENT_PARAMETERS structure is used to create the Direct3D device:
	//All parameters are initially zeroed out
    D3DPRESENT_PARAMETERS d3dpp; 
    ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));
	
	//Specify fields of the D3DPRESENT_PARAMETERS structure
	d3dpp.Windowed						= TRUE;						//Set the display mode to Windowed
	d3dpp.SwapEffect					= D3DSWAPEFFECT_COPY;		//Set the swap effect for the back buffer (swap chain)
	d3dpp.BackBufferFormat				= D3DFMT_X8R8G8B8;			//Set the display format of the back buffer
	d3dpp.BackBufferWidth				= 640;						//Set the width of the back buffer (in pixels)
	d3dpp.BackBufferHeight				= 480;						//Set the height of the buffer (in pixels)
	d3dpp.BackBufferCount				= 1;						//Set the number of back buffers to be created (0/1,2,3)
	d3dpp.MultiSampleType				= D3DMULTISAMPLE_NONE;		//Set the type of multisampling
	d3dpp.hDeviceWindow					= hWnd;						//Set the handle of the window to render to
	d3dpp.EnableAutoDepthStencil		= TRUE;						//Set the depth buffer management
	d3dpp.AutoDepthStencilFormat		= D3DFMT_D16;				//Set the depth buffer format
	d3dpp.FullScreen_RefreshRateInHz	= D3DPRESENT_RATE_DEFAULT;	//Set the refresh rate of the display adapter
	d3dpp.PresentationInterval			= D3DPRESENT_INTERVAL_ONE;	//Set the rate at which the back buffer is presented to the front buffer
	d3dpp.Flags							= D3DPRESENTFLAG_LOCKABLE_BACKBUFFER; //Set the flags to allow a lockable back buffer to be created

	//Store a copy of the device's presentation parameters for use in device validation
	g_OrigD3DParams = d3dpp;

	//Create the Direct3D device
	if (FAILED(hr = g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, 
		                                 D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &g_pDevice)))
		return hr;

	//Use the device interface to clear the back buffer (render target)
	if (FAILED(hr = g_pDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB( 0, 0, 55 ), 1.0f, 0)))
		return hr;


	return S_OK;
}

HRESULT InitDirectInput(HWND hWnd)
{
	HRESULT hr = 0x0000;

	//Create a DirectInput object (Note: DX8 functions are still used for DirectInput)
	if (FAILED(hr = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, 
		                               IID_IDirectInput8, (void **)&lpdi, NULL)))
		return hr;

	//Create and initialise an instance of a keyboard device and obtain a handle to its
	//IDirectInputDevice8 interface
	if (FAILED(hr = lpdi->CreateDevice(GUID_SysKeyboard, &lpdikey, NULL)))
		return hr;
	
	//Use the keyboard device interface to set up the device's cooperation level and data format:
	//Set the level of cooperation between this instance of the keyboard device, other instances
	//of the devices and the rest of the system
	if (FAILED(hr = lpdikey->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND)))
		return hr;

	//Set the data format to specify which controls are used and how they are reported
	//The predefined 'keyboard' format is used (c_dfDIKeyboard) to specify we will be passing an
	//array of 256 bytes to IDirectInputDevice8::GetDeviceState()
	if (FAILED(hr = lpdikey->SetDataFormat(&c_dfDIKeyboard)))
		return hr;

	//Acquire the newly created keyboard device (i.e. obtain access to the keyboard)
	if (FAILED(hr = lpdikey->Acquire()))
		return hr;

	return S_OK;
}
HRESULT InitDXFont()
{
	HRESULT hr = 0x0000;

	if (FAILED(hr = D3DXCreateFont(g_pDevice, 
								   20, 
								   10, 
								   FW_BOLD,
								   1,
								   false,
								   DEFAULT_CHARSET,
								   OUT_DEFAULT_PRECIS,
								   ANTIALIASED_QUALITY,
								   DEFAULT_PITCH | FF_DONTCARE,
								   "Arial",
								   &g_pFont)))
		return hr;

		return S_OK;
}
HRESULT LoadMapFromFile(const char *mapName)
{
	HRESULT hr = 0x0000;

	//Create the input stream
	ifstream in(mapName);
	if (!in)
		return E_FAIL;

	//Read the game map information from the input stream into the PacMap array
	//Note: The input stream is a 20x20 grid of information
	for (int nColumn = 0; nColumn < 20; nColumn++)
	{
		for (int nRow = 0; nRow < 20; nRow++)
		{
			in >> PacMap[nRow][nColumn];
		}
	}

	//Close the input stream
	in.close();

	return S_OK;
}

void SetPositionVector(LPD3DXVECTOR3 pSpritePos, int xpos, int ypos)
{
	(*pSpritePos).x = (FLOAT)xpos;
	(*pSpritePos).y = (FLOAT)ypos;
}


HRESULT SelectActiveSprite(int CurrentSpriteNum, int SurfWidth, int SurfHeight, RECT* SpriteRect)
{
	HRESULT hr = 0x0000;

	int numSprites		= 0;
	int OffsetX			= 0;
	int OffsetY			= 0;
	int SpritesPerRow	= 0;

	div_t Result;

	if ((SurfWidth == 0) || (SurfHeight == 0))
		return E_FAIL;

	//Determine the number of sprites per row
	SpritesPerRow = (SurfWidth/SPRITE_WIDTH);


	//Determine the number of sprites in the surface
	numSprites = ((SurfWidth * SurfHeight)/(SPRITE_WIDTH * SPRITE_HEIGHT));

	//Reset number of sprites if necessary
	if (CurrentSpriteNum > numSprites)
		CurrentSpriteNum = 0;

	//Calculate the number of sprites per row
	Result = div(CurrentSpriteNum, SpritesPerRow);

	//Calcualte offsets
	OffsetX = (Result.rem * SPRITE_WIDTH);
	OffsetY = (Result.quot * SPRITE_HEIGHT);

	//Validate offsets
	if (OffsetX > SurfWidth)
		return E_FAIL;

	if (OffsetY > SurfHeight)
		return E_FAIL;

	//Setup the RECT structure for the sprite to be used in the next frame of animation
	(*SpriteRect).top		= OffsetY;
	(*SpriteRect).left		= OffsetX;
	(*SpriteRect).bottom	= (OffsetY + SPRITE_HEIGHT);
	(*SpriteRect).right		= (OffsetX + SPRITE_WIDTH);

	return S_OK;
}

int GhostDriver( int xPos, int yPos, int nGhost )
{
	int dir = 0;

	getSpritePos(g_PacmanSpritePos);
/** /
	//If ghosts are outside manhattan distance = 12, do random movement.
	if( manhattanDist( xPos, yPos, xIndex, yIndex ) < 12 ) GDMode = 0;
	//If the ghosts are adjacent to their house, do random movement. Just makes them a little less dangerous.
	if( xPos > 6 && xPos < 12 && yPos > 6 && yPos < 11)
		GDMode = 1;
	else
		GDMode = 0;
/**/
	switch( GDMode )
	{
	case 0:	//Aggressive Ghosts - chase when GhostAttack, run when PacAttack
	{
		int dirCnt = 0;
		int choice[10];
		int dists[4] = {INT_MAX, INT_MAX, INT_MAX, INT_MAX};
		
		if( PacMap[xPos][yPos-1] > 1 + !GhostAttack )
		{
			dirCnt++;
			dists[0] = myHeuristicDist( xPos, yPos-1, xIndex, yIndex ); //a_Star( xPos, yPos-1, xIndex, yIndex, false, PacState() );
		}
		if( PacMap[xPos][yPos+1] > 1 + !GhostAttack )
		{
			dirCnt++;
			dists[1] = myHeuristicDist( xPos, yPos+1, xIndex, yIndex ); //a_Star( xPos, yPos+1, xIndex, yIndex, false, PacState() );
		}

		//If this were to evaluate both parts of the statement when a ghost is on a teleporter, we'd get an array index out-of-bounds error
		if( PacMap[xPos][yPos] != 3 && PacMap[xPos+1][yPos] == 1 )
		{
			//nothing
		}else{
			dirCnt++;
			dists[2] = myHeuristicDist( xPos+1, yPos, xIndex, yIndex ); //a_Star( xPos+1, yPos, xIndex, yIndex, false, PacState() );
		}
		if( PacMap[xPos][yPos] != 3 && PacMap[xPos-1][yPos] == 1 )
		{
			//nothing
		}else{
			dirCnt++;
			dists[3] = myHeuristicDist( xPos-1, yPos, xIndex, yIndex ); //a_Star( xPos-1, yPos, xIndex, yIndex, false, PacState() );
		}

		//Set the direction to change only if certain conditions apply
		int threshold = 3;
		if(PacMap[xPos][yPos] == 2)
			threshold++;
		if( dists[g_GhostMvDir[nGhost]] != INT_MAX && dirCnt < threshold )
			return g_GhostMvDir[nGhost];

		int i = 0, prob = 0;
		if( GhostAttack )
			prob = gakProb;
		else
			prob = pacProb;

		dir = lowest( dists, 4 );
		dists[dir] = INT_MAX;
		//Set the first few elements to the lowest distance direction
		for( i = i; i < (prob - dirCnt); i++ )
			choice[i] = dir;
		
		for( int f = 0; f < 4; f++ )
			if( dists[f] == INT_MAX )
				dists[f] = INT_MIN;
		dir = highest( dists, 4 );
		dists[dir] = INT_MIN;
		//Set the middle set of elements to the highest distance direction
		for( i = i; i < (14 - (dirCnt * 2)); i++ )
			choice[i] = dir;

		//Set any remaining elements to the intermediate distance direction(s) [with equal probability?]
		for( i = i; i < 10; i++ )
			for(int j = 0; j < 4; j++ )
				if( dists[j] != INT_MIN )
				{
					choice[i] = j;
					if( i < 9 )
						choice[++i] = j;
					dists[j] = INT_MIN;
					break;
				}
		dir = choice[ rand()%10 ];
/** /
		print("Gst@");
		sprintf_s(out, 99, "%d", xPos ); print(out); print(",");
		sprintf_s(out, 99, "%d", yPos ); print(out); print(":");
		for( int i = 0; i < 9; i++ )
		{
			sprintf_s(out, 99, "%d", choice[i] ); print(out); print(",");
		}
		sprintf_s(out, 99, "%d", choice[9] ); println(out);
/**/
//		GDMode = 1;
		break;
	}
	case 1:	//Random Ghosts
	{
		dir = newRandom[int( 4*( (double)rand() / (double)(RAND_MAX+1) ))];
		switch( dir )
		{
		case 0:
		{
			if( PacMap[xPos][yPos-1] <= 1 + !GhostAttack )
			{
				newRandom[0] = newRandom[int( 3*( (double)rand() / (double)(RAND_MAX+1) ))+1];
				dir = GhostDriver( xPos, yPos, nGhost );
			}
			break;
		}
		case 1:
		{
			if( PacMap[xPos][yPos+1] <= 1 + !GhostAttack )
			{
				int leftovers[3] = {newRandom[0],newRandom[2],newRandom[3]};
				newRandom[1] = leftovers[int( 3*( (double)rand() / (double)(RAND_MAX+1) ))];
				dir = GhostDriver( xPos, yPos, nGhost );
			}
			break;
		}
		case 2:
		{
			if( xPos < 19 && PacMap[xPos+1][yPos] <= 1 + !GhostAttack )
			{
				int leftovers[3] = {newRandom[0],newRandom[1],newRandom[3]};
				newRandom[2] = leftovers[int( 3*( (double)rand() / (double)(RAND_MAX+1) ))];
				dir = GhostDriver( xPos, yPos, nGhost );
			}
			break;
		}
		case 3:
		{
			if( xPos > 0 && PacMap[xPos-1][yPos] <= 1 + !GhostAttack )
			{
				newRandom[3] = newRandom[int( 3*( (double)rand() / (double)(RAND_MAX+1) ))];
				dir = GhostDriver( xPos, yPos, nGhost );
			}
			break;
		}
		default:
			break;
		} //End switch
		break;
	}
	default:
		break;
	}//End Outer switch

	return dir;
}

//	Have ensured movement at every call - if nChoice is blocked, method recurses and replaces nChoice in list of possible dirs
int SetSpritePosition( LPD3DXVECTOR3 pSpritePos , int nGhost )
{
	//Initialise local vars
	int retval = 0;
	int	xPosIndex = 0;
	int yPosIndex = 0;
	int xPosFinal = 0;
	int	yPosFinal = 0;
	bool bRecurse = false;

	getSpritePos( *pSpritePos );
	
	//Setup final values as current position values
	xPosFinal = xPosIndex = xIndex;
	yPosFinal = yPosIndex = yIndex;

	//Adjust sprite position based on choice of moves (nChoice)
	switch( retval = GhostDriver( xPosIndex, yPosIndex, nGhost ) )
	{
	case 0:
	{
		yPosFinal--;
		break;
	}
	case 1:
	{
		yPosFinal++;
		break;
	}
	case 2:
	{
		if( PacMap[xPosIndex][yPosIndex] == 3 )
			xPosFinal = 1;
		else
			xPosFinal++;
		break;
	}
	case 3:
	{
		if( PacMap[xPosIndex][yPosIndex] == 3 )
			xPosFinal = 18;
		else
			xPosFinal--;
		break;
	}
	default:
		break;
	} //End switch

	if( GDMode == 1 )
	{
		// north = 0, south = 1, east = 2, west = 3;
		newRandom[0] = 0;	newRandom[1] = 1;	newRandom[2] = 2;	newRandom[3] = 3;
	}
 
	SetPositionVector(pSpritePos, (GRIDTLX + (xPosFinal * SPRITE_WIDTH)), (GRIDTLY + (yPosFinal * SPRITE_HEIGHT)));

	return retval;
}


BOOL CheckForCollision(LPD3DXVECTOR3 pOrigSpritePos, LPD3DXVECTOR3 pCollisionSpritePos)
{
	//Initialise boolean return value
	BOOL bResult = false;

	//Initialise local RECT structures
	RECT OrigRect;
	RECT CollisionRect;
	RECT TempRect;

	//Obtain RECT information
	OrigRect.top = (int)(*pOrigSpritePos).y;
	OrigRect.left = (int)(*pOrigSpritePos).x;
	OrigRect.right = OrigRect.left + SPRITE_WIDTH;
	OrigRect.bottom = OrigRect.top + SPRITE_HEIGHT;

	CollisionRect.top = (int)(*pCollisionSpritePos).y;
	CollisionRect.left= (int)(*pCollisionSpritePos).x;
	CollisionRect.right = CollisionRect.left + SPRITE_WIDTH;
	CollisionRect.bottom = CollisionRect.top + SPRITE_HEIGHT;

	//Determine if there is an overlap in the RECT structures
	bResult = IntersectRect(&TempRect, &OrigRect, &CollisionRect);

	return bResult;
}