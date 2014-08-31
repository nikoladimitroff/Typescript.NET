#pragma once
#include <fstream>
#include <istream>
#include "Grammar.h"
#include "Parser.h"

namespace Translators
{
	class TsToCSharp
	{
	private:
		Grammar grammar;
		Parser parser;
		static std::string ParserDataFile;

		static bool ParserDataExists();
	public:
		TsToCSharp();
		std::string Translate(std::string source);
	};

}