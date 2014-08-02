#pragma once
#include "Token.h"
#include <string>
#include <regex>
#include <vector>

class LexicalAnalyzer
{
private:
	std::vector<std::pair<std::regex, TokenTag>>& patternToTag;

	Token NextMatch(const std::string& text, int offset) const;

public:
	LexicalAnalyzer();

	std::vector<Token> Tokenize(const std::string& text) const;
};

