#pragma once
#include <string>
#include <regex>
#include <vector>

#include "Token.h"
#include "Utilities.h"


class LexicalAnalyzer
{
private:
	std::vector<std::pair<std::regex, TokenTag>> patternToTag;

	Token NextMatch(const std::string& text, int offset) const;
public:
	dllspec LexicalAnalyzer();

	dllspec std::vector<Token> Tokenize(const std::string& text) const;
	dllspec std::vector<Token> Tokenize(const std::string& text, bool skipWhitespaceComments) const;
};
