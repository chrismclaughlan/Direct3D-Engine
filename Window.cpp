#include "common_functions.h"
#include "Window.h"
#include "resource.h"
#include <sstream>

Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass() noexcept : hInst(GetModuleHandle(nullptr))
{
	HICON hicon = static_cast<HICON>(LoadImage(
		hInst, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 32, 32, NULL));
	HICON hiconsm = static_cast<HICON>(LoadImage(
		hInst, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 16, 16, NULL));
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleMsgSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetInstance();
	wc.hIcon = hicon;
	wc.hCursor = NULL;
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = GetName();
	wc.hIconSm = hiconsm;
	RegisterClassEx(&wc);
}

Window::WindowClass::~WindowClass()
{
	UnregisterClass(wndClassName, GetInstance());
}

const wchar_t* Window::WindowClass::GetName() noexcept
{
	return wndClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
	return wndClass.hInst;
}

Window::Window(int32 width, int32 height, const wchar_t* name)
	: width(width), height(height)
{
#define WINDOW_STYLE (WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU)

	// Calculate window size
	RECT rect;
	rect.left = 100;
	rect.right = width + rect.left;
	rect.top = 100;
	rect.bottom = height + rect.top;
	if (AdjustWindowRect(&rect, WINDOW_STYLE, FALSE) == NULL)
	{
		throw ALEXIS_LAST_EXCEPTION();
	}
	
	hwnd = CreateWindow(
		WindowClass::GetName(), name,
		WINDOW_STYLE,
		CW_USEDEFAULT, CW_USEDEFAULT,
		rect.right - rect.left,
		rect.bottom - rect.top, NULL, NULL,
		WindowClass::GetInstance(), this
		);
	if (hwnd == NULL)
	{
		throw ALEXIS_LAST_EXCEPTION();
	}

	ShowWindow(hwnd, SW_SHOWDEFAULT);
}

Window::~Window()
{
	DestroyWindow(hwnd);
}

void Window::SetTitle(const char* name)
{
	wchar_t* title = charToWchar(name);
	if (SetWindowText(hwnd, title) == NULL)
	{
		throw ALEXIS_LAST_EXCEPTION();
	}
	delete[] title;
}

LRESULT CALLBACK Window::HandleMsgSetup
(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (msg == WM_NCCREATE)
	{
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const pwnd = static_cast<Window*>(pCreate->lpCreateParams);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pwnd));
		SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));
		return pwnd->HandleMsg(hwnd, msg, wParam, lParam);
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK Window::HandleMsgThunk
(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	// Retrieve window ptr
	Window* const pwnd = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	// Forward msg to window instance
	return pwnd->HandleMsg(hwnd, msg, wParam, lParam);
}

LRESULT Window::HandleMsg
(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
#define PREV_KEY_DOWN() (lParam & 0x40000000)

	switch (msg)
	{
	case WM_CLOSE:
	{
		PostQuitMessage(0);
	} return 0;
	case WM_KILLFOCUS:
	{
		keyboard.ClearState();
	} break;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
	{
		if (!PREV_KEY_DOWN() || keyboard.AutorepeatIsEnabled())
		{
			keyboard.OnKeyPressed(static_cast<uint8>(wParam));
		}
	} break;
	case WM_SYSKEYUP:
	case WM_KEYUP:
	{
		keyboard.OnKeyReleased(static_cast<uint8>(wParam));
	} break;
	case WM_CHAR:
	{
		keyboard.OnChar(static_cast<uint8>(wParam));
	} break;

	/******************** MOUSE MESSAGES ********************/
	case WM_MOUSEMOVE:
	{
		const POINTS pts = MAKEPOINTS(lParam);
		if (pts.x >= 0 && pts.x < width && pts.y >= 0 && pts.y < height)
		{
			mouse.OnMouseMove(pts.x, pts.y);
			if (!mouse.IsInWindow())
			{
				SetCapture(hwnd);  // capture mouse outside of window
				mouse.OnMouseEnter();
			}
		}
		else
		{
			if (wParam & (MK_LBUTTON | MK_RBUTTON))
			{
				// If dragging outside of window
				mouse.OnMouseMove(pts.x, pts.y);
			}
			else
			{
				ReleaseCapture();
				mouse.OnMouseLeave();
			}
		}
	} break;
	case WM_LBUTTONDOWN:
	{
		POINTS pts = MAKEPOINTS(lParam);
		mouse.OnLeftPressed(pts.x, pts.y);
	} break;
	case WM_RBUTTONDOWN:
	{
		POINTS pts = MAKEPOINTS(lParam);
		mouse.OnRightPressed(pts.x, pts.y);
	} break;
	case WM_LBUTTONUP:
	{
		const POINTS pts = MAKEPOINTS(lParam);
		mouse.OnLeftReleased(pts.x, pts.y);
	} break;
	case WM_RBUTTONUP:
	{
		const POINTS pts = MAKEPOINTS(lParam);
		mouse.OnRightReleased(pts.x, pts.y);
	} break;
	case WM_MOUSEWHEEL:
	{
		const POINTS pts = MAKEPOINTS(lParam);
		const int32 delta = GET_WHEEL_DELTA_WPARAM(wParam);
		mouse.OnWheelDelta(pts.x, pts.y, delta);
	} break;
	}  // switch (msg)

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

Window::Exception::Exception(int32 line, const char* file, HRESULT hResult)
	: AlexisException(line, file), hResult(hResult) {}

const char* Window::Exception::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error code] " << GetErrorCode() << std::endl
		<< "[Description] " << GetErrorString() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Window::Exception::GetType() const noexcept
{
	return "Alexis Window Exception";
}

std::string Window::Exception::TranslateErrorCode(HRESULT hResult) noexcept
{
	wchar_t* pMsgBuf = nullptr;
	DWORD nMsgLen = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, hResult, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPWSTR>(&pMsgBuf), 0, nullptr);  // originally LPSTR
	if (nMsgLen == 0)
	{
		return "Unidentified error code";
	}

	// wchar_t to string
	std::wstring ws(pMsgBuf);
	std::string errorString(ws.begin(), ws.end());

	LocalFree(pMsgBuf);
	return errorString;
}

HRESULT Window::Exception::GetErrorCode() const noexcept
{
	return hResult;
}

std::string Window::Exception::GetErrorString() const noexcept
{
	return TranslateErrorCode(hResult);
}