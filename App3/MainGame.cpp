#include "pch.h"

using namespace App3;
using namespace Concurrency;
using namespace Windows::Foundation;
using namespace Windows::System::Threading;
using namespace Windows::Storage::Pickers;
using namespace Windows::Storage;

MainGame::MainGame(const shared_ptr<DeviceResources>& deviceResources) :
	m_deviceResources(deviceResources)
{
	m_deviceResources->RegisterDeviceNotify(this);
	OutputDebugString(L"MainGame\n");
	m_input = shared_ptr<InputComponent>(new InputComponent(m_deviceResources));
	m_sceneRenderer = unique_ptr<Sample3DSceneRenderer>(new Sample3DSceneRenderer(m_deviceResources, m_input));
}

MainGame::~MainGame()
{
	m_deviceResources->RegisterDeviceNotify(nullptr);
}

void MainGame::CreateWindowSizeDependentResources()
{
	m_sceneRenderer->CreateWindowSizeDependentResources();
}

void MainGame::StartRenderLoop()
{
	// If the animation render loop is already running then do not start another thread.
	if (m_renderLoopWorker != nullptr && m_renderLoopWorker->Status == AsyncStatus::Started)
	{
		return;
	}

	// Create a task that will be run on a background thread.
	auto workItemHandler = ref new WorkItemHandler([this](IAsyncAction ^ action)
	{
		// Calculate the updated frame and render once per vertical blanking interval.
		while (action->Status == AsyncStatus::Started)
		{
			critical_section::scoped_lock lock(m_criticalSection);
			Update();
			if (Render())
			{
				m_deviceResources->Present();
			}
		}
	});

	// Run task on a dedicated high priority background thread.
	m_renderLoopWorker = ThreadPool::RunAsync(workItemHandler, WorkItemPriority::High, WorkItemOptions::TimeSliced);
}

void MainGame::StopRenderLoop()
{
	m_renderLoopWorker->Cancel();
}

void MainGame::Update()
{
	m_timer.Tick([&]()
	{
		m_input->Update(m_timer);
		m_sceneRenderer->Update(m_timer);
	});
}

bool MainGame::Render()
{
	// Don't try to render anything before the first Update.
	if (m_timer.GetFrameCount() == 0)
		return false;

	auto context = m_deviceResources->GetD3DDeviceContext();

	// Reset the viewport to target the whole screen.
	auto viewport = m_deviceResources->GetScreenViewport();
	context->RSSetViewports(1, &viewport);

	// Reset render targets to the screen.
	ID3D11RenderTargetView *const targets[1] = { m_deviceResources->GetBackBufferRenderTargetView() };
	context->OMSetRenderTargets(1, targets, m_deviceResources->GetDepthStencilView());

	// Clear the back buffer and depth stencil view.
	float color[4] = { 0.785f, 0.902f, 0.980f, 1.0f };
	context->ClearRenderTargetView(m_deviceResources->GetBackBufferRenderTargetView(), color);
	context->ClearDepthStencilView(m_deviceResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	m_sceneRenderer->Render();
	return true;
}

void MainGame::LoadFileActive(StorageFile^ file) {

	auto buffer = create_task(FileIO::ReadBufferAsync(file))
		.then([=](Streams::IBuffer^ fileBuffer)
	{
		auto stl = StlFile(fileBuffer, file->Name);
		m_sceneRenderer->LoadFile(stl);
	});
}

void MainGame::OnDeviceLost()
{
	m_sceneRenderer->ReleaseDeviceDependentResources();
}

void MainGame::OnDeviceRestored()
{
	m_sceneRenderer->CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
}
