#include "Rifle.h"
#include "Engine.h"

Rifle::Rifle()
{
	FireCooldown = Cooldown;
}

void Rifle::PlayerFire(int Id, int projectileId, Vector2 pos, Vector2 dir)
{
}

void Rifle::ServerFire(int Id, int projectileId, Vector2 pos, Vector2 dir)
{
	engPrint("Rifle");
	projectiles[projectileId].spawn(Id, pos, dir);
}
