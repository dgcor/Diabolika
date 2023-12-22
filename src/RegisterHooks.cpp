#include "RegisterHooks.h"
#include "Hooks.h"
#include "Parser/ParseAction2.h"
#include "Parser/ParseFile2.h"

namespace Hooks
{
	void registerHooks()
	{
		ParseDocumentElem = Parser2::parseDocumentElem;
		ParseActionElem = Parser2::parseActionElem;
	}
}
