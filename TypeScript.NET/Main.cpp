#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>

#include "LexicalAnalyzer.h"
#include "ParserHelper.h"

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
	// Lexical Analysis

	/*string filePath = "Sample.ts";

	LexicalAnalyzer lex;
	auto tokens = lex.Tokenize(read_file_to_end(filePath));
	for (Token t : tokens)
	{
		cout << t << endl;
	}*/


	// First
	map<string, vector<vector<string>>> rules = {
			{ "E", { { "T", "E'" } } },
			{ "E'", { { "+", "T", "E'" }, { EPSILON() } } },
			{ "T", { { "F", "T'" } } },
			{ "T'", { { "*", "F", "T'" }, { EPSILON() } } },
			{ "F", { { "(", "E", ")" }, { "id" } } }
	};

	Grammar g(rules);
	cout << g;


	cout << endl << "Press any key to continue" << endl;
	cin.get();

	return 0;
}