#include "Engine.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <WinSock2.h>
#include "Player.h"
#include "Client.h"
#include "Network.h"
#include "MessageType.h"
#include "Projectile.h"
#include "HealthPack.h"

#if CLIENT

void handleMessage(NetMessage msg)
{
	MessageType type = msg.read<MessageType>();

	switch(type)
	{
		case MessageType::PlayerSpawn:
		{
			int id = msg.read<int>();

			Vector2 pos = msg.read<Vector2>();

			//float x = msg.read<float>();
			//float y = msg.read<float>();
			players[id].spawn(id, pos);
			break;
		}

		case MessageType::PlayerName:
		{
			int id = msg.read<int>();
			int nameLen = msg.read<unsigned char>();

			Player* player = &players[id];

			msg.read(player->name, nameLen);
			player->name[nameLen] = 0;
			break;
		}

		case MessageType::PlayerDestroy:
		{
			int id = msg.read<int>();
			players[id].destroy();
			break;
		}

		case MessageType::PlayerPossess:
		{
			possessedPlayerId = msg.read<int>();

			static const char* myName = "KillerMan";
			NetMessage nameMsg;
			nameMsg.write<MessageType>(MessageType::PlayerName);
			nameMsg.write<int>(possessedPlayerId);
			nameMsg.write<unsigned char>(strlen(myName));
			nameMsg.write(myName, strlen(myName));

			clientSend(nameMsg);
			nameMsg.free();
			break;
		}

		case MessageType::PlayerPosition:
		{
			int id = msg.read<int>();
			Player* player = &players[id];
			if (player->hasControl())
				break;

			player->position = msg.read<Vector2>();
			break;
		}

		case MessageType::PlayerInput:
		{
			int id = msg.read<int>();
			Player* player = &players[id];

			if (player->hasControl())
				break;

			Vector2 pos = msg.read<Vector2>();

			player->netReceivePosition(pos);

			player->inputX = msg.read<char>();
			player->inputY = msg.read<char>();

			player->currentSpeed = msg.read<float>();
			break;
		}

		case MessageType::ProjectileSpawn:
		{
			int projectileIndex = msg.read<int>();
			int playerId = msg.read<int>();
			Vector2 pos = msg.read<Vector2>();
			char dirX = msg.read<char>();
			char dirY = msg.read<char>();
			float lastFireTime = msg.read<float>();

			Player* player = &players[playerId];
			player->weapons[player->weaponsIndex]->lastFireTime = lastFireTime;
			player->weapons[player->weaponsIndex]->ServerFire(playerId, projectileIndex, pos, Vector2(dirX, dirY).Normalized());
			break;
		}

		case MessageType::PlayerChangeWeapon:
		{
			int playerID = msg.read<int>();
			int weaponIndex = msg.read<int>();

			Player* player = &players[playerID];
			player->weaponsIndex = weaponIndex;

			//engPrint("ChangeWeapon %d", weaponIndex);
			break;
		}

		case MessageType::PlayerChangeHealth:
		{
			int playerID = msg.read<int>();
			int playerLives = msg.read<int>();
			Player* player = &players[playerID];
			player->Lives = playerLives;
			//engPrint("player lives %d", player->Lives);
			break;
		}

		case MessageType::PlayerUpShield:
		{
			engPrint("PlayerUpShield");

			int playerID = msg.read<int>();
			float time = msg.read<int>();
			bool shieldUp = msg.read<bool>();

			Player* player = &players[playerID];
			player->lastShieldTime = time;
			player->shieldActive = shieldUp;
			break;
		}

		case MessageType::HealthPackSpawn:
		{
			int index = msg.read<int>();
			Vector2 pos = msg.read<Vector2>();

			healthPacks[index].spawnHealthPack(pos);
			//engPrint("healthPacks index %d, pos.x = %d, pos.y = %d", index, pos.x, pos.y);
			break;
		}
	}
}

void Update() 
{
	for (auto& pack : healthPacks)
	{
		if (pack.alive)
			pack.update();
	}

	for (auto& player : players)
	{
		if (player.alive)
			player.update();
	}

	for (auto& projectile : projectiles)
	{
		if (projectile.alive)
			projectile.update();
	}
}

void Draw() 
{
	for (auto& player : players)
	{
		if (player.alive)
			player.draw();
	}

	for (auto& projectile : projectiles)
	{
		if (projectile.alive)
			projectile.draw();
	}

	for (auto& pack : healthPacks)
	{
		if (pack.alive)
			pack.draw();
	}
}


int WinMain(HINSTANCE, HINSTANCE, char*, int)
{
	engInit();
	netInit();

	if (!clientConnect("127.0.0.1", 666))
		return 1;

	while(engBeginFrame() && clientIsConnected())
	{
		NetEvent event;
		while(netPollEvent(&event))
		{
			switch(event.type)
			{
				case NetEventType::Message:
					//engPrint("Received %d bytes", event.message.size);
					handleMessage(event.message);
					break;
			}

			event.free();
		}

		engSetColor(0x4444CCFF);
		engClear();

		Update();

		Draw();
	}

	return 0;
}

#endif
