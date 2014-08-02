#include "LexicalAnalyzer.h"

using namespace std;

namespace
{
	std::string ws = R"(( |\n)+)",
		comment = R"(//.*?\n|/\*.*?\*/)",
		number = R"((-)?\d+(\.\d+)?)",
		stringLiteral = R"(("|').*?\1)",
		relOp = R"(==|!=|<=|>=|>|<)",
		boolOp = R"(&&|\|\||!)",
		specials = R"(,|:|;|\(|\)|\{|\})",
		assignment = R"(=)",
		id = R"([A-Za-z]\w*)",
		keyword = R"(var|if|else if|else|for|while|module|class|interface)";

	typedef std::vector<std::pair<std::regex, TokenTag>> vec;
	vec patternMap =
	{
		make_pair(regex(keyword), TokenTag::Keyword),
		make_pair(regex(id), TokenTag::Id),
		make_pair(regex(number), TokenTag::Number),
		make_pair(regex(stringLiteral), TokenTag::StringLiteral),
		make_pair(regex(relOp), TokenTag::RelativeOp),
		make_pair(regex(boolOp), TokenTag::BoolOp),
		make_pair(regex(specials), TokenTag::SpecialCharacter),
		make_pair(regex(assignment), TokenTag::Assignment),
		make_pair(regex(comment), TokenTag::Comment),
		make_pair(regex(ws), TokenTag::Whitespace),
	};
}

LexicalAnalyzer::LexicalAnalyzer() : patternToTag(patternMap)
{ }


Token LexicalAnalyzer::NextMatch(const string& text, int offset) const
{
	for (auto pair : this->patternToTag)
	{
		regex& pattern = pair.first;
		TokenTag tag = pair.second;

		smatch match;
		regex_search(text.begin() + offset, text.end(), match, pattern);
		if (!match.empty() && match.prefix().length() == 0)
		{
			return Token(match[0], tag);
		}
	}

	return Token();
}

vector<Token> LexicalAnalyzer::Tokenize(const string& text) const
{
	vector<Token> tokens;
	Token t;
	int offset = 0;
	do
	{
		t = this->NextMatch(text, offset);
		tokens.push_back(t);
		offset += t.GetLexeme().length();
	} while (t.GetTag() != TokenTag::EndOfFile);

	return tokens;
}