#pragma once
#include <string>
#include <regex>
#include <vector>

#include "Utilities.h"

enum class dllspec TokenTag
{
	Nonterminal,

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
	KeywordPrivate,
	KeywordPublic,
	KeywordExtends,
	KeywordImplements,

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
	BinaryAssignment,

	Whitespace,
	Comment,
	Number,
	BoolLiteral,
	StringLiteral,
	Assignment,
	AugmentedStart,
	Endmarker
};

namespace
{
	const std::string tagLabels[] = {
		"Nonterminal",
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
		"private",
		"public",
		"extends",
		"implements",

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
		
		"UNARY_OP",
		"BINARY_OP",
		"BINARY_ASSIGNMENT",
		
		"WHITESPACE",
		"COMMENT",
		"NUMBER",
		"BOOL_LITERAL",
		"STRING_LITERAL",
		"=",
		"AUGMENTED_START",
		"ENDMARKER",
	};
}

class dllspec Token
{
private:
	std::string lexeme;
	TokenTag tag;

public:
	Token() : lexeme(""), tag(TokenTag::Nonterminal)
	{}

	Token(const std::string lexeme) : lexeme(lexeme), tag(TokenTag::Nonterminal)
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

	const std::string& GetSymbol() const
	{
		return tagLabels[static_cast<int>(this->tag)];
	}
};


dllspec inline std::ostream&  operator<<(std::ostream& o, const Token& token)
{
	o << token.GetSymbol();
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
