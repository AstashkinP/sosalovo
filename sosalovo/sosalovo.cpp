//kasjbdnksdjasdwasdwa﻿
#include "framework.h"
#include "Abracadabra.h"
#include <iostream>
#pragma comment (lib, "Msimg32.lib")
#pragma comment (lib, "winmm.lib")
#include <cmath>

#ifndef UNICODE
#define UNICODE
#endif 

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

//HBITMAP hBack;// хэндл для фонового изображения



//#define MAX_LOADSTRING 100

// Глобальные переменные:
//HINSTANCE hInst;                                // текущий экземпляр
//WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
//WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

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
    ball.dy = (35 + rand() % 65) / 100.;
    ball.dx = ball.dy - 1;
}

void InitWindow() { // инициализация структуры window

    RECT r;
    GetClientRect(window.hWnd, &r);
    window.width = r.right - r.left;
    window.height = r.bottom - r.top;


}

void ShowBitMap(HDC hDC, int x, int y, int w, int h, HBITMAP picture)
{
    window.context = CreateCompatibleDC(window.device_context);

    BITMAP bm;
    GetObject(picture, sizeof(BITMAP), &bm);

    HBITMAP hOld = (HBITMAP)SelectObject(window.context, picture);
    StretchBlt(window.device_context, x, y, w, h, window.context, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
    SelectObject(window.context, hOld);

    DeleteObject(hOld);
    DeleteDC(window.context);
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
        gameAction = false;
    }
}

void ProcessBall()
{
    //float length = 1 / sqrt(ball.dx * ball.dx + ball.dy * ball.dy);
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

// Отправить объявления функций, включенных в этот модуль кода:
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
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        0, 0, CW_USEDEFAULT, CW_USEDEFAULT,

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

        if (wParam == 1) {
            ProcessGame();
        }


    case WM_KEYDOWN:

        if (GetAsyncKeyState(VK_SPACE)) {
            gameAction = true;
        }
    case WM_CREATE:

        window.hBack = (HBITMAP)LoadImageA(NULL, "back.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

        break;

    case WM_DESTROY:
        PostQuitMessage(0);

        return 0;

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
        ShowBitMap(window.device_context, 0, 0, window.width, window.height, window.hBack);
        ShowBitMap(window.device_context, ball.x, ball.y, ball.width, ball.height, ball.hBitmap);
        ShowBitMap(window.device_context, racket.x, racket.y, racket.width, racket.height, racket.hBitmap);
        EndPaint(hwnd, &ps);
    }
    return 0;

    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

//int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
//                     _In_opt_ HINSTANCE hPrevInstance,
//                     _In_ PWSTR    lpCmdLine,
//                     _In_ int       nCmdShow)
//{
//    UNREFERENCED_PARAMETER(hPrevInstance);
//    UNREFERENCED_PARAMETER(lpCmdLine);
//
//    // TODO: Разместите код здесь.
//
//    // Инициализация глобальных строк
//    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
//    LoadStringW(hInstance, IDC_ABRACADABRA, szWindowClass, MAX_LOADSTRING);
//    InitWindow();
//    InitGame();
//
//    // Выполнить инициализацию приложения:
//    if (!InitInstance (hInstance, nCmdShow))
//    {
//        return FALSE;
//    }
//
//    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ABRACADABRA));
//
//    MSG msg;
//
//    // Цикл основного сообщения:
//    while (GetMessage(&msg, nullptr, 0, 0))
//    {
//        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
//        {
//            TranslateMessage(&msg);
//            DispatchMessage(&msg);
//        }
//    }
//
//    return (int) msg.wParam;
//}


//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
//


//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
//BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
//{
//   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной
//
//   window.hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
//      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
//
//   if (!window.hWnd)
//   {
//      return FALSE;
//   }
//
//   ShowWindow(window.hWnd, nCmdShow);
//   UpdateWindow(window.hWnd);
//   
//   return TRUE;
//}

//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться

//LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
//{
//    switch (message)
//    {
//    //case WM_COMMAND:
//    //    {
//    //        int wmId = LOWORD(wParam);
//    //        // Разобрать выбор в меню:
//    //        switch (wmId)
//    //        {
//    //        case IDM_ABOUT:
//    //            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
//    //            break;
//    //        case IDM_EXIT:
//    //            DestroyWindow(hWnd);
//    //            break;
//    //        default:
//    //            return DefWindowProc(hWnd, message, wParam, lParam);
//    //        }
//    //    }
//    //    break;
//    case WM_PAINT:
//        {
//            PAINTSTRUCT ps;
//            HDC hdc = BeginPaint(hWnd, &ps);
//            ShowBitmap(hdc, ball.x, ball.y, ball.width, ball.height, ball.hBitmap, false);
//            ShowBitmap(hdc, 0, 0, window.width, window.height, hBack);
//            // TODO: Добавьте сюда любой код прорисовки, использующий HDC...
//            EndPaint(hWnd, &ps);
//        }
//        break;
//    case WM_DESTROY:
//        PostQuitMessage(0);
//        break;
//    default:
//        return DefWindowProc(hWnd, message, wParam, lParam);
//    }
//    return 0;
//}

// Обработчик сообщений для окна "О программе".
//INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
//{
//    UNREFERENCED_PARAMETER(lParam);
//    switch (message)
//    {
//    case WM_INITDIALOG:
//        return (INT_PTR)TRUE;
//
//    case WM_COMMAND:
//        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
//        {
//            EndDialog(hDlg, LOWORD(wParam));
//            return (INT_PTR)TRUE;
//        }
//        break;
//    }
//    return (INT_PTR)FALSE;
//}