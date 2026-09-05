#include "gameMain.hpp"
#include <raylib.h>
#include <fstream>
#include <iostream>
#include "physics.hpp"
#include "randomStuff.hpp"
#include <asserts.hpp>

#define NUM_BOXES 5

const Color COLORS[7] = {
	RED,
	GREEN,
	BLUE,
	YELLOW,
	ORANGE,
	PINK,
	PURPLE
};

struct GameData
{
	PhysicalEntity boxes[NUM_BOXES];
	PhysicalEntity insideBoxes[NUM_BOXES];
	Color boxColors[NUM_BOXES];
	Color insideBoxColors[NUM_BOXES];
	std::ranlux24_base rng;
} gameData;

bool initGame()
{
	std::random_device rd;
	gameData.rng.seed(rd());

	for (int i = 0; i < NUM_BOXES; i++)
	{
		int randomSize = getRandomInt(gameData.rng, 200, 600);
		gameData.boxes[i].transform.w = randomSize;
		gameData.boxes[i].transform.h = randomSize;
		gameData.boxes[i].drag = 0.0f;
		gameData.boxes[i].velocity = { getRandomFloat(gameData.rng, 500, 500), getRandomFloat(gameData.rng, 500, 500) };
		gameData.boxes[i].acceleration = { 0.0f, 0.0f };
		gameData.boxColors[i] = COLORS[getRandomInt(gameData.rng, 0, 6)];

		randomSize = getRandomInt(gameData.rng, 50, 199);
		gameData.insideBoxes[i].transform.w = randomSize;
		gameData.insideBoxes[i].transform.h = randomSize;
		gameData.insideBoxes[i].drag = 0.0f;
		gameData.insideBoxes[i].velocity = { 100.0f, 1000.0f };
		gameData.insideBoxes[i].acceleration = { 0.0f, 0.0f };
		gameData.insideBoxColors[i] = COLORS[getRandomInt(gameData.rng, 0, 0)];

		// Middle of the window
		//gameData.boxes[i].teleport({
		//	(win_width / 2) - (0.5f * gameData.boxes[i].transform.w),
		//	(win_height / 2) - (0.5f * gameData.boxes[i].transform.h)
		//});

		float x = getRandomFloat(gameData.rng, 0, win_width - gameData.boxes[i].transform.w);
		float y = getRandomFloat(gameData.rng, 0, win_height - gameData.boxes[i].transform.h);
		// Random spot in the window
		gameData.boxes[i].teleport({
			x,
			y
		});

		// The inside box's position is stored as an OFFSET from its parent's
		// top-left corner, not a screen position. So it starts somewhere in the
		// range [0, parentSize - innerSize]: 0 pins it to the parent's corner,
		// parentSize - innerSize pushes it flush against the far edge.
		// Ex. Larger box: w/h = 400, Smaller box: w/h = 100 -> offset range 0..300.
		gameData.insideBoxes[i].teleport({
			getRandomFloat(gameData.rng, 0, gameData.boxes[i].transform.w - gameData.insideBoxes[i].transform.w),
			getRandomFloat(gameData.rng, 0, gameData.boxes[i].transform.h - gameData.insideBoxes[i].transform.h)
		});
	}

	return true;
}

int counter = 0;

bool updateGame()
{
	float deltaTime = GetFrameTime();
	if (deltaTime > 0.05f) deltaTime = 0.05f; // clamp to 20fps minimum

	for (int i = 0; i < NUM_BOXES; i++)
	{
		auto& box = gameData.boxes[i];
		float right = win_width - box.transform.w;
		float bottom = win_height - box.transform.h;

		// The inside box lives in its parent's local space (its position is an
		// offset from the parent's corner), so its walls are fixed numbers that
		// don't depend on where the parent is: 0 for top/left, and
		// parentSize - innerSize for bottom/right.
		auto& insideBox = gameData.insideBoxes[i];
		float insideBottom = box.transform.h - insideBox.transform.h;
		float insideRight = box.transform.w - insideBox.transform.w;


		float g = 0.0f;
		box.applyGravity(g);
		insideBox.applyGravity(g);
		box.updateForces(deltaTime);
		insideBox.updateForces(deltaTime);

		// Parent box bounces off the window edges. Reflect it back inside the wall
		// instead of clamping to it: clamping deletes the overshoot and quietly
		// drains energy (boxes stop even with drag == 0). Mirror across the edge
		// instead: pos = 2*edge - pos, then flip velocity. For the top/left walls
		// edge is 0, so 2*edge - pos simplifies to -pos.
		if (box.transform.pos.y >= bottom && box.velocity.y > 0) {
			box.transform.pos.y = 2.0f * bottom - box.transform.pos.y;
			box.velocity.y *= -1.0f;
		}
		if (box.transform.pos.y <= 0 && box.velocity.y < 0) {
			box.transform.pos.y = -box.transform.pos.y;
			box.velocity.y *= -1.0f;
		}
		if (box.transform.pos.x >= right && box.velocity.x > 0) {
			box.transform.pos.x = 2.0f * right - box.transform.pos.x;
			box.velocity.x *= -1.0f;
		}
		if (box.transform.pos.x <= 0 && box.velocity.x < 0) {
			box.transform.pos.x = -box.transform.pos.x;
			box.velocity.x *= -1.0f;
		}

		// Inside box bounces off its parent's walls, using the same reflect trick
		// but in local space: edges are 0 (top/left) and insideBottom/insideRight.
		if (insideBox.transform.pos.y >= insideBottom && insideBox.velocity.y > 0) {
			insideBox.transform.pos.y = 2.0f * insideBottom - insideBox.transform.pos.y;
			insideBox.velocity.y *= -1.0f;
		}
		if (insideBox.transform.pos.y <= 0 && insideBox.velocity.y < 0) {
			insideBox.transform.pos.y = -insideBox.transform.pos.y;
			insideBox.velocity.y *= -1.0f;
		}
		if (insideBox.transform.pos.x >= insideRight && insideBox.velocity.x > 0) {
			insideBox.transform.pos.x = 2.0f * insideRight - insideBox.transform.pos.x;
			insideBox.velocity.x *= -1.0f;
		}
		if (insideBox.transform.pos.x <= 0 && insideBox.velocity.x < 0) {
			insideBox.transform.pos.x = -insideBox.transform.pos.x;
			insideBox.velocity.x *= -1.0f;
		}

		box.updateFinal();
		insideBox.updateFinal();

		//DrawRectangle(
		//	box.transform.pos.x,
		//	box.transform.pos.y,
		//	box.transform.w,
		//	box.transform.h,
		//	gameData.boxColors[i]
		//);

		Rectangle r;
		r.x = box.transform.pos.x;
		r.y = box.transform.pos.y;
		r.width = box.transform.w;
		r.height = box.transform.h;

		DrawRectangleLinesEx(
			r,
			1.0f,
			gameData.boxColors[i]
		);

		// Convert the inside box's local offset into a screen position for drawing:
		// parent's corner + offset.
		float screenX = box.transform.pos.x + insideBox.transform.pos.x;
		float screenY = box.transform.pos.y + insideBox.transform.pos.y;
		 
		//DrawRectangle(
		//	screenX,
		//	screenY,
		//	insideBox.transform.w,
		//	insideBox.transform.h,
		//	gameData.boxColors[i]
		//);

		float radius = insideBox.transform.w * 0.5f;
		DrawCircle(screenX + radius, screenY + radius, radius, gameData.insideBoxColors[i]);

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

	// Debug: print box 0's y velocity once per frame (outside the box loop, so it
	// prints once, not once per box).
	//std::cout << gameData.boxes[0].velocity.y << std::endl;

	counter++;
	return true;
}

void closeGame()
{
	std::ofstream f(RESOURCES_PATH "f.txt");
	f << "CLOSED\n";
	f.close();
}