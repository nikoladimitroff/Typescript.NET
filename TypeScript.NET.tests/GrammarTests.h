#pragma once
#include "gtest.h"

#define CLIENT_CODE
#include "Grammar.h"

using namespace std;

class GrammarTests : public ::testing::Test
{
protected:
	Grammar* grammar;
	GrammarTests()
	{
		map<string, vector<vector<string>>> rules = {
				{ "E", { { "T", "E'" } } },
				{ "E'", { { "+", "T", "E'" }, { EPSILON() } } },
				{ "T", { { "F", "T'" } } },
				{ "T'", { { "*", "F", "T'" }, { EPSILON() } } },
				{ "F", { { "(", "E", ")" }, { "id" } } }
		};

		this->grammar = new Grammar("E", rules, false);
	}
};

TEST_F(GrammarTests, First)
{
	map<string, set<string>> expected = 
	{
		{ "F", { "(", "id" } },
		{ "T", { "(", "id" } },
		{ "E", { "(", "id" } },
		{ "E'", { "+", EPSILON() } },
		{ "T'", { "*", EPSILON() } },
		{ "(", { "(" } },
		{ ")", { ")" } },
		{ "*", { "*" } },
		{ "+", { "+" } },
		{ "id", { "id" } },
		{ EPSILON(), { EPSILON() } },
	};

	EXPECT_EQ(expected, this->grammar->first);
}

TEST_F(GrammarTests, Follow)
{
	map<string, set<string>> expected =
	{
		{ "F", { "*", "+", ")", ENDMARKER() } },
		{ "T", { "+", ")", ENDMARKER() } },
		{ "T'", { "+", ")", ENDMARKER() } },
		{ "E", { ")", ENDMARKER() } },
		{ "E'", { ")", ENDMARKER() } },
	};

	EXPECT_EQ(expected, this->grammar->follow);
}

TEST(GrammarUnfixturedTests, Closure)
{
	map<string, vector<vector<string>>> rules = {
			{ "S", { { "C", "C" } } },
			{ "C", { { "c", "C" }, { "d" } } },
	};

	Grammar g("S", rules, true);

	Item i(AUGMENTED_START(), ENDMARKER(), 0, 0);
	i.DotIndex = 0;
	i.Lookahead = ENDMARKER();
	i.ProductionHead = AUGMENTED_START();
	i.RuleIndex = 0;
	set<Item> actual = { i };
	g.Closure(actual);

	set<Item> expected =
	{
		Item(AUGMENTED_START(), ENDMARKER(), 0, 0),
		Item("S", ENDMARKER(), 0, 0),
		Item("C", "c", 0, 0),
		Item("C", "d", 0, 0),
		Item("C", "c", 1, 0),
		Item("C", "d", 1, 0),
	};

	EXPECT_EQ(actual, expected);
}