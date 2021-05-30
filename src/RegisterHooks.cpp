#include "RegisterHooks.h"
#include "Hooks.h"
#include "Parser/ParseAction2.h"
#include "Parser/ParseFile2.h"

namespace Hooks
{
	void registerHooks()
	{
		Hooks::ParseDocumentElem = Parser2::parseDocumentElem;
		Hooks::ParseActionElem = Parser2::parseActionElem;
	}
}
