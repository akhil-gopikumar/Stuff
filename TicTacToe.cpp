
#include "framework.h"
#include "TicTacToe.h"

enum EMarker { kBlank, kX, kO };

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
EMarker GamePlayerTurn = kX;
EMarker GameBoard[3][3] = { { kBlank, kBlank ,kBlank },
                            { kBlank, kBlank ,kBlank },
                            { kBlank, kBlank ,kBlank } };
const int gkiSqrSz = 200;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void ResetGame();
bool PlaceX(LPARAM);
bool PlaceO(LPARAM);
bool HasWon();
bool IsBoardFull();
void DrawBoard(HDC);
void DrawX(HDC, int, int);
void DrawO(HDC, int, int);
void game(HDC, HWND);
void drawScoreBoard(HDC, HWND);
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TICTACTOE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TICTACTOE));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;
    COLORREF qWndColor = RGB(0, 100, 100);
    HBRUSH hW = CreateSolidBrush(qWndColor);

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TICTACTOE));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = hW;
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_TICTACTOE);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
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
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

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
    case WM_LBUTTONDOWN:
    {
        bool bValidMove = PlaceX(lParam);
        if (bValidMove)
        {
            GamePlayerTurn = kO;
        }
        else
        {
            MessageBox(hWnd, L"Invalid Move", L"Error", MB_OK | MB_ICONERROR);
        }
        InvalidateRect(hWnd, 0, TRUE);
        break;
    }
    case WM_RBUTTONDOWN:
    {
        bool bValidMove = PlaceO(lParam);
        if (bValidMove)
        {
            GamePlayerTurn = kX;
        }
        else
        {
            MessageBox(hWnd, L"Invalid Move", L"Error", MB_OK | MB_ICONERROR);

        }
        InvalidateRect(hWnd, 0, TRUE);
        break;
    }
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_NEW_GAME:
                ResetGame();
                InvalidateRect(hWnd, 0, TRUE);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            game(hdc, hWnd);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
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

void ResetGame()
{
    for (int iX = 0; iX < 3; iX++)
        for (int iY = 0; iY < 3; iY++)
            GameBoard[iY][iX] = kBlank;
    GamePlayerTurn = kX;
}

bool PlaceX(LPARAM lParam)
{
    int iPosX = LOWORD(lParam);
    int iPosY = HIWORD(lParam);
    int sqX = (iPosX / gkiSqrSz);
    int sqY = (iPosY / gkiSqrSz);

    if ((GamePlayerTurn == kX)
        &&(sqX < 3) && (sqY < 3)
        &&(GameBoard[sqY][sqX] == kBlank))
    {
        GameBoard[sqY][sqX] = kX;
        return true;
    }
    return false;   
}

bool PlaceO(LPARAM lParam)
{
    int iPosX = LOWORD(lParam);
    int iPosY = HIWORD(lParam);
    int sqX = (iPosX / gkiSqrSz);
    int sqY = (iPosY / gkiSqrSz);

    if ((GamePlayerTurn == kO)
        && (sqX < 3) && (sqY < 3)
        && (GameBoard[sqY][sqX] == kBlank))
    {
        GameBoard[sqY][sqX] = kO;
        return true;
    }
    return false;
}

void DrawBoard(HDC hdc)
{
    HPEN hPenOld, hLinePen;
    COLORREF qLineColor;
    qLineColor = RGB(0, 0, 0);
    hLinePen = CreatePen(PS_SOLID, 7, qLineColor);
    hPenOld = (HPEN)SelectObject(hdc, hLinePen);

    for (int iX = gkiSqrSz; iX < 3 * gkiSqrSz; iX+= gkiSqrSz)
    {
        MoveToEx(hdc, iX, 20, NULL);
        LineTo(hdc, iX, 3 * gkiSqrSz + 20);
    }

    for (int iY = gkiSqrSz; iY < 3 * gkiSqrSz; iY+= gkiSqrSz)
    {
        MoveToEx(hdc, 20, iY, NULL);
        LineTo(hdc, 3 * gkiSqrSz + 20, iY);
    }

    SelectObject(hdc, hPenOld);
    DeleteObject(hLinePen);
}

void drawScoreBoard(HDC hdc, HWND hWnd)
{
    HPEN hPenHold, hLinePen;
    COLORREF qLineColor = RGB(0, 0, 200);
    hLinePen = CreatePen(PS_SOLID, 7, qLineColor);
    hPenHold = (HPEN)SelectObject(hdc, hLinePen);
    const wchar_t* instruction = L"This is a game of TAC TAC TOE. Press the left mouse button on the square you please to play as X. Press the right mouse button on the square you please to play as O. Good Luck";
    HWND hScoreWnd = CreateWindowW(L"static", instruction, WS_CHILD | WS_VISIBLE | WS_BORDER, 800, 50, 1500, 290, hWnd, NULL, NULL, NULL);
    SelectObject(hdc, hPenHold);
    DeleteObject(hLinePen);
}
void DrawX(HDC hdc, int iX, int iY)
{
    HPEN hPenOld, hLinePen;
    COLORREF qLineColor;
    qLineColor = RGB(255, 0, 0);
    const int kPenWidth = 10;
    hLinePen = CreatePen(PS_SOLID, kPenWidth, qLineColor);
    hPenOld = (HPEN)SelectObject(hdc, hLinePen);

    const int hX = iX * gkiSqrSz + 2 * kPenWidth;
    const int lX = (iX + 1) * gkiSqrSz - 2 * kPenWidth;
    const int hY = iY * gkiSqrSz + 2 * kPenWidth;
    const int lY = (iY + 1) * gkiSqrSz - 2 * kPenWidth;

    MoveToEx(hdc, lX, lY, NULL);
    LineTo(hdc, hX, hY);
    MoveToEx(hdc, lX, hY, NULL);
    LineTo(hdc, hX, lY);

    SelectObject(hdc, hPenOld);
    DeleteObject(hLinePen);
}
void DrawO(HDC hdc, int iX, int iY)
{
    HPEN hPenOld, hOPen;
    COLORREF qLineColor;
    qLineColor = RGB(0, 0, 255);
    const int kPenWidth = 10;
    hOPen = CreatePen(PS_SOLID, kPenWidth, qLineColor);
    hPenOld = (HPEN)SelectObject(hdc, hOPen);

    const int hX = iX * gkiSqrSz + 2 * kPenWidth;
    const int lX = (iX + 1) * gkiSqrSz - 2 * kPenWidth;
    const int hY = iY * gkiSqrSz + 2 * kPenWidth;
    const int lY = (iY + 1) * gkiSqrSz - 2 * kPenWidth;

    Arc(hdc, lX, lY, hX, hY, 0, 0, 0, 0);

    SelectObject(hdc, hPenOld);
    DeleteObject(hOPen);
}
bool HasWon()
{
    if (GameBoard[0][0] != kBlank)
    {
        if (GameBoard[0][1] == GameBoard[0][0] &&
            GameBoard[0][2] == GameBoard[0][0]) return true;
        if (GameBoard[1][0] == GameBoard[0][0] &&
            GameBoard[2][0] == GameBoard[0][0]) return true;

    }

    if (GameBoard[1][1] != kBlank)
    {
        if (GameBoard[0][0] == GameBoard[1][1] &&
            GameBoard[2][2] == GameBoard[1][1]) return true;
        if (GameBoard[1][0] == GameBoard[1][1] &&
            GameBoard[1][2] == GameBoard[1][1]) return true;
        if (GameBoard[2][0] == GameBoard[1][1] &&
            GameBoard[0][2] == GameBoard[1][1]) return true;
        if (GameBoard[0][1] == GameBoard[1][1] &&
            GameBoard[2][1] == GameBoard[1][1]) return true;
    }
    if (GameBoard[2][2] != kBlank)
    {
        if (GameBoard[0][2] == GameBoard[2][2] &&
            GameBoard[1][2] == GameBoard[2][2]) return true;
        if (GameBoard[2][0] == GameBoard[2][2] &&
            GameBoard[2][1] == GameBoard[2][2]) return true;
    }
    return false;
}
bool IsBoardFull()
{
    for (int iX = 0; iX < 3; ++iX)
        for (int iY = 0; iY < 3; ++iY)
            if (GameBoard[iX][iY] == kBlank)
                return false;
    return true;
}
void game(HDC hdc, HWND hWnd)
{
    drawScoreBoard(hdc, hWnd);
    DrawBoard(hdc);

    for (int iX = 0; iX < 3; ++iX)
        for (int iY = 0; iY < 3; ++iY)
        {
            if (GameBoard[iY][iX] == kX)
                DrawX(hdc, iX, iY);
            else if (GameBoard[iY][iX] == kO)
                DrawO(hdc, iX, iY);
        }

    if (HasWon())
    {
        if (GamePlayerTurn == kO)
            MessageBox(hWnd, L"Player 1 wins", L"New Game?", MB_OK);
        else
            MessageBox(hWnd, L"Player 2 wins", L"New Game?", MB_OK);
        ResetGame();
        InvalidateRect(hWnd, 0, TRUE);
    }
    else
    {
        if (IsBoardFull())
        {
            MessageBox(hWnd, L"Its a draw", L"New Game?", MB_OK);
            ResetGame();
            InvalidateRect(hWnd, 0, TRUE); 

        }
    }
}
