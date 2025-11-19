//kasjbdnksdjasdwasdwa?
#include "framework.h"
#include "sosalovo.h"
#include <iostream>
#pragma comment (lib, "Msimg32.lib")
#pragma comment (lib, "winmm.lib")
#include <cmath>

//#ifndef UNICODE
#define UNICODE
#define COLUMN 5
#define ROWS 4
//#endif 

typedef struct {
    float x, y, width, height, dx, dy, speed;
    HBITMAP hBitmap;
} sprite;

sprite ball;
sprite racket;
sprite cube;

struct {
    HWND hWnd;
    HDC device_context, context;
    int width, height;
    HBITMAP hBack;
} window;


bool gameAction = false;

//HBITMAP hBack;// ????? ??? ???????? ???????????

sprite blocks[ROWS][COLUMN];

//#define MAX_LOADSTRING 100

// ?????????? ??????????:
//HINSTANCE hInst;                                // ??????? ?????????
//WCHAR szTitle[MAX_LOADSTRING];                  // ????? ?????? ?????????
//WCHAR szWindowClass[MAX_LOADSTRING];            // ??? ?????? ???????? ????

void InitGame()
{

    racket.hBitmap = (HBITMAP)LoadImageA(NULL, "racket.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    ball.hBitmap = (HBITMAP)LoadImageA(NULL, "ball.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    //----------------------------------------------------------------------------------------------
    racket.speed = 20;
    racket.height = 20;
    racket.width = 200;
    racket.x = window.width / 2 - racket.width / 2;
    racket.y = window.height - racket.height;

    ball.speed = 10;
    ball.height = 20;
    ball.width = 20;
    ball.x = racket.x + racket.width / 2;
    ball.y = racket.y - ball.height;
    ball.dx = (-65 + rand() % 130) / 65.;
    ball.dy = 1 - abs(ball.dx);

    for (int i = 0; i < ROWS; i++) {

        for (int j = 0; j < COLUMN; j++) {

            blocks[i][j].width = window.width / ROWS;
            blocks[i][j].height = window.height / 2 / COLUMN;
            blocks[i][j].hBitmap = (HBITMAP)LoadImageA(NULL, "cube.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
            blocks[i][j].x = blocks[i][j].width * i;
            blocks[i][j].y = blocks[i][j].height * j;

        }

    }
}

void InitWindow() { // ????????????? ????????? window

    RECT r;
    GetClientRect(window.hWnd, &r);
    window.device_context = GetDC(window.hWnd);
    window.width = r.right - r.left;
    window.height = r.bottom - r.top;
    window.context = CreateCompatibleDC(window.device_context);
    SelectObject(window.context, CreateCompatibleBitmap(window.device_context, window.width, window.height));
    GetClientRect(window.hWnd, &r);

}

void ShowBitMap(HDC hDC, int x, int y, int w, int h, HBITMAP picture)
{
    HDC hMemDC;
    hMemDC = CreateCompatibleDC(hDC);

    HBITMAP hOld = (HBITMAP)SelectObject(hMemDC, picture);
    if (hOld) {
        BITMAP bm;
        GetObject(picture, sizeof(BITMAP), &bm);

        StretchBlt(hDC, x, y, w, h, hMemDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
        SelectObject(hMemDC, hOld);

        DeleteObject(hOld);
    }
    DeleteDC(hMemDC);
}

void Show()
{
    
    ShowBitMap(window.context, 0, 0, window.width, window.height, window.hBack);
    ShowBitMap(window.context, ball.x, ball.y, ball.width, ball.height, ball.hBitmap);
    ShowBitMap(window.context, racket.x, racket.y, racket.width, racket.height, racket.hBitmap);
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLUMN; j++) {
            ShowBitMap(window.context, blocks[i][j].x, blocks[i][j].y, blocks[i][j].width, blocks[i][j].height, blocks[i][j].hBitmap);
        }
    }

    
}

void Obey()
{
    if (GetAsyncKeyState('D')) {
        racket.x += racket.speed;
    }

    if (GetAsyncKeyState('A')) {
        racket.x -= racket.speed;
    }
}

void Collusion()
{
    if (racket.x < 0) {
        racket.x = 0;
    }

    if (racket.x > window.width - racket.width) {
        racket.x = window.width - racket.width;
    }

    if (ball.x < 0 || ball.x > window.width - ball.width) {
        ball.dx *= -1;
    }

    if (ball.y < 0) {
        ball.dy *= -1;
    }

    if (ball.y > window.height) {
        ball.x = racket.x + racket.width / 2;
        ball.y = racket.y - ball.height;
        ball.dy = (35 + rand() % 65) / 100.;
        ball.dx = ball.dy - 1;
        gameAction = false;
    }

    if (racket.y < ball.y + ball.height && ball.x < racket.x + racket.width / 2) {
        ball.dx += -0.5;
        ball.dy *= -1;
    }

    if (racket.y < ball.y + ball.height && ball.x > racket.x + racket.width / 2) {
        ball.dx += 0.5;
        ball.dy *= -1;
    }

}

void ProcessBall()
{
    ball.y -= ball.dy * ball.speed;
    ball.x += ball.dx * ball.speed;
}

void ProcessGame()
{
    if (gameAction == true) {
        InvalidateRect(window.hWnd, 0, true);
        Obey();
        Collusion();
        ProcessBall();
    }
}


// ????????? ?????????? ???????, ?????????? ? ???? ?????? ????:
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    srand(time(NULL));
    const wchar_t CLASS_NAME[] = L"ABOBA";

    WNDCLASS wc = { };

    wc.lpfnWndProc = WndProc;
    wc.lpszClassName = CLASS_NAME;
    wc.hInstance = hInstance;

    RegisterClass(&wc);

    // Create the window.

    window.hWnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"ABOBA",                       // Window text
        WS_POPUP,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );



    if (window.hWnd == NULL)
    {
        return 0;
    }
    InitWindow();
    InitGame();
    ShowWindow(window.hWnd, nCmdShow);
    SetTimer(window.hWnd, 1, 16, NULL);
    // Run the message loop.

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_TIMER:

        //if (wParam == 1) {
            ProcessGame();
        //}


    case WM_KEYDOWN:

        if (GetAsyncKeyState(VK_SPACE)) {
            gameAction = true;
        }
    case WM_CREATE:

        window.hBack = (HBITMAP)LoadImageA(NULL, "back.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

        break;

    case WM_DESTROY:
            PostQuitMessage(0);

    case WM_SIZE:
    {
        window.width = LOWORD(lParam);
        window.height = HIWORD(lParam);
        racket.x = window.width / 2 - racket.width / 2;
        racket.y = window.height - racket.height;
        ball.x = racket.x + racket.width / 2;
        ball.y = racket.y - ball.height;

        InvalidateRect(hwnd, 0, true);
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        window.device_context = BeginPaint(hwnd, &ps);

        //FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
        Show();
        BitBlt(window.device_context, 0, 0, window.width, window.height, window.context, 0, 0, SRCCOPY);
        EndPaint(hwnd, &ps);
    }
    return 0;

    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
