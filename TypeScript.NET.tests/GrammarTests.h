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