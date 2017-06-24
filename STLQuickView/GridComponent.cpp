#include "pch.h"
#include "GridComponent.h"
#include "Common/DirectXHelper.h"

using namespace App3;
using namespace DirectX;
using namespace Microsoft::WRL;

GridComponent::GridComponent(
	const std::shared_ptr<DX::DeviceResources>& deviceResources,
	const std::shared_ptr<InputComponent>& inputComponent
) :
	m_deviceResources(deviceResources),
	m_isLoadingComplete(false)
{
	// CreateDeviceDependentResources();
	m_input = inputComponent;
}

void GridComponent::Update(DX::StepTimer const& timer, float offsetY)
{
	XMMATRIX translationMatrix = XMMatrixTranslation(0, offsetY, 0);
	XMMATRIX rotationMatrix = m_input->GetRotation();
    XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(translationMatrix * rotationMatrix));

	XMMATRIX viewMatrix = m_input->GetView();
	XMStoreFloat4x4(&m_constantBufferData.view, viewMatrix);
}

void GridComponent::Render()
{
	if (!m_isLoadingComplete)
		return;

	auto context = m_deviceResources->GetD3DDeviceContext();

	context->UpdateSubresource1(
		m_constantBuffer.Get(),
		0,
		NULL,
		&m_constantBufferData,
		0,
		0,
		0
	);
	context->VSSetConstantBuffers1(
		0,
		1,
		m_constantBuffer.GetAddressOf(),
		nullptr,
		nullptr
	);

	context->IASetInputLayout(m_inputLayout.Get());

	context->VSSetShader(
		m_vertexShader.Get(),
		nullptr,
		0
	);

	context->PSSetShader(
		m_pixelShader.Get(),
		nullptr,
		0
	);

	UINT stride = sizeof(VertexPositionColor);
	UINT offset = 0;
	context->IASetVertexBuffers(
		0,
		1,
		m_gridVertexBuffer.GetAddressOf(),
		&stride,
		&offset
	);

	ID3D11RasterizerState* pRSsolidFrame;
	D3D11_RASTERIZER_DESC RSSolidFrameDesc;
	RSSolidFrameDesc.FillMode = D3D11_FILL_SOLID;
	RSSolidFrameDesc.CullMode = D3D11_CULL_FRONT;
	RSSolidFrameDesc.ScissorEnable = false;
	RSSolidFrameDesc.DepthBias = false;
	RSSolidFrameDesc.FrontCounterClockwise = true;
	RSSolidFrameDesc.DepthBiasClamp = 0;
	RSSolidFrameDesc.SlopeScaledDepthBias = 0;
	RSSolidFrameDesc.DepthClipEnable = false;
	RSSolidFrameDesc.MultisampleEnable = false;
	RSSolidFrameDesc.AntialiasedLineEnable = true;

	auto device = m_deviceResources->GetD3DDevice();
	device->CreateRasterizerState(&RSSolidFrameDesc, &pRSsolidFrame);
	context->RSSetState(pRSsolidFrame);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	context->Draw(m_gridVertexCount, 0);

	context->IASetVertexBuffers(
		0,
		1,
		m_quadVertexBuffer.GetAddressOf(),
		&stride,
		&offset
	);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->Draw(6, 0);
}

VertexPositionColor* CreateGrid(int size)
{
	auto vertexList = new VertexPositionColor[(size + 1) * 4];
	float QuadDistance = (float)size / 2;
	float m = 4;
	auto cc = XMFLOAT3(0.8f, 0.8f, 0.8f);
	for (int i = 0; i < size + 1; i++)
	{
		float position = i - QuadDistance;
		vertexList[4 * i/**/] = { XMFLOAT3(position*m,0,QuadDistance*m), cc };
		vertexList[4 * i + 1] = { XMFLOAT3(position*m,0,-QuadDistance*m),cc };
		vertexList[4 * i + 2] = { XMFLOAT3(QuadDistance*m,0,position*m), cc };
		vertexList[4 * i + 3] = { XMFLOAT3(-QuadDistance*m,0,position*m),cc };
	}

	return vertexList;
}

void GridComponent::CreateWindowDependentResources() {
	const XMVECTORF32 eye = { 3.0f, 5.0f, 7.5f, 0.0f };
	const XMVECTORF32 at = { 0.0f, 0.0f, 0.0f, 0.0f };
	const XMVECTORF32 up = { 0.0f, 1.0f, 0.0f, 0.0f };

	// Determine viewport and orientation
	auto deviceOutputSize = m_deviceResources->GetOutputSize();
	float aspectRatio = deviceOutputSize.Width / deviceOutputSize.Height;
	float fovAngleY = 70.0f * XM_PI / 180.0f;

	auto orientation = m_deviceResources->GetOrientationTransform3D();
	auto orientationMatrix = XMLoadFloat4x4(&orientation);

	auto perspectiveMatrix = XMMatrixPerspectiveFovRH(
		fovAngleY,
		aspectRatio,
		0.01f,
		100.0f
	);

	// Build matrices
	auto worldMatrix = XMMatrixIdentity();
	auto viewMatrix = XMMatrixTranspose(XMMatrixLookAtRH(eye, at, up));
	auto projectionMatrix = XMMatrixTranspose(perspectiveMatrix * orientationMatrix);

	// Update the constantbuffer
	XMStoreFloat4x4(&m_constantBufferData.model, worldMatrix);
	XMStoreFloat4x4(&m_constantBufferData.view, viewMatrix);
	XMStoreFloat4x4(&m_constantBufferData.projection, projectionMatrix);
}

void GridComponent::CreateDeviceDependentResources()
{
	m_isLoadingComplete = false;

	auto loadVSTask = DX::ReadDataAsync(L"GridShaderVS.cso");
	auto loadPSTask = DX::ReadDataAsync(L"GridShader.cso");

	auto createVSTask = loadVSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateVertexShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_vertexShader
			)
		);

		static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateInputLayout(
				vertexDesc,
				ARRAYSIZE(vertexDesc),
				&fileData[0],
				fileData.size(),
				&m_inputLayout
			)
		);
	});

	auto createPSTask = loadPSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreatePixelShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_pixelShader
			)
		);

		CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&constantBufferDesc,
				nullptr,
				&m_constantBuffer
			)
		);
	});

	auto createGridTask = (createPSTask).then([this]() {
		m_gridVertexCount = (gridSize + 1) * 4;
		auto grid = CreateGrid(gridSize);

		D3D11_SUBRESOURCE_DATA gridVertexBufferData = { 0 };
		gridVertexBufferData.pSysMem = grid;
		gridVertexBufferData.SysMemPitch = 0;
		gridVertexBufferData.SysMemSlicePitch = 0;

		CD3D11_BUFFER_DESC gridVertexBufferDesc(sizeof(VertexPositionColor) * m_gridVertexCount, D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&gridVertexBufferDesc,
				&gridVertexBufferData,
				&m_gridVertexBuffer
			)
		);

		int quadSize = gridSize * 2;
		auto v1 = XMFLOAT3(-quadSize, -0.02f, -quadSize);
		auto v2 = XMFLOAT3(quadSize,  -0.02f, -quadSize);
		auto v3 = XMFLOAT3(-quadSize, -0.02f, quadSize);
		auto v4 = XMFLOAT3(quadSize,  -0.02f, quadSize);
		auto cc = XMFLOAT3(0.9f, 0.9f, 0.9f);

		VertexPositionColor quadVertices[] =
		{
			{ v1, cc },
			{ v2, cc },
			{ v3, cc },
			{ v3, cc },
			{ v2, cc },
			{ v4, cc }
		};

		D3D11_SUBRESOURCE_DATA quadVertexBufferData = { 0 };
		quadVertexBufferData.pSysMem = quadVertices;
		quadVertexBufferData.SysMemPitch = 0;
		quadVertexBufferData.SysMemSlicePitch = 0;

		CD3D11_BUFFER_DESC quadVertexBufferDesc(sizeof(VertexPositionColor) * 6, D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&quadVertexBufferDesc,
				&quadVertexBufferData,
				&m_quadVertexBuffer
			)
		);

		m_isLoadingComplete = true;
	});
}

void GridComponent::ReleaseDeviceDependentResources()
{
}