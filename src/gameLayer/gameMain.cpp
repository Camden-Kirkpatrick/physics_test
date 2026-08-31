#include "gameMain.hpp"
#include <raylib.h>
#include <fstream>
#include <iostream>
#include "physics.hpp"
#include <asserts.hpp>
#include <random>

#define NUM_BOXES 5

struct GameData
{
	//float posX = 0;
	//float posY = 0;
	//int playerWidth = 50;
	//int playerHeight = 50;
	//Color c{255, 0, 200, 255};

	PhysicalEntity box;
	PhysicalEntity boxes[NUM_BOXES];
}gameData;

bool initGame()
{
	//gameData.box.transform.w = 100.0f;
	//gameData.box.transform.h = 100.0f;
	//gameData.box.drag = 0.0f;
	//gameData.box.velocity = { 100.0f, 0.0f };
	//gameData.box.acceleration = { 0.0f, 0.0f };

	//// Center the box's position
	//gameData.box.teleport({
	//	(win_width / 2) - (0.5f * gameData.box.transform.w),
	//	(win_height / 2) - (0.5f * gameData.box.transform.h)
	//});

	for (int i = 0; i < NUM_BOXES; i++)
	{
		gameData.boxes[i].transform.w = 100.0f;
		gameData.boxes[i].transform.h = 100.0f;
		gameData.boxes[i].drag = 0.0f;
		gameData.boxes[i].velocity = { 100.0f, 0.0f };
		gameData.boxes[i].acceleration = { 0.0f, 0.0f };

		gameData.boxes[i].teleport({
			(win_width / 2) - (0.5f * gameData.boxes[i].transform.w),
			(win_height / 2) - (0.5f * gameData.boxes[i].transform.h)
		});
	}

	return true;
}

bool updateGame()
{
	float deltaTime = GetFrameTime();
	if (deltaTime > 0.05f) deltaTime = 0.05f; // clamp to 20fps minimum

	for (int i = 0; i < NUM_BOXES; i++)
	{
		auto& box = gameData.boxes[i];
		float right = win_width - box.transform.w;
		float bottom = win_height - box.transform.h;

		float g = 0.0f;
		box.applyGravity(g);

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

		box.updateForces(deltaTime);
		//std::cout << gameData.box.velocity.x << ", " << gameData.box.velocity.y << std::endl;

		box.updateFinal();

		DrawRectangle(
			box.transform.pos.x,
			box.transform.pos.y,
			box.transform.w,
			box.transform.h,
			{ 255, 0, 0, 255 }
		);
	}

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