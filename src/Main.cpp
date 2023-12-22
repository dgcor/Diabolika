#include "Game/Game.h"
#include "Game/Utils/CmdLineUtils.h"
#include "Game/Utils/FileUtils.h"
#include <iostream>
#include "RegisterHooks.h"
#include "Utils/Log.h"

int main(int argc, char* argv[])
{
	DGENGINE_INIT_LOGGING();
	SPDLOG_INFO("Welcome to Diabolika!");

	Hooks::registerHooks();

	FileUtils::initPhysFS(argv[0]);

	try
	{
		Game game;

		if (CmdLineUtils::processCmdLine(argc, (const char**)argv) == false)
		{
			if (argc == 2)
			{
				game.load(argv[1], "main.json");
			}
			else if (argc == 3)
			{
				game.load(argv[1], argv[2]);
			}
			else
			{
				game.load(".", "main.json");
			}
			game.play();
		}
	}
	catch (std::exception& ex)
	{
#if DGENGINE_HAS_LOGGING
		SPDLOG_ERROR(ex.what());
#else
		std::cerr << ex.what();
#endif
	}

	FileUtils::deinitPhysFS();

	SPDLOG_INFO("Goodbye!");

	return 0;
}
