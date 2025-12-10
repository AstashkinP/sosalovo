#include "Windows.h"
#include <cmath>

#define ROWS 10
#define COLUMN 10

//стурктура где храняться данные о windows окне
struct
{
	//дескрипторы, контейнеры и буфферы для windows
	RECT rc;
	HINSTANCE hIns;
	HWND hWnd;
	HDC dev_cont, contx;
	MSG msg;
	BOOL gbool = true;
	HBITMAP hBitmap;
	//определяет размер экрана в вашей сиситеме
	int width = GetSystemMetrics(SM_CXSCREEN), height = GetSystemMetrics(SM_CYSCREEN);
} window;

struct sprite
{
	float x, y, width, height, speed, dx, dy;
	bool ischezni;
	HBITMAP hBitmap;
};

//обработка потока сообщений
static LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
};

//создания windows окна
void InitWindow()
{
	//имя класса окна
	const char* NameClass = "Window";

	//размер окна
	window.rc = { 0,0,window.width,window.height
	};

	//учет размера
	AdjustWindowRect(&window.rc, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE);

	//дескриптор класса окна
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.lpszClassName = NameClass;
	wc.hInstance = window.hIns;
	wc.lpfnWndProc = &WindowProc;

	//регистрация класса окна
	auto NameClassId = RegisterClassEx(&wc);

	//деструктор окна
	window.hWnd = CreateWindowEx(
		NULL,
		MAKEINTATOM(NameClassId),
		"practicum5",
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		window.rc.right - window.rc.left,
		window.rc.bottom - window.rc.top,
		NULL,
		NULL,
		window.hIns,
		NULL
	);

	//показ окна
	ShowWindow(window.hWnd, SW_SHOW);
}

//отрисовка изображений .bmp
void ShowBitmap(HDC hDC, int x, int y, int x1, int y1, HBITMAP hBitmapBall)
{
	HBITMAP hbm, hOldbm;
	HDC hMemDC;
	BITMAP bm;

	hMemDC = CreateCompatibleDC(hDC);
	hOldbm = (HBITMAP)SelectObject(hMemDC, hBitmapBall);

	if (hOldbm)
	{
		GetObject(hBitmapBall, sizeof(BITMAP), (LPSTR)&bm);
		StretchBlt(hDC, x, y, x1, y1, hMemDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
		SelectObject(hMemDC, hOldbm);
	}

	DeleteDC(hMemDC);
}

sprite ball;
sprite racket;
sprite blocks[ROWS][COLUMN];



//загрузка модулей приложения
void InitApp()
{
	window.dev_cont = GetDC(window.hWnd);
	window.contx = CreateCompatibleDC(window.dev_cont);
	SelectObject(window.contx, CreateCompatibleBitmap(window.dev_cont, window.width, window.height));

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

bool Colussion(int i, int  j) {
	/*blocks[i][j].ischezni = true;*/
	for (int z = 0; z < 200; z++)
	{
		float ics;
		float igrik;
		float gipoz;
		float koefX;
		float koefY;
		ics = ball.dx * ball.speed;
		igrik = ball.dy * ball.speed;
		gipoz = sqrt(ics * ics + igrik * igrik);
		koefX = ics / gipoz;
		koefY = igrik / gipoz;

		float pixx = koefX * z + ball.x;
		float pixy = koefY * -z + ball.y;

		SetPixel(window.dev_cont, pixx, pixy, RGB(255, 0, 0));
		if (
			pixx > blocks[i][j].x &&
			pixx < blocks[i][j].x + blocks[i][j].width &&
			pixy > blocks[i][j].y &&
			pixy < blocks[i][j].y + blocks[i][j].height
			) {
			float L = blocks[i][j].x + blocks[i][j].width - pixx;
			float R = pixx - blocks[i][j].x;
			float U = blocks[i][j].y + blocks[i][j].height - pixy;
			float D = pixy - blocks[i][j].y;

			float owerX;
			float owerY;
			owerX = min(L, R);
			owerY = min(U, D);

			if (blocks[i][j].ischezni == true) {

				if (owerX < owerY) {
					if (L < R) {
						/*ball.dx *= -1;*/
						blocks[i][j].ischezni;
					}

					if (R < L) {
						/*ball.dx *= -1;*/
						blocks[i][j].ischezni;
					}
				}

				if (owerY < owerX) {
					if (U < D) {
						/*ball.dy *= -1;*/
						blocks[i][j].ischezni;
					}

					if (D < U) {
						/*ball.dy *= -1;*/
						blocks[i][j].ischezni;
					}
				}
			}
			return true;
		}
		/*return false;*/
	}
}

void Obey() {
	if (GetAsyncKeyState('D')) {
		racket.x += racket.speed;
	}

	if (GetAsyncKeyState('A')) {
		racket.x -= racket.speed;
	}
}

void GetCursorPos()
{
	POINT mousePos;
	if (GetCursorPos(&mousePos)) {
		ball.x = mousePos.x;
		ball.y = mousePos.y;
	}
}

void Show() {
	window.hBitmap = (HBITMAP)LoadImageA(NULL, "back.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	ShowBitmap(window.contx, 0, 0, window.width, window.height, window.hBitmap);
	ShowBitmap(window.contx, ball.x, ball.y, ball.width, ball.height, ball.hBitmap);
	ShowBitmap(window.contx, racket.x, racket.y, racket.width, racket.height, racket.hBitmap);
	for (int i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLUMN; j++) {
			/*if (blocks[i][j].ischezni == true) {*/
			ShowBitmap(window.contx, blocks[i][j].x, blocks[i][j].y, blocks[i][j].width, blocks[i][j].height, blocks[i][j].hBitmap);
			//}
		}
	}
}

//обновление приложения
void UpdateApp()
{
	/*Show();*/
	GetCursorPos();
	for (int i = 0; i < ROWS; i++) {

		for (int j = 0; j < COLUMN; j++) {
			/*Colussion(i, j);*/
			if (Colussion(i, j))
				break;
		}
	}
}

//обработка команд устройств ввода
void UpdateKeyCode()
{
	//выход из приложения на ESC
	if (GetAsyncKeyState(VK_ESCAPE))
	{
		window.msg.message = WM_QUIT;
	}
}

//обновление изображений
void UpdateImage()
{
	BitBlt(window.dev_cont, 0, 0, window.width, window.height, window.contx, 0, 0, SRCCOPY);
	//отрисовка заднего фона
	/*ShowBitmap(window.contx, 0, 0, window.width, window.height, (HBITMAP)LoadImageA(NULL, "back.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE));*/
	Show();
}

//вход в программу
int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nShowCmd)
{
	InitWindow();
	InitApp();

	//основной цикл обновления приложения
	while (window.gbool)
	{
		//обработка соощений для окна
		while (PeekMessage(&window.msg, NULL, 0, 0, PM_REMOVE))
		{
			UpdateKeyCode();

			//отбработка сообщений
			if (window.msg.message == WM_QUIT)
			{
				window.gbool = false;
				break;
			}
			TranslateMessage(&window.msg);
			DispatchMessage(&window.msg);
		}

		UpdateImage();
		UpdateApp();

		//задержка обновления
		Sleep(16);
	}
	return 0;
}