#pragma once

#include "Common\StepTimer.h"
#include "Common\DeviceResources.h"

#include "Content\Sample3DSceneRenderer.h"
#include "Content\FpsComponent.h"

#include "InputComponent.h"

#include <ppltasks.h>

using namespace Concurrency;
using namespace DX;
using namespace std;
using namespace Windows::Foundation;
using namespace Windows::Storage;

// Renders Direct2D and 3D content on the screen.
namespace App3
{
	class MainGame : public IDeviceNotify
	{
	public:
		critical_section& GetCriticalSection() {
			return m_criticalSection;
		}

		MainGame(const shared_ptr<DeviceResources>& deviceResources);
		~MainGame();

		void CreateWindowSizeDependentResources();
		void StartRenderLoop();
		void StopRenderLoop();

		void LoadFileActive(StorageFile^ file);

		void StartTracking(float positionX, float positionY, bool isRightMouseButton) {
			m_input->StartTracking(positionX, positionY, isRightMouseButton);
		}
		void TrackingUpdate(float positionX, float positionY) {
			m_input->TrackingUpdate(positionX, positionY);
		}
		void StopTracking() {
			m_input->StopTracking();
		}
		void ZoomUpdate(int zoomDelta) {
			m_input->SetZoomDelta(zoomDelta);
		}
		void SetWireframe(bool isEnabled) {
			m_sceneRenderer->SetWireframe(isEnabled);
		}
		void SetShowLights(bool isEnabled) {
			m_sceneRenderer->SetShowLights(isEnabled);
		}
		void SetShowGrid(bool isEnabled) {
			m_sceneRenderer->SetShowGrid(isEnabled);
		}
		void Reset(float x, float y) {
			m_input->SetTranslation(0, 260);
			m_input->SetRotation(x, y);
			m_input->SetZoom(15);
		}
		shared_ptr<InputComponent> GetInput() {
			return m_input;
		}
		virtual void OnDeviceLost();
		virtual void OnDeviceRestored();

		uint32 GetFps() {
			return m_timer.GetFramesPerSecond();
		};
	private:
		shared_ptr<DeviceResources> m_deviceResources;
		unique_ptr<Sample3DSceneRenderer> m_sceneRenderer;
		shared_ptr<InputComponent> m_input;

		critical_section m_criticalSection;
		IAsyncAction^ m_renderLoopWorker;
		StepTimer m_timer;

		void Update();
		bool Render();
	};
}