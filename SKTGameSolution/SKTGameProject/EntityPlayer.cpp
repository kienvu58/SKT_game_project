#include <json.hpp>
#include <fstream>
#include "EntityPlayer.h"
#include "SingletonClasses.h"
#include "../GraphicsEngine/Globals.h"
#include <Box2D/Collision/Shapes/b2PolygonShape.h>
#include <Box2D/Dynamics/b2Fixture.h>
#include "EntityBullet.h"
#include "RayCastMultipleCallback.h"


EntityPlayer::EntityPlayer(): m_fMaxKi(0),
                              m_fCurrentKi(0),
                              m_iNormalPID(0), m_iSpecialPID(0), m_iUltimatePID(0), m_iAuraPID(0),
                              m_pStateMachine(new StateMachine<EntityPlayer>(this))
{
	m_fCurrentHealth = 100;
	m_fMaxHealth = 100;

	m_pStateMachine->SetGlobalState(PS_Global::GetInstance());
	m_pStateMachine->SetCurrentState(PS_Standing::GetInstance());
}


EntityPlayer::~EntityPlayer()
{
	delete m_pStateMachine;
}

void EntityPlayer::Render()
{
	if (IsActive())
	{
		m_Sprite.Render();
	}
}

void EntityPlayer::Update()
{
	if (IsActive())
	{
		EntityLiving::Update();
		m_pStateMachine->Update();
	}
}

EntityType EntityPlayer::GetType()
{
	return ENTITY_PLAYER;
}

bool EntityPlayer::HandleMessage(const Telegram& telegram)
{
	return m_pStateMachine->HandleMessage(telegram);
}

StateMachine<EntityPlayer>* EntityPlayer::GetFSM() const
{
	return m_pStateMachine;
}

EntityLiving* EntityPlayer::Clone()
{
	return nullptr;
}

void EntityPlayer::Init(int prototypeId, const char* dataPath)
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
	for (auto maskBits : fixtureData["filter"]["maskBits"])
	{
		m_b2FixtureDef.filter.maskBits |= maskBits.get<int>();
	}

	m_iNormalPID = data["skillPIDs"]["normal"].get<int>();
	m_iSpecialPID = data["skillPIDs"]["special"].get<int>();
	m_iUltimatePID = data["skillPIDs"]["ultimate"].get<int>();
	m_iAuraPID = data["skillPIDs"]["aura"].get<int>();

	m_fCurrentKi = m_fMaxKi = data["maxKi"].get<float>();
	m_fMaxSpeed = data["maxSpeed"].get<float>();
	m_fCurrentHealth = m_fMaxHealth = data["maxHealth"].get<float>();
	m_fMovementSpeed = data["movementSpeed"].get<float>();
	m_fAttackDamage = data["attackDamage"].get<float>();
	m_fVisionRange = data["visionRange"].get<float>();
	m_fVisionFreq = data["visionFreq"].get<float>();

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

bool EntityPlayer::IsOnTheGround() const
{
	b2Vec2 currentPosition = m_pBody->GetPosition();
	auto groundY = MetersFromPixels(
		float(Globals::screenHeight - m_Sprite.GetModel()->GetModelHeight()) / 2);

	return currentPosition.y <= -groundY;
}

void EntityPlayer::DetectMinions()
{
	static auto angle = 0.0f;
	angle += Globals::deltaTime * m_fVisionFreq * Radians(360);
	auto eye = m_pBody->GetPosition();
	auto target = eye + m_fVisionRange * b2Vec2(cosf(angle), sinf(angle));
	RayCastMultipleCallback callback;
	callback.maskBits = CATEGORY_MINION;
	PhysicsMgr->GetWorld()->RayCast(&callback, eye, target);
	for (auto fixture : callback.fixtures)
	{
		auto minion = fixture->GetBody()->GetUserData();
		auto position = m_pBody->GetPosition();
		Dispatcher->DispatchMessageA(this, static_cast<EntityMinion*>(minion), MSG_MINION_INSIDE_VISION_RANGE, &position);
	}
}

void EntityPlayer::Fire() const
{
	auto bullet = static_cast<EntityBullet*>(PoolMgr->GetEntityByPrototypeId(m_iNormalPID));
	auto position = m_pBody->GetPosition() + b2Vec2(0.4f, 0.2f);
	bullet->Fire(position, 1);
	GS_GamePlay::GetInstance()->AddEntityToTheScreen(bullet);
}
