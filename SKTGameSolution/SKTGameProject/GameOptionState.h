#pragma once
#include <Common/FSM/State.h>
#include "Game.h"
#include "EntityStatic.h"


class GameOptionState : public State<Game>
{
public:
	GameOptionState();
	~GameOptionState();

	void Enter(Game* game) override;
	void Execute(Game* game) override;
	void Exit(Game* game) override;
	void Render(Game* game) override;
	void Init(const char* filePath);

private:
	EntityStatic *m_Background;
};
