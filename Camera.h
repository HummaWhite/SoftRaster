#ifndef CAMERA_H
#define CAMERA_H

#include <iostream>

#include "math/Vector.h"
#include "math/Matrix.h"

const float CAMERA_ROTATE_SENSITIVITY = 0.05f;
const float CAMERA_MOVE_SENSITIVITY = 0.1f;
const float CAMERA_ROLL_SENSITIVITY = 0.05f;
const float CAMERA_FOV_SENSITIVITY = 150.0f;
const float CAMERA_PITCH_LIMIT = 88.0f;
const float CAMERA_DEFAULT_FOV = 45.0f;
const float CAMERA_DEFAULT_NEAR = 0.1f;
const float CAMERA_DEFAULT_FAR = 100.0f;
const Vec3 CAMERA_VEC_UP = { 0.0f, 0.0f, 1.0f };

class Camera
{
public:
	Camera(Vec3 pos = { 0.0f, 0.0f, 0.0f }, Vec3 angle = { 90.0f, 0.0f, 0.0f })
		:m_Pos(pos), m_Angle(angle), m_FOV(CAMERA_DEFAULT_FOV), m_CameraUp(CAMERA_VEC_UP),
		m_Near(CAMERA_DEFAULT_NEAR), m_Far(CAMERA_DEFAULT_FAR) {}

	void move(Vec3 vect);
	void move(int key);
	void roll(float angle);
	void rotate(Vec3 angle);
	void changeFOV(float offset);
	void setFOV(float fov);
	void lookAt(Vec3 pos);
	void setPos(Vec3 pos) { m_Pos = pos; }
	void setAngle(Vec3 angle) { m_Angle = angle; }
	void setDir(Vec3 dir);
	void setPlanes(float zNear, float zFar) { m_Near = zNear, m_Far = zFar; }

	float FOV() const { return m_FOV; }
	float nearPlane() const { return m_Near; }
	float farPlane() const { return m_Far; }
	Vec3 pos() const { return m_Pos; }
	Vec3 angle() const { return m_Angle; }
	Vec3 pointing();
	Mat4 viewMatrix();
	Mat4 viewMatrix(Vec3 focus) const;
	Mat4 projMatrix(int width, int height) const;

private:
	Vec3 m_Pos;
	Vec3 m_Angle;
	Vec3 m_Pointing;
	Vec3 m_CameraUp;
	float m_FOV;
	float m_Near;
	float m_Far;
};

#endif
