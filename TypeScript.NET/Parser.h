#pragma once
#include "Grammar.h"
#include <map>

enum class ParsingActionType
{
	Error,
	Shift,
	Reduce,
	Accept,
};

struct ParsingAction
{
	ParsingActionType Type;
	int StateIndex;
	int RuleIndex;
	std::string ProductionHead;

	ParsingAction() : Type(ParsingActionType::Error), StateIndex(-1), RuleIndex(-1)
	{ }

	friend std::ostream& operator<<(std::ostream& o, const ParsingAction& parser);
};

class Parser
{
private:

	int startState;
	Grammar grammar;
	std::map<std::pair<int, std::string>, ParsingAction> actionTable;
	

	ParsingAction ComputeAction(int i, const std::string& symbol);
	void ComputeActionTable(void);

public:
	Parser(const Grammar& grammar) : grammar(grammar)
	{
		this->ComputeActionTable();
	}

	void Parse(std::vector<std::string>& text) const;
	friend std::ostream& operator<<(std::ostream& o, const Parser& parser);
};

