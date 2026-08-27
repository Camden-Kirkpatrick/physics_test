#include "gameMain.hpp"
#include <raylib.h>
#include <fstream>
#include <iostream>
#include <asserts.hpp>

struct GameData
{
	float posX = 0;
	float posY = 0;
	int playerWidth = 50;
	int playerHeight = 50;
	Color c{255, 0, 200, 255};
}gameData;

bool initGame()
{
	return true;
}

bool updateGame()
{
	float deltaTime = GetFrameTime();
	if (deltaTime > 0.05f) deltaTime = 0.05f; // clamp to 20fps minimum

	// Player movement
	if (IsKeyDown(KEY_A)) { gameData.posX -= 200 * deltaTime; }
	if (IsKeyDown(KEY_D)) { gameData.posX += 200 * deltaTime; }
	if (IsKeyDown(KEY_W)) { gameData.posY -= 200 * deltaTime; }
	if (IsKeyDown(KEY_S)) { gameData.posY += 200 * deltaTime; }


	// Prevent the player from going out of bounds
	if (gameData.posX < 0) gameData.posX = 0;
	if (gameData.posX + gameData.playerWidth > win_width)
		gameData.posX = win_width - gameData.playerWidth;
	 
	if (gameData.posY < 0) gameData.posY = 0;
	if (gameData.posY + gameData.playerHeight > win_height)
		gameData.posY = win_height - gameData.playerHeight;

	// Draw the player
	DrawRectangle(gameData.posX, gameData.posY, gameData.playerWidth, gameData.playerHeight, gameData.c);

	return true;
}

void closeGame()
{
	std::ofstream f(RESOURCES_PATH "f.txt");
	f << "CLOSED\n";
	f.close();
}