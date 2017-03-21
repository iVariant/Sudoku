#include "stdafx.h"
#include "Sudoku.h"
#include "SudocuGeneration.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
HWND editBox[9][9];
HWND hwndSOlVE;
HWND hwndHELP;
HWND hwndERROR;

static int count = 0;
static int gameLevel = 2;

int **data = NULL;

FILE *Fz;

struct TZap
{
	int counts;
	wchar_t date[256];
} Zap;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
int                  printSudoku(int level);


int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_SUDOKU, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SUDOKU));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SUDOKU));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_SUDOKU);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}


BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable


   RECT rect;
   SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
   int width = 863;
   int height = 575;

   hWnd = CreateWindow(szWindowClass, szTitle, WS_SYSMENU | WS_MINIMIZEBOX | WS_CLIPCHILDREN,
	   (rect.right - width) / 2.0, (rect.bottom - height) / 2.0,
	   width, height, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message)
	{
		PAINTSTRUCT ps;

		int wmId, wmEvent;
		static HDC hCmpDC, hDC;
		HBITMAP hBitmap;
		static HDC memBit;
		static BITMAP bm;

		static HFONT hFont;
		RECT Rect;
		TEXTMETRIC tm;
		TCHAR str[256];

	//--------------------------------------------------------------------------------
	case WM_CREATE:
	{
		SetTimer(hWnd, 1, 1000, NULL);
		hFont = CreateFont(33, 0, 0, 0, 700, 1, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Brush Script MT");
		
		PlaySound(MAKEINTRESOURCE(IDR_WAVE1), GetModuleHandle(NULL), SND_RESOURCE | SND_ASYNC);

		// sudoku array
		data = new int*[9]; 		
		for (int i = 0; i < 9; i++)
			data[i] = new int[9]; 		



		int numberX = 0;
		int numberY = 0;
		for (int i = 0; i < 9; i++)
		{
			for (int j = 0; j < 9; j++)
			{
				editBox[i][j] = CreateWindow(_T("EDIT"), NULL, WS_BORDER | WS_VISIBLE | WS_CHILD | ES_CENTER | ES_NUMBER, 400 + numberX, 70 + numberY, 38, 38, hWnd, 0, NULL, NULL);
				SendMessage(editBox[i][j], WM_SETFONT, (WPARAM)hFont, NULL);
				SendMessage(editBox[i][j], EM_SETLIMITTEXT, 1, 0);
				if ((j + 1) % 3 == 0) numberX += 42;
				else numberX += 39;
			}

			if ((i + 1) % 3 == 0)
			{
				numberX = 0;
				numberY += 42;
			}
			else
			{
				numberY += 39;
				numberX = 0;
			}

		}

		hFont = CreateFont(33, 0, 0, 0, 700, 1, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial Rounded MT Bold");
		SudocuGeneration *NewSudo = new SudocuGeneration();

		NewSudo->generationMas(data);

		printSudoku(gameLevel);
		delete NewSudo;


		hwndHELP = CreateWindow(L"button", L"", WS_VISIBLE | WS_CHILD | BS_BITMAP | BS_ICON, 770, 250, 55, 55, hWnd, 0, NULL, NULL);
		hwndERROR = CreateWindow(L"button", L"", WS_VISIBLE | WS_CHILD | BS_BITMAP | BS_ICON, 770, 310, 55, 55, hWnd, 0, NULL, NULL);
		hwndSOlVE = CreateWindow(L"button", L"", WS_VISIBLE | WS_CHILD | BS_BITMAP | BS_ICON, 770, 370, 55, 55, hWnd, 0, NULL, NULL);

		SendMessage(hwndERROR, BM_SETIMAGE, IMAGE_ICON,
			(LPARAM)LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON,
			0, 0, LR_DEFAULTCOLOR));

		SendMessage(hwndHELP, BM_SETIMAGE, IMAGE_ICON,
			(LPARAM)LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON2), IMAGE_ICON,
			0, 0, LR_DEFAULTCOLOR));


		SendMessage(hwndSOlVE, BM_SETIMAGE, IMAGE_ICON,
			(LPARAM)LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON3), IMAGE_ICON,
			0, 0, LR_DEFAULTCOLOR));


		//Create Menu
		HMENU MainMenu = CreateMenu();
		HMENU hPopupMenu = CreatePopupMenu();

		AppendMenu(MainMenu, MF_STRING | MF_POPUP, (UINT)hPopupMenu, L"&New Game");
		{
			AppendMenu(hPopupMenu, MF_STRING, ID_CLEAN, L"Clean");
			AppendMenu(hPopupMenu, MF_SEPARATOR, 1000, L"");

			AppendMenu(hPopupMenu, MF_STRING, ID_NEWGAME_EASY, L"Easy");
			AppendMenu(hPopupMenu, MF_STRING, ID_NEWGAME_MEDIUM, L"Medium");
			AppendMenu(hPopupMenu, MF_STRING, ID_NEWGAME_HARD, L"Hard");
		}

		AppendMenu(MainMenu, MF_STRING | MF_POPUP, ID_STATISTIC, L"&Statistic");
		AppendMenu(MainMenu, MF_STRING | MF_POPUP, ID_ABOUT, L"&About");

		SetMenu(hWnd, MainMenu);

	}
	break;


	//--------------------------------------------------------------------------------
	case WM_PAINT:
	{
		GetClientRect(hWnd, &Rect);
		hDC = BeginPaint(hWnd, &ps);

		hCmpDC = CreateCompatibleDC(hDC);

		hBitmap = LoadBitmap(GetModuleHandle(0), MAKEINTRESOURCE(IDB_BITMAP1));
		GetObject(hBitmap, sizeof(bm), &bm);
		SelectObject(hCmpDC, hBitmap);

		SetTextColor(hCmpDC, RGB(255, 49, 1));
		SetBkMode(hCmpDC, TRANSPARENT); // Прозрачный фон
		SelectObject(hCmpDC, hFont);

		GetTextMetrics(hCmpDC, &tm);
		int a = count / 60, b = count % 60;

		if (a < 10 && b < 10) _stprintf_s(str, L" 0%d : 0%d ", a, b);
			else if (b < 10) _stprintf_s(str, L" %d : 0%d ", a, b);
				else if (a < 10) _stprintf_s(str, L" 0%d : %d ", a, b);

		SetRect(&Rect, 730, 15, 860, 100);
		DrawText(hCmpDC, str, _tcslen(str), &Rect, DT_LEFT);


		SetStretchBltMode(hDC, COLORONCOLOR);
		BitBlt(hDC, 0, 0, bm.bmWidth, bm.bmHeight, hCmpDC, 0, 0, SRCCOPY);

		DeleteDC(hCmpDC);
		DeleteObject(hBitmap);
		DeleteObject(hCmpDC);
		hCmpDC = NULL;

		EndPaint(hWnd, &ps);
	}
	break;


	//--------------------------------------------------------------------------------
	case WM_COMMAND:
	{

		TCHAR buf[256];
		TCHAR buf2[256];

		// Button ERROR
		if ((HWND)lParam == hwndERROR)
		{
			int countError = 0;

			for (int i = 0; i < 9; i++)
			{
				for (int j = 0; j < 9; j++)
				{
					GetWindowText(editBox[i][j], buf, 256);
					_stprintf_s(buf2, L"%d", data[i][j]);
					if (wcscmp(buf, L"") && wcscmp(buf, buf2)) countError++;
				}
			}

			_stprintf_s(buf, L"You have %d errors!", countError);
			MessageBox(hWnd, buf, L"Errors", 0);
		}

		// Button HELP
		if ((HWND)lParam == hwndHELP)
		{

			int countError = 0;

			for (int i = 0; i < 9; i++)
			{
				for (int j = 0; j < 9; j++)
				{
					GetWindowText(editBox[i][j], buf, 256);
					_stprintf_s(buf2, L"%d", data[i][j]);
					if (wcscmp(buf, buf2)) countError++;
				}
			}

			if (!countError){
				MessageBox(hWnd, L"The game is solved!", L"Help", 0);
				break;
			}

			int number1 = 0, number2 = 0;
			srand(time(0));

			do {
				number1 = 0 + rand() % 9;
				number2 = 0 + rand() % 9;
				GetWindowText(editBox[number1][number2], buf, 256);

				_stprintf_s(buf2, L"%d", data[number1][number2]);


				if (wcscmp(buf, buf2) || !(wcscmp(buf, L""))) break;

			} while (1);


			_stprintf_s(buf, L"%d", data[number1][number2]);
			SetWindowText(editBox[number1][number2], buf);

		}


		// Button SOLVE 
		if ((HWND)lParam == hwndSOlVE)
		{
			int countError = 0;

			for (int i = 0; i < 9; i++)
			{
				for (int j = 0; j < 9; j++)
				{
					GetWindowText(editBox[i][j], buf, 256);
					_stprintf_s(buf2, L"%d", data[i][j]);
					if (wcscmp(buf, buf2)) countError++;
				}
			}


			if (!countError){

				int size = sizeof(TZap);

				Fz = nullptr;
				if ((fopen_s(&Fz, "statistic.dat", "ab")) == NULL)
				{
					_wstrdate_s(Zap.date, 256);

					Zap.counts = count;
					fwrite(&Zap, size, 1, Fz);
				}
				else
				{
					MessageBox(hWnd, L"File statistic not found!", L"Message", MB_OK);
				}


				TCHAR ST[256];
				_stprintf_s(ST, L" You win! Your time  %d : %d ", count / 60, count % 60);

				MessageBox(hWnd, ST, L"Rezult", 0);

				fclose(Fz);

				SudocuGeneration *NewSudo = new SudocuGeneration();

				NewSudo->generationMas(data);

				printSudoku(gameLevel);
				delete NewSudo;
			}
			else
			{
				MessageBox(hWnd,L"The game has not been solved!",L"Rezult",MB_OK);
			}

		}



		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);

		//------------------------------------------------------------------------------------

		// Parse the menu selections:
		switch (wmId)
		{
			
		// Menu LEVEL

		// Menu LEVEL EASY
		case ID_NEWGAME_EASY:
		{
			SudocuGeneration *NewSudo = new SudocuGeneration();

			NewSudo->generationMas(data);
			gameLevel = 1;
			printSudoku(gameLevel);
			delete NewSudo;
		}
		break;

		// Menu LEVEL MEDIUM
		case ID_NEWGAME_MEDIUM:
		{
			SudocuGeneration *NewSudo = new SudocuGeneration();

			NewSudo->generationMas(data);
			gameLevel = 2;
			printSudoku(gameLevel);
			delete NewSudo;
		}
		break;

		// Menu LEVEL HARD
		case ID_NEWGAME_HARD:
		{
			SudocuGeneration *NewSudo = new SudocuGeneration();

			NewSudo->generationMas(data);
			gameLevel = 3;
			printSudoku(gameLevel);
			delete NewSudo;
		}
		break;


		// Menu STATISTIC
		case ID_STATISTIC:
		{
		
			int size = sizeof(TZap);

			Fz = nullptr;
			if ((fopen_s(&Fz, "statistic.dat", "rb")) == NULL)
			{

				int kod, D_f, kol;
				long len;

				D_f = _fileno(Fz);
				len = _filelength(D_f); 
				kol = len / size;

				TZap *mas_Z = new TZap[kol];

				for (int i = 0; i < kol; i++)
					fread((mas_Z + i), size, 1, Fz);

				for (int i = 0; i < kol; i++)
				{
					for (int j = 0; j < kol - 1; j++)
					{
						if (mas_Z[j].counts > mas_Z[j + 1].counts)
						{

							wchar_t buffer1[30];
							wcscpy_s(buffer1, mas_Z[j].date);
							wcscpy_s(mas_Z[j].date, mas_Z[j + 1].date);
							wcscpy_s(mas_Z[j + 1].date, buffer1);


							int buffer2 = mas_Z[j].counts;
							mas_Z[j].counts = mas_Z[j + 1].counts;
							mas_Z[j + 1].counts = buffer2;

						}
					}

				}

				wchar_t ST[256];
				wchar_t ST2[11512];
				_stprintf_s(ST2, L"");


				if (kol > 10)
				{
					for (int i = 0; i < 10; i++){
						_stprintf_s(ST, L"%d. Time  %d : %d , date %10ls \n", i + 1, mas_Z[i].counts / 60, mas_Z[i].counts % 60, mas_Z[i].date);
						wcscat_s(ST2, ST);
					}
				}
				else
				{
					for (int i = 0; i < kol; i++){
						_stprintf_s(ST, L"%d. Time  %d : %d , date %10ls \n", i + 1, mas_Z[i].counts / 60, mas_Z[i].counts % 60, mas_Z[i].date);
						wcscat_s(ST2, ST);
					}
				}
				MessageBox(hWnd, ST2, L"TOP GAMES", MB_OK);

				delete[]mas_Z;
				fclose(Fz);
			}
			else
			{
				MessageBox(hWnd, L"File statistic not found!", L"Message", MB_OK);
			}

		}
		break;


		// Menu ABOUT
		case ID_ABOUT:
		{
			MessageBox(hWnd, L"Program: Sudoku v.1.0\nDeveloper: IX Variant\nE-mail: ix.variant@gmail.com",NULL, MB_OK);
		}
		break;

		case ID_CLEAN:
		{
				//createee

		}
		break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}


	}
	break;


	//--------------------------------------------------------------------------------
	case WM_TIMER:
		count++;
		InvalidateRect(hWnd, NULL, FALSE);
		break;


	//--------------------------------------------------------------------------------
	case WM_DESTROY:
		KillTimer(hWnd, 1);
		delete data;
		PostQuitMessage(0);
		break;


	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


int printSudoku(int level)
{
	TCHAR buuf[256];
	int number1 = 0, number2 = 0;
	count = 0;


	for (int i = 0; i < 9; i++)
		for (int j = 0; j < 9; j++)
		{
			SetWindowText(editBox[i][j], L"");
			SendMessage(editBox[i][j], EM_SETREADONLY, FALSE, 0);
		}


	if (level == 1)		 level = 60;
		else if (level == 2) level = 40;
			else if (level == 3) level = 20;

	srand(time(0));

	for (int i = 0; i < level; i++)
	{

		do{
			number1 = 0 + rand() % 9;
			number2 = 0 + rand() % 9;
			GetWindowText(editBox[number1][number2], buuf, 256);

		} while (wcscmp(buuf, L""));

		_stprintf_s(buuf, L"%d", data[number1][number2]);
		SetWindowText(editBox[number1][number2], buuf);
		SendMessage(editBox[number1][number2], EM_SETREADONLY, TRUE, 0);

	}

	return 0;
}