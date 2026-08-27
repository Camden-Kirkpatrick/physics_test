#include <iostream>
#include <raylib.h>
#include "gameMain.hpp"



int main()
{
#if PRODUCTION_BUILD == 1
	SetTraceLogLevel(LOG_NONE); // Don't open the console for the production build
#endif

	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(win_width, win_height, "game");
	SetTargetFPS(FPS);


	if (!initGame())
	{
		return 1;
	}

	while (!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(BLACK);

		if (!updateGame())
		{
			CloseWindow();
		}

		EndDrawing();
	}

	CloseWindow();

	closeGame();

	return 0;
}