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
//	PhysicalEntity objects[NUM_BOXES];
//	PhysicalEntity insideObjectes[NUM_BOXES];
//	Color objectColors[NUM_BOXES];
//	Color insideObjectColors[NUM_BOXES];
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
//		gameData.objects[i].transform.w = randomSize;
//		gameData.objects[i].transform.h = randomSize;
//		gameData.objects[i].drag = 0.0f;
//		gameData.objects[i].velocity = { getRandomFloat(gameData.rng, 500, 500), getRandomFloat(gameData.rng, 500, 500) };
//		gameData.objectColors[i] = COLORS[getRandomInt(gameData.rng, 0, 6)];
//
//		randomSize = getRandomInt(gameData.rng, 50, 199);
//		gameData.insideObjectes[i].transform.w = randomSize;
//		gameData.insideObjectes[i].transform.h = randomSize;
//		gameData.insideObjectes[i].drag = 0.0f;
//		gameData.insideObjectes[i].velocity = { 100.0f, 1000.0f };
//		gameData.insideObjectes[i].acceleration = { 0.0f, 0.0f };
//		gameData.insideObjectColors[i] = COLORS[getRandomInt(gameData.rng, 0, 6)];
//
//		// Middle of the window
//		//gameData.objects[i].teleport({
//		//	(win_width / 2) - (0.5f * gameData.objects[i].transform.w),
//		//	(win_height / 2) - (0.5f * gameData.objects[i].transform.h)
//		//});
//
//		float x = getRandomFloat(gameData.rng, 0, win_width - gameData.objects[i].transform.w);
//		float y = getRandomFloat(gameData.rng, 0, win_height - gameData.objects[i].transform.h);
//		// Random spot in the window
//		gameData.objects[i].teleport({
//			x,
//			y
//		});
//
//		// The inside object's position is stored as an OFFSET from its parent's
//		// top-left corner, not a screen position. So it starts somewhere in the
//		// range [0, parentSize - innerSize]: 0 pins it to the parent's corner,
//		// parentSize - innerSize pushes it flush against the far edge.
//		// Ex. Larger object: w/h = 400, Smaller object: w/h = 100 -> offset range 0..300.
//		gameData.insideObjectes[i].teleport({
//			getRandomFloat(gameData.rng, 0, gameData.objects[i].transform.w - gameData.insideObjectes[i].transform.w),
//			getRandomFloat(gameData.rng, 0, gameData.objects[i].transform.h - gameData.insideObjectes[i].transform.h)
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
//		auto& object = gameData.objects[i];
//		float right = win_width - object.transform.w;
//		float bottom = win_height - object.transform.h;
//
//		// The inside object lives in its parent's local space (its position is an
//		// offset from the parent's corner), so its walls are fixed numbers that
//		// don't depend on where the parent is: 0 for top/left, and
//		// parentSize - innerSize for bottom/right.
//		auto& insideObject = gameData.insideObjectes[i];
//		float insideBottom = object.transform.h - insideObject.transform.h;
//		float insideRight = object.transform.w - insideObject.transform.w;
//
//
//		float g = 0.0f;
//		object.applyGravity(g);
//		insideObject.applyGravity(g);
//		object.updateForces(deltaTime);
//		insideObject.updateForces(deltaTime);
//
//		// Parent object bounces off the window edges. Reflect it back inside the wall
//		// instead of clamping to it: clamping deletes the overshoot and quietly
//		// drains energy (objects stop even with drag == 0). Mirror across the edge
//		// instead: pos = 2*edge - pos, then flip velocity. For the top/left walls
//		// edge is 0, so 2*edge - pos simplifies to -pos.
//		if (object.transform.pos.y >= bottom && object.velocity.y > 0) {
//			object.transform.pos.y = 2.0f * bottom - object.transform.pos.y;
//			object.velocity.y *= -1.0f;
//		}
//		if (object.transform.pos.y <= 0 && object.velocity.y < 0) {
//			object.transform.pos.y = -object.transform.pos.y;
//			object.velocity.y *= -1.0f;
//		}
//		if (object.transform.pos.x >= right && object.velocity.x > 0) {
//			object.transform.pos.x = 2.0f * right - object.transform.pos.x;
//			object.velocity.x *= -1.0f;
//		}
//		if (object.transform.pos.x <= 0 && object.velocity.x < 0) {
//			object.transform.pos.x = -object.transform.pos.x;
//			object.velocity.x *= -1.0f;
//		}
//
//		// Inside object bounces off its parent's walls, using the same reflect trick
//		// but in local space: edges are 0 (top/left) and insideBottom/insideRight.
//		if (insideObject.transform.pos.y >= insideBottom && insideObject.velocity.y > 0) {
//			insideObject.transform.pos.y = 2.0f * insideBottom - insideObject.transform.pos.y;
//			insideObject.velocity.y *= -1.0f;
//		}
//		if (insideObject.transform.pos.y <= 0 && insideObject.velocity.y < 0) {
//			insideObject.transform.pos.y = -insideObject.transform.pos.y;
//			insideObject.velocity.y *= -1.0f;
//		}
//		if (insideObject.transform.pos.x >= insideRight && insideObject.velocity.x > 0) {
//			insideObject.transform.pos.x = 2.0f * insideRight - insideObject.transform.pos.x;
//			insideObject.velocity.x *= -1.0f;
//		}
//		if (insideObject.transform.pos.x <= 0 && insideObject.velocity.x < 0) {
//			insideObject.transform.pos.x = -insideObject.transform.pos.x;
//			insideObject.velocity.x *= -1.0f;
//		}
//
//		object.updateFinal();
//		insideObject.updateFinal();
//
//		//DrawRectangle(
//		//	object.transform.pos.x,
//		//	object.transform.pos.y,
//		//	object.transform.w,
//		//	object.transform.h,
//		//	gameData.objectColors[i]
//		//);
//
//		Rectangle r;
//		r.x = object.transform.pos.x;
//		r.y = object.transform.pos.y;
//		r.width = object.transform.w;
//		r.height = object.transform.h;
//
//		DrawRectangleLinesEx(
//			r,
//			1.0f,
//			gameData.objectColors[i]
//		);
//
//		// Convert the inside object's local offset into a screen position for drawing:
//		// parent's corner + offset.
//		float screenX = object.transform.pos.x + insideObject.transform.pos.x;
//		float screenY = object.transform.pos.y + insideObject.transform.pos.y;
//		 
//		//DrawRectangle(
//		//	screenX,
//		//	screenY,
//		//	insideObject.transform.w,
//		//	insideObject.transform.h,
//		//	gameData.objectColors[i]
//		//);
//
//		float radius = insideObject.transform.w * 0.5f;
//		DrawCircle(screenX + radius, screenY + radius, radius, gameData.insideObjectColors[i]);
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
//	// Debug: print object 0's y velocity once per frame (outside the object loop, so it
//	// prints once, not once per object).
//	//std::cout << gameData.objects[0].velocity.y << std::endl;
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

enum class ObjectSizeOptions
{
	SMALL = 0,
	MED,
	LARGE
};

constexpr Range BOX_SPEED_RANGES[] = {
	{0, 0},
	{-100, 100},
	{-1000, 1000},
	{-5000, 5000}
};

enum class ObjectSpeedOptions
{
	STATIONARY = 0,
	SLOW,
	MEDIUM,
	FAST
};

constexpr float GRAVITY[] = {
	0.0f,
	1000.0f,
	5000.0f,
	10000.0f
};

enum class ObjectGravOptions
{
	NONE = 0,
	LOW,
	MEDIUM,
	HIGH
};

constexpr float AIR_RES[] = {
	0.0f,
	0.0001f,
	0.001f,
	0.01f
};

enum class ObjectAirResOptions
{
	NONE = 0,
	LOW,
	MEDIUM,
	HIGH
};

enum class Shape
{
	SQUARE = 0,
	CIRCLE
};

struct GameData
{
	int numObjects = 0;
	int gameMode = DEMO;
	float grav = GRAVITY[0];
	float airRes = AIR_RES[0];
	Shape shape = Shape::SQUARE;
	std::vector<PhysicalEntity> objects = {};
	std::vector<Color> objectColors = {};
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
		std::cout << "How many objects do you want? ";
		std::cin >> gameData.numObjects;
		while (gameData.numObjects > MAX_NUM_BOXES || gameData.numObjects < MIN_NUM_BOXES)
		{
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cerr << "The number of objects must be in the range [1, 100,000]\n";
			std::cout << "How many objects do you want? ";
			std::cin >> gameData.numObjects;
		}

		gameData.objects.resize(gameData.numObjects);
		gameData.objectColors.resize(gameData.numObjects);

		// Choose a size for the objects
		int objectSize = -1;
		std::cout << "How big do you want the objects to be? (0: SMALL, 1: MEDIUM, 2: LARGE) ";
		std::cin >> objectSize;

		while (!(std::cin) || (objectSize != (int)ObjectSizeOptions::SMALL && objectSize != (int)ObjectSizeOptions::MED && objectSize != (int)ObjectSizeOptions::LARGE))
		{
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cerr << "Invalid object size entered\n";
			std::cout << "Enter a corresponding number for the object size you want: (0: SMALL, 1: MEDIUM, 2: LARGE) ";
			std::cin >> objectSize;
		}
		Range range = BOX_SIZE_RANGES[objectSize];

		// Choose a speed for the objects
		int objectSpeed = -1;
		Range r = { 0, 0 };
		std::cout << "Choose a speed for the objects' movement: (0: STATIONARY, 1: SLOW, 2: MEDIUM, 3: FAST) ";
		std::cin >> objectSpeed;

		while (!(std::cin) || (objectSpeed != (int)ObjectSpeedOptions::STATIONARY && objectSpeed != (int)ObjectSpeedOptions::SLOW && objectSpeed != (int)ObjectSpeedOptions::MEDIUM && objectSpeed != (int)ObjectSpeedOptions::FAST))
		{
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cerr << "Incorrect speed entered\n";
			std::cout << "Choose a speed for the objects' movement: (0: STATIONARY, 1: SLOW, 2: MEDIUM, 3: FAST) ";
			std::cin >> objectSpeed;
		}
		r = BOX_SPEED_RANGES[objectSpeed];

		// Choose the gravity for the objects
		int gravSelection = -1;
		std::cout << "Choose a setting for the objects' gravity (0: NONE, 1: LOW, 2: MED, 3: HIGH) ";
		std::cin >> gravSelection;
		while (!(std::cin) || (gravSelection != (int)ObjectGravOptions::NONE && gravSelection != (int)ObjectGravOptions::LOW && gravSelection != (int)ObjectGravOptions::MEDIUM && gravSelection != (int)ObjectGravOptions::HIGH))
		{
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cerr << "Incorrect gravity selected\n";
			std::cout << "Choose a setting for the objects' gravity (0: NONE, 1: LOW, 2: MED, 3: HIGH) ";
			std::cin >> gravSelection;
		}
		gameData.grav = GRAVITY[gravSelection];

		// Choose the air resistance for the objects
		int airResSelection = -1;
		std::cout << "Choose a setting for the objects' air resistance (0: NONE, 1: LOW, 2: MED, 3: HIGH) ";
		std::cin >> airResSelection;
		while (!(std::cin) || (airResSelection != (int)ObjectAirResOptions::NONE && airResSelection != (int)ObjectAirResOptions::LOW && airResSelection != (int)ObjectAirResOptions::MEDIUM && airResSelection != (int)ObjectAirResOptions::HIGH))
		{
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cerr << "Incorrect air resistance selected\n";
			std::cout << "Choose a setting for the objects' air resistance (0: NONE, 1: LOW, 2: MED, 3: HIGH) ";
			std::cin >> airResSelection;
		}
		gameData.airRes = AIR_RES[airResSelection];

		// Choose the shape of the object
		int shapeSelection = -1;
		std::cout << "Choose a setting for the objects shape (0: SQUARE, 1: CIRCLE) ";
		std::cin >> shapeSelection;
		while (!(std::cin) || (shapeSelection != (int)Shape::SQUARE && shapeSelection != (int)Shape::CIRCLE))
		{
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cerr << "Incorrect shape selected\n";
			std::cout << "Choose a setting for the objects shape (0: SQUARE, 1: CIRCLE) ";
			std::cin >> shapeSelection;
		}
		gameData.shape = (Shape)shapeSelection;

		for (int i = 0; i < gameData.numObjects; i++)
		{
			int randomSize = getRandomInt(gameData.rng, range.min, range.max);
			gameData.objects[i].transform.w = randomSize;
			gameData.objects[i].transform.h = randomSize;
			gameData.objects[i].drag = gameData.airRes;
			gameData.objects[i].velocity = { getRandomFloat(gameData.rng, r.min, r.max), getRandomFloat(gameData.rng, r.min, r.max) };

			gameData.objectColors[i] = COLORS[getRandomInt(gameData.rng, 0, 6)];

			// Random spot in the window
			gameData.objects[i].teleport({
				getRandomFloat(gameData.rng, 0, win_width - gameData.objects[i].transform.w),
				getRandomFloat(gameData.rng, 0, win_height - gameData.objects[i].transform.h),
			});
		}
	}

	// Hardcoded demo
	else
	{
		gameData.numObjects = 25;
		gameData.objects.resize(gameData.numObjects);
		gameData.objectColors.resize(gameData.numObjects);

		for (int i = 0; i < gameData.numObjects; i++)
		{
			int randomSize = getRandomInt(gameData.rng, 50, 250);
			gameData.objects[i].transform.w = randomSize;
			gameData.objects[i].transform.h = randomSize;
			gameData.objects[i].drag = AIR_RES[1];
			gameData.objects[i].velocity = { getRandomFloat(gameData.rng, -1000, 1000), getRandomFloat(gameData.rng, -1000, 1000) };

			gameData.objectColors[i] = COLORS[getRandomInt(gameData.rng, 0, 6)];

			// Random spot in the window
			gameData.objects[i].teleport({
				getRandomFloat(gameData.rng, 0, win_width - gameData.objects[i].transform.w),
				getRandomFloat(gameData.rng, 0, win_height - gameData.objects[i].transform.h),
			});
		}
	}

	return true;
}

bool updateGame()
{
	float deltaTime = GetFrameTime();
	if (deltaTime > 0.05f) deltaTime = 0.05f; // clamp to 20fps minimum

	// Bounce-only physics: there is no "normal force" holding a object on the floor
	// -- the only response to a wall is a bounce. Each object: apply gravity,
	// integrate, then reflect off any wall it crossed. Reflecting (not clamping)
	// conserves energy, so drag == 0 means the objects bounce forever.
	//
	// The catch: with no normal force, gravity re-nudges a grounded object every
	// frame, so it would bounce/jitter forever. To fix that, the floor check also
	// "pins" a object to rest once its bounce is too small to see. That pin is what
	// lets a object (with drag > 0) actually come to a full stop -- it stands in for
	// the missing normal force. With drag == 0 the bounces never get small enough
	// to trip it, so it never fires.
	for (int i = 0; i < gameData.numObjects; i++)
	{
		auto& object = gameData.objects[i];
		float right = win_width - object.transform.w;
		float bottom = win_height - object.transform.h;

		object.applyGravity(gameData.grav);

		// Integrate first, resolve collisions after, so the rest-pin is the last
		// thing to touch the object before it's drawn (not undone by integration).
		object.updateForces(deltaTime);

		// A bounce at speed u rises u*u/(2g), so the speed that rises exactly
		// restRise is sqrt(2*g*restRise): below that a bounce is invisible (<~1px).
		//
		// The second term is NOT optional. A resting object is re-nudged by gravity
		// every frame by g*deltaTime, and the pin only fires when that nudge is
		// below the threshold. deltaTime spikes on slow/hitched frames (and is
		// clamped up to 0.05 above), so on a bad frame g*deltaTime can blow past a
		// fixed threshold and the object hops -- intermittent, "out of nowhere" jitter.
		// Flooring the threshold at 2*g*deltaTime makes it scale with the nudge, so
		// the residual is always caught no matter how slow the frame runs.
		float restRise = 1.0f; // px
		float restThreshold = fmaxf(sqrtf(2.0f * gameData.grav * restRise), 2.0f * gameData.grav * deltaTime);

		// Bounce off the walls: reflect across the edge (pos = 2*edge - pos) and
		// flip velocity; edge 0 makes 2*edge - pos == -pos. Clamping to the edge
		// instead would delete the overshoot and leak energy every bounce.
		if (object.transform.pos.y >= bottom && object.velocity.y > 0)
		{
			object.transform.pos.y = 2.0f * bottom - object.transform.pos.y;
			object.velocity.y *= -1.0f;
			// velocity.y is now negative (upward); -velocity.y is its speed.
			// Too slow to make a visible bounce -> pin to the floor and stop.
			if (-object.velocity.y < restThreshold) {
				object.transform.pos.y = bottom;
				object.velocity.y = 0.0f;
			}
		}
		if (object.transform.pos.y <= 0 && object.velocity.y < 0)
		{
			object.transform.pos.y = -object.transform.pos.y;
			object.velocity.y *= -1.0f;
		}
		if (object.transform.pos.x >= right && object.velocity.x > 0)
		{
			object.transform.pos.x = 2.0f * right - object.transform.pos.x;
			object.velocity.x *= -1.0f;
		}
		if (object.transform.pos.x <= 0 && object.velocity.x < 0)
		{
			object.transform.pos.x = -object.transform.pos.x;
			object.velocity.x *= -1.0f;
		}

		object.updateFinal();

		if (gameData.shape == Shape::SQUARE)
		{ 
			DrawRectangle(
				object.transform.pos.x,
				object.transform.pos.y,
				object.transform.w,
				object.transform.h,
				gameData.objectColors[i]
			);
		}

		else if (gameData.shape == Shape::CIRCLE)
		{
			float radius = object.transform.w * 0.5f;
			DrawCircle(object.transform.pos.x + radius, object.transform.pos.y + radius, radius, gameData.objectColors[i]);
		}
	}

	return true;
}

void closeGame()
{
	std::ofstream f(RESOURCES_PATH "f.txt");
	f << "CLOSED\n";
	f.close();
}
