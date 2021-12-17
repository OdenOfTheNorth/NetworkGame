#pragma once
#include "Vector2.h"
#define PROJECTILE_MAX 512
	
const float projectileSpeed = 600.f;
const float despawnDelay = 5.f;
const float projectileRadius = 4.f;
const float playerFireCooldown = 1.f;

class Projectile
{
public:
	void RifleFire(int player, Vector2 pos, Vector2 vel);
	void ShotgunFire(int player, Vector2 pos, Vector2 vel);
	void spawn(int player, Vector2 pos, Vector2 vel);
	void destroy();
	void update();
	void draw();

	bool alive = false;

	Vector2 position;
	Vector2 velocity;

	float spawnTime = 0.f;
	int ownerPlayer;
};
extern Projectile projectiles[PROJECTILE_MAX];