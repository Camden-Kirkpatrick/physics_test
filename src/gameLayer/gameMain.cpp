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
	gameData.box.transform.w = 100.0f;
	gameData.box.transform.h = 100.0f;
	gameData.box.drag = 0.01f;
	//gameData.box.velocity = { 0.0f, 0.0f };
	//gameData.box.acceleration = { 100.0f, 100.0f };

	// Center the box's horizontal position
	gameData.box.teleport({ (win_width / 2) - (0.5f * gameData.box.transform.w), 0 });

	return true;
}

bool updateGame()
{
	float deltaTime = GetFrameTime();
	if (deltaTime > 0.05f) deltaTime = 0.05f; // clamp to 20fps minimum

	//gameData.box.acceleration.x += 5.0f;

	float g = 200.0f;
	gameData.box.applyGravity(g);

	auto& box = gameData.box;
	float right = win_width - box.transform.w;
	float bottom = win_height - box.transform.h;

	// Bounce off walls: only when past an edge AND moving into it (so we don't
	// re-flip a box that's already leaving or resting). Clamp back to the edge
	// so the box can't sit outside the bounds and re-trigger next frame.
	if (box.transform.pos.y >= bottom && box.velocity.y > 0) {
		box.transform.pos.y = bottom;
		box.velocity.y *= -1.0f;
	}
	if (box.transform.pos.y <= 0 && box.velocity.y < 0) {
		box.transform.pos.y = 0;
		box.velocity.y *= -1.0f;
	}
	if (box.transform.pos.x >= right && box.velocity.x > 0) {
		box.transform.pos.x = right;
		box.velocity.x *= -1.0f;
	}
	if (box.transform.pos.x <= 0 && box.velocity.x < 0) {
		box.transform.pos.x = 0;
		box.velocity.x *= -1.0f;
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