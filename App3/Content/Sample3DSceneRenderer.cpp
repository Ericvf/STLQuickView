#include "pch.h"
#include "Sample3DSceneRenderer.h"
#include "..\Common\DirectXHelper.h"

using namespace App3;
using namespace DirectX;
using namespace Windows::Foundation;

Sample3DSceneRenderer::Sample3DSceneRenderer(
	const std::shared_ptr<DX::DeviceResources>& deviceResources,
	const shared_ptr<InputComponent>& inputComponent
) :
	m_loadingComplete(false),
	m_indexCount(0),
	m_deviceResources(deviceResources),
	m_input(inputComponent),
	file(),
	grid(std::unique_ptr<GridComponent>(new GridComponent(deviceResources, inputComponent))
)
{
	OutputDebugString(L"Sample3DSceneRenderer\n");
	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
	grid->CreateDeviceDependentResources();
}

void Sample3DSceneRenderer::Update(DX::StepTimer const& timer)
{
	float worldScale = file.scale * 25;

	//grid->Update(timer, -(file.msize.Y/2) * worldScale);
	grid->Update(timer, 0);
	if (!m_loadingComplete)
		return;

	XMMATRIX translationMatrix = XMMatrixTranslation(file.translate.X, file.translate.Y + (file.msize.Y / 2), file.translate.Z);
	XMMATRIX scaleMatrix = XMMatrixScaling(worldScale, worldScale, worldScale);
	XMMATRIX rotationMatrix = m_input->GetRotation();
	XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(translationMatrix * scaleMatrix * rotationMatrix));

	XMMATRIX viewMatrix = m_input->GetView();
	XMStoreFloat4x4(&m_constantBufferData.view, viewMatrix);

	rotationMatrix = XMMatrixInverse(nullptr, rotationMatrix);
	XMStoreFloat4x4(&m_constantBufferData.WorldInverseTranspose, XMMatrixTranspose(rotationMatrix));

	auto eye = m_input->GetEye();
	XMStoreFloat4(&m_constantBufferData.vecEye, eye * -1);

	XMStoreFloat4(&m_constantBufferData.wire, { 
		isWireframeEnabled ? 1.0f : 0.0f,
		isLightsEnabled ? 1.0f : 0.0f,
		0,
		0
	});
	XMStoreFloat4(&m_constantBufferData.color, { 1, 1, 0, 1});
}

void Sample3DSceneRenderer::Render()
{
	if (this->isGridEnabled)
		grid->Render();

	if (!m_loadingComplete)
		return;

	if (!hasFile)
		return;

	auto context = m_deviceResources->GetD3DDeviceContext();

	// Prepare the constant buffer to send it to the graphics device.
	context->UpdateSubresource1(
		m_constantBuffer.Get(),
		0,
		NULL,
		&m_constantBufferData,
		0,
		0,
		0
	);

	// Each vertex is one instance of the VertexPositionColor struct.
	UINT stride = sizeof(VertexPositionColorNormal);
	UINT offset = 0;
	context->IASetVertexBuffers(
		0,
		1,
		m_vertexBuffer.GetAddressOf(),
		&stride,
		&offset
	);

	context->IASetIndexBuffer(
		m_indexBuffer.Get(),
		DXGI_FORMAT_R16_UINT, // Each index is one 16-bit unsigned integer (short).
		0
	);

	//ID3D11RasterizerState* pRSsolidFrame;
	//D3D11_RASTERIZER_DESC RSSolidFrameDesc;
	//RSSolidFrameDesc.FillMode = D3D11_FILL_SOLID;
	//RSSolidFrameDesc.CullMode = D3D11_CULL_FRONT;
	//RSSolidFrameDesc.ScissorEnable = false;
	//RSSolidFrameDesc.DepthBias = false;
	//RSSolidFrameDesc.FrontCounterClockwise = true;
	//RSSolidFrameDesc.DepthBiasClamp = 0;
	//RSSolidFrameDesc.SlopeScaledDepthBias = 0;
	//RSSolidFrameDesc.DepthClipEnable = false;
	//RSSolidFrameDesc.MultisampleEnable = false;
	//RSSolidFrameDesc.AntialiasedLineEnable = false;

	//ID3D11RasterizerState* pRSwireFrame;
	//D3D11_RASTERIZER_DESC RSWireFrameDesc;
	//RSWireFrameDesc.FillMode = D3D11_FILL_WIREFRAME;
	//RSWireFrameDesc.CullMode = D3D11_CULL_NONE;
	//RSWireFrameDesc.ScissorEnable = false;
	//RSWireFrameDesc.DepthBias = false;
	//RSWireFrameDesc.FrontCounterClockwise = true;
	//RSWireFrameDesc.DepthBiasClamp = 0;
	//RSWireFrameDesc.SlopeScaledDepthBias = 0;
	//RSWireFrameDesc.DepthClipEnable = false;
	//RSWireFrameDesc.MultisampleEnable = false;
	//RSWireFrameDesc.AntialiasedLineEnable = true;

	//auto device = m_deviceResources->GetD3DDevice();
	//if (isWireframeEnabled) {
	//	device->CreateRasterizerState(&RSWireFrameDesc, &pRSwireFrame);
	//	context->RSSetState(pRSwireFrame);
	//}
	//else {
	//	device->CreateRasterizerState(&RSSolidFrameDesc, &pRSsolidFrame);
	//	context->RSSetState(pRSsolidFrame);
	//}

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(m_inputLayout.Get());

	// Send the constant buffer to the graphics device.
	context->VSSetConstantBuffers1(
		0,
		1,
		m_constantBuffer.GetAddressOf(),
		nullptr,
		nullptr
	);

	// Attach our vertex shader.
	context->VSSetShader(
		m_vertexShader.Get(),
		nullptr,
		0
	);

	// Attach our pixel shader.
	context->PSSetShader(
		m_pixelShader.Get(),
		nullptr,
		0
	);

	auto vertexCount = static_cast<unsigned int>(file.facets.size() * 3);
	context->Draw(vertexCount, 0);

	//context->DrawIndexed(
	//	m_indexCount,
	//	0,
	//	0
	//);
	
	//context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	//context->IASetVertexBuffers(
	//	0,
	//	1,
	//	m_vertexNormalBuffer.GetAddressOf(),
	//	&stride,
	//	&offset
	//);
	//context->Draw(m_verticesCount * 2, 0);
}

void Sample3DSceneRenderer::LoadFile(StlFile file2) {

	hasFile = false;
	this->m_loadingComplete = false;
	this->file = file2;
	hasFile = true;
	CreateWindowSizeDependentResources();
	CreateDeviceDependentResources();
	//m_input->SetZoom(file.scale + 1);
}

#pragma region 
XMFLOAT3 normalize(XMFLOAT3 i) {
	auto m = sqrt(i.x * i.x + i.y * i.y + i.z * i.z);
	return { i.x / m, i.y / m, i.z / m };
};

XMFLOAT3 add(XMFLOAT3 n, XMFLOAT3 m) {
	return { n.x + m.x, n.y + m.y, n.z + m.z };
};
#pragma endregion XMFloat Helpers

#pragma region 

void Sample3DSceneRenderer::CreateWindowSizeDependentResources()
{
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

	auto worldMatrix = XMMatrixIdentity();
	XMStoreFloat4x4(&m_constantBufferData.model, worldMatrix);
	
	auto projectionMatrix = XMMatrixTranspose(perspectiveMatrix * orientationMatrix);
	XMStoreFloat4x4(&m_constantBufferData.projection, projectionMatrix);

	grid->CreateWindowDependentResources();
}

void Sample3DSceneRenderer::CreateDeviceDependentResources()
{
	if (!hasFile)
		return;

	// Load shaders asynchronously.
	auto loadVSTask = DX::ReadDataAsync(L"SampleVertexShader.cso");
	auto loadPSTask = DX::ReadDataAsync(L"SamplePixelShader.cso");

	// After the vertex shader file is loaded, create the shader and input layout.
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
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
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

	// After the pixel shader file is loaded, create the shader and constant buffer.
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

	// Once both shaders are loaded, create the mesh.
	auto createCubeTask = (createPSTask && createVSTask).then([this]() {
		int segmentRadius = 20;
		int tubeRadius = 8;
		int segments = 40;
		int tubes = 50;

		int totalVertices = segments * tubes;
		int totalPrimitives = totalVertices * 2;
		int totalIndices = totalPrimitives * 3;

		auto vertexNormalData = new VertexPositionColorNormal[totalVertices * 2];
		auto vertexData = new VertexPositionColorNormal[totalVertices];
		//auto flatVertexData = new VertexPositionColorNormal[totalIndices];
		auto indexData = new short[totalIndices];

		int countNormalVertices = 0;
		int countVertices = 0;
		int countIndices = 0;

		// Calculate size of segment and tube
		float segmentSize = 2 * (float)M_PI / segments;
		float tubeSize = 2 * (float)M_PI / tubes;

		// Create doubles for our xyz coordinates
		float x = 0;
		float y = 0;
		float z = 0;

		XMFLOAT3 xxx[] = {
			XMFLOAT3(1, 0, 0),
			XMFLOAT3(0, 1, 0),
			XMFLOAT3(0, 0, 1)
		};

		for (int i = 0; i < segments; i++)
		{
			// Save segments
			int currentSegment = i * tubes;
			int nextSegment = ((i + 1) % segments) * tubes;

			// Loop through the number of tubes
			for (int j = 0; j < tubes; j++)
			{
				// Save next tube offset
				int m = (j + 1) % tubes;

				// Calculate X, Y, Z coordinates.. 
				x = (segmentRadius + tubeRadius * cos(j * tubeSize)) * cos(i * segmentSize);
				y = (segmentRadius + tubeRadius * cos(j * tubeSize)) * sin(i * segmentSize);
				z = tubeRadius * sin(j * tubeSize);

				// Add the vertex to global vertex list
				vertexData[countVertices++] = {
					XMFLOAT3(x, y, z),
					XMFLOAT3(0, 0, 0),
					XMFLOAT3(0, 0, 0)
				};

				short v1 = currentSegment + j;
				short v2 = currentSegment + m;
				short v3 = nextSegment + m;
				short v4 = nextSegment + j;

				// Draw the first triangle
				indexData[countIndices++] = v1;
				indexData[countIndices++] = v2;
				indexData[countIndices++] = v3;

				// Finish the quad
				indexData[countIndices++] = v3;
				indexData[countIndices++] = v4;
				indexData[countIndices++] = v1;
			}
		}
		for (int i = 0; i < totalIndices; i += 3)
		{
			// Find all 3 vertices
			auto v0 = vertexData[indexData[i]];
			auto v1 = vertexData[indexData[i + 1]];
			auto v2 = vertexData[indexData[i + 2]];

			// Calculate the triangle normal
			auto vt2 = XMFLOAT3(
				v1.pos.x - v0.pos.x,
				v1.pos.y - v0.pos.y,
				v1.pos.z - v0.pos.z
			);
			auto vt1 = XMFLOAT3(
				v2.pos.x - v0.pos.x,
				v2.pos.y - v0.pos.y,
				v2.pos.z - v0.pos.z
			);

			auto triangleCenter = XMFLOAT3(
				(v0.pos.x + v1.pos.x + v2.pos.x) / 3,
				(v0.pos.y + v1.pos.y + v2.pos.y) / 3,
				(v0.pos.z + v1.pos.z + v2.pos.z) / 3
			);

			auto triangleNormal = XMFLOAT3(
				(vt1.y * vt2.z) - (vt1.z * vt2.y),
				(vt1.z * vt2.x) - (vt1.x * vt2.z),
				(vt1.x * vt2.y) - (vt1.y * vt2.x)
			);

			v0.normal = triangleNormal;
			v1.normal = triangleNormal;
			v2.normal = triangleNormal;

			//flatVertexData[i] = v0;
			//flatVertexData[i + 1] = v1;
			//flatVertexData[i + 2] = v2;

			vertexData[indexData[i]].normal = add(vertexData[indexData[i]].normal, triangleNormal);
			vertexData[indexData[i + 1]].normal = add(vertexData[indexData[i + 1]].normal, triangleNormal);
			vertexData[indexData[i + 2]].normal = add(vertexData[indexData[i + 2]].normal, triangleNormal);
		}
		for (int i = 0, k = 0; i < totalVertices; i++)
		{
			auto n = normalize(vertexData[i].normal);
			auto posn = XMFLOAT3(
				vertexData[i].pos.x + (n.x * 10),
				vertexData[i].pos.y + (n.y * 10),
				vertexData[i].pos.z + (n.z * 10)
			);

			vertexNormalData[k++] = {
				XMFLOAT3(vertexData[i].pos.x, vertexData[i].pos.y, vertexData[i].pos.z),
				XMFLOAT3(0, 0, 0),
				XMFLOAT3(0, 0, 0)
			};

			vertexNormalData[k++] = {
				XMFLOAT3(posn.x, posn.y, posn.z),
				XMFLOAT3(0,0,0),
				XMFLOAT3(0,0,0)
			};
		}

		// Stl
		int facetCount = (int)file.facets.size();
		int vertexCount = facetCount * 3;

		auto stlVertices = new VertexPositionColorNormal[vertexCount];
		for (int i = 0, d = 0; i < facetCount; i++)
		{
			auto facet = file.facets[i];
			stlVertices[d++] = { XMFLOAT3(facet.v1.X, facet.v1.Y, facet.v1.Z), XMFLOAT3(1,0,0), XMFLOAT3(facet.normal.X, facet.normal.Y, facet.normal.Z) };
			stlVertices[d++] = { XMFLOAT3(facet.v2.X, facet.v2.Y, facet.v2.Z), XMFLOAT3(0,1,0), XMFLOAT3(facet.normal.X, facet.normal.Y, facet.normal.Z) };
			stlVertices[d++] = { XMFLOAT3(facet.v3.X, facet.v3.Y, facet.v3.Z), XMFLOAT3(0,0,1), XMFLOAT3(facet.normal.X, facet.normal.Y, facet.normal.Z) };
		}

		D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
		vertexBufferData.pSysMem = stlVertices;
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(VertexPositionColorNormal) * vertexCount, D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&vertexBufferDesc,
				&vertexBufferData,
				&m_vertexBuffer
			)
		);

		D3D11_SUBRESOURCE_DATA vertexBufferData2 = { 0 };
		vertexBufferData2.pSysMem = vertexData;
		vertexBufferData2.SysMemPitch = 0;
		vertexBufferData2.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC vertexBufferDesc2(sizeof(VertexPositionColorNormal) * totalVertices, D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&vertexBufferDesc2,
				&vertexBufferData2,
				&m_vertexTorusBuffer
			)
		);

		D3D11_SUBRESOURCE_DATA vertexBufferData1 = { 0 };
		vertexBufferData1.pSysMem = vertexNormalData;
		vertexBufferData1.SysMemPitch = 0;
		vertexBufferData1.SysMemSlicePitch = 0;

		CD3D11_BUFFER_DESC vertexBufferDesc1(sizeof(VertexPositionColorNormal) * totalVertices * 2, D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&vertexBufferDesc1,
				&vertexBufferData1,
				&m_vertexNormalBuffer
			)
		);

		m_indexCount = countIndices;// ARRAYSIZE(indexData);
		//m_verticesCount = totalVertices;
		m_verticesCount = totalVertices;

		D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
		indexBufferData.pSysMem = indexData;
		indexBufferData.SysMemPitch = 0;
		indexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC indexBufferDesc(sizeof(short) * countIndices, D3D11_BIND_INDEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&indexBufferDesc,
				&indexBufferData,
				&m_indexBuffer
			)
		);
	});

	// Once the cube is loaded, the object is ready to be rendered.
	createCubeTask.then([this]() {
		m_loadingComplete = true;
	});
}

void Sample3DSceneRenderer::ReleaseDeviceDependentResources()
{
	grid->ReleaseDeviceDependentResources();

	m_loadingComplete = false;
	m_vertexShader.Reset();
	m_inputLayout.Reset();
	m_pixelShader.Reset();
	m_constantBuffer.Reset();
	m_vertexBuffer.Reset();
	m_indexBuffer.Reset();
}

#pragma endregion Resources