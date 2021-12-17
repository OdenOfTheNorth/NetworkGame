#include "HealthPack.h"
#include "Player.h"
#include "Engine.h"
#include "Server.h"
#include "Network.h"
#include "MessageType.h"

HealthPack healthPacks[MAX_HEALTH_PACKS];

HealthPack::HealthPack()
{

}

void HealthPack::update()
{
	if (engElapsedTime() - spawnTime > despawnTimer)
	{
		destroy();
		return;
	}

	for (auto& player : players)
	{
		if (!player.alive)
			continue;

		Vector2 diffVector = position - player.position;

		float distSqr = (diffVector.x * diffVector.x + diffVector.y * diffVector.y);
		float radiusSqr = healthPackRadius + playerRadius;
		radiusSqr = radiusSqr * radiusSqr;

		if (distSqr < radiusSqr)
		{
			destroy();

#if SERVER
			if (player.Lives == PlayerMaxLives)
			{
				return;
			}

			player.Lives++;

			NetMessage msg;
			msg.write<MessageType>(MessageType::PlayerChangeHealth);
			msg.write<int>(player.id);
			msg.write<int>(player.Lives);
			serverBroadcast(msg);
			msg.free();			
#endif
			return;
		}
	}
}

void HealthPack::spawnHealthPack(Vector2 pos)
{
	spawnTime = engElapsedTime();
	alive = true;
	position = pos;
}

void HealthPack::pickUpHealth()
{
	destroy();
}

void HealthPack::destroy()
{
	//engPrint("dead");
	alive = false;
}

void HealthPack::draw()
{
	engSetColor(0x990000FF);
	engFillRect((int)position.x - healthPackRadius, (int)position.y - healthPackRadius, 16, 16);
}
