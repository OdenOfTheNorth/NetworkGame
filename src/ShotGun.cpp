#include "ShotGun.h"
#include "MessageType.h"
#include "Network.h"
#include "Server.h"
#include "Engine.h"
#include <math.h>
#define PI 3.14159265359

float Rad2Deg()
{
	return 180 / PI;
}

static Vector2 Rotate(Vector2 v, float radians)
{	
	float NewX = v.x * cos(radians) - v.y * sin(radians);
	float NewY = v.x * sin(radians) + v.y * cos(radians);
	return Vector2(NewX, NewY);
}

ShotGun::ShotGun()
{
	FireCooldown = Cooldown;
}

void ShotGun::PlayerFire(int Id, int projectileId, Vector2 pos, Vector2 dir)
{
}

void ShotGun::ServerFire(int Id, int projectileId, Vector2 pos, Vector2 dir)
{
	//engPrint("ShotGun");
	Vector2 directions[3];

	float angleVariance = 0.1f;

	directions[0] = dir;
	directions[1] = Rotate(dir, angleVariance);
	directions[2] = Rotate(dir, -angleVariance);

	for (int i = 0; i < 3; i++)
	{
		projectiles[projectileId + i].spawn(Id, pos, directions[i].Normalized());
	}

	return;
}

