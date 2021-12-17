#pragma once
#include "Vector2.h"

#define MAX_HEALTH_PACKS 10

const float healthPackRadius = 8;
const float despawnTimer = 6.f;

class HealthPack
{
public:
	HealthPack();

	void update();
	void spawnHealthPack(Vector2 pos); //int healthId
	void pickUpHealth();
	void destroy();
	void draw();

	Vector2 position;
	float spawnTime = 0.f;
	bool alive = false;
};
extern HealthPack healthPacks[MAX_HEALTH_PACKS];
