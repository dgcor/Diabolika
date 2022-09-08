#pragma once

#include "Game/Action.h"
#include "Game/Game.h"
#include "Game/Level/Level.h"
#include "Game/Utils/GameUtils.h"

class ActLevelAddHighScore : public Action
{
private:
	std::string id;
	LevelScore levelScore;

public:
	ActLevelAddHighScore(const std::string_view id_, const std::string_view name,
		uint32_t level, uint64_t score) : id(id_)
	{
		levelScore.name = name;
		levelScore.level = level;
		levelScore.score = score;
	}

	bool execute(Game& game) override
	{
		auto level = game.Resources().getLevel<Level>(id);
		if (level != nullptr)
		{
			level->HighScores().add(levelScore);
		}
		return true;
	}
};

class ActLevelLoadGame : public Action
{
private:
	std::string id;
	std::string file;

public:
	ActLevelLoadGame(const std::string_view id_, const std::string_view file_)
		: id(id_), file(file_) {}

	bool execute(Game& game) noexcept override
	{
		auto level = game.Resources().getLevel<Level>(id);
		if (level != nullptr)
		{
			level->loadGame(game, GameUtils::replaceStringWithVarOrProp(file, game));
		}
		return true;
	}
};

class ActLevelMakeMove : public Action
{
private:
	std::string id;

public:
	ActLevelMakeMove(const std::string_view id_) : id(id_) {}

	bool execute(Game& game) override
	{
		auto level = game.Resources().getLevel<Level>(id);
		if (level != nullptr)
		{
			level->makeMove(game);
		}
		return true;
	}
};

class ActLevelMoveSelectedTile : public Action
{
private:
	std::string id;
	PairInt16 offset;

public:
	ActLevelMoveSelectedTile(const std::string_view id_, PairInt16 offset_)
		: id(id_), offset(offset_) {}

	bool execute(Game& game) override
	{
		auto level = game.Resources().getLevel<Level>(id);
		if (level != nullptr)
		{
			level->moveSelectedTile(game, offset);
		}
		return true;
	}
};

class ActLevelNewGame : public Action
{
private:
	std::string id;

public:
	ActLevelNewGame(const std::string_view id_) : id(id_) {}

	bool execute(Game& game) override
	{
		auto level = game.Resources().getLevel<Level>(id);
		if (level != nullptr)
		{
			level->newGame(game);
		}
		return true;
	}
};

class ActLevelPause : public Action
{
private:
	std::string id;
	bool pause;

public:
	ActLevelPause(const std::string_view id_, bool pause_) : id(id_), pause(pause_) {}

	bool execute(Game& game) noexcept override
	{
		auto level = game.Resources().getLevel<Level>(id);
		if (level != nullptr)
		{
			level->Pause(pause);
		}
		return true;
	}
};

class ActLevelQueueUnit : public Action
{
private:
	std::string id;
	std::string unit;

public:
	ActLevelQueueUnit(const std::string_view id_, const std::string_view unit_)
		: id(id_), unit(unit_) {}

	bool execute(Game& game) override
	{
		auto level = game.Resources().getLevel<Level>(id);
		if (level != nullptr)
		{
			level->queueUnit(unit);
		}
		return true;
	}
};

class ActLevelRotateSelectedUnit : public Action
{
private:
	std::string id;

public:
	ActLevelRotateSelectedUnit(const std::string_view id_) : id(id_) {}

	bool execute(Game& game) override
	{
		auto level = game.Resources().getLevel<Level>(id);
		if (level != nullptr)
		{
			level->rotateSelectedUnit();
		}
		return true;
	}
};

class ActLevelSaveGame : public Action
{
private:
	std::string id;
	std::string file;

public:
	ActLevelSaveGame(const std::string_view id_, const std::string_view file_)
		: id(id_), file(file_) {}

	bool execute(Game& game) noexcept override
	{
		auto level = game.Resources().getLevel<Level>(id);
		if (level != nullptr)
		{
			level->saveGame(GameUtils::replaceStringWithVarOrProp(file, game));
		}
		return true;
	}
};

class ActLevelSelectTile : public Action
{
private:
	std::string id;
	PairInt16 position;

public:
	ActLevelSelectTile(const std::string_view id_, PairInt16 position_)
		: id(id_), position(position_) {}

	bool execute(Game& game) override
	{
		auto level = game.Resources().getLevel<Level>(id);
		if (level != nullptr)
		{
			level->selectTile(game, position);
		}
		return true;
	}
};

class ActLevelSelectUnitQueue : public Action
{
private:
	std::string id;
	int16_t queue;

public:
	ActLevelSelectUnitQueue(const std::string_view id_, int16_t queue_)
		: id(id_), queue(queue_) {}

	bool execute(Game& game) override
	{
		auto level = game.Resources().getLevel<Level>(id);
		if (level != nullptr)
		{
			level->selectUnitQueue(queue);
		}
		return true;
	}
};

class ActLevelSetShader : public Action
{
private:
	std::string id;
	std::string idShader;

public:
	ActLevelSetShader(const std::string_view id_, const std::string_view idShader_)
		: id(id_), idShader(idShader_) {}

	bool execute(Game& game) override
	{
		auto level = game.Resources().getLevel<Level>(id);
		if (level != nullptr)
		{
			auto shader = game.Resources().Shaders().get(idShader);
			level->setShader(shader);
		}
		return true;
	}
};
