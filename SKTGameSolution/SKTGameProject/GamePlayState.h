#pragma once
#include <Common/FSM/State.h>
#include "EntityPlayer.h"
#include "Game.h"
#include "EntityMinion.h"
#include "Pool.h"
#include "KiBlast.h"
#include "FactoryEntity.h"
#include "BeamWave.h"
#include "Spawner.h"

class GamePlayState : public State<Game>
{
public:
	GamePlayState();
	~GamePlayState();
	void Enter(Game* game) override;
	void Execute(Game* game) override;
	void Exit(Game* game) override;
	void Render(Game* game) override;
	void Init(const char* filePath);

	bool OnMessage(Game*, const Telegram&) override;

	void AddEntitesToTheScreen(EntityType type, Entity* entity);
	void RemoveEntitiesOnTheScreen(EntityType type, Entity* entity);
	int GetNumEntitiesByType(EntityType type);
	int GetNumAllEntities();
	std::vector<Entity*>* GetEntitiesByType(EntityType type);
	EntityPlayer* GetPlayer() const;
private:
	EntityPlayer* m_Goku;

	//factory stores prototypes
	FactoryEntity* m_PFactory;

	//pools
	Pool<EntityMinion>* m_pMinionPool;

	//current entity on the screen
	std::vector<KiBlast*> m_vCurrentKiBlasts;
	std::vector<BeamWave*> m_vCurrentBeamWaves;

	std::map<EntityType, std::vector<Entity*>*> m_mapCurrentEntities;

	//spawner
	Spawner m_spawner;
};
