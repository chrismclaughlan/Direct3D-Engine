#pragma once
#include "common_windows.h"
#include "Window.h"

class App
{
private:
	Window wnd;
	void DoFrame()
	{
		angle += 0.01f;
		wnd.Gfx().ClearBuffer(0.5f, 0.5f, 1.0f);
		wnd.Gfx().DrawTestTriangle(
			angle,
			wnd.mouse.GetPosX() / 400.0f - 1.0f,
			-wnd.mouse.GetPosY() / 300.0f + 1.0f
			);
		wnd.Gfx().DrawTestTriangle(
			-angle,
			0,
			0
			);
		wnd.Gfx().EndFrame();
	}
	void HandleInput()
	{
		// while(!wnd.keyboard...
		while (!wnd.mouse.IsEmpty())
		{
			const auto e = wnd.mouse.Read();
			switch (e.GetType())
			{
			case Mouse::Event::Type::WheelDown:
			{
				angle += 0.1f;
			} break;
			case Mouse::Event::Type::WheelUp:
			{
				angle -= 0.1f;
			} break;
			}
		}
	}
	float angle = 0;  // test

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
			HandleInput();
			DoFrame();
		}
	}
};