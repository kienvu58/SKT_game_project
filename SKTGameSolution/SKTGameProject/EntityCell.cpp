#include <json.hpp>
#include <fstream>
#include "EntityCell.h"
#include "SingletonClasses.h"
#include "../GraphicsEngine/Globals.h"

void EntityCell::InitStateMachine()
{
	InitSteeringBehavior();
	m_pSteeringBehavior->WanderOn();

	m_pStateMachine = new StateMachine<EntityCell>(this);
	m_pStateMachine->SetGlobalState(CS_Global::GetInstance());
	m_pStateMachine->SetCurrentState(CS_Wandering::GetInstance());
}

EntityCell::EntityCell()
{
	m_fAttackDamage = 10;
	m_fCurrentHealth = 30;
	m_fMaxHealth = 30;
	m_fCurrentDodgingOverheat = 0;

	InitStateMachine();
}

EntityCell::~EntityCell()
{
	delete m_pStateMachine;
}

StateMachine<EntityCell>* EntityCell::GetFSM() const
{
	return m_pStateMachine;
}

void EntityCell::Init(int prototypeId, const char* dataPath)
{
	m_iPrototypeId = prototypeId;

	std::ifstream fin(dataPath);
	nlohmann::json data(fin);
	fin.close();
	auto bodyData = data["physics"]["bodyDef"];
	auto fixtureData = data["physics"]["fixtureDef"];

	auto positionX = bodyData["position"]["x"].get<float>();
	auto positionY = bodyData["position"]["y"].get<float>();
	auto physicsPosition = b2Vec2(positionX, positionY);

	auto modelId = data["graphics"]["modelId"].get<int>();
	auto frameId = data["graphics"]["frameId"].get<int>();
	auto shaderId = data["graphics"]["shaderId"].get<int>();
	auto isReversed = data["graphics"]["isReversed"].get<bool>();

	m_b2BodyDef.type = b2_dynamicBody;
	m_b2BodyDef.position = physicsPosition;

	auto boxHalfWidth = fixtureData["boxShape"]["hx"].get<float>();
	auto boxHalfHeight = fixtureData["boxShape"]["hy"].get<float>();
	m_b2PolygonShape.SetAsBox(boxHalfWidth, boxHalfHeight);

	m_b2FixtureDef.shape = &m_b2PolygonShape;
	m_b2FixtureDef.friction = fixtureData["friction"].get<float>();
	m_b2FixtureDef.restitution = fixtureData["restitution"].get<float>();
	m_b2FixtureDef.filter.categoryBits = fixtureData["filter"]["categoryBits"].get<int>();
	m_b2FixtureDef.filter.maskBits = 0;
	for (auto maskBits : fixtureData["filter"]["maskBits"])
	{
		m_b2FixtureDef.filter.maskBits |= maskBits.get<int>();
	}

	m_iExplosionPID = data["explosionPID"].get<int>();
	m_iPrize = data["prize"].get<int>();
	m_fMaxSpeed = data["maxSpeed"].get<float>();
	m_fCurrentHealth = m_fMaxHealth = data["maxHealth"].get<float>();
	m_fMovementSpeed = data["movementSpeed"].get<float>();
	m_fAttackDamage = data["attackDamage"].get<float>();

	m_iGreenKiBlastID = data["skillPIDs"]["greenKiBlast"].get<int>();

	std::vector<Animation*> animations;
	for (auto animationId : data["animationIds"])
	{
		animations.push_back(AnimationMgr->GetAnimationById(animationId.get<int>()));
	}

	SetAnimations(animations);
	InitBody(m_b2BodyDef, m_b2FixtureDef);
	InitSprite(modelId, frameId, shaderId);
	ReverseSprite(isReversed);
}

Entity* EntityCell::Clone()
{
	EntityCell* cloneMinion = new EntityCell(*this);
	cloneMinion->InitStateMachine();
	return cloneMinion;
}

void EntityCell::Update()
{
	EntityMinion::Update();
	m_pStateMachine->Update();
}

bool EntityCell::HandleMessage(const Telegram& telegram)
{
	return m_pStateMachine->HandleMessage(telegram);
}

void EntityCell::Reset()
{
	EntityMinion::Reset();
	m_pStateMachine->ChangeState(CS_Wandering::GetInstance());
	m_fCurrentDodgingOverheat = MIN_DODGING_OVERHEAT;
}

void EntityCell::Fire() const
{
	auto bullet = static_cast<EntityBullet*>(PoolMgr->GetEntityByPrototypeId(m_iGreenKiBlastID));
	auto position = m_pBody->GetPosition() - b2Vec2(0.5f, 0.0f);
	bullet->Fire(position, -1);
	GS_GamePlay::GetInstance()->AddEntityToTheScreen(bullet);
}


void EntityCell::IncreaseDodgingOverHeat(float amount)
{
	m_fCurrentDodgingOverheat += amount;
	if (m_fCurrentDodgingOverheat >= MAX_DODGING_OVERHEAT)
	{
		m_fCurrentDodgingOverheat = MAX_DODGING_OVERHEAT;
		m_bIsDodgingOverheatd = true;
	}
}

void EntityCell::DecreaseDodgingOverHeat(float amount)
{
	m_fCurrentDodgingOverheat -= Globals::deltaTime*amount;
	if (m_fCurrentDodgingOverheat <= MIN_DODGING_OVERHEAT)
	{
		m_fCurrentDodgingOverheat = MIN_DODGING_OVERHEAT;
		m_bIsDodgingOverheatd = false;
	}
}

bool EntityCell::IsDodgingOverHeated() const
{
	return m_bIsDodgingOverheatd;
}
