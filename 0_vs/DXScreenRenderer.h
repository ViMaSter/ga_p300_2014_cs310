#pragma once
#include "IScreenRenderer.h"
#include "DXImage.h"
#include "d3dcompiler.h"
#include "d3d11.h"
#include "DirectXMath.h"
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

struct Vertex {
	int X;
	int Y;
	int Z;

	float R;
	float G;
	float B;
	float A;

	float U;
	float V;
};

class DXScreenRenderer : public IScreenRenderer
{
public:
	// Helper-function for error handling
	bool Failed(HRESULT aResult, int errNr);
	// Helper-function for clean-up
	void ReleaseInterfaces();

	// DirectX-internal variables
	IDXGISwapChain* SwapChain;
	ID3D11Device* Device;
	ID3D11DeviceContext* DeviceContext;
	ID3D11Texture2D* RenderTarget;
	ID3D11RenderTargetView* RenderTargetView;
	ID3D11Buffer* DynamicVertexBuffer = nullptr;
	Vertex* DynamicVertexBufferCache = nullptr;
	ID3D11Buffer* VertexBuffer = nullptr;
	ID3D11Buffer* ConstantBuffer = nullptr;

#pragma region DynamicVertexBuffer
	int DynamicVertexBufferSize = 4;
	int DynamicVertexBufferCounter = 0;
	ID3D11Texture2D* DynamicVertexLastTextureUsed = nullptr;

	void AddVertex(Vertex vertex, ID3D11Texture2D* textureUsed);
	void Flush();
#pragma endregion DynamicVertexBuffer

	// IInterface-functions
	bool DXScreenRenderer::InitDirectX();
	bool DXScreenRenderer::InitShader(LPCWSTR aFilename);
	virtual bool Initialize(HWND* hWnd) override;
	virtual void Shutdown(HWND* hWnd) override;

	virtual IImage* LoadImage(const TCHAR* filename) override;

	virtual void BeginDraw() override;
	virtual void EndDraw() override;

	virtual void DrawImage(int MapId, const RECT* screenPosition, const RECT* imagePosition) override;
	virtual void DrawImage(IImage* image, const RECT* screenPosition, const RECT* imagePosition) override;
	virtual void DrawText(const RECT* screenPosition, const TCHAR* string) override;
};

struct ConstantBufferStruct {
	float ScreenSize[4];
	DirectX::XMMATRIX transformationMatrix;
};