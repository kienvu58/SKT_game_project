#pragma once
#include <Common/FSM/StateMachine.h>
#include "Entity.h"

class Game : public Entity
{
public:
	Game();
	~Game();

	static void Init();
	static void CreateStateInstances();
	static void DestroyStateInstances();
	void Update() override;
	void Render() override;

	bool HandleMessage(const Telegram& telegram) override;
	EntityType GetType() override;
	Entity* Clone() override;

	StateMachine<Game>* GetFSM() const;

	void IncreasePlayingTime(float amount);
	float GetPlayingTime() const;
	void UpdateDifficulty(int currentScore);
	float GetDifficulty() const;
	void Reset() override;
private:
	StateMachine<Game>* m_pStateMachine;
	float m_fPlayingTime;
	float m_fDifficulty;
};

