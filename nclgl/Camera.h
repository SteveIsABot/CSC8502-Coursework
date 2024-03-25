#pragma once
#include "Matrix4.h"
#include "Vector3.h"

class Camera {

public:
    Camera(void) {
        yaw = 0.0f;
        pitch = 0.0f;
        position = Vector3(0.0f, 0.0f, 0.0f);
        
        minSpeed = 30.0f;
        baseSpeed = minSpeed;
        maxSpeed = 1000.0f;
    };

    Camera(float pitch, float yaw, Vector3 position) {
        this->pitch = pitch;
        this->yaw = yaw;
        this->position = position;
        
        minSpeed = 30.0f;
        baseSpeed = minSpeed;
        maxSpeed = 1000.0f;
    }

    ~Camera(void){};

    void UpdateCamera(float dt = 1.0f);
    
    Matrix4 BuildViewMatrix();
    
    Vector3 GetPosition() const { return position; }
    void SetPosition(Vector3 val) { position = val; }
    float GetYaw() const { return yaw; }
    void SetYaw(float y) { yaw = y; }
    float GetPitch() const { return pitch; }
    void SetPitch(float p) { pitch = p; }

    Matrix4 LookAt(Vector3 at, Vector3 up);

protected:
    float yaw;
    float pitch;
    Vector3 position;

    float baseSpeed;
    float maxSpeed;
    float minSpeed;
};