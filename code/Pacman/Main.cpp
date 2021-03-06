#include "stdafx.h"
#include "GraphicsManager.h"
#include "Game.h"
#include "KeyManager.h"

HINSTANCE				hInst					= NULL;  
HWND					hWnd					= NULL;

__int64 currTimeStamp = 0, prevTimeStamp = 0, cntsPerSec = 0;
double dt, secsPerCnt, time;

HRESULT             InitWindow( int nCmdShow );
LRESULT CALLBACK	WndProc( HWND, UINT, WPARAM, LPARAM);
void				updateTimeStamp();
void				Cleanup();

GraphicsManager *graphicsManager;
Game* game;

int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
	hInst = hInstance;
	//_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	if( FAILED( InitWindow( nCmdShow ) ) )
		return 0;

	SetWindowPos(hWnd, HWND_TOP, 50, 50, SCREENWIDTH, _SCREENHEIGHT + 32, SWP_SHOWWINDOW);

	graphicsManager = GraphicsManager::createInstance(hWnd);

	graphicsManager->clearRenderTarget();
	game = Game::getInstance();
	
	QueryPerformanceFrequency((LARGE_INTEGER*)&cntsPerSec);
	time = 0;
	secsPerCnt = 1.0 / (double)cntsPerSec;
	
	
	QueryPerformanceCounter((LARGE_INTEGER*)&currTimeStamp);
	prevTimeStamp = currTimeStamp;

	// Main message loop
	MSG msg = {0};
	while(WM_QUIT != msg.message)
	{
		updateTimeStamp();

		time += dt;

		if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else if(time >= (1.f / 6000.f))
		{
			
			if(GetActiveWindow() == hWnd)
			{
				game->update(time);
				//SetCursorPos((int)(SCREENWIDTH * 0.5f), (int)(_SCREENHEIGHT * 0.5f));
			}
			else
				updateTimeStamp();

			graphicsManager->clearRenderTarget();
			game->draw(time);
			
			graphicsManager->swapChain();
			time = 0;
		}
	}

	Cleanup();
	_CrtDumpMemoryLeaks();
	return (int) msg.wParam;
}

void updateTimeStamp()
{
	prevTimeStamp = currTimeStamp;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTimeStamp);

	dt = (currTimeStamp - prevTimeStamp) * secsPerCnt;
}

HRESULT InitWindow( int nCmdShow )
{
	char *windowName = "Pacman Reloded";
	char *className = "pac";
	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style          = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc    = WndProc;
	wcex.cbClsExtra     = 0;
	wcex.cbWndExtra     = 0;
	wcex.hInstance      = hInst;
	wcex.hIcon          = 0;
	wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName   = NULL;
	wcex.lpszClassName  = className;
	wcex.hIconSm        = 0;
	if( !RegisterClassEx(&wcex) )
		return E_FAIL;

	// Create window
	RECT rc = { 0, 0, SCREENWIDTH, _SCREENHEIGHT };
	AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );

	if(!(hWnd = CreateWindow(	className, windowName,
								WS_OVERLAPPEDWINDOW,
								CW_USEDEFAULT, CW_USEDEFAULT,
								rc.right - rc.left, rc.bottom - rc.top,
								NULL, NULL,
								hInst,
								NULL)))
	{
		return E_FAIL;
	}

	ShowWindow( hWnd, nCmdShow );

	return S_OK;
}

LRESULT CALLBACK WndProc( HWND hw, UINT message, WPARAM wParam, LPARAM lParam )
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
	case WM_PAINT:
		hdc = BeginPaint(hw, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hw, message, wParam, lParam);
	}

	return 0;
}

void Cleanup()
{
	SAFE_DELETE(graphicsManager);
	SAFE_DELETE(game);
}