#pragma once
#include <Common/FSM/State.h>
#include "EntityPlayer.h"
#include "Game.h"
#include "Pool.h"
#include "EntityStatic.h"
#include "Spawner.h"

class GamePlayState : public State<Game>
{
public:
	GamePlayState();
	~GamePlayState();
	void Enter(Game* game) override;
	void PressButton(Game* game) const;
	void UpdateHpBar();
	void UpdateKiBar();
	void UpdateOverheatBar();
	void Execute(Game* game) override;
	void Exit(Game* game) override;
	void RenderText() const;
	void RenderVirtualGamePad();
	void Render(Game* game) override;
	void MoveBackground();
	void HandleVirtualGamePad();
	void InitVirtualGamePad();
	void Init(const char* filePath);

	bool OnMessage(Game*, const Telegram&) override;

	void AddEntityToTheScreen(Entity* entity);
	void RemoveEntityFromTheScreen(Entity* entity);
	int GetNumberOfEntitiesByPrototypeId(int prototypeId);
	int GetNumberOfAllEntities();
	void ClearEntitiesOnTheScreen();
	std::vector<Entity*>* GetEntitiesByType(EntityType type);
	EntityPlayer* GetPlayer() const;

	void IncreaseScore(int amount);
	void Reset();
	int GetCurrentScore() const;
private:
#ifndef WIN32
	EntityStatic m_CircleWithDirections;
	EntityStatic m_Circle4Dash;
	EntityStatic m_Circle2Dash_J;
	EntityStatic m_Circle2Dash_K;
	EntityStatic m_Circle2Dash_L;
	EntityStatic m_Circle2Dash_I;
	EntityStatic m_ImageKiBlast;
	EntityStatic m_ImageKamehameha;
	EntityStatic m_ImageAura;
	EntityStatic m_ImageBarrier;
	Vector2 m_Circle4DashPos;
#endif
	
	EntityStatic m_Background;
	EntityStatic m_Background_Clone;
	EntityStatic m_Button_Pause;
	Vector2 m_BackgroundPosition;
	
	// Bars
	EntityStatic m_HpBar;
	EntityStatic m_KiBar;
	EntityStatic m_OverheatBar;
	EntityStatic m_HpOutline;
	EntityStatic m_KiOutline;
	EntityStatic m_OverheatOutline;

	EntityStatic m_Avatar;

	EntityPlayer* m_Player;
	
	// map to store minion by protypeId
	std::map<int, std::vector<Entity*>> m_mapCurrentEntities;

	//spawner
	Spawner m_spawner;

	int m_iScore;
	int m_TheBestScore;
};
