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
	public:
		TsToCSharp();
		TsToCSharp(bool load);
		std::string Translate(std::string source);
	};

}