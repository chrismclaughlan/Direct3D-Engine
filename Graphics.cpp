#include "common_types.h"
#include "Graphics.h"
#include <wrl/client.h>
#include <d3dcompiler.h>
#include <array>
#include <DirectXMath.h>
#include <sstream>

#pragma comment(lib, "D3D11.lib")
#pragma comment(lib, "D3DCompiler.lib")

namespace wrl = Microsoft::WRL;
namespace dx = DirectX;

#define ALEXIS_THROW_EXCEPTION(hResult)\
if ((int32)(hResult) != 0)\
{\
	throw Graphics::Exception(__LINE__, __FILE__, hResult);\
}\

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
	wrl::ComPtr<ID3D11Resource> pBackBuffer;
	ALEXIS_THROW_EXCEPTION(pSwap->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer));
	ALEXIS_THROW_EXCEPTION(pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &pTarget));

	// Create Z-Buffer
	D3D11_DEPTH_STENCIL_DESC dsd = {};
	dsd.DepthEnable = TRUE;
	dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsd.DepthFunc = D3D11_COMPARISON_LESS;
	wrl::ComPtr<ID3D11DepthStencilState> pDSState;
	ALEXIS_THROW_EXCEPTION(pDevice->CreateDepthStencilState(&dsd, &pDSState));

	// Bind depth state
	pContext->OMSetDepthStencilState(pDSState.Get(), 1u);

	wrl::ComPtr<ID3D11Texture2D> pDepthStencil;
	D3D11_TEXTURE2D_DESC dd = {};
	dd.Width = 800u;
	dd.Height = 600u;
	dd.MipLevels = 1u;
	dd.ArraySize = 1u;
	dd.Format = DXGI_FORMAT_D32_FLOAT;
	dd.SampleDesc.Count = 1u;
	dd.SampleDesc.Quality = 0u;
	dd.Usage = D3D11_USAGE_DEFAULT;
	dd.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	ALEXIS_THROW_EXCEPTION(pDevice->CreateTexture2D(&dd, nullptr, &pDepthStencil));

	// Create view of depth stencil texture
	D3D11_DEPTH_STENCIL_VIEW_DESC  ddsv = {};
	ddsv.Format = DXGI_FORMAT_D32_FLOAT;
	ddsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	ddsv.Texture2D.MipSlice = 0u;
	ALEXIS_THROW_EXCEPTION(pDevice->CreateDepthStencilView(pDepthStencil.Get(), &ddsv, &pDSV));

	pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), pDSV.Get());
}

Graphics::~Graphics()
{
	//if (pTarget.GetAddressOf() != nullptr)
	//{
	//	pTarget.Get()->Release();
	//}
	//if (pContext != nullptr)
	//{
	//	pContext->Release();
	//}
	//if (pSwap != nullptr)
	//{
	//	pSwap->Release();
	//}
	//if (pDevice != nullptr)
	//{
	//	pDevice->Release();
	//}
}

void Graphics::EndFrame()
{
	pSwap->Present(1u, 0u);
}

void Graphics::ClearBuffer(float r, float g, float b) noexcept
{
	const float colour[] = { r,  g, b, 1.0f };
	pContext->ClearRenderTargetView(pTarget.Get(), colour);
	pContext->ClearDepthStencilView(pDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
}

void Graphics::DrawTestTriangle(float angle, float x, float z)
{
	struct Vertex
	{
		struct
		{
			float x;
			float y;
			float z;
		} pos;
	};

	// Create triangle
	Vertex vertices[] =
	{
		{-1.0f,	-1.0f,	-1.0f	},
		{1.0f,	-1.0f,	-1.0f	},
		{-1.0f,	1.0f,	-1.0f	},
		{1.0f,	1.0f,	-1.0f	},
		{-1.0f,	-1.0f,	1.0f	},
		{1.0f,	-1.0f,	1.0f	},
		{-1.0f,	1.0f,	1.0f	},
		{1.0f,	1.0f,	1.0f	},
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
	ALEXIS_THROW_EXCEPTION(pDevice->CreateBuffer(&bd, &sd, &pVertexBuffer));

	// Bind vertex buffer to pipeline
	const UINT stride = sizeof(Vertex);
	const UINT offset = 0u;
	pContext->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);

	// Create index buffer
	const uint16 indices[] =
	{
		0,2,1, 2,3,1,
		1,3,5, 3,7,5,
		2,6,3, 3,6,7,
		4,5,7, 4,7,6,
		0,4,2, 2,4,6,
		0,1,4, 1,5,4
	};
	wrl::ComPtr<ID3D11Buffer> pIndexBuffer;
	D3D11_BUFFER_DESC ibd = {};
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0u;
	ibd.MiscFlags = 0u;
	ibd.ByteWidth = sizeof(indices);
	ibd.StructureByteStride = sizeof(uint16);
	D3D11_SUBRESOURCE_DATA isd = {};
	isd.pSysMem = indices;
	ALEXIS_THROW_EXCEPTION(pDevice->CreateBuffer(&ibd, &isd, &pIndexBuffer));

	// Bind index buffer
	pContext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);

	struct ConstantBuffer
	{
		dx::XMMATRIX transform;
	};
	const ConstantBuffer cb =
	{
		{
			dx::XMMatrixTranspose(
				dx::XMMatrixRotationZ(angle) *
				dx::XMMatrixRotationX(angle) *
				dx::XMMatrixTranslation(x, 0.0f, z + 4.0f) *
				dx::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 10.0f)
				)
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
	ALEXIS_THROW_EXCEPTION(pDevice->CreateBuffer(&cbd, &csd, &pConstantBuffer));

	// Bind constant buffer to vertex shader
	pContext->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf());

	struct ConstantBuffer2
	{
		struct
		{
			float r;
			float g;
			float b;
			float a;
		} face_colours[6];
	};
	const ConstantBuffer2 cb2 =
	{
		{
			{1.0f, 0.0f, 1.0f},
			{1.0f, 0.0f, 0.0f},
			{0.0f, 1.0f, 0.0f},
			{0.0f, 0.0f, 1.0f},
			{1.0f, 1.0f, 0.0f},
			{0.0f, 1.0f, 1.0f},
		}
	};
	wrl::ComPtr<ID3D11Buffer> pConstantBuffer2;
	D3D11_BUFFER_DESC cbd2;
	cbd2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd2.Usage = D3D11_USAGE_DEFAULT;
	cbd2.CPUAccessFlags = 0u;
	cbd2.MiscFlags = 0u;
	cbd2.ByteWidth = sizeof(cb2);
	cbd2.StructureByteStride = 0u;
	D3D11_SUBRESOURCE_DATA csd2 = {};
	csd2.pSysMem = &cb2;
	ALEXIS_THROW_EXCEPTION(pDevice->CreateBuffer(&cbd2, &csd2, &pConstantBuffer2));

	// Bind constant buffer
	pContext->PSSetConstantBuffers(0u, 1u, pConstantBuffer2.GetAddressOf());

	wrl::ComPtr<ID3DBlob> pBlob;
	// Create pixel shader
	wrl::ComPtr<ID3D11PixelShader> pPixelShader;
	ALEXIS_THROW_EXCEPTION(D3DReadFileToBlob(L"PixelShader.cso", &pBlob));
	ALEXIS_THROW_EXCEPTION(pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader));

	// Bind pixel shader
	pContext->PSSetShader(pPixelShader.Get(), nullptr, 0u);

	// Create Vertex shader
	wrl::ComPtr<ID3D11VertexShader> pVertexShader;
	//D3DReadFileToBlob(L"VertexShader.cso", &pBlob);

	ALEXIS_THROW_EXCEPTION(D3DReadFileToBlob(L"VertexShader.cso", &pBlob));
	ALEXIS_THROW_EXCEPTION(pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader));

	// Bind vertex shader
	pContext->VSSetShader(pVertexShader.Get(), nullptr, 0u);

	//
	wrl::ComPtr<ID3D11InputLayout> pInputLayout;
	const D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
		D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	ALEXIS_THROW_EXCEPTION(pDevice->CreateInputLayout(ied, (UINT)std::size(ied), pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &pInputLayout));
	
	pContext->IASetInputLayout(pInputLayout.Get());
	
	// Bind render target
	//pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), nullptr);  // pTarget.GetAddressOf()

	// Set primitive topology
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

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
	pContext->DrawIndexed((UINT)std::size(indices), 0u, 0u);
}

/**//******************** EXCEPTIONS ********************/

Graphics::Exception::Exception(int32 line, const char* file, HRESULT hResult)
	: AlexisException(line, file), hResult(hResult) {}

const char* Graphics::Exception::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error code] " << GetErrorCode() << std::endl
		<< "[Description] " << GetErrorString() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Graphics::Exception::GetType() const noexcept
{
	return "Alexis Graphics Exception";
}

std::string Graphics::Exception::TranslateErrorCode(HRESULT hResult) noexcept
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

HRESULT Graphics::Exception::GetErrorCode() const noexcept
{
	return hResult;
}

std::string Graphics::Exception::GetErrorString() const noexcept
{
	return TranslateErrorCode(hResult);
}