#include "common_types.h"
#include "Graphics.h"
#include <wrl/client.h>
#include <d3dcompiler.h>
#include <array>

#pragma comment(lib, "D3D11.lib")
#pragma comment(lib, "D3DCompiler.lib")

namespace wrl = Microsoft::WRL;

Graphics::Graphics(HWND hwnd)
{
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = hwnd;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	D3D11CreateDeviceAndSwapChain(
		NULL, D3D_DRIVER_TYPE_HARDWARE,
		nullptr, 0, nullptr, 0,
		D3D11_SDK_VERSION,
		&sd, &pSwap, &pDevice,
		nullptr, &pContext
		);
	ID3D11Resource* pBackBuffer = nullptr;
	pSwap->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&pBackBuffer));
	pDevice->CreateRenderTargetView(
		pBackBuffer, nullptr, &pTarget
		);
	pBackBuffer->Release();
}

Graphics::~Graphics()
{
	//if (pTarget.GetAddressOf() != nullptr)
	//{
	//	pTarget.Get()->Release();
	//}
	if (pContext != nullptr)
	{
		pContext->Release();
	}
	if (pSwap != nullptr)
	{
		pSwap->Release();
	}
	if (pDevice != nullptr)
	{
		pDevice->Release();
	}
}

void Graphics::EndFrame()
{
	pSwap->Present(1u, 0u);
}

void Graphics::ClearBuffer(float r, float g, float b) noexcept
{
	const float colour[] = { r,  g, b, 1.0f };
	pContext->ClearRenderTargetView(pTarget.Get(), colour);
}

void Graphics::DrawTestTriangle(float angle)
{
	struct Vertex
	{
		float x;
		float y;
		uint8 r;
		uint8 g;
		uint8 b;
		uint8 a;
	};

	// Create triangle
	const Vertex vertices[] =
	{
		{0.0f, 0.5f, 255, 0, 0, 255},
		{0.5f, -0.5f, 0, 255, 0, 255},
		{-0.5f, -0.5f, 0, 0, 255, 255},
	};

	wrl::ComPtr<ID3D11Buffer> pVertexBuffer;
	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0u;
	bd.MiscFlags = 0u;
	bd.ByteWidth = sizeof(vertices);
	bd.StructureByteStride = sizeof(Vertex);
	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = vertices;

	// TODO error handling
	HRESULT hResult;
	pDevice->CreateBuffer(&bd, &sd, &pVertexBuffer);

	// Bind vertex buffer to pipeline
	const UINT stride = sizeof(Vertex);
	const UINT offset = 0u;
	pContext->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);

	struct ConstantBuffer
	{
		struct
		{
			float element[4][4];
		} transformation;
	};
	// https://www.youtube.com/watch?v=VELCxc0fmwY
	// 05:11
	const ConstantBuffer cb =
	{
		{
			(3.0f/4.0f)*std::cos(angle),	std::sin(angle),	0.0f, 0.0f,
			(3.0f/4.0f)*-std::sin(angle),	std::cos(angle),	0.0f, 0.0f,
			0.0f,				0.0f,				1.0f, 0.0f,
			0.0f,				0.0f,				0.0f, 1.0f,
		}
	};
	wrl::ComPtr<ID3D11Buffer> pConstantBuffer;
	D3D11_BUFFER_DESC cbd;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd.MiscFlags = 0u;
	cbd.ByteWidth = sizeof(cb);
	cbd.StructureByteStride = 0u;
	D3D11_SUBRESOURCE_DATA csd = {};
	csd.pSysMem = &cb;
	// TODO GFX_THROW_INFO
	pDevice->CreateBuffer(&cbd, &csd, &pConstantBuffer);

	// Bind constant buffer to vertex shader
	pContext->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf());

	wrl::ComPtr<ID3DBlob> pBlob;

	// Create pixel shader
	wrl::ComPtr<ID3D11PixelShader> pPixelShader;
	// TODO error handling
	/*
	https://www.youtube.com/watch?v=KR8bP0G07fc
	03:00
	*/
	D3DReadFileToBlob(L"PixelShader.cso", &pBlob);
	pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader);

	// Bind pixel shader
	pContext->PSSetShader(pPixelShader.Get(), nullptr, 0u);

	// Create Vertex shader
	wrl::ComPtr<ID3D11VertexShader> pVertexShader;
	//D3DReadFileToBlob(L"VertexShader.cso", &pBlob);

	// TODO error handling
	/*
	https://www.youtube.com/watch?v=pfbWt1BnPIo
	27:00
	*/
	D3DReadFileToBlob(L"VertexShader.cso", &pBlob);
	pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader);

	// Bind vertex shader
	pContext->VSSetShader(pVertexShader.Get(), nullptr, 0u);


	// input ...
	wrl::ComPtr<ID3D11InputLayout> pInputLayout;
	const D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{"Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0,
		D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"Colour", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	// TODO error handling
	/*
	https://www.youtube.com/watch?v=KR8bP0G07fc
	20:00
	*/

	pDevice->CreateInputLayout(
		ied, (UINT)std::size(ied),
		pBlob->GetBufferPointer(), pBlob->GetBufferSize(),
		&pInputLayout
		);

	pContext->IASetInputLayout(pInputLayout.Get());
	
	// Bind render target
	pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), nullptr);  // pTarget.GetAddressOf()

	// Set primitive topology
	pContext->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Configure viewport
	D3D11_VIEWPORT vp;
	vp.Width = 800;
	vp.Height = 600;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	pContext->RSSetViewports(1u, &vp);

	// TODO error handling
	/*
	https://www.youtube.com/watch?v=pfbWt1BnPIo
	18:00
	*/

	pContext->Draw((UINT)std::size(vertices), 0u);
}
