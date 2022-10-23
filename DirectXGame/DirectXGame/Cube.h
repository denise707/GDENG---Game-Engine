#pragma once
#include "AGameObject.h"
#include "ConstantBuffer.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"

class Cube : public AGameObject
{
public:
	Cube(string name, void* shaderByteCode, size_t sizeShader);
	~Cube();

	void update(float deltaTime) override;
	void draw(int width, int height, VertexShader* vertexShader, PixelShader* pixelShader) override;
	void setAnimSpeed(float speed);

private:
	VertexBuffer* vertexBuffer;
	IndexBuffer* indexBuffer;
	ConstantBuffer* constantBuffer;

	float speed = 0.55f;
	Matrix4x4 m_world_cam;
};

