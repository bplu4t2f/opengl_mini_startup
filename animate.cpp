// mostly stolen from animate example at https://www.opengl.org/archives/resources/code/samples/win32_tutorial/

#pragma comment (lib, "opengl32.lib")


#include <Windows.h>
#include <GL/gl.h>
#include <stdio.h>


static HWND CreateOpenGLWindow(HINSTANCE hInstance, HDC *hdc);
static const wchar_t *shitty_get_last_error_message(const char *description);
static void debug_print_time(const char *message, LARGE_INTEGER start);


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPreviousInst, LPSTR lpszCmdLine, int nCmdShow)
{
	LARGE_INTEGER sw;

	QueryPerformanceCounter(&sw);

	HDC hdc;
	HWND hWnd = CreateOpenGLWindow(hInstance, &hdc);
	if (hWnd == NULL) {
		return 1;
	}

	debug_print_time("init bullshit", sw);

	HGLRC opengl_context = wglCreateContext(hdc);
	wglMakeCurrent(hdc, opengl_context);

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	while (true)
	{
		MSG msg;
		while (PeekMessageW(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			if (GetMessageW(&msg, NULL, 0, 0)) {
				TranslateMessage(&msg);
				DispatchMessageW(&msg);
			} else {
				goto __quit;
			}
		}

#ifndef NO_CLEAR_NO_BUG
		QueryPerformanceCounter(&sw);
		glClear(GL_COLOR_BUFFER_BIT);
		debug_print_time("glClear", sw);
#endif

		// doesn't seem to matter
		//Sleep(1000);

		QueryPerformanceCounter(&sw);
		SwapBuffers(hdc);
		debug_print_time("SwapBuffers", sw);
	}

__quit:

	return 0;
}


LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}


HWND CreateOpenGLWindow(HINSTANCE hInstance, HDC *hdc)
{
	const wchar_t *wndclassname = L"OpenGL";

	WNDCLASS wc = { 0 };
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = (WNDPROC)WindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = wndclassname;

	const wchar_t *title = L"stupid";

	if (!RegisterClassW(&wc)) {
		MessageBoxW(NULL, shitty_get_last_error_message("RegisterClassW failed"), title, MB_OK);
		return NULL;
	}

	DWORD exStyle = 0;
	DWORD style = WS_OVERLAPPEDWINDOW;
	HWND parent = NULL;
	HMENU menu = NULL;
	LPVOID param = nullptr;
	int width = 800, height = 600;
	HWND hWnd = CreateWindowExW(exStyle, wndclassname, title, style, CW_USEDEFAULT, CW_USEDEFAULT, width, height, parent, menu, hInstance, param);

	if (hWnd == NULL) {
		MessageBoxW(NULL, shitty_get_last_error_message("CreateWindowExW failed"), title, MB_OK);
		return NULL;
	}

	*hdc = GetDC(hWnd);

	PIXELFORMATDESCRIPTOR pfd = { 0 };
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;

	int pixel_format = ChoosePixelFormat(*hdc, &pfd);
	if (pixel_format == 0) {
		MessageBoxW(NULL, shitty_get_last_error_message("ChoosePixelFormat failed"), title, MB_OK);
		return NULL;
	}

	if (SetPixelFormat(*hdc, pixel_format, &pfd) == FALSE) {
		MessageBoxW(NULL, shitty_get_last_error_message("SetPixelFormat failed"), title, MB_OK);
		return NULL;
	}

	DescribePixelFormat(*hdc, pixel_format, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

	return hWnd;
}


const wchar_t *shitty_get_last_error_message(const char *description)
{
	static wchar_t buf[2000];
	int cursor = swprintf_s(buf, L"%hs: ", description);
	FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		buf + cursor, (sizeof(buf) / sizeof(wchar_t) - cursor), NULL);
	return buf;
}


void debug_print_time(const char *message, LARGE_INTEGER start)
{
	LARGE_INTEGER end;
	LARGE_INTEGER freq;
	QueryPerformanceCounter(&end);
	QueryPerformanceFrequency(&freq);
	double elapsed_ms = ((double)(end.QuadPart - start.QuadPart) / (double)freq.QuadPart) * 1000.0;
	char buf[200];
	sprintf_s(buf, "%s: %lf\n", message, elapsed_ms);
	OutputDebugStringA(buf);
}


