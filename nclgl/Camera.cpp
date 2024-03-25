#include "Camera.h"
#include "Window.h"
#include <algorithm>

void Camera::UpdateCamera(float dt) {
    
    pitch -= (Window::GetMouse()->GetRelativePosition().y);
    yaw -= (Window::GetMouse()->GetRelativePosition().x);

    pitch = std::min(pitch, 90.0f);
    pitch = std::max(pitch, -90.0f);

    if(yaw < 0) yaw += 360.0f;
    if(yaw > 0) yaw -= 360.0f;

    Matrix4 rotation = Matrix4::Rotation(yaw, Vector3(0, 1, 0));

    Vector3 forward = rotation * Vector3(0, 0, -1);
    Vector3 right = rotation * Vector3(1, 0, 0);

    baseSpeed = Window::GetKeyboard()->KeyDown(KEYBOARD_CONTROL) ? baseSpeed + 10.0f : baseSpeed - 30.0f;
    if(baseSpeed > maxSpeed) baseSpeed = maxSpeed;
    if(baseSpeed < minSpeed) baseSpeed = minSpeed;

    float speed = baseSpeed * dt;

    if(Window::GetKeyboard()->KeyDown(KEYBOARD_W)) position += forward * speed;
    if(Window::GetKeyboard()->KeyDown(KEYBOARD_S)) position -= forward * speed;
    if(Window::GetKeyboard()->KeyDown(KEYBOARD_A)) position -= right * speed;
    if(Window::GetKeyboard()->KeyDown(KEYBOARD_D)) position += right * speed;

    if(Window::GetKeyboard()->KeyDown(KEYBOARD_SPACE)) position.y += speed;
    if(Window::GetKeyboard()->KeyDown(KEYBOARD_SHIFT)) position.y -= speed;

}

Matrix4 Camera::BuildViewMatrix() {
    return Matrix4::Rotation(-pitch, Vector3(1, 0, 0)) *
           Matrix4::Rotation(-yaw, Vector3(0, 1, 0)) *
           Matrix4::Translation(-position);
}

Matrix4 Camera::LookAt(Vector3 at, Vector3 up) {
	
	Vector3 camPos = this->GetPosition();
	Vector3 X, Y, Z;
	Matrix4 mat;

	Z = camPos - at;
	Z.Normalise();

	X = Vector3::Cross(up, Z);
	Y = Vector3::Cross(Z, X);
	X.Normalise();
	Y.Normalise();

	mat.values[0] = X.x;
	mat.values[4] = X.y;
	mat.values[8] = X.z;
	mat.values[12] = Vector3::Dot(-X, camPos);
	mat.values[1] = Y.x;
	mat.values[5] = Y.y;
	mat.values[9] = Y.z;
	mat.values[13] = Vector3::Dot(-Y, camPos);
	mat.values[2] = Z.x;
	mat.values[6] = Z.y;
	mat.values[10] = Z.z;
	mat.values[14] = Vector3::Dot(-Z, camPos);
	mat.values[3] = 0;
	mat.values[7] = 0;
	mat.values[11] = 0;
	mat.values[15] = 1.0f;

	return mat;
}