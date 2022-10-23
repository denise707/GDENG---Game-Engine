#include "Cube.h"

#include <iostream>

#include "DeviceContext.h"
#include "GraphicsEngine.h"
#include "SceneCameraHandler.h"

Cube::Cube(string name, void* shaderByteCode, size_t sizeShader) :AGameObject(name)
{
	//Create buffers for drawing. Vertex data that needs to be drawn are temporarily placed here.
	Vertex vertex_list[] =
	{
		//FRONT FACE
		{Vector3D(-0.5f,-0.5f,-0.5f),	Vector3D(255/255.0f,153/255.0f,214/255.0f), Vector3D(252/255.0f,76/255.0f,181/255.0f) },
		{Vector3D(-0.5f,0.5f,-0.5f),    Vector3D(255/255.0f,248/255.0f,153/255.0f), Vector3D(250/255.0f,238/255.0f,75/255.0f) },
		{ Vector3D(0.5f,0.5f,-0.5f),     Vector3D(153/255.0f,255/255.0f,175/255.0f),  Vector3D(87/255.0f,250/255.0f,123/255.0f) },
		{ Vector3D(0.5f,-0.5f,-0.5f),     Vector3D(153/255.0f,253/255.0f,255/255.0f), Vector3D(76/255.0f,249/255.0f,252/255.0f) },

		//BACK FACE
		{Vector3D(0.5f,-0.5f,0.5f),	Vector3D(255/255.0f,153/255.0f,214/255.0f), Vector3D(252/255.0f,76/255.0f,181/255.0f) },
		{Vector3D(0.5f,0.5f,0.5f),    Vector3D(255/255.0f,248/255.0f,153/255.0f), Vector3D(250/255.0f,238/255.0f,75/255.0f) },
		{ Vector3D(-0.5f,0.5f,0.5f),     Vector3D(153/255.0f,255/255.0f,175/255.0f),  Vector3D(87/255.0f,250/255.0f,123/255.0f) },
		{ Vector3D(-0.5f,-0.5f,0.5f),     Vector3D(153/255.0f,253/255.0f,255/255.0f), Vector3D(76/255.0f,249/255.0f,252/255.0f) }
	};

	this->vertexBuffer = GraphicsEngine::get()->createVertexBuffer();
	this->vertexBuffer->load(vertex_list, sizeof(Vertex), ARRAYSIZE(vertex_list), shaderByteCode, sizeShader);

	unsigned int index_list[] =
	{
		//FRONT SIDE
		0, 1, 2, //FIRST TRIANGLE
		2, 3, 0,
		//BACK SIDE
		4, 5, 6,
		6, 7, 4,
		//TOP SIDE
		1, 6, 5,
		5, 2, 1,
		//BOTTOM SIDE
		7, 0, 3,
		3, 4, 7,
		//RIGHT SIDE
		3, 2, 5,
		5, 4, 3,
		//LEFT SIDE
		7, 6, 1,
		1, 0, 7
	};

	this->indexBuffer = GraphicsEngine::get()->createIndexBuffer();
	this->indexBuffer->load(index_list, ARRAYSIZE(index_list));

	//Create constant buffer
	CBData cbData = {};
	cbData.time = 0;
	this->constantBuffer = GraphicsEngine::get()->createConstantBuffer();
	this->constantBuffer->load(&cbData, sizeof(CBData));

	m_world_cam.setTranslation(Vector3D(0, 0, -2));
	setScale(1, 1, 1);
	setAnimSpeed(4);
}

Cube::~Cube()
{
	this->vertexBuffer->release();
	this->indexBuffer->release();
}

void Cube::update(float delta_time)
{
	float newRotX = getLocalRotation().m_x + (delta_time * speed);
	float newRotY = getLocalRotation().m_y + (delta_time * speed);
	float newRotZ = getLocalRotation().m_z + (delta_time * speed);
	setRotation(newRotX, newRotY, newRotZ);
}

void Cube::draw(int width, int height, VertexShader* vertex_shader, PixelShader* pixel_shader)
{
	GraphicsEngine* graphEngine = GraphicsEngine::get();
	DeviceContext* deviceContext = graphEngine->getImmediateDeviceContext();

	CBData cbData = {};

	cbData.isSelected = this->isSelected;

	//Add object transformation
	Matrix4x4 temp;

	cbData.worldMatrix.setIdentity();

	Matrix4x4 world_cam;
	world_cam.setIdentity();

	temp.setIdentity();
	temp.setScale(getLocalScale());
	cbData.worldMatrix *= temp;

	temp.setIdentity();
	temp.setRotationX(getLocalRotation().m_x);
	cbData.worldMatrix *= temp;

	temp.setIdentity();
	temp.setRotationY(getLocalRotation().m_y);
	cbData.worldMatrix *= temp;

	temp.setIdentity();
	temp.setRotationZ(getLocalRotation().m_z);
	cbData.worldMatrix *= temp;

	temp.setIdentity();
	temp.setTranslation(getLocalPosition());
	cbData.worldMatrix *= temp;

	//Add camera transformation
	/*Matrix4x4 world_cam;
	world_cam.setIdentity();

	temp.setIdentity();
	temp.setRotationX(getLocalRotation().m_x);
	world_cam *= temp;

	temp.setIdentity();
	temp.setRotationY(getLocalRotation().m_y);
	world_cam *= temp;

	Vector3D new_pos = m_world_cam.getTranslation() + world_cam.getZDirection() * (forward * 0.1f);

	new_pos = new_pos + world_cam.getXDirection() * (right * 0.1f);

	world_cam.setTranslation(new_pos);

	m_world_cam = world_cam;

	world_cam.inverse();

	cbData.viewMatrix = world_cam;*/

	Matrix4x4 cameraMatrix = SceneCameraHandler::getInstance()->getSceneCameraViewMatrix();
	cbData.viewMatrix = cameraMatrix;

	cbData.projMatrix.setPerspectiveFovLH(1.57f, ((float)width / (float)height), 0.1f, 100.0f);

	this->constantBuffer->update(deviceContext, &cbData);
	deviceContext->setConstantBuffer(vertex_shader, this->constantBuffer);
	deviceContext->setConstantBuffer(pixel_shader, this->constantBuffer);

	deviceContext->setIndexBuffer(this->indexBuffer);
	deviceContext->setVertexBuffer(this->vertexBuffer);

	deviceContext->drawIndexedTriangleList(this->indexBuffer->getSizeIndexList(), 0, 0);
}

void Cube::setAnimSpeed(float speed)
{
	this->speed = speed;
}


