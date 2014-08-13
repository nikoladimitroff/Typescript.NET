#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>

#undef CLIENT_CODE

#include "LexicalAnalyzer.h"
#include "Grammar.h"
#include "Utilities.h"

using namespace std;

vector<string> all_matches(const string& text, const regex& pattern)
{
	sregex_iterator it(text.begin(), text.end(), pattern),
		end;

	if (it == end)
		return{ "EOF" };


	vector<string> matches;
	for (; it != end; it++)
	{
		matches.push_back(it->str());
	}
	return matches;
}

vector<string> all_matches(const string& text, const string& pattern)
{
	return all_matches(text, regex(pattern));
}



string read_file_to_end(const string& path)
{
	fstream file(path);
	file.seekg(0, std::ios::end);
	int size = file.tellg();
	file.seekg(0);
	string text;
	text.reserve(size);

	text.assign((std::istreambuf_iterator<char>(file)),
				std::istreambuf_iterator<char>());

	return text;
}


int main()
{


	map<string, vector<vector<string>>> rules = {
			{ "S", { { "C", "C" } } },
			{ "C", { { "c", "C" }, { "d" } } },
	};

	Grammar g("S", rules, true);

	/*Item i(AUGMENTED_START(), ENDMARKER(), 0, 0);
	i.DotIndex = 0;
	i.Lookahead = ENDMARKER();
	i.ProductionHead = AUGMENTED_START();
	i.RuleIndex = 0;
	set<Item> actual = { i };
	g.Closure(actual);

	cout << "S:" << endl;
	PrintClosure(g.GoTo(actual, "S"), g);
	cout << "C:" << endl;
	PrintClosure(g.GoTo(actual, "C"), g);
	cout << "c:" << endl;
	PrintClosure(g.GoTo(actual, "c"), g);
	cout << "d:" << endl;
	PrintClosure(g.GoTo(actual, "d"), g);
*/

	g.ComputeItems();
	for (int i = 0; i < g.items.size(); i++)
	{
		cout << "I" << i << " -----------" << endl;
		PrintClosure(g.items[i] , g);
	}

	cout << endl << "Press any key to continue" << endl;
	cin.get();

	return 0;
}