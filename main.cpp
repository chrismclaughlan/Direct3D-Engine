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
		Window window(800, 600, L"Alexis Engine: Direct3D");
		static float time = 0.0f;

		MSG msg;
		BOOL gResult;

		int32 mouseWheelValue = 0;
		while ((gResult = GetMessage(&msg, NULL, NULL, NULL)) > 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			// test
			while (!window.mouse.IsEmpty())
			{
				const auto e = window.mouse.Read();
				switch (e.GetType())
				{
				//case Mouse::Event::Type::Leave:
				//{
				//	window.SetTitle("Gone!");
				//} break;
				//case Mouse::Event::Type::Move:
				//{
				//	int32 intx = e.GetPosX();
				//	int32 inty = e.GetPosY();
				//	char str[40] = { 0 };
				//	std::sprintf(str, "Mouse moved to: %d %d", intx, inty);
				//	window.SetTitle(str);
				//} break;
				case Mouse::Event::Type::WheelDown:
				{
					mouseWheelValue += 1;
				} break;
				case Mouse::Event::Type::WheelUp:
				{
					mouseWheelValue -= 1;
				} break;
				}

				char str[40] = { 0 };
				std::sprintf(str, "MouseWheel: %d", mouseWheelValue);
				window.SetTitle(str);

			}

			window.Gfx().ClearBuffer(0.5f, 0.5f, 1.0f);
			time += 0.1f;
			if (time >= 6.0f)
				time = 0.0f;
			window.Gfx().DrawTestTriangle(time);
			window.Gfx().EndFrame();
		}

		if (gResult == -1)
		{
			return -1;
		}

		return msg.wParam;  // Successful end of program
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