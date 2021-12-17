#pragma once
#include "Engine.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <WinSock2.h>
#include <stdio.h>
#include <stdlib.h>
#include "Player.h"
#include "Server.h"
#include "Network.h"
#include "MessageType.h"
#include "Weapon.h"
#include "HealthPack.h"

#if SERVER

//Delegate
float HealthSpawnRate = 1.f;
float LastHealthSpawnTime = 0;
bool gameStarted = false;

void handleMessage(int userId, NetMessage msg)
{
	MessageType type = msg.read<MessageType>();
	switch(type)
	{
		case MessageType::PlayerName:
		{
			int playerId = msg.read<int>();
			if (playerId != userId)
			{
				serverKickUser(userId);
				break;
			}

			int nameLen = msg.read<unsigned char>();
			if(nameLen > PLAYER_NAME_MAX)
			{
				serverKickUser(userId);
				break;
			}

			Player* player = &players[userId];
			msg.read(player->name, nameLen);

			player->name[nameLen] = 0;

			serverBroadcast(msg);
			break;
		}

		case MessageType::PlayerPosition:
		{
			int playerId = msg.read<int>();
			if(playerId != userId)
			{
				serverKickUser(userId);
				break;
			}

			Player* player = &players[userId];
			Vector2 newPos = msg.read<Vector2>();
			player->netReceivePosition(newPos);

			serverBroadcast(msg);
			break;
		}

		case MessageType::PlayerInput:
		{
			int playerId = msg.read<int>();
			if (playerId != userId)
			{
				serverKickUser(userId);
				break;
			}

			Player* player = &players[userId];
			Vector2 newPos = msg.read<Vector2>();
			player->netReceivePosition(newPos);

			player->inputX = msg.read<char>();
			player->inputY = msg.read<char>();

			player->currentSpeed = msg.read<float>();

			serverBroadcast(msg);
			break;
		}

		case MessageType::PlayerRequestFire:
		{
			if (!gameStarted)
				break;

			int projectileIndex = -1;
			for(int i = 0; i < PROJECTILE_MAX; i++)
			{
				if (projectiles[i].alive)
					continue;

				projectileIndex = i;
				break;
			}

			if(projectileIndex == -1)
			{
				engError("Ran out of projectiles");
				break;
			}

			Player* player = &players[userId];

			if (player->inputX == 0 && player->inputY == 0)
				break;

			float time = engElapsedTime() - player->weapons[player->weaponsIndex]->lastFireTime;
			float cooldown = player->weapons[player->weaponsIndex]->FireCooldown;

			engPrint("time = %f", time);

			if (time < cooldown)
				break;			
			
			player->weapons[player->weaponsIndex]->lastFireTime = engElapsedTime();

			Vector2 playerInput(player->inputX, player->inputY);
						
			player->weapons[player->weaponsIndex]->ServerFire(userId, projectileIndex, player->position, playerInput.Normalized());
						
			NetMessage response;
			response.write<MessageType>(MessageType::ProjectileSpawn);
			response.write<int>(projectileIndex);
			response.write<int>(userId);
			response.write<Vector2>(player->position);
			response.write<char>(player->inputX);
			response.write<char>(player->inputY);
			response.write<float>(player->weapons[player->weaponsIndex]->lastFireTime);

			serverBroadcast(response);
			response.free();
			break;
		}

		case MessageType::PlayerChangeWeapon:
		{
			int playerIndex = msg.read<int>();
			int weaponindex = msg.read<int>();

			Player* player = &players[playerIndex];
			player->weaponsIndex = weaponindex;
			engPrint("playerID %d weaponindex ChangeWeapon %d", playerIndex,  weaponindex);

			//NetMessage response;
			//response.write<MessageType>(MessageType::PlayerChangeWeapon);
			//response.write<int>(player->id);
			//response.write<int>(weaponindex);
			//serverBroadcast(response);
			//response.free();

			serverBroadcast(msg);			
			break;
		}

		case MessageType::PlayerChangeHealth:
		{
			int playerIndex = msg.read<int>();	

			if (playerIndex != userId)
			{
				serverKickUser(userId);
			}

			Player* player = &players[playerIndex];
			player->Lives--;
			engPrint("player lives %d", player->Lives);
			serverBroadcast(msg);
			break;
		}

		case MessageType::PlayerRequestShield:
		{
			if (!gameStarted)
			{
				break;
			}

			Player* player = &players[userId];
			
			float time = engElapsedTime() - player->lastShieldTime;

			if (time < player->ShieldCooldown)
				break;

			engPrint("ServerMain PlayerRequestShield");

			player->lastShieldTime = engElapsedTime();
			player->shieldActive = true;

			NetMessage response;
			response.write<MessageType>(MessageType::PlayerUpShield);
			response.write<int>(player->id);
			response.write<float>(player->lastShieldTime);
			response.write<bool>(player->shieldActive);

			serverBroadcast(response);
			break;
		}
	}
}

void HealthSpawner()
{
	if (!gameStarted)
	{
		return;
	}

	if (engElapsedTime() - LastHealthSpawnTime > HealthSpawnRate)
	{
		LastHealthSpawnTime = engElapsedTime();

		int healthIndex = -1;

		for (int i = 0; i < MAX_HEALTH_PACKS; i++)
		{
			if (healthPacks[i].alive) 
			{
				//engPrint("healthPacks continue %d", i);
				continue;
			}				

			healthIndex = i;
			break;
		}

		if (healthIndex == -1)
		{
			engPrint("ran out of healthpacks");
			return;
		}

		//engPrint("healthIndex %d", healthIndex);

		Vector2 randomVectorSpawn(rand() % 800 , rand() % 600);

		randomVectorSpawn = randomVectorSpawn + (healthIndex * 9);

		randomVectorSpawn.Clamp(Vector2(healthPackRadius, healthPackRadius), 
			Vector2(800 - healthPackRadius, 600 - healthPackRadius));

		//engPrint("x %d, y %d", randomVectorSpawn.x, randomVectorSpawn.y);

		NetMessage msg;
		msg.write<MessageType>(MessageType::HealthPackSpawn);
		msg.write<int>(healthIndex);
		msg.write<Vector2>(randomVectorSpawn);
		serverBroadcast(msg);
		msg.free();

		healthPacks[healthIndex].spawnHealthPack(randomVectorSpawn);
	}
}

void CheakPlayerShields() 
{
	if (!gameStarted)
	{
		return;
	}

	for (auto& player : players)
	{
		if (!player.alive)
			continue;
		if (!player.shieldActive)
			continue;

		float time = engElapsedTime() - player.lastShieldTime;

		engPrint("PlayerUpShield = %f", time);

		if (time < player.ShieldUpTime)
			break;

		player.lastShieldTime = engElapsedTime();
		player.shieldActive = false;

		NetMessage msg;
		msg.write<MessageType>(MessageType::PlayerUpShield);
		msg.write<int>(player.id);
		msg.write<float>(player.lastShieldTime);
		msg.write<bool>(player.shieldActive);
		serverBroadcast(msg);
		msg.free();
	}
}

void Update() 
{
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

	for (auto& pack : healthPacks)
	{
		if (pack.alive)
			pack.update();
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

	if (!serverStartup(666))
		return 1;

	while(engBeginFrame())
	{
		NetEvent event;
		while(netPollEvent(&event))
		{
			switch(event.type)
			{
				case NetEventType::UserConnected:
				{
					if (gameStarted)
					{
						//serverKickUser(event.userId);
						//break;
					}

					engPrint("User %d connected", event.userId);
					serverAcceptUser(event.userId);

					for (int i = 0; i < PLAYER_MAX; i++)
					{
						if (!players[i].alive)
							continue;

						{
							NetMessage spawnMsg;
							spawnMsg.write<MessageType>(MessageType::PlayerSpawn);
							spawnMsg.write<int>(i);
							spawnMsg.write<Vector2>(players[i].position);
							serverSendTo(spawnMsg, event.userId);
							spawnMsg.free();
						}
						{
							NetMessage nameMsg;
							nameMsg.write<MessageType>(MessageType::PlayerName);
							nameMsg.write<int>(i);
							nameMsg.write<unsigned char>(strlen(players[i].name));
							nameMsg.write(players[i].name, strlen(players[i].name));

							serverSendTo(nameMsg, event.userId);
							nameMsg.free();
						}	
					}

					for (int j = 0; j < MAX_HEALTH_PACKS; j++)
					{
						if (!healthPacks[j].alive)
							continue;

						NetMessage nameMsg;
						nameMsg.write<MessageType>(MessageType::HealthPackSpawn);
						nameMsg.write<int>(j);
						nameMsg.write<Vector2>(healthPacks[j].position);
						serverSendTo(nameMsg, event.userId);
						nameMsg.free();
					}

					{
						Player* player = &players[event.userId];

						player->spawn(event.userId, Vector2(rand() % 800, rand() % 600));

						NetMessage msg;
						msg.write<MessageType>(MessageType::PlayerSpawn);
						msg.write<int>(event.userId);
						msg.write<Vector2>(player->position);

						serverBroadcast(msg);
						msg.free();
					}

					{
						NetMessage msg;
						msg.write<MessageType>(MessageType::PlayerPossess);
						msg.write<int>(event.userId);

						serverSendTo(msg, event.userId);
						msg.free();
					}
					break;
				}

				case NetEventType::UserDisconnected:
				{
					engPrint("User %d disconnected", event.userId);
					players[event.userId].destroy();

					NetMessage destroyMsg;
					destroyMsg.write<MessageType>(MessageType::PlayerDestroy);
					destroyMsg.write<int>(event.userId);

					serverBroadcast(destroyMsg);
					destroyMsg.free();
					break;
				}

				case NetEventType::Message:
				{
					handleMessage(event.userId, event.message);
					break;
				}
			}

			event.free();
		}

		engSetColor(0xCC4444FF);
		engClear();

		if (engKeyPressed(Key::Space))
			gameStarted = !gameStarted;

		engSetColor(0xFFFFFFFF);
		engText(400, 12, gameStarted ? "STARTED" : "WAITING...");

		// Check if we have a winner
		if(gameStarted)
		{
			int numAlivePlayers = 0;
			int lastAlivePlayer = -1;
			for (auto& player : players)
			{
				if (player.alive)
				{
					++numAlivePlayers;
					lastAlivePlayer = player.id;
				}
			}

			if (numAlivePlayers == 1)
				engTextf(400, 300, "'%s' WINS!", players[lastAlivePlayer].name);
			else if (numAlivePlayers == 0)
				engText(400, 300, "Draw :(");
		}

		HealthSpawner();

		CheakPlayerShields();

		Update();

		Draw();
	}

	return 0;
}

#endif
