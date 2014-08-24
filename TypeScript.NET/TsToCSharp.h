#pragma once
#include "Grammar.h"
namespace Translators
{
	class TsToCSharp
	{
	private:
		Grammar grammar;
	public:
		TsToCSharp();
		std::string Translate(std::string source);
	};

}