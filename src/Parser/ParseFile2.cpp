#include "ParseFile2.h"
#include "Parser/ParseFile.h"
#include "Parser/ParseLevel.h"
#include "Parser/ParseUnit.h"
#include "Parser/ParseUnitClass.h"
#include "Utils/Utils.h"

namespace Parser2
{
	using namespace Parser;
	using namespace rapidjson;

	bool parseDocumentElem(Game& game, uint16_t nameHash16, const Value& elem,
		ReplaceVars& replaceVars, MemoryPoolAllocator<CrtAllocator>& allocator)
	{
		switch (nameHash16)
		{
		case str2int16("level"):
		{
			if (elem.IsArray() == false)
			{
				parseLevel(game, elem);
			}
			else
			{
				for (const auto& val : elem)
				{
					parseDocumentElemHelper(game, nameHash16, val, replaceVars, allocator);
				}
			}
			break;
		}
		case str2int16("unit"):
		{
			if (elem.IsArray() == false)
			{
				parseUnit(game, elem);
			}
			else
			{
				for (const auto& val : elem)
				{
					parseDocumentElemHelper(game, nameHash16, val, replaceVars, allocator);
				}
			}
			break;
		}
		case str2int16("unitClass"):
		{
			if (elem.IsArray() == false)
			{
				parseUnitClass(game, elem);
			}
			else
			{
				for (const auto& val : elem)
				{
					parseDocumentElemHelper(game, nameHash16, val, replaceVars, allocator);
				}
			}
			break;
		}
		default:
			return false;
		}
		return true;
	}
}
