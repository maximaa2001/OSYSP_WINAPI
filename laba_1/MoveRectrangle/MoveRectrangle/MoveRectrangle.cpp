#include <Windows.h>
#include <gdiplus.h>
#include <time.h>

#pragma comment(lib, "gdiplus.lib")
using namespace Gdiplus;
using namespace std;


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void DrawImage(HDC hDC, RECT rc);
UINT autoMove(HWND);

MSG msg;
HWND hwnd;
WNDCLASS wc;


int CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE prev, PWSTR szCmdLine, int nCndShow) {
	GdiplusStartupInput gdiplusStartupInput; // Связано с загрузкой картинки
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL); // Включаем GDI+
	srand(time(NULL));

	wc.hInstance = hInstance;
	wc.lpszClassName = L"MyApp";
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszMenuName = NULL;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.lpfnWndProc = WndProc;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	if (!RegisterClass(&wc)) {
		return 0;
	}
	hwnd = CreateWindow(wc.lpszClassName, L"Заголовок", WS_OVERLAPPEDWINDOW, 0, 0, 800, 600, NULL, NULL, wc.hInstance, NULL);

	ShowWindow(hwnd, nCndShow);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	GdiplusShutdown(gdiplusToken); // Завершение работы GDI+
	return 0;
}



RECT rect;
long sx;
long sy;
int width;
int hight;
bool isMoved = false;
bool isImage = false;
POINT pointPressed;
int timer = 0;

int side;
int autoSide;
bool isAutoMove = false;


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT paint;
	HDC hdc;
	switch (message)
	{
	case WM_CREATE:
		rect.left = 50;
		rect.top = 50;
		rect.right = 200;
		rect.bottom = 120;
		width = rect.right - rect.left;
		hight = rect.bottom - rect.top;
		break;
	case WM_SIZE:
		sx = LOWORD(lParam);
		sy = HIWORD(lParam);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &paint);
		if (isImage) {
			DrawImage(hdc, rect);
		}
		else {
			Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
			FillRect(hdc, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
		}
		EndPaint(hWnd, &paint);
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_RIGHT:
			if (!isAutoMove) {
				if (rect.right >= sx) {
					SetTimer(hWnd, 1, 30, NULL);
					side = 1;
				}
				else {
					rect.right = rect.right + 10;
					rect.left = rect.left + 10;
					InvalidateRect(hWnd, NULL, TRUE);
				}
			}
			break;
		case VK_LEFT:
			if (!isAutoMove) {
				if (rect.left <= 0) {
					SetTimer(hWnd, 1, 30, NULL);
					side = 3;
				}
				else {
					rect.right = rect.right - 10;
					rect.left = rect.left - 10;
					InvalidateRect(hWnd, NULL, TRUE);
				}
			}
			break;
		case VK_UP:
			if (!isAutoMove) {
				if (rect.top <= 0) {
					SetTimer(hWnd, 1, 30, NULL);
					side = 0;
				}
				else {
					rect.top = rect.top - 10;
					rect.bottom = rect.bottom - 10;
					InvalidateRect(hWnd, NULL, TRUE);
				}
			}
			break;
		case VK_DOWN:
			if (!isAutoMove) {
				if (rect.bottom >= sy) {
					SetTimer(hWnd, 1, 30, NULL);
					side = 2;
				}
				else {
					rect.top = rect.top + 10;
					rect.bottom = rect.bottom + 10;
					InvalidateRect(hWnd, NULL, TRUE);
				}
			}
			break;
		case VK_TAB:
			if (!isAutoMove) {
				autoSide = rand() % 4;
				isAutoMove = true;
				SetTimer(hWnd, 2, 30, (TIMERPROC)&autoMove);
			}
			else {
				KillTimer(hWnd, 2);
				isAutoMove = false;
			}
		}
		break;
	case WM_LBUTTONDOWN:
		if (!isAutoMove) {
			if (LOWORD(lParam) > rect.left && LOWORD(lParam) < rect.right && HIWORD(lParam) > rect.top && HIWORD(lParam) < rect.bottom) {
				isMoved = true;
				pointPressed.x = LOWORD(lParam) - rect.left;
				pointPressed.y = HIWORD(lParam) - rect.top;
			}
		}
		break;
	case WM_MOUSEMOVE:
		if (isMoved) {
			if (LOWORD(lParam) - pointPressed.x + width >= sx) {
				SetTimer(hWnd, 1, 30, NULL);
				side = 1;
				isMoved = false;
			}
			else if (LOWORD(lParam) - pointPressed.x <= 0) {
				SetTimer(hWnd, 1, 30, NULL);
				side = 3;
				isMoved = false;
			}
			else if (HIWORD(lParam) - pointPressed.y <= 0) {
				SetTimer(hWnd, 1, 30, NULL);
				side = 0;
				isMoved = false;
			}
			else if (HIWORD(lParam) - pointPressed.y + hight >= sy) {
				SetTimer(hWnd, 1, 30, NULL);
				side = 2;
				isMoved = false;
			}
			else {
				rect.left = LOWORD(lParam) - pointPressed.x;
				rect.right = LOWORD(lParam) - pointPressed.x + width;
				rect.top = HIWORD(lParam) - pointPressed.y;
				rect.bottom = HIWORD(lParam) - pointPressed.y + hight;
				InvalidateRect(hWnd, NULL, TRUE);
			}
		}
		break;
	case WM_LBUTTONUP:
		if (isMoved) {
			isMoved = false;
		}
		break;
	case WM_MOUSEWHEEL:
		if (!isAutoMove) {
			if (LOWORD(wParam) == MK_SHIFT) {
				if ((short)HIWORD(wParam) > 0) {
					if (rect.right >= sx) {
						SetTimer(hWnd, 1, 30, NULL);
						side = 1;
					}
					else {
						rect.right = rect.right + 10;
						rect.left = rect.left + 10;
						InvalidateRect(hWnd, NULL, TRUE);
					}
				}
				else {
					if (rect.left <= 0) {
						SetTimer(hWnd, 1, 30, NULL);
						side = 3;
					}
					else {
						rect.right = rect.right - 10;
						rect.left = rect.left - 10;
						InvalidateRect(hWnd, NULL, TRUE);
					}
				}
			}
			else if ((short)HIWORD(wParam) > 0) {
				if (rect.top <= 0) {
					SetTimer(hWnd, 1, 30, NULL);
					side = 0;
				}
				else {
					rect.top = rect.top - 10;
					rect.bottom = rect.bottom - 10;
					InvalidateRect(hWnd, NULL, TRUE);
				}
			}
			else {
				if (rect.bottom >= sy) {
					SetTimer(hWnd, 1, 30, NULL);
					side = 2;
				}
				else {
					rect.top = rect.top + 10;
					rect.bottom = rect.bottom + 10;
					InvalidateRect(hWnd, NULL, TRUE);
				}
			}
		}
		break;
	case WM_RBUTTONDOWN:
		if (isImage) {
			isImage = false;
		}
		else {
			isImage = true;
		}
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_TIMER:
		timer++;
		if (timer <= 10)
		{
			int bouncе = timer + 1;
			switch (side)
			{
			case 0:
				rect.top = rect.top + bouncе;
				rect.bottom = rect.bottom + bouncе;
				break;
			case 1:
				rect.right = rect.right - bouncе;
				rect.left = rect.left - bouncе;
				break;
			case 2:
				rect.top = rect.top - bouncе;
				rect.bottom = rect.bottom - bouncе;
				break;
			case 3:
				rect.right = rect.right + bouncе;
				rect.left = rect.left + bouncе;
				break;
			}
			InvalidateRect(hWnd, NULL, TRUE);
		}
		else {
			timer = 0;
			KillTimer(hWnd, 1);
		}
		break;
	case WM_DESTROY: PostQuitMessage(0);
		break;
	default: return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


void DrawImage(HDC hdc, RECT rect)
{
	Graphics graphics(hdc);
	Bitmap BackBuf(rect.right, rect.bottom, &graphics);
	Graphics temp(&BackBuf);

	Image img = L"mike.png";
	temp.DrawImage(&img, 0, 0);

	graphics.DrawImage(&BackBuf, rect.left, rect.top, 0, 0, rect.right, rect.bottom, UnitPixel);
}

UINT autoMove(HWND hWnd) {
	if (isAutoMove) {
		if (rect.right < sx && rect.top > 0 && rect.left > 0 && rect.bottom < sy) {
			switch (autoSide) {
			case 0:
				rect.right = rect.right + 5;
				rect.left = rect.left + 5;
				rect.top = rect.top - 5;
				rect.bottom = rect.bottom - 5;
				break;
			case 1:
				rect.right = rect.right + 5;
				rect.left = rect.left + 5;
				rect.top = rect.top + 5;
				rect.bottom = rect.bottom + 5;
				break;
			case 2:
				rect.right = rect.right - 5;
				rect.left = rect.left - 5;
				rect.top = rect.top + 5;
				rect.bottom = rect.bottom + 5;
				break;
			case 3:
				rect.right = rect.right - 5;
				rect.left = rect.left - 5;
				rect.top = rect.top - 5;
				rect.bottom = rect.bottom - 5;
				break;
			}
			InvalidateRect(hWnd, NULL, TRUE);
		}
		else if (rect.top <= 0) {
			int temp = rand() % 2;
			if (temp == 0) {
				autoSide = 1;
			}
			else {
				autoSide = 2;
			}
			rect.top = 1;
			rect.bottom = hight + 1;
		}
		else if (rect.right >= sx) {
			int temp = rand() % 2;
			if (temp == 0) {
				autoSide = 2;
			}
			else {
				autoSide = 3;
			}
			rect.right = sx - 1;
			rect.left = sx - 1 - width;
		}
		else if (rect.bottom >= sy) {
			int temp = rand() % 2;
			if (temp == 0) {
				autoSide = 0;
			}
			else {
				autoSide = 3;
			}
			rect.bottom = sy - 1;
			rect.top = sy - 1 - hight;
		}
		else if (rect.left <= 0) {
			int temp = rand() % 2;
			if (temp == 0) {
				autoSide = 0;
			}
			else {
				autoSide = 1;
			}
			rect.left = 1;
			rect.right = width + 1;
		}
	}
	return 0;
}



