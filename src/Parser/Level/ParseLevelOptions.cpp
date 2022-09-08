#include "ParseLevel.h"
#include "Game/Level/LevelOptionsManager.h"
#include "Parser/Utils/ParseUtils.h"

namespace Parser
{
	using namespace rapidjson;
	using namespace std::literals;

	void parseLevelOptions(LevelOptionsManager& optionsManager, const Value& elem)
	{
		LevelOptions options;
		std::vector<LevelOptions> allOptions;
		for (const auto& val : elem)
		{
			options.level = getUIntKey(val, "level", options.level + 1);
			options.numUnits = (uint16_t)getUIntKey(val, "units", options.numUnits);
			options.numDetonators = (uint16_t)getUIntKey(val, "exploders", options.numDetonators);
			options.maxDemons = (uint16_t)getUIntKey(val, "maxDemons", options.maxDemons);

			if (isValidArray(val, "insertUnits") == true)
			{
				options.insertUnits.clear();
				for (const auto& val2 : val["insertUnits"sv])
				{
					auto id = getStringKey(val2, "id");
					if (id.empty() == true)
					{
						continue;
					}
					auto count = getStringKey(val2, "count");
					options.insertUnits.push_back(std::make_pair(id, Formula(count)));
				}
			}
			if (isValidArray(val, "queueUnits") == true)
			{
				options.queueUnits.clear();
				for (const auto& val2 : val["queueUnits"sv])
				{
					auto id = getStringVal(val2);
					if (id.empty() == true)
					{
						continue;
					}
					options.queueUnits.push_back(id);
				}
			}
			if (isValidArray(val, "maxUnitsById") == true)
			{
				options.maxUnitsById.clear();
				for (const auto& val2 : val["maxUnitsById"sv])
				{
					auto id = getStringKey(val2, "id");
					if (id.empty() == true)
					{
						continue;
					}
					options.maxUnitsById[id] = (uint16_t)getUIntKey(val2, "count");
				}
			}
			if (isValidArray(val, "maxUnitsByType") == true)
			{
				options.maxUnitsByType.clear();
				for (const auto& val2 : val["maxUnitsByType"sv])
				{
					auto type = getStringKey(val2, "type");
					if (type.empty() == true)
					{
						continue;
					}
					options.maxUnitsByType[type] = (uint16_t)getUIntKey(val2, "count");
				}
			}
			allOptions.push_back(options);
		}
		optionsManager.set(allOptions);
	}
}
