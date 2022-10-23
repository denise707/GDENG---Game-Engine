#include "Camera.h"
#include "InputSystem.h"
#include <iostream>

#define PI 3.1459265

Camera::Camera(string name) : AGameObject(name)
{
	this->setPosition(0.0f, 0.0f, -4.0f);
	m_world_cam.setTranslation(this->getLocalPosition());
	this->updateViewMatrix();
	InputSystem::get()->addListener(this);
};

Camera::~Camera()
{
}

void Camera::update(float delta_time, int widthd, int heightd)
{
	this->width = widthd;
	this->height = heightd;
	m_delta_time = delta_time;

	Vector3D localPos = this->getLocalPosition();
	x = localPos.m_x;
	y = localPos.m_y;
	z = localPos.m_z;

	/*if(InputSystem::get()->isKeyDown('W'))
	{
		z += delta_time * moveSpeed;
		this->setPosition(x, y, z);
		this->updateViewMatrix();
	}

	if (InputSystem::get()->isKeyDown('S'))
	{
		z -= delta_time * moveSpeed;
		this->setPosition(x, y, z);
		this->updateViewMatrix();
	}

	if (InputSystem::get()->isKeyDown('A'))
	{
		x += delta_time * moveSpeed;
		this->setPosition(x, y, z);
		this->updateViewMatrix();
	}

	if (InputSystem::get()->isKeyDown('D'))
	{
		x -= delta_time * moveSpeed;
		this->setPosition(x, y, z);
		this->updateViewMatrix();
	}*/
}

Matrix4x4 Camera::getViewMatrix()
{
	return this->localMatrix;
}

void Camera::onKeyDown(int key)
{
	if (key == 'W')
	{
		m_forward = 1.0f;
		/*z += m_delta_time * moveSpeed;
		this->setPosition(x, y, z);*/
		this->updateViewMatrix();
		
	}
	else if (key == 'S')
	{
		m_forward = -1.0f;
		/*z -= m_delta_time * moveSpeed;
		this->setPosition(x, y, z);*/
		this->updateViewMatrix();
	
	}
	else if (key == 'A')
	{
		m_rightward = -1.0f;
		/*x -= m_delta_time * moveSpeed;
		this->setPosition(x, y, z);*/
		this->updateViewMatrix();
		
	}
	else if (key == 'D')
	{
		m_rightward = 1.0f;
		/*x += m_delta_time * moveSpeed;
		this->setPosition(x, y, z);*/
		this->updateViewMatrix();
	}
}

void Camera::onKeyUp(int key)
{
	m_forward = 0.0f;
	m_rightward = 0.0f;
}

void Camera::onMouseMove(const Point& mouse_pos)
{
	if(lmbDown)
	{
		m_rot_x += (mouse_pos.m_y - (height / 2.0f)) * m_delta_time * 0.1f;
		m_rot_y += (mouse_pos.m_x - (width / 2.0f)) * m_delta_time * 0.1f;

		this->setRotation(m_rot_x, m_rot_y, this->getLocalRotation().m_z);
		this->updateViewMatrix();
	}
	//InputSystem::get()->setCursorPosition(Point((int)(width / 2.0f), (int)(height / 2.0f)));
}

void Camera::onLeftMouseDown(const Point& mouse_pos)
{
	//m_scale_cube = 0.5f;
	lmbDown = true;
}

void Camera::onLeftMouseUp(const Point& mouse_pos)
{
	m_scale_cube = 1.0f;
	lmbDown = false;
}

void Camera::onRightMouseDown(const Point& mouse_pos)
{
	m_scale_cube = 2.0f;
}

void Camera::onRightMouseUp(const Point& mouse_pos)
{
	m_scale_cube = 1.0f;
}

void Camera::updateViewMatrix()
{
	//Matrix4x4 worldCam; worldCam.setIdentity();
	Matrix4x4 temp; temp.setIdentity();

	//Vector3D localRot = this->getLocalRotation();

	//temp.setIdentity();
	//temp.setRotationX(localRot.m_x);
	//worldCam *= temp;

	//temp.setIdentity();
	//temp.setRotationY(localRot.m_y);
	//worldCam *= temp;

	//worldCam.setTranslation(this->getLocalPosition());
	////worldCam *= temp;

	//worldCam.inverse();
	//
	//this->localMatrix = worldCam;

	//---
	Matrix4x4 world_cam;
	world_cam.setIdentity();

	temp.setIdentity();
	temp.setRotationX(getLocalRotation().m_x);
	world_cam *= temp;

	temp.setIdentity();
	temp.setRotationY(getLocalRotation().m_y);
	world_cam *= temp;

	temp.setIdentity();
	temp.setTranslation(getLocalPosition());
	world_cam *= temp;

	Vector3D new_pos = m_world_cam.getTranslation() + world_cam.getZDirection() * (m_forward * 0.1f);

	new_pos = new_pos + world_cam.getXDirection() * (m_rightward * 0.1f);

	world_cam.setTranslation(new_pos);

	m_world_cam = world_cam;

	world_cam.inverse();

	this->localMatrix = world_cam;
	
}
