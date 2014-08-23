#pragma once
#include "gtest.h"

#define CLIENT_CODE
#include "LexicalAnalyzer.h"

using namespace std;

class LexerTests : public ::testing::Test
{
protected:
	typedef Token T;
	typedef TokenTag Tag;
	LexicalAnalyzer lex;
};

TEST_F(LexerTests, ForHeader)
{
	string forHeader = "for (var i : number = 0; i < 10; i++) {";
	vector<Token> expected =
	{
		T("for", Tag::KeywordFor), T("(", Tag::LeftParenthesis), T("var", Tag::KeywordVar), T("i", Tag::Id),
		T(":", Tag::Colon), T("number", Tag::Id), T("=", Tag::Assignment), T("0", Tag::Number), T(";", Tag::Semicolon),
		T("i", Tag::Id), T("<", Tag::RelativeOp), T("10", Tag::Number), T(";", Tag::Semicolon), T("i", Tag::Id),
		T("++", Tag::UnaryOp), T(")", Tag::RightParenthesis), T("{", Tag::LeftBrace),
	};
	vector<Token> actual = this->lex.Tokenize(forHeader, true);
	EXPECT_EQ(expected, actual);
}

TEST_F(LexerTests, Switch)
{
	string switchCode = "switch (x) { case 10: alert(\"Hello, world!\"); break; }";
	vector<Token> expected = 
	{
		T("switch", Tag::KeywordSwitch), T("(", Tag::LeftParenthesis), T("x", Tag::Id), T(")", Tag::RightParenthesis),
		T("{", Tag::LeftBrace), T("case", Tag::KeywordCase), T("10", Tag::Number), T(":", Tag::Colon),
		T("alert", Tag::Id), T("(", Tag::LeftParenthesis), T("\"Hello, world!\"", Tag::StringLiteral),
		T(")", Tag::RightParenthesis), T(";", Tag::Semicolon), T("break", Tag::KeywordBreak),
		T(";", Tag::Semicolon), T("}", Tag::RightBrace)
	};
	vector<Token> actual = this->lex.Tokenize(switchCode, true);
	EXPECT_EQ(expected, actual);
}