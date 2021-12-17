#pragma once
#define PLAYER_MAX 20
#define PLAYER_NAME_MAX 15
#define PLAYER_MAX_WEAPONS 2
#include "Vector2.h"
#include "ShotGun.h"
#include "Rifle.h"

const float playerSpeed = 180.f;
const float playerErrorCorrectionStrength = 1.5f;
const float playerRadius = 16;
const float playerDashSpeed = 250;
const int PlayerMaxLives = 3;

class Player
{
public:
	Player();
	int id = -1;
	bool alive = false;
	bool shieldActive = false;
	
	Vector2 position;
	Vector2 errorPosition;

	Vector2 dashStartPoint;
	
	Weapon* weapons[PROJECTILE_WEAPONS];
	int weaponsIndex = 1;

	float currentSpeed = playerSpeed;
	bool inputShoot = false;
	bool inputRun = false;
	int inputX = 0;
	int inputY = 0;

	int Lives = PlayerMaxLives;

	float lastFireTime = 0.f;
	float lastShieldTime = 0.f;
	float ShieldUpTime = 3.f;
	float currentShieldUpTime = 3.f;
	float ShieldCooldown = 6.f;

	char name[PLAYER_NAME_MAX + 1];

	void netReceivePosition(Vector2 newVector);
	void spawn(int id, Vector2 pos);

	void destroy();

	bool hasControl();
	void update();

	void draw();
};

extern Player players[PLAYER_MAX];

#if CLIENT
extern int possessedPlayerId;
#endif