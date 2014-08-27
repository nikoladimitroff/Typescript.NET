#pragma once
#include "Token.h"
#include "Grammar.h"
#include <map>
#include <memory>
#include <ostream>
#include <istream>

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
	Token Node;

	SyntaxTree() : Node("")
	{ }

	SyntaxTree(const Token token) : Node(token)
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
	Parser()
	{}

	Parser(const Grammar& grammar) : grammar(grammar)
	{
		this->ComputeActionTable();
	}

	void Save(std::ostream& stream) const;
	void Load(std::istream& stream);

	std::shared_ptr<SyntaxTree> Parse(std::vector<Token>& text) const;
	friend std::ostream& operator<<(std::ostream& o, const Parser& parser);
};

