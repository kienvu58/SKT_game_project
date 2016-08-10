#include "ContactListener.h"
#include "Box2D/Dynamics/Contacts/b2Contact.h"
#include "Entity.h"
#include <iostream>
#include "MessageTypes.h"
#include "SingletonClasses.h"
#include "EntityKiBlast.h"

ContactListener::ContactListener()
{
}


ContactListener::~ContactListener()
{
}

void ContactListener::BeginContact(b2Contact* contact)
{
	//	std::cout << "Begin contact.\n";
	void* bodyUserDataA = contact->GetFixtureA()->GetBody()->GetUserData();
	void* bodyUserDataB = contact->GetFixtureB()->GetBody()->GetUserData();

	if (bodyUserDataA && bodyUserDataB)
	{
		Entity* entityA = static_cast<Entity*>(bodyUserDataA);
		Entity* entityB = static_cast<Entity*>(bodyUserDataB);

		EntityType typeA = entityA->GetType();
		EntityType typeB = entityB->GetType();

		if (typeB == ENTITY_MINION && typeA == ENTITY_KIBLAST)
		{
			KiBlastHitsMinion(static_cast<EntityKiBlast*>(entityA), static_cast<EntityMinion*>(entityB));
		}

		if (typeA == ENTITY_MINION && typeB == ENTITY_KIBLAST)
		{
			KiBlastHitsMinion(static_cast<EntityKiBlast*>(entityB), static_cast<EntityMinion*>(entityA));
		}

		if (typeA == ENTITY_MINION && typeB == ENTITY_PLAYER)
		{
			MinionHitsPlayer(static_cast<EntityMinion*>(entityA), static_cast<EntityPlayer*>(entityB));
		}

		if (typeB == ENTITY_MINION && typeA == ENTITY_PLAYER)
		{
			MinionHitsPlayer(static_cast<EntityMinion*>(entityB), static_cast<EntityPlayer*>(entityA));
		}

	}
}

void ContactListener::EndContact(b2Contact* contact)
{
	//	std::cout << "End contact.\n";
}

void ContactListener::KiBlastHitsMinion(EntityKiBlast* kiBlast, EntityMinion* minion)
{
	auto damage = kiBlast->GetAttackDamage();
	Dispatcher->DispatchMessageA(nullptr, minion, MSG_MINION_TAKE_DAMAGE, &damage);
	Dispatcher->DispatchMessageA(nullptr, kiBlast, MSG_EXPLODE, nullptr);
}

void ContactListener::MinionHitsPlayer(EntityMinion* minion, EntityPlayer* player)
{
	auto damage = minion->GetAttackDamage();
	Dispatcher->DispatchMessageA(nullptr, player, MSG_PLAYER_TAKE_DAMAGE, &damage);
	Dispatcher->DispatchMessageA(nullptr, minion, MSG_EXPLODE, nullptr);
}
