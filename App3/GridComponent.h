#pragma once

#include "..\Common\DeviceResources.h"
#include "..\Common\StepTimer.h"
#include "InputComponent.h"

namespace App3
{
	// Renders the current FPS value in the bottom right corner of the screen using Direct2D and DirectWrite.
	class GridComponent
	{
	public:
		GridComponent(const std::shared_ptr<DX::DeviceResources>& deviceResources
			, const std::shared_ptr<InputComponent>& inputComponent);
			
		void CreateWindowDependentResources();
		void CreateDeviceDependentResources();
		void ReleaseDeviceDependentResources();
		void Update(DX::StepTimer const& timer, float offsetY);
		void Render();

	private:
		const int gridSize = 12;

		std::shared_ptr<DX::DeviceResources> m_deviceResources;
		std::shared_ptr<InputComponent> m_input;

		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_constantBuffer;
		Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_inputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_gridVertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_quadVertexBuffer;

		Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_vertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_pixelShader;

		ModelViewProjectionConstantBuffer	m_constantBufferData;
		int m_gridVertexCount;
		bool m_isLoadingComplete;
	};
}