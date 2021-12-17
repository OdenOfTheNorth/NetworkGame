#include "Projectile.h"
#include "Engine.h"
#include "Player.h"
#include "Server.h"
#include "Network.h"
#include "MessageType.h"
#include "Client.h"

Projectile projectiles[PROJECTILE_MAX];

void Projectile::RifleFire(int player, Vector2 pos, Vector2 vel)
{
}

void Projectile::ShotgunFire(int player, Vector2 pos, Vector2 vel)
{
}

void Projectile::spawn(int player, Vector2 pos, Vector2 vel)
{
	alive = true;
	position = pos;

	ownerPlayer = player;

	velocity = vel * projectileSpeed;

	spawnTime = engElapsedTime();
}

void Projectile::destroy()
{
	//engPrint("destroy");
	alive = false;
}

void Projectile::update()
{
	if (engElapsedTime() - spawnTime > despawnDelay)
	{
		destroy();
		return;
	}

	position += velocity * engDeltaTime();
	
	for(auto& player : players)
	{
		if (!player.alive)
			continue;

		if (player.id == ownerPlayer)
			continue;

		Vector2 diffVector = position - player.position;

		float distSqr = (diffVector.x * diffVector.x + diffVector.y * diffVector.y);
		float radiusSqr = projectileRadius + playerRadius;
		radiusSqr = radiusSqr * radiusSqr;

		if(distSqr < radiusSqr)
		{
			destroy();

#if SERVER
			if (player.shieldActive)
			{
				engPrint("hit player but shiled up");
				return;
			}				

			player.Lives--;

			NetMessage msg;			
			msg.write<MessageType>(MessageType::PlayerChangeHealth);
			msg.write<int>(player.id);
			msg.write<int>(player.Lives);
			serverBroadcast(msg);
			msg.free();

			if (player.Lives <= 0)
			{
				player.alive = false;
				serverKickUser(player.id);
				engPrint("'%s' --> '%s'", players[ownerPlayer].name, player.name);
			}			
#endif
			return;
		}
	}
}

void Projectile::draw()
{
	engSetColor(0x000000FF);
	engFillRect((int)position.x - projectileRadius, (int)position.y - projectileRadius, 8, 8);
}

void HitPlayer(Player* player) {

}
