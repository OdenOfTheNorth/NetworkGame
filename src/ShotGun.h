#pragma once
#include "Weapon.h"

class ShotGun : public Weapon
{
public:
	ShotGun();
	//~ShotGun();

	virtual void PlayerFire(int Id, int projectileId, Vector2 pos, Vector2 dir) override;
	virtual void ServerFire(int Id, int projectileId, Vector2 pos, Vector2 dir) override;

	float Cooldown = 1.0f;
};