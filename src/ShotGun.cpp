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

static Vector2 Rotate(Vector2 v, float degrees) 
{	
	float NewX = v.x * cos(degrees) - v.y * sin(degrees);
	float NewY = v.x * sin(degrees) + v.y * cos(degrees);
	
	//float sinFloat = sin(degrees * Vector2::radToDeg());
	//float cosFloat = cos(degrees * Vector2::radToDeg());
	//
	//float tx = v.x;
	//float ty = v.y;
	//v.x = (cosFloat * tx) - (sinFloat * ty);
	//v.y = (sinFloat * tx) + (cosFloat * ty);
	
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
	engPrint("ShotGun");
	Vector2 directions[3];

	float angleVariance = 0.1f;

	/*
	float r = Vector2::Distance(pos, target);
	float x = dir.x;//abs(pos.x - target.x);
	float y = dir.y;//abs(pos.y - target.y);
   
	float initialAngle = atan2(y, x) * Vector2::radToDeg();
	float secondAngle = (initialAngle + angleVariance) * Vector2::radToDeg();
	float thirdAngle = (initialAngle - angleVariance) * Vector2::radToDeg();

	//Calculate x2 and y2
	float x2 = r * cos(secondAngle);
	float y2 = r * sin(secondAngle);

	//Calculate x3 and y3
	float x3 = r * cos(thirdAngle);
	float y3 = r * sin(thirdAngle);

	//Verify if X is positive or negative
	if (target.x < pos.x) {
		x2 = x2 * -1;
		x3 = x3 * -1;
	}

	//Verify if Y is positive or negative
	if (target.y < pos.y) {
		y2 = y2 * -1;
		y3 = y3 * -1;
	}

	directions[0] = dir;
	directions[1] = Vector2(x2, y2);
	directions[2] = Vector2(x3, y3);
	*/

	directions[0] = dir;
	directions[1] = Rotate(dir, angleVariance);
	directions[2] = Rotate(dir, -angleVariance);

	for (int i = 0; i < 3; i++)
	{
		projectiles[projectileId + i].spawn(Id, pos, directions[i].Normalized());
	}

	return;
}

