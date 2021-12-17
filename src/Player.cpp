#include "Player.h"
#include "Client.h"
#include "Engine.h"
#include "MessageType.h"
#include "Network.h"
#include <cmath>
#include "Projectile.h"
#include "ShotGun.h"
#include "Rifle.h"

#define clamp(a, min, max) (a < min ? min : (a > max ? max : a))

Player players[PLAYER_MAX];
#if CLIENT
int possessedPlayerId = -1;
#endif

Player::Player()
{
	weapons[0] = (Weapon*)new Rifle();
	weapons[1] = (Weapon*)new ShotGun();
}

void Player::netReceivePosition(Vector2 newVector)
{
	Vector2 min(playerRadius, playerRadius);
	Vector2 max(800.f - playerRadius, 600.f - playerRadius);

	newVector.Clamp(min, max);

	errorPosition = newVector - position;
}

void Player::spawn(int id, Vector2 pos)
{
	Lives = PlayerMaxLives;
	this->id = id;
	alive = true;
	position = pos;
	currentSpeed = playerSpeed;
}

void Player::destroy()
{
	alive = false;
}

bool Player::hasControl()
{
#if SERVER
	return false;
#else
	return id == possessedPlayerId;
#endif
}

void Player::update()
{
#if CLIENT
	if(hasControl())
	{
		int frameInputX = 0;
		int frameInputY = 0;
		bool oldFrameInputRun = inputRun;

		if (engKeyDown(Key::A)) frameInputX -= 1;
		if (engKeyDown(Key::D)) frameInputX += 1;
		if (engKeyDown(Key::W)) frameInputY -= 1;
		if (engKeyDown(Key::S)) frameInputY += 1;

		if (engKeyPressed(Key::NUM1))
		{
			int weaponindex = 0;

			NetMessage msg;
			msg.write<MessageType>(MessageType::PlayerChangeWeapon);
			msg.write<int>(id);
			msg.write<int>(weaponindex);
			clientSend(msg);
			msg.free();

			//engPrint("player id %d sent ChangeWeapon 1 ", id);
		}
		if (engKeyPressed(Key::NUM2))
		{
			int weaponindex = 1;

			NetMessage msg;
			msg.write<MessageType>(MessageType::PlayerChangeWeapon);
			msg.write<int>(id);
			msg.write<int>(weaponindex);
			clientSend(msg);
			msg.free();

			//engPrint("player id %d sent ChangeWeapon 2 ", id);
		}

		if (engKeyDown(Key::Space)) 
		{
			inputRun = true;
			currentSpeed = playerDashSpeed;
		}
		else
		{
			inputRun = false;
			currentSpeed = playerSpeed;
		}

		if (frameInputX != inputX || frameInputY != inputY || oldFrameInputRun != inputRun)
		{
			NetMessage msg;
			msg.write<MessageType>(MessageType::PlayerInput);
			msg.write<int>(id);
			msg.write<Vector2>(position);

			msg.write<char>(frameInputX);
			msg.write<char>(frameInputY);
			msg.write<float>(currentSpeed);

			clientSend(msg);
			msg.free();

			//engPrint("sent PlayerInput");
		}

		oldFrameInputRun = inputRun;
		inputX = frameInputX;
		inputY = frameInputY;

		if(engKeyPressed(Key::V))
		{
			//engPrint("sent PlayerRequestFire");
			NetMessage msg;
			msg.write<MessageType>(MessageType::PlayerRequestFire);
			clientSend(msg);
			msg.free();
		}

		if (engKeyPressed(Key::E))
		{
			//engPrint("sent PlayerRequestShield");
			NetMessage msg;
			msg.write<MessageType>(MessageType::PlayerRequestShield);
			clientSend(msg);
			msg.free();
		}
	}
#endif

	if(!hasControl())
	{
		Vector2 errorDelta = errorPosition * playerErrorCorrectionStrength * engDeltaTime();

		position += errorDelta;
		errorDelta -= errorDelta;

		if (errorDelta.x != 0 || errorDelta.y != 0)
		{
			engPrint("errorDelta x = %d, errorDelta y = %d", errorDelta.x, errorDelta.y);
		}		
	}

	//engPrint("position x = %d, position y = %d", (int)position.x, (int)position.y);

	position.x += inputX * currentSpeed * engDeltaTime();
	position.y += inputY * currentSpeed * engDeltaTime();

	Vector2 min(playerRadius, playerRadius);
	Vector2 max(800.f - playerRadius, 600.f - playerRadius);

	position.Clamp(min, max);
}

void Player::draw()
{
	int color = (PlayerMaxLives - Lives) + 1;

	if (shieldActive)
	{
		engSetColor(0, 0, 255, 255);
	}
	else 
	{
		engSetColor(255 / color,0, 0, 255);
	}	

#if CLIENT
	if (hasControl()) 
	{
		for (int i = 0; i < Lives; i++)
		{
			engSetColor(255 / color, 0, 0, 255);
			engFillRect(20 + (i * 20), 20, 16, 16);
		}		

		if (shieldActive)
		{
			engSetColor(0, 0, 255, 255);
		}
		else
		{
			engSetColor(0, 255 / color, 0, 255);
		}		
	}		
#endif

	engFillRect((int)position.x - playerRadius, (int)position.y - playerRadius, 32, 32);
	engText((int)position.x - playerRadius, (int)position.y - playerRadius - 16, name);
}
