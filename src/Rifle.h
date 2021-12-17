#pragma once
#include "Weapon.h"

class Rifle : public Weapon
{
public:
	Rifle();
	//~Rifle();

	virtual void PlayerFire(int Id, int projectileId, Vector2 pos, Vector2 dir) override;
	virtual void ServerFire(int Id, int projectileId, Vector2 pos, Vector2 dir) override;

	float Cooldown = 0.2f;
};