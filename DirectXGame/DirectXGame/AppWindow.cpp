#include "AppWindow.h"
#include <Windows.h>

#include "EngineTime.h"
#include "Vector3D.h"
#include "Matrix4x4.h"
#include "InputSystem.h"
#include "SceneCameraHandler.h"

struct vertex
{
	Vector3D position;
	Vector3D color;
	Vector3D color1;
};

__declspec(align(16))
struct constant
{
	Matrix4x4 m_world;
	Matrix4x4 m_view;
	Matrix4x4 m_proj;
	unsigned int m_time;
};

AppWindow::AppWindow()
{
}

void AppWindow::update()
{
	/*constant cc;
	cc.m_time = ::GetTickCount();

	m_delta_pos += m_delta_time / 10.0f;
	if (m_delta_pos > 1.0f)
		m_delta_pos = 0;*/
}

AppWindow::~AppWindow()
{
}

void AppWindow::onCreate()
{
	Window::onCreate();

	InputSystem::get()->addListener(this);
	InputSystem::get()->showCursor(false);
	EngineTime::initialize();

	GraphicsEngine::get()->init();
	m_swap_chain = GraphicsEngine::get()->createSwapChain();

	RECT rc = this->getClientWindowRect();
	m_swap_chain->init(this->m_hwnd, rc.right - rc.left, rc.bottom - rc.top);
	
	//Initialize Scene Camera
	SceneCameraHandler::initialize();

	shader_byte_code = nullptr;
	size_shader = 0;

	//Vertex Shader
	GraphicsEngine::get()->compileVertexShader(L"VertexShader.hlsl", "vsmain", &shader_byte_code, &size_shader);
	m_vs = GraphicsEngine::get()->createVertexShader(shader_byte_code, size_shader);

	//Create Object Instances

	//Release Compiled Shader
	GraphicsEngine::get()->releaseCompiledShader();

	//Pixel Shader
	GraphicsEngine::get()->compilePixelShader(L"PixelShader.hlsl", "psmain", &shader_byte_code, &size_shader);
	m_ps = GraphicsEngine::get()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->releaseCompiledShader();
}

void AppWindow::onUpdate()
{
	Window::onUpdate();

	InputSystem::get()->update();

	//CLEAR THE RENDER TARGET 
	GraphicsEngine::get()->getImmediateDeviceContext()->clearRenderTargetColor(this->m_swap_chain,
		171/255.0f, 135/255.0f, 237/255.0f, 1);

	//SET VIEWPORT OF RENDER TARGET IN WHICH WE HAVE TO DRAW
	RECT rc = this->getClientWindowRect();
	width = rc.right - rc.left;
	height = rc.bottom - rc.top;

	GraphicsEngine::get()->getImmediateDeviceContext()->setViewportSize(rc.right - rc.left, rc.bottom - rc.top);

	//SET DEFAULT SHADER IN THE GRAPHICS PIPELINE TO BE ABLE TO DRAW
	GraphicsEngine::get()->getImmediateDeviceContext()->setVertexShader(m_vs);
	GraphicsEngine::get()->getImmediateDeviceContext()->setPixelShader(m_ps);

	//UPDATE CAMERA
	SceneCameraHandler::getInstance()->update(EngineTime::getDeltaTime(), width, height);

	//FINALLY DRAW THE OBJECTS

	for(int i = 0; i < objList.size(); i++)
	{
		objList[i]->draw(width, height, this->m_vs, this->m_ps);
		if (playAnim)
			objList[i]->update(EngineTime::getDeltaTime());
	}

	m_swap_chain->present(true);

	//FOR ANIMATION
	m_old_delta = m_new_delta;
	m_new_delta = ::GetTickCount();
	m_delta_time = (m_old_delta) ? ((m_new_delta - m_old_delta) / 1000.0f) : 0;
}

void AppWindow::onDestroy()
{
	Window::onDestroy();

	m_swap_chain->release();
	m_vs->release();
	m_ps->release();
	GraphicsEngine::get()->release();
}

void AppWindow::onFocus()
{
	InputSystem::get()->addListener(this);
}

void AppWindow::onKillFocus()
{
	InputSystem::get()->removeListener(this);
}

void AppWindow::onKeyDown(int key)
{
	if (key == 'Q')
	{
		transformation = 'Q';
	}

	else if (key == 'E')
	{
		transformation = 'E';
	}

	else if (key == 'R')
	{
		transformation = 'R';
	}

	else if(transformation == 'Q' && operation == '+' && key == 'X')
	{
		if(!objList.empty())
		{
			Vector3D objPos = objList[index]->getLocalPosition();
			float newX = objPos.m_x + 1;
			objList[index]->setPosition(newX, objPos.m_y, objPos.m_z);
		}
	}
}

void AppWindow::onKeyUp(int key)
{
	if (key == '1')
	{
		//Vertex Shader
		GraphicsEngine::get()->compileVertexShader(L"VertexShader.hlsl", "vsmain", &shader_byte_code, &size_shader);
		m_vs = GraphicsEngine::get()->createVertexShader(shader_byte_code, size_shader);

		//Create obj
		
		Cube* cube = new Cube("Cube", shader_byte_code, size_shader);
		this->objList.push_back((cube));
		index++;

		for (int i = 0; i < objList.size(); i++)
		{
			objList[i]->setSelected(false);
		}


		//Release Compiled Shader
		GraphicsEngine::get()->releaseCompiledShader();

		//Pixel Shader
		GraphicsEngine::get()->compilePixelShader(L"PixelShader.hlsl", "psmain", &shader_byte_code, &size_shader);
		m_ps = GraphicsEngine::get()->createPixelShader(shader_byte_code, size_shader);
		GraphicsEngine::get()->releaseCompiledShader();
	}

	if (key == '2')
	{
		//Vertex Shader
		GraphicsEngine::get()->compileVertexShader(L"VertexShader.hlsl", "vsmain", &shader_byte_code, &size_shader);
		m_vs = GraphicsEngine::get()->createVertexShader(shader_byte_code, size_shader);

		//Create obj
		for (int i = 0; i < objList.size(); i++)
		{
			objList[i]->setSelected(false);
		}

		Plane* plane = new Plane("Plane", shader_byte_code, size_shader);
		this->objList.push_back((plane));
		index++;

		//Release Compiled Shader
		GraphicsEngine::get()->releaseCompiledShader();

		//Pixel Shader
		GraphicsEngine::get()->compilePixelShader(L"PixelShader.hlsl", "psmain", &shader_byte_code, &size_shader);
		m_ps = GraphicsEngine::get()->createPixelShader(shader_byte_code, size_shader);
		GraphicsEngine::get()->releaseCompiledShader();
	}

	else if (key == VK_LEFT)
	{
		for (int i = 0; i < objList.size(); i++)
		{
			objList[i]->setSelected(false);
		}
		if(index > 0)
			index--;
		//objList[index]->setSelected(true);
	}

	else if(key == VK_RIGHT)
	{
		for (int i = 0; i < objList.size(); i++)
		{
			objList[i]->setSelected(false);
		}
		if (index < objList.size() - 1)
			index++;
		//objList[index]->setSelected(true);
	}
}

void AppWindow::onMouseMove(const Point& mouse_pos)
{
	int width = (this->getClientWindowRect().right - this->getClientWindowRect().left);
	int height = (this->getClientWindowRect().bottom - this->getClientWindowRect().top);

	InputSystem::get()->setCursorPosition(Point((int)(width / 2.0f), (int)(height / 2.0f)));
}

void AppWindow::onLeftMouseDown(const Point& mouse_pos)
{
	objList[index]->setSelected(true);

}

void AppWindow::onLeftMouseUp(const Point& mouse_pos)
{
	for (int i = 0; i < objList.size(); i++)
	{
		objList[i]->setSelected(false);
	}
}

void AppWindow::onRightMouseDown(const Point& mouse_pos)
{
	playAnim = true;
}

void AppWindow::onRightMouseUp(const Point& mouse_pos)
{
	playAnim = false;
	for (int i = 0; i < objList.size(); i++)
	{
		objList[i]->reset();
	}
}