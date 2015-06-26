#include "main.h"
#include "DXScreenRenderer.h"
#include "GLScreenRenderer.h"
#include "GDIScreenRenderer.h"
#include "Button.h"

#define STRINGLENGTH 100

TCHAR WindowTitle[] = "title";
TCHAR WindowClassName[] = "TestWindowClass";
HWND windowHandle;
RECT wr = { 0, 0, 800, 600 };

DXScreenRenderer renderer;

HBRUSH backgroundBrsh;
HBRUSH brsh;

RECT btnPos = { 100, 100, 128, 40 };
Button btn;

// Error handler
bool Failed(HRESULT aResult) {
	if (FAILED(aResult)) {
		LPTSTR buffer;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, nullptr, (DWORD)aResult, LANG_USER_DEFAULT, (LPTSTR)&buffer, 0, nullptr);

		MessageBox(0, buffer, _T("Fatal error"), MB_OK | MB_ICONERROR);
		LocalFree(buffer);
		return true;
	}
	return false;
}

bool mouseClicked = false;

LRESULT CALLBACK processMessages(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	int wmId, wmEvent;
	RECT StartPosition = { 20, 20, 20, 20 };
	RECT CurrentPosition = StartPosition;

	RECT ImageUV = { 0, 0, 150, 150 };
	RECT DestPos = { 0, 200, 300, 300 };
	RECT TextPos = { 400, 20, 500, 100 };

	DWORD d;
	POINT p;

	int sinusOffset = ((sin((float)GetTickCount()/1000)*20)+40)/2;

	switch (message) {
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_MOUSEFIRST:
		break;
	case WM_PAINT:
		renderer.BeginDraw();

		GetCursorPos((LPPOINT)&p);
		ScreenToClient(hWnd, &p);

		// Buttons
		for (int x = 0; x < 5; x++) {
			for (int y = 0; y < 10; y++) {
				std::string s = "";
				s += "X:";
				s += std::to_string(x);
				s += "-Y:";
				s += std::to_string(y);

				RECT r = btnPos;
				r.left += 138 * x;
				r.top += 50 * y;

				if (renderer.ImageVector.size() > 0) {
					Button btn(r, renderer.ImageVector[0], "Test!");
				
					btn.TextContent = s;
					btn.UpdateStateByMouseInput(p.x, p.y, GetAsyncKeyState(VK_LBUTTON));
					btn.Draw(&renderer);
				}
			}
		}

		// Cyan-colored squares
		for (int x = 0; x < 20; x++) {
			for (int y = 0; y < 20; y++) {
				CurrentPosition = StartPosition;
				CurrentPosition.left    += (int)sinusOffset * x;
				CurrentPosition.right   += (int)sinusOffset * x + 5;
				CurrentPosition.top		+= 10 * y;
				CurrentPosition.bottom	+= 10 * y + 5;

				//FillRect(renderer.ScreenBuffer2, (const RECT*)&CurrentPosition, brsh);		// <-- GDI specific
			}
		}

		//SetBkMode(renderer.ScreenBuffer2, TRANSPARENT);
		renderer.DrawText(&TextPos, "butts.");

		renderer.EndDraw();

		InvalidateRect(hWnd, &wr, false);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

ATOM RegisterClass(HINSTANCE hInstance, TCHAR* windowClassName) {
	WNDCLASSEX wcex = { 0 };

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = processMessages;
	wcex.hInstance = hInstance;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)nullptr;

	wcex.lpszClassName = windowClassName;

	return RegisterClassEx(&wcex);
}

BOOL InitWindow(HINSTANCE hInstance, int nCmdShow, TCHAR* windowTitle, TCHAR* windowClassName) {

	DWORD windowStyle = (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX);

	AdjustWindowRect(&wr, windowStyle, FALSE);

	windowHandle = CreateWindow(windowClassName, windowTitle, windowStyle, 1920, 0, wr.right - wr.left, wr.bottom - wr.top, NULL, NULL, hInstance, NULL);

	if (!windowHandle) {
		Failed(GetLastError());
		return false;
	}

	brsh = CreateSolidBrush(0x00ffc000);

	renderer.Initialize(&windowHandle);
	
	renderer.LoadImage("../1_files/button.bmp");
	renderer.LoadImage("../1_files/font.bmp");

	ShowWindow(windowHandle, nCmdShow);

	UpdateWindow(windowHandle);

	return true;
}

int APIENTRY _tWinMain(
	_In_ HINSTANCE instanceHandler,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR lpCmdLine,
	_In_ int nCmdShow
) {

	if (RegisterClass(instanceHandler, WindowClassName) == 0) {
		if (Failed(GetLastError())) {
			return 0;
		}
	}

	if (!InitWindow(instanceHandler, nCmdShow, WindowTitle, WindowClassName)) {
		return false;
	}

	MSG systemMessages;
	HACCEL inputAccelTable;

	while (GetMessage(&systemMessages, NULL, 0, 0)) {
		TranslateMessage(&systemMessages);
		DispatchMessage(&systemMessages);
	}

	return (int)systemMessages.wParam;
}