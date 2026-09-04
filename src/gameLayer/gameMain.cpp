#include "gameMain.hpp"
#include <raylib.h>
#include <fstream>
#include <iostream>
#include <cmath>
#include "physics.hpp"
#include "randomStuff.hpp"
#include <asserts.hpp>

#define NUM_BOXES 10

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
	Color boxColors[NUM_BOXES];
	std::ranlux24_base rng;
} gameData;

bool initGame()
{
	std::random_device rd;
	gameData.rng.seed(rd());

	for (int i = 0; i < NUM_BOXES; i++)
	{
		int randomSize = getRandomInt(gameData.rng, 50, 250);
		gameData.boxes[i].transform.w = randomSize;
		gameData.boxes[i].transform.h = randomSize;
		gameData.boxes[i].drag = 0.0025f;
		//gameData.boxes[i].velocity = { getRandomFloat(gameData.rng, 0, 1000), getRandomFloat(gameData.rng, 1, 1000) };
		gameData.boxes[i].acceleration = { 0.0f, 0.0f };

		gameData.boxColors[i] = COLORS[getRandomInt(gameData.rng, 0, 6)];

		// Middle of the window
		//gameData.boxes[i].teleport({
		//	(win_width / 2) - (0.5f * gameData.boxes[i].transform.w),
		//	(win_height / 2) - (0.5f * gameData.boxes[i].transform.h)
		//});

		// Random spot in the window
		gameData.boxes[i].teleport({
			getRandomFloat(gameData.rng, 0, win_width - gameData.boxes[i].transform.w),
			getRandomFloat(gameData.rng, 0, win_height - gameData.boxes[i].transform.h),
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

		float g = 5000.0f;
		box.applyGravity(g);

		// Integrate FIRST (move the box), then handle collisions. This way the
		// collision/rest fix is the last thing to touch the box before it's drawn,
		// so it isn't immediately undone by the integration step.
		box.updateForces(deltaTime);

		// Treat the box as resting once a bounce would rise no more than ~1px.
		// A bounce back at speed u rises u*u/(2g). Below ~1px the bounce is
		// invisible, but drag is far too weak at those speeds to kill it quickly,
		// so without this the box does a multi-second tail of 1px jitter before
		// finally stopping. Pinning at the "can't rise a pixel" speed skips that.
		// Keep it at least ~2 gravity-steps so the per-frame gravity residual
		// (~g*dt) can't sneak under the threshold either.
		float restRise = 1.0f; // pixels
		float restThreshold = fmaxf(sqrtf(2.0f * g * restRise), 2.0f * g * deltaTime);

		// Box bounces off the window edges. Reflect it back inside the wall
		// instead of clamping to it: clamping deletes the overshoot and quietly
		// drains energy. Mirror across the edge: pos = 2*edge - pos, then flip
		// velocity. For the top/left walls edge is 0, so 2*edge - pos == -pos.
		if (box.transform.pos.y >= bottom && box.velocity.y > 0) {
			box.transform.pos.y = 2.0f * bottom - box.transform.pos.y;
			box.velocity.y *= -1.0f;
			// velocity.y is now negative (upward); -velocity.y is its speed.
			if (-box.velocity.y < restThreshold) {
				box.transform.pos.y = bottom;
				box.velocity.y = 0.0f;
			}
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

		box.updateFinal();

		DrawRectangle(
			box.transform.pos.x,
			box.transform.pos.y,
			box.transform.w,
			box.transform.h,
			gameData.boxColors[i]
		);
	}

	return true;
}

void closeGame()
{
	std::ofstream f(RESOURCES_PATH "f.txt");
	f << "CLOSED\n";
	f.close();
}
