#include "LevelSave.h"
#include "Game/Utils/FileUtils.h"
#include "Json/JsonParser.h"
#include "Json/JsonUtils.h"
#include "Json/SaveUtils.h"
#include "Level.h"
#include "LevelSaveObject.h"
#include "Parser/Level/ParseLevelSave.h"

using namespace rapidjson;
using namespace SaveUtils;

bool LevelSave::load(Level& level, const std::string_view filePath)
{
	Document doc;
	if (JsonUtils::loadFile(filePath, doc) == true)
	{
		return load(level, Parser::parseLevelSaveObj(level, doc));
	}
	return false;
}

bool LevelSave::load(Level& level, const LevelSaveObject& levelState)
{
	level.gameState.state = levelState.state;

	level.gameState.level = levelState.level;
	level.gameState.score = levelState.score;
	level.gameState.chain = levelState.chain;

	level.gameState.units = levelState.units;
	level.gameState.detonators = levelState.detonators;

	level.gameState.selectedPosition = levelState.selectedPosition;

	for (const auto& pos : levelState.boardWalls)
	{
		level.addWall(pos);
	}

	for (const auto& unit : levelState.boardUnits)
	{
		level.addUnit(unit);
	}

	for (int16_t i = 0; (size_t)i < levelState.queues.size(); i++)
	{
		for (const auto& unit : levelState.queues[i])
		{
			level.unitManager.queueUnit(unit, i);
		}
	}

	level.unitManager.updateUnitQueues(level);
	level.options = level.optionsManager.get(level.gameState.level);

	for (const auto& link : levelState.links)
	{
		std::shared_ptr<Unit> unit;
		std::shared_ptr<Unit> unitToLink;

		if (link.first.x >= 0)
		{
			unit = level.board.get(link.first).unit;
		}
		else
		{
			unit = level.unitManager.getQueuedUnit(link.first);
		}

		if (link.second.x >= 0)
		{
			unitToLink = level.board.get(link.second).unit;
		}
		else
		{
			unitToLink = level.unitManager.getQueuedUnit(link.second);
		}

		if (unit != nullptr &&
			unitToLink != nullptr &&
			unit != unitToLink)
		{
			Unit::updateLink(unit, unitToLink);
		}
	}

	for (auto explosion : levelState.explosions)
	{
		if (level.board.isCoordValid(explosion) == true)
		{
			level.currentExplosions.push_back(explosion);
		}
	}

	return true;
}

void LevelSave::save(const Level& level, const std::string_view filePath)
{
	StringBuffer buffer(0, std::numeric_limits<uint16_t>::max());
	PrettyWriter<StringBuffer> writer(buffer);
	writer.SetIndent(' ', 2);

	serialize(level, &writer);

	FileUtils::saveText(filePath, { buffer.GetString(), buffer.GetSize() });
}

void LevelSave::serializeUnit(const Level& level, const Unit& unit, bool unitInBoard, void* serializeObj)
{
	auto& writer = *((PrettyWriter<StringBuffer>*)serializeObj);

	writeString(writer, "unit", unit.Class()->Id());

	if (unitInBoard == true)
	{
		writeVector2i(writer, "position", unit.BoardPosition());
		writeUInt(writer, "direction", unit.Direction());
	}

	if (auto link = unit.LinkedTo().lock())
	{
		auto& boardPos = link->BoardPosition();
		if (boardPos.x >= 0 && boardPos.y >= 0)
		{
			writeVector2i(writer, "linkedTo", boardPos);
		}
		else
		{
			auto queuePos = level.unitManager.getQueuedUnitPosition(link.get());
			writeVector2i(writer, "linkedTo", queuePos);
		}
	}
}

void LevelSave::serialize(const Level& level, void* serializeObj)
{
	auto& writer = *((PrettyWriter<StringBuffer>*)serializeObj);

	// root
	writer.StartObject();

	writeInt(writer, "state", (int32_t)level.gameState.state);
	writeUInt(writer, "level", level.gameState.level);
	writeUInt64(writer, "score", level.gameState.score);
	writeUInt(writer, "chain", level.gameState.chain);
	writeUInt(writer, "units", level.gameState.units);
	writeUInt(writer, "detonators", level.gameState.detonators);
	writeVector2i(writer, "selectedPosition", level.gameState.selectedPosition);

	// board
	writeKeyStringView(writer, "board");
	writer.StartArray();

	for (int16_t y = 0; y < level.board.Size().y; y++)
	{
		for (int16_t x = 0; x < level.board.Size().x; x++)
		{
			if (level.board.isCoordFree({ x, y }) == true)
			{
				continue;
			}

			const auto& cell = level.board.get(x, y);

			// wall or unit
			writer.StartObject();

			if (cell.isWall == true)
			{
				auto& writer = *((PrettyWriter<StringBuffer>*)serializeObj);

				writeBool(writer, "wall", true);
				writeVector2i(writer, "position", PairInt16{ x, y });
			}
			else
			{
				serializeUnit(level, *cell.unit, true, serializeObj);
			}

			// wall or unit
			writer.EndObject();
		}
	}

	// board
	writer.EndArray();

	// queues
	writeKeyStringView(writer, "queue");
	writer.StartArray();

	for (const auto& queue : level.unitManager)
	{
		// queue
		writer.StartArray();

		if (queue.unit != nullptr)
		{
			writer.StartObject();
			serializeUnit(level, *queue.unit, false, serializeObj);
			writer.EndObject();
		}
		else if (level.unitManager.hasSelectedUnit() == true)
		{
			writer.StartObject();
			serializeUnit(level, *level.unitManager.SelectedUnit(), false, serializeObj);
			writer.EndObject();
		}

		for (const auto& unit : queue.queue)
		{
			writer.StartObject();
			serializeUnit(level, *unit, false, serializeObj);
			writer.EndObject();
		}

		// queue
		writer.EndArray();
	}
	{
		// shared queue
		writer.StartArray();

		for (const auto& unit : level.unitManager.SharedUnitsQueue())
		{
			writer.StartObject();
			serializeUnit(level, *unit, false, serializeObj);
			writer.EndObject();
		}

		// shared queue
		writer.EndArray();
	}

	// queues
	writer.EndArray();

	if (level.gameState.state == LevelState::Exploding)
	{
		// explosions
		writeKeyStringView(writer, "explosions");
		writer.StartArray();

		for (const auto& explosion : level.currentExplosions)
		{
			writeVector2i(writer, explosion);
		}

		// explosions
		writer.EndArray();
	}

	// root
	writer.EndObject();
}
