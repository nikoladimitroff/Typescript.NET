#pragma once
#include "Grammar.h"
#include <map>
#include <memory>

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

class SyntaxTree
{
private:
	void PrintTree(std::ostream& o, int indentation) const;

public:
	std::vector<std::shared_ptr<SyntaxTree>> Children;
	std::string Node;

	SyntaxTree() : Node("")
	{ }

	SyntaxTree(const std::string token) : Node(token)
	{ }

	void ReverseChildren();

	friend std::ostream& operator<<(std::ostream& o, const SyntaxTree& tree);
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

	std::shared_ptr<SyntaxTree> Parse(std::vector<std::string>& text) const;
	friend std::ostream& operator<<(std::ostream& o, const Parser& parser);
};

