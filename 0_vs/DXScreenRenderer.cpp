#include "DXScreenRenderer.h"

bool DXScreenRenderer::Failed(HRESULT aResult, int errNr) {
	if (FAILED(aResult)) {
		LPTSTR buffer;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, (DWORD)aResult, LANG_USER_DEFAULT, (LPTSTR)&buffer, 0, nullptr);

		MessageBox(0, buffer, _T("Fatal error: Err nr. #" + errNr), MB_OK | MB_ICONERROR);
		LocalFree(buffer);

		ReleaseInterfaces();
		return true;
	}
	return false;
}

#pragma region Dynamic Vertex Buffer Helper Functions
void DXScreenRenderer::AddVertex(Vertex vertex, ID3D11Texture2D* textureUsed) {
	if (DynamicVertexLastTextureUsed != textureUsed && DynamicVertexLastTextureUsed != nullptr) {
		Flush();
	}

	if (DynamicVertexBufferCounter >= DynamicVertexBufferSize) {
		Flush();
	}

	DynamicVertexLastTextureUsed = textureUsed;
	DynamicVertexBufferCache[DynamicVertexBufferCounter] = vertex;
	DynamicVertexBufferCounter++;
}

void DXScreenRenderer::Flush() {
	// Do magical things with buffer here <-->

	// Clear buffer
	D3D11_MAPPED_SUBRESOURCE subresource;

	HRESULT successfullyMapped = DeviceContext->Map(DynamicVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource);
	if (Failed(successfullyMapped, -15)) {
		return;
	}

	Vertex* bufferPointer = (Vertex*)subresource.pData;
	memcpy(bufferPointer, DynamicVertexBufferCache, sizeof(Vertex) * DynamicVertexBufferCounter);

	DeviceContext->Unmap(DynamicVertexBuffer, 0);

	DeviceContext->Draw(DynamicVertexBufferCounter, 0);

	DynamicVertexBufferCounter = 0;
}
#pragma endregion

void DXScreenRenderer::ReleaseInterfaces() {
	SwapChain->Release();
	Device->Release();
	DeviceContext->Release();
}

bool DXScreenRenderer::InitDirectX() {
	#pragma region BufferDescription
	DXGI_MODE_DESC bufferDescription;
	bufferDescription.Width = 800;
	bufferDescription.Height = 600;
	bufferDescription.RefreshRate = DXGI_RATIONAL{ 60, 1 };
	bufferDescription.Format = DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM;
	bufferDescription.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER::DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
	bufferDescription.Scaling = DXGI_MODE_SCALING::DXGI_MODE_SCALING_CENTERED;
	#pragma endregion

	#pragma region SwapChainDescription
	DXGI_SWAP_CHAIN_DESC swapChainDescription;
	swapChainDescription.BufferDesc = bufferDescription;
	swapChainDescription.BufferCount = 1;
	swapChainDescription.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDescription.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	swapChainDescription.OutputWindow = *WindowHandle;
	swapChainDescription.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_DISCARD; // best performance
	swapChainDescription.Windowed = TRUE;
	swapChainDescription.SampleDesc.Count = 1;
	swapChainDescription.SampleDesc.Quality = 0;
	#pragma endregion

	#pragma region RenderTargetViewDescription
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDescription;
	renderTargetViewDescription.Format = DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM;
	renderTargetViewDescription.ViewDimension = D3D11_RTV_DIMENSION::D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDescription.Texture2D.MipSlice = { 0 };
	#pragma endregion

	#pragma region Initialize variables
	D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE;
	D3D_FEATURE_LEVEL supportedFeaturelevel = D3D_FEATURE_LEVEL_11_0;
	UINT creationFlags = D3D11_CREATE_DEVICE_DEBUG;

	// Create device and swap chain
	HRESULT successfullyCreatedDeviceAndSwapChain = D3D11CreateDeviceAndSwapChain(
		NULL,
		driverType,
		NULL,
		creationFlags,
		NULL,
		0,
		D3D11_SDK_VERSION,
		&swapChainDescription,
		&SwapChain,
		&Device,
		&supportedFeaturelevel,
		&DeviceContext
	);
	if (Failed(successfullyCreatedDeviceAndSwapChain, 0)) {
		return false;
	}

	// Create swap chain buffer
	HRESULT successfullyCreatedSwapChainBuffer = SwapChain->GetBuffer(
		0,
		__uuidof(ID3D11Texture2D),
		reinterpret_cast<void**>(&RenderTarget)
	);
	if (Failed(successfullyCreatedDeviceAndSwapChain, 1)) {
		return false;
	}

	// Create render target view
	HRESULT a = Device->CreateRenderTargetView(RenderTarget, &renderTargetViewDescription, &RenderTargetView);
	if (Failed(successfullyCreatedDeviceAndSwapChain, 2)) {
		return false;
	}
	#pragma endregion

	return true;
}

bool DXScreenRenderer::InitShader(LPCWSTR aFilename) {
	// General shader-compiler variables
	ID3D10Blob* dataBlob = nullptr;
	ID3D10Blob* errorBlob = nullptr;
	ID3D11VertexShader* vertexShader = nullptr;
	ID3D11PixelShader* pixelShader = nullptr;

	#pragma region VertexShader
	// Open and compile vertex shader
	HRESULT successfullyCompiledVS = D3DCompileFromFile(aFilename, NULL, NULL, "VS_main", "vs_4_0", D3DCOMPILE_DEBUG, 0, &dataBlob, &errorBlob);

	// Create vertex shader
	if (errorBlob != nullptr) {
		const char* error = (const char*)errorBlob->GetBufferPointer();
		OutputDebugString(error);
		OutputDebugString("\n");
		if (Failed(successfullyCompiledVS, -5)) {
			return false;
		}
	}


	HRESULT vertexShaderCreationResult = Device->CreateVertexShader( dataBlob->GetBufferPointer(), dataBlob->GetBufferSize(), 0, &vertexShader );

	if (Failed(vertexShaderCreationResult, -6)) {
		return false;
	}
	#pragma endregion

	#pragma region InputLayout
	// Create vertex shader input layout
	ID3D11InputLayout* inputLayout;
	D3D11_INPUT_ELEMENT_DESC inputLayoutDescription[3];

	inputLayoutDescription[0].SemanticName = "POSITION";
	inputLayoutDescription[0].SemanticIndex = 0;
	inputLayoutDescription[0].Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32_UINT;
	inputLayoutDescription[0].InputSlot = 0;
	inputLayoutDescription[0].AlignedByteOffset = 0;
	inputLayoutDescription[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputLayoutDescription[0].InstanceDataStepRate = 0;

	inputLayoutDescription[1].SemanticName = "COLOR";
	inputLayoutDescription[1].SemanticIndex = 0;
	inputLayoutDescription[1].Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputLayoutDescription[1].InputSlot = 0;
	inputLayoutDescription[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	inputLayoutDescription[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputLayoutDescription[1].InstanceDataStepRate = 0;

	inputLayoutDescription[2].SemanticName = "TEXCOORD";
	inputLayoutDescription[2].SemanticIndex = 0;
	inputLayoutDescription[2].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputLayoutDescription[2].InputSlot = 0;
	inputLayoutDescription[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	inputLayoutDescription[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputLayoutDescription[2].InstanceDataStepRate = 0;

	HRESULT inputLayoutCreationResult = Device->CreateInputLayout(inputLayoutDescription, 3, dataBlob->GetBufferPointer(), dataBlob->GetBufferSize(), &inputLayout);
	if (Failed(inputLayoutCreationResult, -9)) {
		return false;
	}
	#pragma endregion

	// Reset blobs
	if (dataBlob != 0)
		dataBlob->Release();

	if (errorBlob != 0)
		errorBlob->Release();

	#pragma region PixelShader
	// Open and compile pixel shader
	HRESULT successfullyCompiledPS = D3DCompileFromFile( aFilename, NULL, NULL, "PS_main", "ps_4_0", 0, 0, &dataBlob, &errorBlob );
	if (errorBlob != nullptr) {
		const char* error = (const char*)errorBlob->GetBufferPointer();
		OutputDebugString(error);
		OutputDebugString("\n");
		if (Failed(successfullyCompiledPS, -7)) {
			return false;
		}
	}

	HRESULT pixelShaderCreationResult = Device->CreatePixelShader( dataBlob->GetBufferPointer(), dataBlob->GetBufferSize(), 0, &pixelShader );

	if (Failed(pixelShaderCreationResult, -8)) {
		return false;
	}
	#pragma endregion

	// Reset blobs
	if (dataBlob != 0)
		dataBlob->Release();

	if (errorBlob != 0)
		errorBlob->Release();

	#pragma region ConstantBuffer
	// Create screensize constantbuffer
	ConstantBufferStruct activeBuffer =
	{
		{ 800, 600, 0, 0 },
		DirectX::XMMatrixIdentity()
	};

	D3D11_BUFFER_DESC constantBufferDescription;
	constantBufferDescription.ByteWidth = sizeof(ConstantBufferStruct);
	constantBufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	constantBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constantBufferDescription.MiscFlags = 0;
	constantBufferDescription.StructureByteStride = 16;

	D3D11_SUBRESOURCE_DATA constentBufferSubResData;
	constentBufferSubResData.SysMemPitch = 0;
	constentBufferSubResData.SysMemSlicePitch = 0;
	constentBufferSubResData.pSysMem = &activeBuffer;

	HRESULT constantBufferCreationResult = Device->CreateBuffer(
		&constantBufferDescription,
		&constentBufferSubResData,
		&ConstantBuffer
	);

	if (Failed(constantBufferCreationResult, -11)) {
		return false;
	}

	DeviceContext->VSSetConstantBuffers(0, 1, &ConstantBuffer);
	DeviceContext->PSSetConstantBuffers(0, 1, &ConstantBuffer);
	#pragma endregion

	#pragma region DynamicVertexBuffer
	D3D11_BUFFER_DESC DynamicVertexBufferDescription;
	DynamicVertexBufferDescription.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
	DynamicVertexBufferDescription.ByteWidth = sizeof(Vertex) * DynamicVertexBufferSize;
	DynamicVertexBufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
	DynamicVertexBufferDescription.MiscFlags = 0;
	DynamicVertexBufferDescription.StructureByteStride = sizeof(Vertex);
	DynamicVertexBufferDescription.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;

	HRESULT successfullyCreatedDynamicVertexBuffer = Device->CreateBuffer(
		&DynamicVertexBufferDescription,
		NULL, // filled this value before, since we filled a static buffer - now we'll fill in the values afterwards and don't need initial data
		&DynamicVertexBuffer
	);
	if (Failed(successfullyCreatedDynamicVertexBuffer, -13)) {
		return false;
	}
	#pragma endregion

	#pragma region VertexShaderCache
	DynamicVertexBufferCache = new Vertex[DynamicVertexBufferSize];
	#pragma endregion

	#pragma region VertexBuffer
	// VertexBuffer is commented in - code was based on the old DirectX-code, this time we're using a dynamic vertex buffer
	/*
	// Create vertices required for the buffer
	Vertex vertices[] = {
		{
			200, 0, 0,
			0, 0.74, 1, 0,
			1.0f, 0.0f
		},

		{
			0, 0, 0,
			1, 0.74, 0, 1,
			0.0f, 0.0f
		},

		{
			200, 200, 0,
			1, 0.74, 0, 0,
			1.0f, 1.0f
		},

		{
			0, 200, 0,
			0, 0.74, 1, 1,
			0.0f, 1.0f
		}
		/*
		200, 0, 0.0f, 0.8f, 0.72f, 1.0f, 1.0f, 1.0f, 1.0f,
		0, 0, 0.0f, 0.0f, 0.72f, 0.3f, 1.0f, 1.0f, 1.0f,
		200, 200, 0.0f, 0.1f, 0.12f, 1.0f, 1.0f, 1.0f, 1.0f,
		0, 200, 0.0f, 0.0f, 0.66f, 0.7f, 1.0f, 1.0f, 1.0f,

		600, 400, 0.0f, 0.2f, 0.72f, 1.0f, 1.0f, 1.0f, 1.0f,
		400, 400, 0.0f, 0.0f, 1.00f, 0.3f, 1.0f, 1.0f, 1.0f,
		600, 600, 0.0f, 0.1f, 0.12f, 0.2f, 1.0f, 1.0f, 1.0f,
		400, 600, 0.0f, 0.2f, 0.66f, 0.7f, 1.0f, 1.0f, 1.0f
		*
	};

	D3D11_BUFFER_DESC bufferDescription;
	bufferDescription.ByteWidth = sizeof(vertices);
	bufferDescription.Usage = D3D11_USAGE_DEFAULT;
	bufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDescription.CPUAccessFlags = 0;
	bufferDescription.MiscFlags = 0;
	bufferDescription.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA subResData;
	subResData.SysMemPitch = 0;
	subResData.SysMemSlicePitch = 0;
	subResData.pSysMem = vertices;

	HRESULT bufferCreationResult = Device->CreateBuffer(
		&bufferDescription,
		&subResData,
		&VertexBuffer
	);

	if (Failed(inputLayoutCreationResult, -10)) {
		return false;
	}
	*/
	#pragma endregion

	#pragma region Fill device context
	D3D11_VIEWPORT viewport;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width =  800;
	viewport.Height = 600;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;

	// Set all things created as active
	DeviceContext->OMSetRenderTargets(1, &RenderTargetView, 0);
	DeviceContext->RSSetViewports(1, &viewport);

	DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	DeviceContext->IASetInputLayout(inputLayout);

	DeviceContext->VSSetShader(vertexShader, 0, 0);
	DeviceContext->PSSetShader(pixelShader, 0, 0);

	// Additional vertex buffer information
	UINT strides = sizeof(Vertex);
	UINT offset = 0;

	DeviceContext->IASetVertexBuffers(0, 1, &DynamicVertexBuffer, &strides, &offset);
	#pragma endregion

	return true;
}

bool DXScreenRenderer::Initialize(HWND* hWnd) {
	WindowHandle = hWnd;

	if (!InitDirectX()) {
		return false;
	}

	if (!InitShader(L"shader.hlsl")) {
		return false;
	}

	return true;
}

void DXScreenRenderer::Shutdown(HWND* hWnd) {
	for (int i = 0; i < ImageVector.size(); i++) {
		delete ImageVector[i];
	}
	delete[] DynamicVertexBufferCache;
}


IImage* DXScreenRenderer::LoadImage(const TCHAR* filename) {
	// Create our own image
	DXImage* image = new DXImage();
	image->CreateFromFile(filename);

	D3D11_TEXTURE2D_DESC textureDescription;
	textureDescription.Width = image->Size.x;
	textureDescription.Height = image->Size.y;
	textureDescription.MipLevels = 1;
	textureDescription.ArraySize = 1;
	textureDescription.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	textureDescription.SampleDesc.Count = 1;
	textureDescription.SampleDesc.Quality = 0;
	textureDescription.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	textureDescription.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDescription.CPUAccessFlags = 0;
	textureDescription.MiscFlags = 0;

	image->InternalData.pSysMem = &(image->PixelWithAlpha[0]);
	image->InternalData.SysMemPitch = sizeof(ColorInfoA)* image->Size.x;
	image->InternalData.SysMemSlicePitch = NULL;

	HRESULT successfullyCreatedTexture = Device->CreateTexture2D(&textureDescription, &(image->InternalData), &image->InternalTexture);
	if (Failed(successfullyCreatedTexture, -20)) {
		return nullptr;
	}

	ID3D11ShaderResourceView* shaderResourceView[1];

	HRESULT ShaderFromFileResult = Device->CreateShaderResourceView(image->InternalTexture, NULL, &shaderResourceView[0]);
	if (Failed(ShaderFromFileResult, -21))
		return nullptr;

	D3D11_SAMPLER_DESC samplerDescription;
	ZeroMemory(&samplerDescription, sizeof(D3D11_SAMPLER_DESC));
	samplerDescription.Filter = D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
	samplerDescription.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDescription.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDescription.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	ID3D11SamplerState* samplerState;


	HRESULT SamplerStateCreateResult = Device->CreateSamplerState(&samplerDescription, &samplerState);

	if (Failed(SamplerStateCreateResult, -13)) {
		return nullptr;
	}

	DeviceContext->PSSetSamplers(ImageVector.size(), 1, &samplerState);

	DeviceContext->VSSetShaderResources(ImageVector.size(), 1, shaderResourceView);
	DeviceContext->PSSetShaderResources(ImageVector.size(), 1, shaderResourceView);

	// Add it to our internal storage
	ImageVector.push_back(image);

	return image;
}

bool dir = false;
float screenSize[4] = { 800, 600, 0, 0 };

void DXScreenRenderer::BeginDraw() {
	float bgColor[4] = {
		sin(GetTickCount() / 250.f) / 4,
		sin(GetTickCount() / 250.f) / 4,
		sin(GetTickCount() / 250.f) / 4,
		sin(GetTickCount() / 250.f) / 4
	};

	HRESULT PresentResult = SwapChain->Present(1, 0);
	if (Failed(PresentResult, -4)) {
		return;
	}

	DeviceContext->ClearRenderTargetView(RenderTargetView, bgColor);
	return;
}

void DXScreenRenderer::EndDraw() {
	Flush();
	HRESULT PresentResult = SwapChain->Present(1, 0);
	if (Failed(PresentResult, -4)) {
		return;
	}
}

void DXScreenRenderer::DrawImage(int MapId, const RECT* screenPosition, const RECT* imagePosition) {
	return DrawImage(ImageVector[MapId], screenPosition, imagePosition);
}

void DXScreenRenderer::DrawImage(IImage* image, const RECT* screenPosition, const RECT* imagePosition) {
	float texCoord[] = { 0.0f, 0.0f };

	texCoord[0] = imagePosition->left;
	texCoord[1] = imagePosition->top;
	((DXImage*)image)->GetTextureCoordinate(&texCoord[0], &texCoord[1]);
	Vertex vertexTL = Vertex();
	vertexTL.X = screenPosition->left;
	vertexTL.Y = screenPosition->top;
	vertexTL.Z = 0;
	vertexTL.U = texCoord[0];
	vertexTL.V = texCoord[1];
	vertexTL.R = 1.0f;
	vertexTL.G = 1.0f;
	vertexTL.B = 1.0f;
	vertexTL.A = 1.0f;

	texCoord[0] = imagePosition->left + imagePosition->right;
	texCoord[1] = imagePosition->top;
	((DXImage*)image)->GetTextureCoordinate(&texCoord[0], &texCoord[1]);
	Vertex vertexTR = Vertex();
	vertexTR.X = screenPosition->left + screenPosition->right;
	vertexTR.Y = screenPosition->top;
	vertexTR.Z = 0;
	vertexTR.U = texCoord[0];
	vertexTR.V = texCoord[1];
	vertexTR.R = 0.2f;
	vertexTR.G = 0.2f;
	vertexTR.B = 0.2f;
	vertexTR.A = 1.0f;

	texCoord[0] = imagePosition->left;
	texCoord[1] = imagePosition->top + imagePosition->bottom;
	((DXImage*)image)->GetTextureCoordinate(&texCoord[0], &texCoord[1]);
	Vertex vertexBL = Vertex();
	vertexBL.X = screenPosition->left;
	vertexBL.Y = screenPosition->top + screenPosition->bottom;
	vertexBL.Z = 0;
	vertexBL.U = texCoord[0];
	vertexBL.V = texCoord[1];
	vertexBL.R = 0.5f;
	vertexBL.G = 0.5f;
	vertexBL.B = 0.5f;
	vertexBL.A = 1.0f;

	texCoord[0] = imagePosition->left + screenPosition->right;
	texCoord[1] = imagePosition->top + imagePosition->bottom;
	((DXImage*)image)->GetTextureCoordinate(&texCoord[0], &texCoord[1]);
	Vertex vertexBR = Vertex();
	vertexBR.X = screenPosition->left + screenPosition->right;
	vertexBR.Y = screenPosition->top + screenPosition->bottom;
	vertexBR.Z = 0;
	vertexBR.U = texCoord[0];
	vertexBR.V = texCoord[1];
	vertexBR.R = 1.0f;
	vertexBR.G = 1.0f;
	vertexBR.B = 1.0f;
	vertexBR.A = 1.0f;

	AddVertex(vertexTR, nullptr);
	AddVertex(vertexBR, nullptr);
	AddVertex(vertexTL, nullptr);
	AddVertex(vertexBL, nullptr);
}

void DXScreenRenderer::DrawText(const RECT* screenPosition, const TCHAR* string) {
	
}