#pragma once
#include "Projectile.h"
#define PROJECTILE_WEAPONS 2

enum class WeaponType : unsigned char
{
	Rifle,
	Shotgun,
};

class Weapon
{
public:
	 Weapon();	 	 
	 //~ Weapon();
	 virtual void PlayerFire(int Id, int projectileId, Vector2 pos, Vector2 dir);
	 virtual void ServerFire(int Id,int projectileId, Vector2 pos, Vector2 dir);
	 
	 float FireCooldown = 1.f;
	 float lastFireTime = 0.f;
};
//extern Weapon Weapons[PROJECTILE_WEAPONS];
