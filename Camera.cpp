#include "Camera.h"

#include <Windows.h>

#include "math/Vector.h"
#include "math/Matrix.h"

void Camera::move(Vec3 vect)
{
	m_Pos += vect;
}

void Camera::move(int key)
{
	switch (key)
	{
	case 'W':
		m_Pos[0] += CAMERA_MOVE_SENSITIVITY * cos(toRad(m_Angle[0]));
		m_Pos[1] += CAMERA_MOVE_SENSITIVITY * sin(toRad(m_Angle[0]));
		break;
	case 'S':
		m_Pos[0] -= CAMERA_MOVE_SENSITIVITY * cos(toRad(m_Angle[0]));
		m_Pos[1] -= CAMERA_MOVE_SENSITIVITY * sin(toRad(m_Angle[0]));
		break;
	case 'A':
		m_Pos[1] += CAMERA_MOVE_SENSITIVITY * cos(toRad(m_Angle[0]));
		m_Pos[0] -= CAMERA_MOVE_SENSITIVITY * sin(toRad(m_Angle[0]));
		break;
	case 'D':
		m_Pos[1] -= CAMERA_MOVE_SENSITIVITY * cos(toRad(m_Angle[0]));
		m_Pos[0] += CAMERA_MOVE_SENSITIVITY * sin(toRad(m_Angle[0]));
		break;
	case 'Q':
		roll(-CAMERA_ROLL_SENSITIVITY);
		break;
	case 'E':
		roll(CAMERA_ROLL_SENSITIVITY);
		break;
	case 'R':
		m_CameraUp = CAMERA_VEC_UP;
		break;
	case VK_SPACE:
		m_Pos[2] += CAMERA_MOVE_SENSITIVITY;
		break;
	case VK_SHIFT:
		m_Pos[2] -= CAMERA_MOVE_SENSITIVITY;
		break;
	}
	m_Pointing = pointing();
}

void Camera::roll(float angle)
{
	Mat4 mul(1.0f);
	mul = ::rotate(mul, m_Pointing, angle);
	Vec4 tmp = { m_CameraUp[0], m_CameraUp[1], m_CameraUp[2], 1.0f };
	tmp = mul * tmp;
	m_CameraUp = Vec3(tmp);
}

void Camera::rotate(Vec3 angle)
{
	m_Angle += angle * CAMERA_ROTATE_SENSITIVITY;
	if (m_Angle[1] > CAMERA_PITCH_LIMIT) m_Angle[1] = CAMERA_PITCH_LIMIT;
	if (m_Angle[1] < -CAMERA_PITCH_LIMIT) m_Angle[1] = -CAMERA_PITCH_LIMIT;
	m_Pointing = pointing();
}

void Camera::changeFOV(float offset)
{
	m_FOV -= toRad(offset) * CAMERA_FOV_SENSITIVITY;
	if (m_FOV > 90.0) m_FOV = 90.0;
	if (m_FOV < 15.0) m_FOV = 15.0;
}

void Camera::setFOV(float fov)
{
	m_FOV = fov;
	if (m_FOV > 90.0f) m_FOV = 90.0f;
	if (m_FOV < 15.0f) m_FOV = 15.0f;
}

void Camera::lookAt(Vec3 pos)
{
	setDir(pos - m_Pos);
}

void Camera::setDir(Vec3 dir)
{
	dir = normalize(dir);
	m_Angle[1] = toDeg(asin(dir[2] / length(dir)));
	Vec2 dxy(dir);
	m_Angle[0] = toDeg(asin(dir[1] / length(dxy)));
	m_Pointing = pointing();
}

Vec3 Camera::pointing()
{
	float aX = cos(toRad(m_Angle[1])) * cos(toRad(m_Angle[0]));
	float aY = cos(toRad(m_Angle[1])) * sin(toRad(m_Angle[0]));
	float aZ = sin(toRad(m_Angle[1]));
	return normalize(Vec3{ aX, aY, aZ });
}

Mat4 Camera::viewMatrix()
{
	float aX = cos(toRad(m_Angle[1])) * cos(toRad(m_Angle[0]));
	float aY = cos(toRad(m_Angle[1])) * sin(toRad(m_Angle[0]));
	float aZ = sin(toRad(m_Angle[1]));
	Vec3 lookingAt = m_Pos + Vec3{ aX, aY, aZ };
	Mat4 view = ::lookAt(m_Pos, lookingAt, m_CameraUp);
	return view;
}

Mat4 Camera::viewMatrix(Vec3 focus) const
{
	Mat4 view = ::lookAt(m_Pos, focus, m_CameraUp);
	return view;
}

Mat4 Camera::projMatrix(int width, int height) const
{
	Mat4 proj = ::perspective(m_FOV, (float)width / (float)height, m_Near, m_Far);
	return proj;
}
