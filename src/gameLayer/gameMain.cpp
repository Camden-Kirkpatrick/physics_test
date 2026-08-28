#include "gameMain.hpp"
#include <raylib.h>
#include <fstream>
#include <iostream>
#include "physics.hpp"
#include <asserts.hpp>

struct GameData
{
	//float posX = 0;
	//float posY = 0;
	//int playerWidth = 50;
	//int playerHeight = 50;
	//Color c{255, 0, 200, 255};

	PhysicalEntity box;
}gameData;

bool initGame()
{
	gameData.box.teleport({ 0, 0 });
	gameData.box.transform.w = 100.0f;
	gameData.box.transform.h = 100.0f;
	gameData.box.velocity = { 300.0f, 300.0f };
	gameData.box.acceleration = { 100.0f, 100.0f };

	return true;
}

bool updateGame()
{
	float deltaTime = GetFrameTime();
	if (deltaTime > 0.05f) deltaTime = 0.05f; // clamp to 20fps minimum

	//gameData.box.applyGravity();

	if (gameData.box.transform.pos.y >= win_height - gameData.box.transform.h)
	{
		gameData.box.velocity.y *= -1.0f;
		gameData.box.acceleration.y *= -1.0f;
	}
	else if (gameData.box.transform.pos.y < 0)
	{
		gameData.box.velocity.y *= -1.0f;
		gameData.box.acceleration.y *= -1.0f;
	}
	else if (gameData.box.transform.pos.x >= win_width - gameData.box.transform.w)
	{
		gameData.box.velocity.x *= -1.0f;
		gameData.box.acceleration.x *= -1.0f;
	}
	else if (gameData.box.transform.pos.x < 0)
	{
		gameData.box.velocity.x *= -1.0f;
		gameData.box.acceleration.x *= -1.0f;
	}


	gameData.box.updateForces(deltaTime);
	std::cout << gameData.box.velocity.x << ", " << gameData.box.velocity.y << std::endl;

	gameData.box.updateFinal();

	DrawRectangle(
		gameData.box.transform.pos.x,
		gameData.box.transform.pos.y,
		gameData.box.transform.w,
		gameData.box.transform.h,
		{255, 0, 0, 255}
	);

	//// Player movement
	//if (IsKeyDown(KEY_A)) { gameData.posX -= 200 * deltaTime; }
	//if (IsKeyDown(KEY_D)) { gameData.posX += 200 * deltaTime; }
	//if (IsKeyDown(KEY_W)) { gameData.posY -= 200 * deltaTime; }
	//if (IsKeyDown(KEY_S)) { gameData.posY += 200 * deltaTime; }


	//// Prevent the player from going out of bounds
	//if (gameData.posX < 0) gameData.posX = 0;
	//if (gameData.posX + gameData.playerWidth > win_width)
	//	gameData.posX = win_width - gameData.playerWidth;
	// 
	//if (gameData.posY < 0) gameData.posY = 0;
	//if (gameData.posY + gameData.playerHeight > win_height)
	//	gameData.posY = win_height - gameData.playerHeight;

	//// Draw the player
	//DrawRectangle(gameData.posX, gameData.posY, gameData.playerWidth, gameData.playerHeight, gameData.c);

	return true;
}

void closeGame()
{
	std::ofstream f(RESOURCES_PATH "f.txt");
	f << "CLOSED\n";
	f.close();
}