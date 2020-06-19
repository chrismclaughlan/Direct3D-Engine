#pragma once
#include "common_windows.h"
#include "common_types.h"
#include "alexisException.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Graphics.h"
#include <memory>
//#include <optional>

#define ALEXIS_EXCEPTION(hResult) (Window::Exception(__LINE__, __FILE__, hResult))
#define ALEXIS_LAST_EXCEPTION() (Window::Exception(__LINE__, __FILE__, GetLastError()))

struct Optional
{
	int32 wParam = {};
	bool quit = false;
};

class Window
{
public:
	class Exception : public AlexisException
	{
	public:
		Exception(int32 line, const char* file, HRESULT hResult);
		const char* what() const noexcept override;
		virtual const char* GetType() const noexcept;
		static std::string TranslateErrorCode(HRESULT hResult) noexcept;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
	private:
		HRESULT hResult;
	};
private:
	class WindowClass
	{
	public:
		static const wchar_t* GetName() noexcept;
		static HINSTANCE GetInstance() noexcept;
	private:
		WindowClass() noexcept;
		~WindowClass();
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator=(const WindowClass&) = delete;
		static constexpr const wchar_t* wndClassName = L"Alexis Engine: Direct3D";
		static WindowClass wndClass;
		HINSTANCE hInst;
	};
public:
	Window(int32 width, int32 height, const wchar_t* name); // noexcept;
	~Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	void SetTitle(const char* name);
	static bool ProcessMessages(int32* exitCode) noexcept;
	Graphics& Gfx();
private:
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
public:
	Keyboard keyboard;
	Mouse mouse;
private:
	int32 width;
	int32 height;
	HWND hwnd;
	std::unique_ptr<Graphics> pGraphics;
	//std::optional<Graphics> gfx;  // if using c++17
};