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

	EXPECT_EQ(expected, actual);
}


TEST(GrammarUnfixturedTests, Items)
{
	map<string, vector<vector<string>>> rules = {
			{ "S", { { "C", "C" } } },
			{ "C", { { "c", "C" }, { "d" } } },
	};

	Grammar g("S", rules, true);
	g.ComputeItems();

	set<set<Item>> expected =
	{
		{
			Item(AUGMENTED_START(), ENDMARKER(), 0, 0),
			Item("S", ENDMARKER(), 0, 0),
			Item("C", "c", 0, 0),
			Item("C", "d", 0, 0),
			Item("C", "c", 1, 0),
			Item("C", "d", 1, 0),
		},
		{
			Item(AUGMENTED_START(), ENDMARKER(), 0, 1)
		},
		{
			Item("S", ENDMARKER(), 0, 1),
			Item("C", ENDMARKER(), 0, 0),
			Item("C", ENDMARKER(), 1, 0),
		},
		{ 
			Item("C", "c", 0, 0),
			Item("C", "c", 0, 1),
			Item("C", "c", 1, 0),
			Item("C", "d", 0, 0),
			Item("C", "d", 0, 1),
			Item("C", "d", 1, 0),
		},
		{
			Item("C", "c", 1, 1),
			Item("C", "d", 1, 1),
		},
		{
			Item("S", ENDMARKER(), 0, 2),
		},
		{
			Item("C", ENDMARKER(), 0, 0),
			Item("C", ENDMARKER(), 0, 1),
			Item("C", ENDMARKER(), 1, 0),
		},
		{
			Item("C", ENDMARKER(), 1, 1),
		},
		{
			Item("C", "c", 0, 2),
			Item("C", "d", 0, 2),
		},
		{
			Item("C", ENDMARKER(), 0, 2),
		},
	};
	set<set<Item>> actual;
	for (const auto& itemSet : g.items)
	{
		actual.insert(itemSet);
	}
	EXPECT_EQ(expected, actual);
}


TEST(GrammarUnfixturedTests, GotoTable)
{
	map<string, vector<vector<string>>> rules = {
			{ "S", { { "C", "C" } } },
			{ "C", { { "c", "C" }, { "d" } } },
	};

	Grammar g("S", rules, true);
	g.ComputeItems();

	map<pair<int, string>, int> expected =
	{
		{ make_pair(0, "C"), 1 },
		{ make_pair(0, "S"), 2 },
		{ make_pair(0, "c"), 3 },
		{ make_pair(0, "d"), 4 },

		{ make_pair(1, "C"), 5 },
		{ make_pair(1, "c"), 6 },
		{ make_pair(1, "d"), 7 },

		// empty for 2

		{ make_pair(3, "C"), 8 },
		{ make_pair(3, "c"), 3 },
		{ make_pair(3, "d"), 4 },
		
		// empty for 4 and 5

		{ make_pair(6, "C"), 9 },
		{ make_pair(6, "c"), 6 },
		{ make_pair(6, "d"), 7 },
		// the rest are empty
	};

	EXPECT_EQ(expected, g.gotoTable);
}

