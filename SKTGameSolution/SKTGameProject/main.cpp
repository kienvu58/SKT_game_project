#include "../Utilities/utilities.h"
#include <Box2D/Box2d.h>
#include <iostream>
#include <conio.h>
#include "../GraphicsEngine/Globals.h"
#include "../GraphicsEngine/ResourceManager.h"
#include "../GraphicsEngine/SceneManager.h"
#include "../GraphicsEngine/InputManager.h"
#include "Game.h"
#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include "../GraphicsEngine/FrameManager.h"
#include "../GraphicsEngine/AnimationManager.h"
#include "GamePlayState.h"
#include "../GraphicsEngine/TextManager.h"
#include "PhysicsManager.h"

float Globals::deltaTime = 0;

int Init(ESContext* esContext)
{
	TextMgr->Init("../Resources/Fonts/arial.ttf");
	ResourceMgr->Init("../Resources/Data/RM.json");
	FrameMgr->Init("../Resources/Data/FM.json");
	AnimationMgr->Init("../Resources/Data/AM.json");
	PhysicsMgr->Init();
//	SceneMgr->Init("../Resources/Data/SM.txt");
	Game::GetInstance()->Init();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	return 0;
}

void Draw(ESContext* esContext)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
//	SceneMgr->Draw();
	Game::GetInstance()->Render();
	eglSwapBuffers(esContext->eglDisplay, esContext->eglSurface);
}

void Update(ESContext* esContext, float deltaTime)
{
	if (deltaTime)
	{
		Globals::deltaTime = deltaTime;

//		SceneMgr->Update(deltaTime);
		Game::GetInstance()->Update();
	}
}

void Key(ESContext* esContext, unsigned char key, bool isPressed)
{
	switch (key)
	{
	case 'A':
		InputMgr->SetKeyEvent(KEY_A, isPressed);
		break;
	case 'W':
		InputMgr->SetKeyEvent(KEY_W, isPressed);
		break;
	case 'S':
		InputMgr->SetKeyEvent(KEY_S, isPressed);
		break;
	case 'D':
		InputMgr->SetKeyEvent(KEY_D, isPressed);
		break;
	}
}

void MouseMove(ESContext* ESContext, float x, float y)
{
	InputMgr->SetCurrentMousePosition(x, y);
}

void MouseDown(ESContext* esContext, float x, float y)
{
	InputMgr->SetCurrentMousePosition(x, y);
	InputMgr->SetLastMousePosition(x, y);
}

void CleanUp()
{
	TextManager::DestroyInstance();
	InputManager::DestroyInstance();
	ResourceManager::DestroyInstance();
	AnimationManager::DestroyInstance();
	FrameManager::DestroyInstance();
	Game::DestroyInstance();
	PhysicsManager::DestroyInstance();
}

int main(int argc, char* argv[])
{
	ESContext esContext;

	esInitContext(&esContext);

	esCreateWindow(&esContext, "SKT Game", Globals::screenWidth, Globals::screenHeight, ES_WINDOW_RGB | ES_WINDOW_DEPTH);

	if (Init(&esContext) != 0)
		return 0;
	esRegisterDrawFunc(&esContext, Draw);
	esRegisterUpdateFunc(&esContext, Update);
	esRegisterKeyFunc(&esContext, Key);
	esRegisterMouseMoveFunc(&esContext, MouseMove);
	esRegisterMouseDownFunc(&esContext, MouseDown);
	esMainLoop(&esContext);

	//releasing OpenGL resources
	CleanUp();

	//identifying memory leaks
	MemoryDump();

	printf("Press any key...\n");
	_getch();

	return 0;
}
