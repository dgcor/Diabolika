#include "ParseFile2.h"
#include "Parser/ParseFile.h"
#include "Parser/Level/ParseLevel.h"
#include "Parser/Unit/ParseUnit.h"
#include "Parser/Unit/ParseUnitClass.h"
#include "Utils/StringHash.h"

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
			parseDocumentElemArray(parseLevel, game, nameHash16, elem, replaceVars, allocator);
			break;
		}
		case str2int16("unit"):
		{
			parseDocumentElemArray(parseUnit, game, nameHash16, elem, replaceVars, allocator);
			break;
		}
		case str2int16("unitClass"):
		{
			parseDocumentElemArray(parseUnitClass, game, nameHash16, elem, replaceVars, allocator);
			break;
		}
		default:
			return false;
		}
		return true;
	}
}
