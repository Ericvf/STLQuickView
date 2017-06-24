#pragma once

using namespace DirectX;

namespace App3
{
	class InputComponent
	{
	public:
		InputComponent(const std::shared_ptr<DX::DeviceResources>& deviceResources);

		void Update(DX::StepTimer const& timer);

		void StartTracking(float positionX, float positionY, bool isRightMouseButton);
		void TrackingUpdate(float positionX, float positionY);
		void StopTracking();
		void SetZoom(float zoom);
		void SetZoomDelta(float zoomDelta);

		void SetTranslation(float x, float y) {
			newTranslationX = x;
			newTranslationY = y;
		};

		void SetRotation(float x, float y) {
			newRotationX = x;
			newRotationY = y;
		};

		XMMATRIX GetRotation();
		XMMATRIX GetView();
		XMVECTORF32 GetEye();
	private:
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		XMMATRIX viewMatrix;
		bool m_tracking;
		bool isRightMouseButton;

		float startDeltaX = 0;
		float startDeltaY = 0;
		float deltaX = 0;
		float deltaY = 0;

		//Rotation - 701.368530 79.799988
		//Position - 25.587402 164.000000

		float rotationX = -676;
		float rotationY = 79;
		float translationX = 0;
		float translationY = 260;
		float zoom = 15;

		float newRotationX = rotationX;
		float newRotationY = rotationY;
		float newTranslationX = translationX;
		float newTranslationY = translationY;
		float newzoom = zoom;
	};
}