#pragma once
#include <string>
#include <regex>
#include <vector>

#include "Utilities.h"

enum class dllspec TokenTag
{
	KeywordVar,
	KeywordIf,
	KeywordElseIf,
	KeywordElse,
	KeywordFor,
	KeywordWhile,
	KeywordContinue,
	KeywordSwitch,
	KeywordCase,
	KeywordDefault,
	KeywordBreak,
	KeywordReturn,
	KeywordModule,
	KeywordClass,
	KeywordInterface,

	Id,

	LeftParenthesis,
	RightParenthesis,
	LeftSquareBracket,
	RightSquareBracket,
	LeftBrace,
	RightBrace,

	Dot,
	Comma,
	Colon,
	Semicolon,

	UnaryOp,
	BinaryOp,

	Whitespace,
	Comment,
	Number,
	StringLiteral,
	Assignment,
	RelativeOp,
	BoolOp,
	EndOfFile
};

namespace
{
	const std::string tagLabels[] = {
		"var",
		"if",
		"else if",
		"else",
		"for",
		"while",
		"continue",
		"switch",
		"case",
		"default",
		"break",
		"return",
		"module",
		"class",
		"interface",

		"id",

		"(",
		")",
		"[",
		"]",
		"{",
		"}",

		".",
		",",
		":",
		";",
		
		"UnaryOp",
		"BinaryOp",
		
		"Whitespace",
		"Comment",
		"Number",
		"StringLiteral",
		"=",
		"RelativeOp",
		"BoolOp",
		"EndOfFile"
	};
}

class dllspec Token
{
private:
	std::string lexeme;
	TokenTag tag;

public:
	Token() : lexeme(""), tag(TokenTag::EndOfFile)
	{}

	Token(const std::string lexeme) : lexeme(lexeme), tag(TokenTag::EndOfFile)
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

	const std::string& GetTerminal() const
	{
		return tagLabels[static_cast<int>(this->tag)];
	}
};


dllspec inline std::ostream&  operator<<(std::ostream& o, const Token& token)
{
	o << token.GetLexeme();
	//o << tagLabels[static_cast<int>(token.GetTag())];
	//o << "<" << tagLabels[static_cast<int>(token.GetTag())] << ", " << token.GetLexeme() << ">";
	return o;
}

dllspec inline bool operator==(const Token& lhs, const Token& rhs)
{
	return  lhs.GetTag() == rhs.GetTag() && lhs.GetLexeme() == rhs.GetLexeme();
}

dllspec inline bool operator!=(const Token& lhs, const Token& rhs)
{
	return !(lhs == rhs);
}
