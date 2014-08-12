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


	map<string, vector<vector<string>>> rules2 = {
			{ "S", { { "C", "C'" } } },
			{ "C", { { "c", "C" }, { "D" } } },
	};

	Grammar g("S", rules2, true);
	Item i;
	i.DotIndex = 0;
	i.Lookahead = ENDMARKER();
	i.ProductionHead = "AUGMENTED_START";
	i.RuleIndex = 0;
	set<Item> s = { i };
	g.Closure(s);
	for (auto& i : s)
	{
		vector<string> body = g.rules[i.ProductionHead][i.RuleIndex];
		vector<string> firstHalf(body.begin(), body.begin() + i.DotIndex);
		vector<string> secondHalf(body.begin() + i.DotIndex + 1, body.end());
		cout << i.ProductionHead << " -> "; 
		JoinCollection(secondHalf, JoinCollection(firstHalf, cout) << "@") << endl;
	}


	cout << endl << "Press any key to continue" << endl;
	cin.get();

	return 0;
}