#pragma once

#include "..\Common\DeviceResources.h"
#include "..\Common\StepTimer.h"
#include "ShaderStructures.h"
#include "GridComponent.h"
#include "InputComponent.h"

namespace App3
{
	// This sample renderer instantiates a basic rendering pipeline.
	class Sample3DSceneRenderer
	{
	public:
		Sample3DSceneRenderer(
			const std::shared_ptr<DX::DeviceResources>& deviceResources,
			const std::shared_ptr<InputComponent>& inputComponent
		);
		
		void CreateDeviceDependentResources();
		void CreateWindowSizeDependentResources();
		void ReleaseDeviceDependentResources();
		
		void Update(DX::StepTimer const& timer);
		void Render();
		
		void LoadFile(StlFile file);
		bool m_loadingComplete;
		bool hasFile;

		void SetWireframe(bool isEnabled) {
			isWireframeEnabled = isEnabled;
		};
		void SetShowLights(bool isEnabled) {
			isLightsEnabled = isEnabled;
		};
		void SetShowGrid(bool isEnabled) {
			isGridEnabled = isEnabled;
		};

	private:
		std::shared_ptr<InputComponent> input;
		std::unique_ptr<GridComponent> grid;
		bool isWireframeEnabled = false;
		bool isLightsEnabled = true;
		bool isGridEnabled = true;
		StlFile file;

		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> m_deviceResources;
		std::shared_ptr<InputComponent> m_input;

		// Direct3D resources for cube geometry.
		Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_inputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_vertexNormalBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_vertexTorusBuffer;

		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_indexBuffer;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_vertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_pixelShader;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_constantBuffer;

		// System resources for cube geometry.
		ModelViewProjectionConstantBuffer	m_constantBufferData;
		uint32	m_indexCount;
		uint32	m_verticesCount;
	};
}

