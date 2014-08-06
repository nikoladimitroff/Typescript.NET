#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <ostream>

struct Rule
{
	std::string Lhs;
	std::vector<std::string> Rhs;
};

struct Item
{
	int RuleIndex,
		DotIndex;
};


const std::string& EPSILON();
const std::string& ENDMARKER();

class Grammar
{
private:
	typedef std::vector<std::string> Rule;
	typedef std::vector<Rule> RuleList;

	std::map<std::string, RuleList> rules;
	std::set<std::string> terminals;
	std::set<std::string> nonterminals;
	std::string startSymbol;


	int ComputeFirstStep(std::string symbol);
	void ComputeFirst();
	int ComputeFollowStep(std::string symbol);
	void ComputeFollow();
public:

	std::map<std::string, std::set<std::string>> first;
	std::map<std::string, std::set<std::string>> follow;

	Grammar(std::map<std::string, RuleList> RuleList);
	std::set<std::string> ComputeFirstWord(const std::vector<std::string>& word);
	friend std::ostream& operator<<(std::ostream& stream, const Grammar& grammar);
};