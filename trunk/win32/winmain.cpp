// cv.cpp : Defines the entry point for the application.
//

//#include "stdafx.h"
//#include "cv.h"
#include "unixstuff.h"
#include "prefix.h"
#include "inttypes.h"
#include "windows.h"
#include "resource.h"
#include "mednafen.h"
#include "SDL.h"
#include "ramwatch.h"
#include "ramsearch.h"
#include "movie.h"

HWND med_hWnd;
HINSTANCE hAppInst = NULL;

bool finished=true;

#define MAX_LOADSTRING 100

extern char *DrBaseDirectory;
extern char *GetBaseDirectory(void);
extern bool RemoteOn;
extern bool InitSTDIOInterface(void);
extern MDFNSetting DriverSettings[];

static void UnEscapeQuotes( char *arg )
{
	char *last = NULL;

	while( *arg ) {
		if( *arg == '"' && *last == '\\' ) {
			char *c_curr = arg;
			char *c_last = last;

			while( *c_curr ) {
				*c_last = *c_curr;
				c_last = c_curr;
				c_curr++;
			}
			*c_last = '\0';
		}
		last = arg;
		arg++;
	}
}

/* Parse a command line buffer into arguments */
static int ParseCommandLine(char *cmdline, char **argv)
{
	char *bufp;
	char *lastp = NULL;
	int argc, last_argc;

	argc = last_argc = 0;
	for ( bufp = cmdline; *bufp; ) {
		/* Skip leading whitespace */
		while ( isspace(*bufp) ) {
			++bufp;
		}
		/* Skip over argument */
		if ( *bufp == '"' ) {
			++bufp;
			if ( *bufp ) {
				if ( argv ) {
					argv[argc] = bufp;
				}
				++argc;
			}
			/* Skip over word */
			while ( *bufp && ( *bufp != '"' || *lastp == '\\' ) ) {
				lastp = bufp;
				++bufp;
			}
		} else {
			if ( *bufp ) {
				if ( argv ) {
					argv[argc] = bufp;
				}
				++argc;
			}
			/* Skip over word */
			while ( *bufp && ! isspace(*bufp) ) {
				++bufp;
			}
		}
		if ( *bufp ) {
			if ( argv ) {
				*bufp = '\0';
			}
			++bufp;
		}

		/* Strip out \ from \" sequences */
		if( argv && last_argc != argc ) {
			UnEscapeQuotes( argv[last_argc] );	
		}
		last_argc = argc;	
	}
	if ( argv ) {
		argv[argc] = NULL;
	}
	return(argc);
}

//#include <stdio.h>
#include <fcntl.h>
#include <io.h>
HANDLE hConsole;
void OpenConsole() 
{
	COORD csize;
	CONSOLE_SCREEN_BUFFER_INFO csbiInfo; 
	SMALL_RECT srect;
	char buf[256];

	if (hConsole) return;
	AllocConsole();

	//redirect stdio
	long lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
	int hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	FILE *fp = _fdopen( hConHandle, "w" );
	*stdout = *fp;
}

HACCEL hAccelTable;

extern int main(int argc, char *argv[]);

//int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int sw)
// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;

	OpenConsole();
	

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_CV, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CV));

	//-------------------------
	char *bufp;
	char *lastp = NULL;
	int argc, last_argc;
	char *cmdline;
	char **argv;
	size_t nLen;

	/* Grab the command line */
	bufp = GetCommandLine();
	nLen = SDL_strlen(bufp)+1;
	cmdline = SDL_stack_alloc(char, nLen);
	if ( cmdline == NULL ) {
//		return OutOfMemory();
	}
	SDL_strlcpy(cmdline, bufp, nLen);
//#endif

	/* Parse it into argv and argc */
	argc = ParseCommandLine(cmdline, NULL);
	argv = SDL_stack_alloc(char*, argc+1);
	if ( argv == NULL ) {
//		return OutOfMemory();
	}
	ParseCommandLine(cmdline, argv);

//int console_main(int argc, char *argv[])
//{
	size_t n;
	char// *bufp, 
		*appname;
	int status;

	/* Get the class name from argv[0] */
	appname = argv[0];
	if ( (bufp=SDL_strrchr(argv[0], '\\')) != NULL ) {
		appname = bufp+1;
	} else
	if ( (bufp=SDL_strrchr(argv[0], '/')) != NULL ) {
		appname = bufp+1;
	}

	if ( (bufp=SDL_strrchr(appname, '.')) == NULL )
		n = SDL_strlen(appname);
	else
		n = (bufp-appname);

	bufp = SDL_stack_alloc(char, n+1);
	if ( bufp == NULL ) {
//		return OutOfMemory();
	}
	SDL_strlcpy(bufp, appname, n+1);
	appname = bufp;

	/* Load SDL dynamic link library */
	if ( SDL_Init(SDL_INIT_NOPARACHUTE) < 0 ) {
//		ShowError("WinMain() error", SDL_GetError());
		return(FALSE);
	}
//	atexit(cleanup_output);
//	atexit(cleanup);

	/* Sam:
	   We still need to pass in the application handle so that
	   DirectInput will initialize properly when SDL_RegisterApp()
	   is called later in the video initialization.
	 */
	SDL_SetModuleHandle(GetModuleHandle(NULL));

	/* Run the application main() code */
//	status = SDL_main(argc, argv);

	/* Exit cleanly, calling atexit() functions */
//	exit(status);

	/* Hush little compiler, don't you cry... */
//	return 0;
//}


	//-----------------------------------------------------------------

	main(argc, argv);

	//-----------------------------------------------------------------

	return (int) msg.wParam;
}

void CheckMessages()
{
	MSG msg;
//	HWND hwnd = MainWindow->getHWnd();

	#ifdef WX_STUB
	wxDesmumeApp::frameUpdate();
	#endif

	while( PeekMessage( &msg, 0, 0, 0, PM_NOREMOVE ) )
	{
		if( GetMessage( &msg, 0,  0, 0)>0 )
		{
/*			if (RamWatchHWnd && IsDialogMessage(RamWatchHWnd, &msg))
			{
				if(msg.message == WM_KEYDOWN) // send keydown messages to the dialog (for accelerators, and also needed for the Alt key to work)
					SendMessage(RamWatchHWnd, msg.message, msg.wParam, msg.lParam);
				continue;
			}
			if (SoundView_GetHWnd() && IsDialogMessage(SoundView_GetHWnd(), &msg))
				continue;
*/
			if(!TranslateAccelerator(med_hWnd,hAccelTable,&msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hAppInst;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CV));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_CV);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//

#include "SDL.h"

// C++ method to convert an integer to a hexadecimal representation
#include <sstream>
#include <string>


// Attaches the SDL graphics to a pre-created Win32 window
void attachSDL(HWND window)
{
	char winhack[1024];
        snprintf( winhack, sizeof winhack, "SDL_WINDOWID=%i", window);
        SDL_putenv( winhack ); 
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   med_hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 328, 278, NULL, NULL, hInstance, NULL);//CW_USEDEFAULT, 0,

   if (!med_hWnd)
   {
      return FALSE;
   }

   ShowWindow(med_hWnd, nCmdShow);
   UpdateWindow(med_hWnd);

   attachSDL(med_hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//

				char szChoice[MAX_PATH]={0};
				std::string fname;//[MAX_PATH]={0};
				int x;
				std::wstring la = L"";
				OPENFILENAME ofn;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_ENTERMENULOOP:
			EnableMenuItem(GetMenu(hWnd), IDM_RECORD_MOVIE,      MF_BYCOMMAND | (movieMode == MOVIEMODE_INACTIVE) ? MF_ENABLED : MF_GRAYED);
			EnableMenuItem(GetMenu(hWnd), IDM_PLAY_MOVIE,        MF_BYCOMMAND | (movieMode == MOVIEMODE_INACTIVE) ? MF_ENABLED : MF_GRAYED);
			EnableMenuItem(GetMenu(hWnd), IDM_STOPMOVIE,         MF_BYCOMMAND | (movieMode != MOVIEMODE_INACTIVE) ? MF_ENABLED : MF_GRAYED);
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_RESET:
			{
			MDFN_DoSimpleCommand(1);
			}
		case IDM_RECORD_MOVIE:
//				OPENFILENAME ofn;

				// browse button
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = hWnd;
				ofn.lpstrFilter = "Mednafen Movie File (*.mc2)\0*.mc2\0All files(*.*)\0*.*\0\0";
				ofn.lpstrFile = (LPSTR)szChoice;
				ofn.lpstrTitle = "Record a new movie";
				ofn.lpstrDefExt = "mc2";
				ofn.nMaxFile = MAX_PATH;
				ofn.Flags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
				GetSaveFileName(&ofn);
			
				//If user did not specify an extension, add .dsm for them
//				fname = szChoice;
//				x = fname.find_last_of(".");
//				if (x < 0)
//					fname.append(".mc2");

				FCEUI_SaveMovie(szChoice, la);

//				SetDlgItemText(hwndDlg, IDC_EDIT_FILENAME, fname.c_str());
				//if(GetSaveFileName(&ofn))
				//	UpdateRecordDialogPath(hwndDlg,szChoice);

				return true;
		//	MovieRecordTo();
			return 0;
		case IDM_PLAY_MOVIE:

				// browse button
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = hWnd;
				ofn.lpstrFilter = "Mednafen Movie File (*.mc2)\0*.mc2\0All files(*.*)\0*.*\0\0";
				ofn.lpstrFile = (LPSTR)szChoice;
				ofn.lpstrTitle = "Play a movie";
				ofn.lpstrDefExt = "mc2";
				ofn.nMaxFile = MAX_PATH;
				ofn.Flags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
				GetOpenFileName(&ofn);
//			Replay_LoadMovie();
				FCEUI_LoadMovie(szChoice, 1, 0, 0);
			return 0;
		case IDM_STOPMOVIE:
			FCEUI_StopMovie();
			return 0;
		case ID_RAM_SEARCH:
					if(!RamSearchHWnd)
					{
						InitRamSearch();
						RamSearchHWnd = CreateDialog(hAppInst, MAKEINTRESOURCE(IDD_RAMSEARCH), hWnd, (DLGPROC) RamSearchProc);
					}
					else
						SetForegroundWindow(RamSearchHWnd);
					break;

		case ID_RAM_WATCH:
			if(!RamWatchHWnd)
			{
				RamWatchHWnd = CreateDialog(hAppInst, MAKEINTRESOURCE(IDD_RAMWATCH), hWnd, (DLGPROC) RamWatchProc);
			}
			else
				SetForegroundWindow(RamWatchHWnd);
			return 0;

		case IDM_ABOUT:
			

			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
	case WM_CLOSE:
		finished=true;
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void setClientSize(int width, int height)
{
	int xborder, yborder;
	int ymenu, ymenunew;
	int ycaption;

	MENUBARINFO mbi;

	RECT wndRect;
	int finalx, finaly;

	/* Get the size of the border */
	xborder = GetSystemMetrics(SM_CXSIZEFRAME);
	yborder = GetSystemMetrics(SM_CYSIZEFRAME);
	
	/* Get the size of the menu bar */
	ZeroMemory(&mbi, sizeof(mbi));
	mbi.cbSize = sizeof(mbi);
	GetMenuBarInfo(med_hWnd, OBJID_MENU, 0, &mbi);
	ymenu = (mbi.rcBar.bottom - mbi.rcBar.top + 1);

	/* Get the size of the caption bar */
	ycaption = GetSystemMetrics(SM_CYCAPTION);

	/* Finally, resize the window */
	GetWindowRect(med_hWnd, &wndRect);
	finalx = (xborder + width + xborder);
	finaly = (ycaption + yborder + ymenu + height + yborder);
//	MoveWindow(med_hWnd, wndRect.left, wndRect.top, finalx, finaly, TRUE);

	/* Oops, we also need to check if the height */
	/* of the menu bar has changed after the resize */
	ZeroMemory(&mbi, sizeof(mbi));
	mbi.cbSize = sizeof(mbi);
	GetMenuBarInfo(med_hWnd, OBJID_MENU, 0, &mbi);
	ymenunew = (mbi.rcBar.bottom - mbi.rcBar.top + 1);

//	if(ymenunew != ymenu)
//		MoveWindow(med_hWnd, wndRect.left, wndRect.top, finalx, (finaly + (ymenunew - ymenu)), TRUE);
}