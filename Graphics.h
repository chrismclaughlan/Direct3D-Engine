#pragma once
#include "common_windows.h"
#include <D3D11.h>
#include <wrl/client.h>

class Graphics
{
public:
	Graphics(HWND hwnd);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics();
	void EndFrame();
	void ClearBuffer(float r, float g, float b) noexcept;
	void DrawTestTriangle();
private:
	ID3D11Device* pDevice = nullptr;
	IDXGISwapChain* pSwap = nullptr;
	ID3D11DeviceContext* pContext = nullptr;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget = nullptr;
};