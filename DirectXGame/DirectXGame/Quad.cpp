#include "Quad.h"
#include <iostream>
#include "DeviceContext.h"
#include "EngineTime.h"
#include "GraphicsEngine.h"
#include "Vector3D.h"

__declspec(align(16))
struct constant
{
	float m_angle;
};

Quad::Quad(string name, void* shaderByteCode, size_t sizeShader) :AGameObject(name)
{
	Vertex vertex_list[] =
	{
		{Vector3D(-0.5f,-0.5f,0.0f),	Vector3D(1,0,0), Vector3D(0.2f,0,0) },
		{Vector3D(-0.5f,0.5f,0.0f),    Vector3D(1,1,0), Vector3D(0.2f,0.2f,0) },
		{ Vector3D(0.5f,-0.5f,0.0f),     Vector3D(1,1,0),  Vector3D(0.2f,0.2f,0) },
		{ Vector3D(0.5f,0.5f,0.0f),     Vector3D(1,0,0), Vector3D(0.2f,0,0) }
	};

	vertexBuffer = GraphicsEngine::get()->createVertexBuffer();
	vertexBuffer->load(vertex_list, sizeof(Vertex), ARRAYSIZE(vertex_list), shaderByteCode, sizeShader);

	//Create constant buffer
	CBData cbData = {};
	cbData.time = 0;
	this->constantBuffer = GraphicsEngine::get()->createConstantBuffer();
	this->constantBuffer->load(&cbData, sizeof(CBData));

	m_world_cam.setTranslation(Vector3D(0, 0, -2));
}

Quad::~Quad()
{
	this->vertexBuffer->release();
	this->constantBuffer->release();
}

void Quad::update(float deltaTime)
{

}

void Quad::draw(int width, int height, VertexShader* vertexShader, PixelShader* pixelShader)
{
	GraphicsEngine* graphEngine = GraphicsEngine::get();
	DeviceContext* deviceContext = graphEngine->getImmediateDeviceContext();

	CBData cbData = {};

	//Add object transformation
	Matrix4x4 temp;

	cbData.worldMatrix.setIdentity();

	//Add camera transformation
	Matrix4x4 world_cam;
	world_cam.setIdentity();

	temp.setIdentity();
	temp.setRotationX(getLocalRotation().m_x);
	world_cam *= temp;

	temp.setIdentity();
	temp.setRotationY(getLocalRotation().m_y);
	world_cam *= temp;

	//Vector3D new_pos = m_world_cam.getTranslation() + world_cam.getZDirection() * (forward * 0.1f);

	//new_pos = new_pos + world_cam.getXDirection() * (right * 0.1f);

	//world_cam.setTranslation(new_pos);

	m_world_cam = world_cam;

	world_cam.inverse();

	cbData.viewMatrix = world_cam;

	cbData.projMatrix.setPerspectiveFovLH(1.57f, ((float)width / (float)height), 0.1f, 100.0f);

	this->constantBuffer->update(deviceContext, &cbData);
	deviceContext->setConstantBuffer(vertexShader, this->constantBuffer);
	deviceContext->setConstantBuffer(pixelShader, this->constantBuffer);

	deviceContext->setVertexBuffer(this->vertexBuffer);

	// FINALLY DRAW THE QUAD
	GraphicsEngine::get()->getImmediateDeviceContext()->drawTriangleStrip(vertexBuffer->getSizeVertexList(), 0);
}

void Quad::release()
{
	vertexBuffer->release();
	delete this;
}

