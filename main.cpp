#define _CRT_SECURE_NO_WARNINGS
#include "common_windows.h"  // comes first
#include "common_types.h"
#include "Window.h"
#include "alexisException.h"
#include "common_functions.h"

int32 CALLBACK WinMain(
	HINSTANCE	hInstance,
	HINSTANCE	hPrevInstance,
	LPSTR		lpCmdLine,
	int32		nCmdShow)
{
	try
	{
		Window window(940, 540, L"Alexis Engine: Direct3D");

		MSG msg;
		BOOL gResult;

		int32 mouseWheelValue = 0;
		while ((gResult = GetMessage(&msg, NULL, NULL, NULL)) > 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (gResult == -1)
		{
			return -1;
		}

		return msg.wParam;  // Successful end of program
	}
	catch (const AlexisException& e)
	{
		wchar_t* wstrWhat = charToWchar(e.what());
		wchar_t* wstrGetType = charToWchar(e.GetType());
		MessageBox(nullptr, wstrWhat, wstrGetType, MB_OK | MB_ICONEXCLAMATION);
		delete[] wstrGetType;
		delete[] wstrWhat;
	}
	catch (const std::exception& e)
	{
		wchar_t* wstrWhat = charToWchar(e.what());
		MessageBox(nullptr, wstrWhat, L"Standard Exception", MB_OK | MB_ICONEXCLAMATION);
		delete[] wstrWhat;
	}
	catch (...)
	{
		MessageBox(nullptr, L"No details available", L"Unknown Exception", 
			MB_OK | MB_ICONEXCLAMATION);
	}
	return -1;
}