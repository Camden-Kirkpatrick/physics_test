//#include "gameMain.hpp"
//#include <raylib.h>
//#include <fstream>
//#include <iostream>
//#include "physics.hpp"
//#include "randomStuff.hpp"
//#include <asserts.hpp>
//
//#define NUM_BOXES 5
//
//const Color COLORS[7] = {
//	RED,
//	GREEN,
//	BLUE,
//	YELLOW,
//	ORANGE,
//	PINK,
//	PURPLE
//};
//
//struct GameData
//{
//	PhysicalEntity boxes[NUM_BOXES];
//	PhysicalEntity insideBoxes[NUM_BOXES];
//	Color boxColors[NUM_BOXES];
//	Color insideBoxColors[NUM_BOXES];
//	std::ranlux24_base rng;
//} gameData;
//
//bool initGame()
//{
//	std::random_device rd;
//	gameData.rng.seed(rd());
//
//	for (int i = 0; i < NUM_BOXES; i++)
//	{
//		int randomSize = getRandomInt(gameData.rng, 200, 600);
//		gameData.boxes[i].transform.w = randomSize;
//		gameData.boxes[i].transform.h = randomSize;
//		gameData.boxes[i].drag = 0.0f;
//		gameData.boxes[i].velocity = { getRandomFloat(gameData.rng, 500, 500), getRandomFloat(gameData.rng, 500, 500) };
//		gameData.boxColors[i] = COLORS[getRandomInt(gameData.rng, 0, 6)];
//
//		randomSize = getRandomInt(gameData.rng, 50, 199);
//		gameData.insideBoxes[i].transform.w = randomSize;
//		gameData.insideBoxes[i].transform.h = randomSize;
//		gameData.insideBoxes[i].drag = 0.0f;
//		gameData.insideBoxes[i].velocity = { 100.0f, 1000.0f };
//		gameData.insideBoxes[i].acceleration = { 0.0f, 0.0f };
//		gameData.insideBoxColors[i] = COLORS[getRandomInt(gameData.rng, 0, 6)];
//
//		// Middle of the window
//		//gameData.boxes[i].teleport({
//		//	(win_width / 2) - (0.5f * gameData.boxes[i].transform.w),
//		//	(win_height / 2) - (0.5f * gameData.boxes[i].transform.h)
//		//});
//
//		float x = getRandomFloat(gameData.rng, 0, win_width - gameData.boxes[i].transform.w);
//		float y = getRandomFloat(gameData.rng, 0, win_height - gameData.boxes[i].transform.h);
//		// Random spot in the window
//		gameData.boxes[i].teleport({
//			x,
//			y
//		});
//
//		// The inside box's position is stored as an OFFSET from its parent's
//		// top-left corner, not a screen position. So it starts somewhere in the
//		// range [0, parentSize - innerSize]: 0 pins it to the parent's corner,
//		// parentSize - innerSize pushes it flush against the far edge.
//		// Ex. Larger box: w/h = 400, Smaller box: w/h = 100 -> offset range 0..300.
//		gameData.insideBoxes[i].teleport({
//			getRandomFloat(gameData.rng, 0, gameData.boxes[i].transform.w - gameData.insideBoxes[i].transform.w),
//			getRandomFloat(gameData.rng, 0, gameData.boxes[i].transform.h - gameData.insideBoxes[i].transform.h)
//		});
//	}
//
//	return true;
//}
//
//int counter = 0;
//
//bool updateGame()
//{
//	float deltaTime = GetFrameTime();
//	if (deltaTime > 0.05f) deltaTime = 0.05f; // clamp to 20fps minimum
//
//	for (int i = 0; i < NUM_BOXES; i++)
//	{
//		auto& box = gameData.boxes[i];
//		float right = win_width - box.transform.w;
//		float bottom = win_height - box.transform.h;
//
//		// The inside box lives in its parent's local space (its position is an
//		// offset from the parent's corner), so its walls are fixed numbers that
//		// don't depend on where the parent is: 0 for top/left, and
//		// parentSize - innerSize for bottom/right.
//		auto& insideBox = gameData.insideBoxes[i];
//		float insideBottom = box.transform.h - insideBox.transform.h;
//		float insideRight = box.transform.w - insideBox.transform.w;
//
//
//		float g = 0.0f;
//		box.applyGravity(g);
//		insideBox.applyGravity(g);
//		box.updateForces(deltaTime);
//		insideBox.updateForces(deltaTime);
//
//		// Parent box bounces off the window edges. Reflect it back inside the wall
//		// instead of clamping to it: clamping deletes the overshoot and quietly
//		// drains energy (boxes stop even with drag == 0). Mirror across the edge
//		// instead: pos = 2*edge - pos, then flip velocity. For the top/left walls
//		// edge is 0, so 2*edge - pos simplifies to -pos.
//		if (box.transform.pos.y >= bottom && box.velocity.y > 0) {
//			box.transform.pos.y = 2.0f * bottom - box.transform.pos.y;
//			box.velocity.y *= -1.0f;
//		}
//		if (box.transform.pos.y <= 0 && box.velocity.y < 0) {
//			box.transform.pos.y = -box.transform.pos.y;
//			box.velocity.y *= -1.0f;
//		}
//		if (box.transform.pos.x >= right && box.velocity.x > 0) {
//			box.transform.pos.x = 2.0f * right - box.transform.pos.x;
//			box.velocity.x *= -1.0f;
//		}
//		if (box.transform.pos.x <= 0 && box.velocity.x < 0) {
//			box.transform.pos.x = -box.transform.pos.x;
//			box.velocity.x *= -1.0f;
//		}
//
//		// Inside box bounces off its parent's walls, using the same reflect trick
//		// but in local space: edges are 0 (top/left) and insideBottom/insideRight.
//		if (insideBox.transform.pos.y >= insideBottom && insideBox.velocity.y > 0) {
//			insideBox.transform.pos.y = 2.0f * insideBottom - insideBox.transform.pos.y;
//			insideBox.velocity.y *= -1.0f;
//		}
//		if (insideBox.transform.pos.y <= 0 && insideBox.velocity.y < 0) {
//			insideBox.transform.pos.y = -insideBox.transform.pos.y;
//			insideBox.velocity.y *= -1.0f;
//		}
//		if (insideBox.transform.pos.x >= insideRight && insideBox.velocity.x > 0) {
//			insideBox.transform.pos.x = 2.0f * insideRight - insideBox.transform.pos.x;
//			insideBox.velocity.x *= -1.0f;
//		}
//		if (insideBox.transform.pos.x <= 0 && insideBox.velocity.x < 0) {
//			insideBox.transform.pos.x = -insideBox.transform.pos.x;
//			insideBox.velocity.x *= -1.0f;
//		}
//
//		box.updateFinal();
//		insideBox.updateFinal();
//
//		//DrawRectangle(
//		//	box.transform.pos.x,
//		//	box.transform.pos.y,
//		//	box.transform.w,
//		//	box.transform.h,
//		//	gameData.boxColors[i]
//		//);
//
//		Rectangle r;
//		r.x = box.transform.pos.x;
//		r.y = box.transform.pos.y;
//		r.width = box.transform.w;
//		r.height = box.transform.h;
//
//		DrawRectangleLinesEx(
//			r,
//			1.0f,
//			gameData.boxColors[i]
//		);
//
//		// Convert the inside box's local offset into a screen position for drawing:
//		// parent's corner + offset.
//		float screenX = box.transform.pos.x + insideBox.transform.pos.x;
//		float screenY = box.transform.pos.y + insideBox.transform.pos.y;
//		 
//		//DrawRectangle(
//		//	screenX,
//		//	screenY,
//		//	insideBox.transform.w,
//		//	insideBox.transform.h,
//		//	gameData.boxColors[i]
//		//);
//
//		float radius = insideBox.transform.w * 0.5f;
//		DrawCircle(screenX + radius, screenY + radius, radius, gameData.insideBoxColors[i]);
//
//	}
//
//	//// Player movement
//	//if (IsKeyDown(KEY_A)) { gameData.posX -= 200 * deltaTime; }
//	//if (IsKeyDown(KEY_D)) { gameData.posX += 200 * deltaTime; }
//	//if (IsKeyDown(KEY_W)) { gameData.posY -= 200 * deltaTime; }
//	//if (IsKeyDown(KEY_S)) { gameData.posY += 200 * deltaTime; }
//
//
//	//// Prevent the player from going out of bounds
//	//if (gameData.posX < 0) gameData.posX = 0;
//	//if (gameData.posX + gameData.playerWidth > win_width)
//	//	gameData.posX = win_width - gameData.playerWidth;
//	// 
//	//if (gameData.posY < 0) gameData.posY = 0;
//	//if (gameData.posY + gameData.playerHeight > win_height)
//	//	gameData.posY = win_height - gameData.playerHeight;
//
//	//// Draw the player
//	//DrawRectangle(gameData.posX, gameData.posY, gameData.playerWidth, gameData.playerHeight, gameData.c);
//
//	// Debug: print box 0's y velocity once per frame (outside the box loop, so it
//	// prints once, not once per box).
//	//std::cout << gameData.boxes[0].velocity.y << std::endl;
//
//	counter++;
//	return true;
//}
//
//void closeGame()
//{
//	std::ofstream f(RESOURCES_PATH "f.txt");
//	f << "CLOSED\n";
//	f.close();
//}



















































#include "gameMain.hpp"
#include <raylib.h>
#include <fstream>
#include <cmath>
#include <vector>
#include <iostream>
#include <limits>
#include "physics.hpp"
#include "randomStuff.hpp"
#include <asserts.hpp>

#define NUM_BOXES 10
#define MAX_NUM_BOXES 100'000
#define MIN_NUM_BOXES 1

#define NO_AIR_RES 0.0f
#define LOW_AIR_RES 0.0001f
#define MED_AIR_RES 0.001f
#define HIGH_AIR_RES 0.01f

#define NO_GRAV 0
#define LOW_GRAV 1000
#define MED_GRAV 5000
#define HIGH_GRAV 10000

#define DEMO 0
#define CUSTOM 1

#define STATIC 0
#define MOVING 1

#define DAMPED_MOTION 0
#define PERPETUAL_MOTION 1

const Color COLORS[7] = {
	RED,
	GREEN,
	BLUE,
	YELLOW,
	ORANGE,
	PINK,
	PURPLE
};

struct Range
{
	int min;
	int max;
};

constexpr Range BOX_SIZE_RANGES[] = {
	{5, 50},
	{50, 250},
	{100, 400}
};

enum class BoxSizeOptions
{
	SMALL = 0,
	MED,
	LARGE
};

constexpr Range BOX_SPEED_RANGES[] = {
	{-100, 100},
	{-1000, 1000},
	{-5000, 5000}
};

enum class BoxSpeedOptions
{
	SLOW = 0,
	MEDIUM,
	FAST
};

struct GameData
{
	//PhysicalEntity boxes[NUM_BOXES];

	int numBoxes = 0;
	int gameMode = DEMO;
	float airRes = NO_AIR_RES;
	float grav = NO_GRAV;
	std::vector<PhysicalEntity> boxes = {};
	std::vector<Color> boxColors = {};
	std::ranlux24_base rng = {};
} gameData;

bool initGame()
{
	std::random_device rd;
	gameData.rng.seed(rd());

	std::cout << "Enter 0 for an immediate demo, or 1 for a custom experience: ";
	std::cin >> gameData.gameMode;
	while (!(std::cin) || (gameData.gameMode != 0 && gameData.gameMode != 1))
	{
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::cerr << "Invalid option selected\n";
		std::cout << "Enter 0 for an immediate demo, or 1 for a custom experience: ";
		std::cin >> gameData.gameMode;
	}

	// Interactive demo
	if (gameData.gameMode == CUSTOM)
	{
		std::cout << "How many boxes do you want? ";
		std::cin >> gameData.numBoxes;
		while (gameData.numBoxes > MAX_NUM_BOXES || gameData.numBoxes < MIN_NUM_BOXES)
		{
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cerr << "The number of boxes must be in the range [1, 100,000]\n";
			std::cout << "How many boxes do you want? ";
			std::cin >> gameData.numBoxes;
		}

		gameData.boxes.resize(gameData.numBoxes);
		gameData.boxColors.resize(gameData.numBoxes);

		int boxSize = -1;
		std::cout << "How big do you want the boxes to be? (0: SMALL, 1: MEDIUM, 2: LARGE) ";
		std::cin >> boxSize;

		while (!(std::cin) || (boxSize != (int)BoxSizeOptions::SMALL && boxSize != (int)BoxSizeOptions::MED && boxSize != (int)BoxSizeOptions::LARGE))
		{
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cerr << "Invalid box size entered\n";
			std::cout << "Enter a corresponding number for the box size you want: (0: SMALL, 1: MEDIUM, 2: LARGE) ";
			std::cin >> boxSize;
		}

		Range range = BOX_SIZE_RANGES[boxSize];

		int boxMovement = -1;
		std::cout << "Do you want the boxes to have their own speed? (0: NO, 1: YES) ";
		std::cin >> boxMovement;

		while (!(std::cin) || (boxMovement != STATIC && boxMovement != MOVING))
		{
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cerr << "You must enter either 0 or 1\n";
			std::cout << "Do you want the boxes to have their own speed? (0: NO, 1: YES) ";
			std::cin >> boxMovement;
		}

		int boxSpeed = -1;
		Range r = { 0, 0 };
		if (boxMovement == 1)
		{
			std::cout << "Choose a speed for the boxes' movement: (0: SLOW, 1: MEDIUM, 2: FAST) ";
			std::cin >> boxSpeed;

			while (!(std::cin) || (boxSpeed != (int)BoxSpeedOptions::SLOW && boxSpeed != (int)BoxSpeedOptions::MEDIUM && boxSpeed != (int)BoxSpeedOptions::FAST))
			{
				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				std::cerr << "Incorrect speed entered\n";
				std::cout << "Choose a speed for the boxes' movement: (0: SLOW, 1: MEDIUM, 2: FAST) ";
				std::cin >> boxSpeed;
			}
			r = BOX_SPEED_RANGES[boxSpeed];
		}

		int motion = -1;
		std::cout << "Do you want the boxes to move forever? (0: NO, 1: YES) ";
		std::cin >> motion;
		while (!(std::cin) || (motion != DAMPED_MOTION && motion != PERPETUAL_MOTION))
		{
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cerr << "Incorrect option selected\n";
			std::cout << "Do you want the boxes to move forever? (0: NO, 1: YES) ";
			std::cin >> motion;
		}

		if (motion == DAMPED_MOTION)
		{
			gameData.airRes = MED_AIR_RES;
			gameData.grav = MED_GRAV;
		}


		for (int i = 0; i < gameData.numBoxes; i++)
		{
			int randomSize = getRandomInt(gameData.rng, range.min, range.max);
			gameData.boxes[i].transform.w = randomSize;
			gameData.boxes[i].transform.h = randomSize;
			gameData.boxes[i].drag = gameData.airRes;
			if (boxMovement)
				gameData.boxes[i].velocity = { getRandomFloat(gameData.rng, r.min, r.max), getRandomFloat(gameData.rng, r.min, r.max) };

			gameData.boxColors[i] = COLORS[getRandomInt(gameData.rng, 0, 6)];

			// Random spot in the window
			gameData.boxes[i].teleport({
				getRandomFloat(gameData.rng, 0, win_width - gameData.boxes[i].transform.w),
				getRandomFloat(gameData.rng, 0, win_height - gameData.boxes[i].transform.h),
			});
		}
	}

	// Hardcoded demo
	else
	{
		gameData.numBoxes = 25;
		gameData.boxes.resize(gameData.numBoxes);
		gameData.boxColors.resize(gameData.numBoxes);

		for (int i = 0; i < gameData.numBoxes; i++)
		{
			int randomSize = getRandomInt(gameData.rng, 50, 250);
			gameData.boxes[i].transform.w = randomSize;
			gameData.boxes[i].transform.h = randomSize;
			gameData.boxes[i].drag = LOW_AIR_RES;
			gameData.boxes[i].velocity = { getRandomFloat(gameData.rng, -1000, 1000), getRandomFloat(gameData.rng, -1000, 1000) };

			gameData.boxColors[i] = COLORS[getRandomInt(gameData.rng, 0, 6)];

			// Random spot in the window
			gameData.boxes[i].teleport({
				getRandomFloat(gameData.rng, 0, win_width - gameData.boxes[i].transform.w),
				getRandomFloat(gameData.rng, 0, win_height - gameData.boxes[i].transform.h),
			});
		}
	}

	return true;
}

bool updateGame()
{
	float deltaTime = GetFrameTime();
	if (deltaTime > 0.05f) deltaTime = 0.05f; // clamp to 20fps minimum

	// Bounce-only physics: there is no "normal force" holding a box on the floor
	// -- the only response to a wall is a bounce. Each box: apply gravity,
	// integrate, then reflect off any wall it crossed. Reflecting (not clamping)
	// conserves energy, so drag == 0 means the boxes bounce forever.
	//
	// The catch: with no normal force, gravity re-nudges a grounded box every
	// frame, so it would bounce/jitter forever. To fix that, the floor check also
	// "pins" a box to rest once its bounce is too small to see. That pin is what
	// lets a box (with drag > 0) actually come to a full stop -- it stands in for
	// the missing normal force. With drag == 0 the bounces never get small enough
	// to trip it, so it never fires.
	for (int i = 0; i < gameData.numBoxes; i++)
	{
		auto& box = gameData.boxes[i];
		float right = win_width - box.transform.w;
		float bottom = win_height - box.transform.h;

		box.applyGravity(gameData.grav);

		// Integrate first, resolve collisions after, so the rest-pin is the last
		// thing to touch the box before it's drawn (not undone by integration).
		box.updateForces(deltaTime);

		// A bounce at speed u rises u*u/(2g), so the speed that rises exactly
		// restRise is sqrt(2*g*restRise): below that a bounce is invisible (<~1px).
		//
		// The second term is NOT optional. A resting box is re-nudged by gravity
		// every frame by g*deltaTime, and the pin only fires when that nudge is
		// below the threshold. deltaTime spikes on slow/hitched frames (and is
		// clamped up to 0.05 above), so on a bad frame g*deltaTime can blow past a
		// fixed threshold and the box hops -- intermittent, "out of nowhere" jitter.
		// Flooring the threshold at 2*g*deltaTime makes it scale with the nudge, so
		// the residual is always caught no matter how slow the frame runs.
		float restRise = 1.0f; // px
		float restThreshold = fmaxf(sqrtf(2.0f * gameData.grav * restRise), 2.0f * gameData.grav * deltaTime);

		// Bounce off the walls: reflect across the edge (pos = 2*edge - pos) and
		// flip velocity; edge 0 makes 2*edge - pos == -pos. Clamping to the edge
		// instead would delete the overshoot and leak energy every bounce.
		if (box.transform.pos.y >= bottom && box.velocity.y > 0) {
			box.transform.pos.y = 2.0f * bottom - box.transform.pos.y;
			box.velocity.y *= -1.0f;
			// velocity.y is now negative (upward); -velocity.y is its speed.
			// Too slow to make a visible bounce -> pin to the floor and stop.
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
