#include "Game.h"
#include "GamePlayState.h"

Game* Game::s_Instance = nullptr;

Game::Game(): m_pStateMachine(new StateMachine<Game>(this)) 
{
	m_pStateMachine->SetCurrentState(GamePlayState::GetInstance());
	m_pStateMachine->ChangeState(GamePlayState::GetInstance());
}


Game::~Game()
{
	delete m_pStateMachine;
}

void Game::Init()
{
}

void Game::Update()
{
	m_pStateMachine->Update();
}

void Game::Render()
{
	m_pStateMachine->Render();
}

Game* Game::GetInstance()
{
	if (!s_Instance)
	{
		s_Instance = new Game();
	}
	return s_Instance;
}

void Game::DestroyInstance()
{
	if (s_Instance)
	{
		delete s_Instance;
		s_Instance = nullptr;
	}
}
