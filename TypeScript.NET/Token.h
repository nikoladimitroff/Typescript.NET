#pragma once
#include <string>
#include <regex>
#include <vector>

enum class TokenTag
{
	Id,
	Keyword,
	Whitespace,
	Comment,
	Number,
	StringLiteral,
	Assignment,
	RelativeOp,
	BoolOp,
	SpecialCharacter,
	EndOfFile
};

namespace
{
	const std::string tagLabels[] = {
		"Id",
		"Keyword",
		"Whitespace",
		"Comment",
		"Number",
		"StringLiteral",
		"Assignment",
		"RelativeOp",
		"BoolOp",
		"SpecialCharacter",
		"EndOfFile"
	};
}

class Token
{
private:
	std::string lexeme;
	TokenTag tag;

public:
	Token() : lexeme(""), tag(TokenTag::EndOfFile)
	{}

	Token(const std::string lexeme, TokenTag tag) : lexeme(lexeme), tag(tag)
	{}
	const std::string& GetLexeme() const
	{
		return this->lexeme;
	}

	const TokenTag GetTag() const
	{
		return this->tag;
	}
};


inline std::ostream&  operator<<(std::ostream& o, const Token& token)
{
	o << "<" << tagLabels[static_cast<int>(token.GetTag())] << ", " << token.GetLexeme() << ">";
	return o;
}
