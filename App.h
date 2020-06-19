#pragma once
#include "common_windows.h"
#include "Window.h"

class App
{
private:
	Window wnd;
	void DoFrame()
	{
		wnd.Gfx().ClearBuffer(0.5f, 0.5f, 1.0f);
		wnd.Gfx().DrawTestTriangle(
			0.0f,
			wnd.mouse.GetPosX() / 400.0f - 1.0f,
			-wnd.mouse.GetPosY() / 300.0f + 1.0f
			);
		wnd.Gfx().EndFrame();
	}

public:
	App()
		: wnd(800, 600, L"Alexis Engine: Direct3D")
	{

	}
	~App() {}

	int32 Run()
	{
		int32 exitCode;
		while (true)
		{
			if (Window::ProcessMessages(&exitCode))
			{
				return exitCode;
			}
			// dt
			//HandleInput(dt);
			//DoFrame(dt);
			DoFrame();
		}
	}
};