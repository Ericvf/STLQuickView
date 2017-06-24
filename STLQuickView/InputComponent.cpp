#include "pch.h"
#include "winuser.h"

using namespace App3;
using namespace DX;
using namespace DirectX;
using namespace Windows::Devices::Input;

InputComponent::InputComponent(const std::shared_ptr<DX::DeviceResources>& deviceResources)
	: m_deviceResources(deviceResources)
{
}



void Print(const char* x, float f) {
	char buffer[1000];
	sprintf_s(buffer, x, f);
	OutputDebugStringA(buffer);
}
void Print(const char* x, float f1, float f2) {
	char buffer[1000];
	sprintf_s(buffer, x, f1, f2);
	OutputDebugStringA(buffer);
}
void ease(float * currentval, float newval)
{
	if (*currentval != newval) {
		if (abs(*currentval - newval) < 0.01f)
		{
			*currentval = newval;
			Print("Zoom %f \n", *currentval);
			return;
		}
		else {
			*currentval += (newval - *currentval) / 10;
		}
	}
}
void InputComponent::Update(DX::StepTimer const & timer)
{
	ease(&zoom, newzoom);
	ease(&translationX, newTranslationX);
	ease(&translationY, newTranslationY);
	ease(&rotationX, newRotationX);
	ease(&rotationY, newRotationY);
}

void InputComponent::StartTracking(float positionX, float positionY, bool isRightMouseButton)
{
	startDeltaX = positionX;
	startDeltaY = positionY;
	deltaX = 0;
	deltaY = 0;
	m_tracking = true;
	this->isRightMouseButton = isRightMouseButton;
}

void InputComponent::TrackingUpdate(float positionX, float positionY)
{
	if (m_tracking) {
		deltaX = positionX - startDeltaX;
		deltaY = positionY - startDeltaY;
	}
}

void InputComponent::StopTracking()
{
	m_tracking = false;

	if (!isRightMouseButton) {
		rotationX += deltaX;
		rotationY += deltaY;
		if (rotationX < -800) rotationX += 1600;
		if (rotationX > 800) rotationX -= 1600;

		newRotationX = rotationX;
		newRotationY = rotationY;
		Print("Rotation %f %f\n", rotationX, rotationY);
	}
	else {
		translationX += deltaX;
		translationY += deltaY;
		newTranslationX = translationX;
		newTranslationY = translationY;
		Print("Position %f %f\n", translationX, translationY);
	}

	deltaX = 0;
	deltaY = 0;
}

void InputComponent::SetZoom(float p_zoom)
{
	newzoom = p_zoom;
}

void InputComponent::SetZoomDelta(float zoomDelta)
{
	newzoom = zoom + -(zoomDelta / 30);
}

XMMATRIX InputComponent::GetRotation()
{
	float rX = rotationX;
	float rY = rotationY;

	if (m_tracking && !isRightMouseButton) {
		rX += deltaX;
		rY += deltaY;
	}

	float radiansX = XM_2PI * 2.0f * (rX) / 3200;
	float radiansY = XM_2PI * 2.0f * (rY) / 3200;
	XMMATRIX rotationMatrix = XMMatrixRotationY(radiansX);
	rotationMatrix *= XMMatrixRotationX(radiansY);

	return rotationMatrix;
}

XMMATRIX InputComponent::GetView()
{
	float dx = translationX;
	float dy = translationY;

	if (m_tracking && isRightMouseButton) {
		dx += deltaX;
		dy += deltaY;
	}

	dx /= 25;
	dy /= 25;

	XMVECTORF32 eye = { 0.0f - dx, 0.0f + dy, 25.5f + zoom, 0.0f };
	XMVECTORF32 at = { 0.0f - dx, 0.0f + dy, 0.0f, 0.0f };
	XMVECTORF32 up = { 0.0f, 1.0f, 0.0f, 0.0f };

	viewMatrix = XMMatrixLookAtRH(eye, at, up);
	viewMatrix = XMMatrixTranspose(viewMatrix);

	return viewMatrix;
}

XMVECTORF32 InputComponent::GetEye()
{
	float dx = translationX;
	float dy = translationY;

	if (m_tracking && isRightMouseButton) {
		dx += deltaX;
		dy += deltaY;
	}

	dx /= 25;
	dy /= 25;

	XMVECTORF32 eye = { 0.0f - dx, 0.0f + dy, 25.5f + zoom, 0.0f };
	return eye;
}
