#pragma once
#include "AGameObject.h"
#include "ConstantBuffer.h"
#include "PixelShader.h"
#include "VertexBuffer.h"
#include "VertexShader.h"

class Quad : public AGameObject
{
public:
	Quad(string name, void* shaderByteCode, size_t sizeShader);
	~Quad();

	void update(float deltaTime) override;
	void draw(int width, int height, VertexShader* vertexShader, PixelShader* pixelShader) override;
	void release();
protected:
	VertexBuffer* vertexBuffer;
	ConstantBuffer* constantBuffer;

	float speed = 0.55f;
	Matrix4x4 m_world_cam;
};

