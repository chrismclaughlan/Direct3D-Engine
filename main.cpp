#define _CRT_SECURE_NO_WARNINGS
#include "common_windows.h"  // comes first
#include "common_types.h"
//#include "Window.h"
#include "alexisException.h"
#include "common_functions.h"
#include "App.h"

int32 CALLBACK WinMain(
	HINSTANCE	hInstance,
	HINSTANCE	hPrevInstance,
	LPSTR		lpCmdLine,
	int32		nCmdShow)
{
	try
	{
		return App().Run();
	}
	catch (const AlexisException& e)
	{
		const std::wstring what = CharToWString(e.what());
		const std::wstring type = CharToWString(e.GetType());
		MessageBox(nullptr, what.c_str(), type.c_str(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& e)
	{
		const std::wstring what = CharToWString(e.what());
		MessageBox(nullptr, what.c_str(), L"Standard Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBox(nullptr, L"No details available", L"Unknown Exception", 
			MB_OK | MB_ICONEXCLAMATION);
	}
	return -1;
}