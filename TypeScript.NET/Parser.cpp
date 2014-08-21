#include "Parser.h"
#include <stack>

using namespace std;


void Parser::ComputeActionTable()
{
	// see page 288 in the pdf (265 in the book)
	set<string> symbols = this->grammar.GetSymbols();

	for (int i = 0; i < this->grammar.items.size(); i++)
	{
		for (const string& symbol : symbols)
		{
			auto key = make_pair(i, symbol);
			ParsingAction action = this->ComputeAction(i, symbol);
			//if (action.Type != ParsingActionType::Error)
				this->actionTable[key] = action;
		}
	}
}

ParsingAction Parser::ComputeAction(int i, const string& symbol)
{
	ParsingAction action;
	for (const Item& item : this->grammar.items[i])
	{
		vector<string>& rule = this->grammar.rules[item.ProductionHead][item.RuleIndex];
		// Is this starting state?
		if (item.ProductionHead == AUGMENTED_START() && item.Lookahead == ENDMARKER() &&
			item.RuleIndex == 0 && item.DotIndex == 0)
			this->startState = i;

		// Reduce / accept
		if (item.DotIndex == rule.size())
		{
			if (item.Lookahead == symbol)
			{
				if (item.ProductionHead == AUGMENTED_START() && symbol == ENDMARKER())
				{
					if (action.Type != ParsingActionType::Error) throw std::invalid_argument("Grammar is not LR(1)");
					action.Type = ParsingActionType::Accept;
					continue;
				}
				if (action.Type != ParsingActionType::Error) throw std::invalid_argument("Grammar is not LALR(1)");
				action.Type = ParsingActionType::Reduce;
				action.ProductionHead = item.ProductionHead;
				action.RuleIndex = item.RuleIndex;
			}
			continue;
		}

		string& dotSymbol = rule[item.DotIndex];
		// Shift
		if (dotSymbol == symbol && this->grammar.terminals.find(dotSymbol) != this->grammar.terminals.end())
		{
			//if (action.Type == ParsingActionType::Error ) throw std::invalid_argument("Grammar is not LALR(1)");
			auto key = make_pair(i, symbol);
			action.Type = ParsingActionType::Shift;
			action.StateIndex = this->grammar.gotoTable[key];
		}
	}
	return action;
}

void Parser::Parse(vector<string>& text) const
{
	text.push_back(ENDMARKER());
	stack<int> states;
	states.push(this->startState);

	int tokenIndex = 0;
	while (true)
	{
		const string& token = text[tokenIndex];
		auto key = make_pair(states.top(), token);

		ParsingAction action = this->actionTable.at(key);

		if (action.Type == ParsingActionType::Shift)
		{
			states.push(action.StateIndex);
			++tokenIndex;
		}
		else if (action.Type == ParsingActionType::Reduce)
		{
			vector<string> ruleBody = this->grammar.rules.at(action.ProductionHead)[action.RuleIndex];
			for (int i = 0; i < ruleBody.size(); i++)
				states.pop();

			auto gotoKey = make_pair(states.top(), action.ProductionHead);
			states.push(this->grammar.gotoTable.at(gotoKey));
			cout << "REDUCED: " << action.ProductionHead << " -> ";
			JoinCollection(ruleBody, cout);
			cout << endl;
		}
		else if (action.Type == ParsingActionType::Accept)
		{
			break;
		}
		else
		{
			throw invalid_argument("Invalid syntax!");
		}
	}
	text.pop_back();
}


ostream& operator<<(ostream& o, const ParsingAction& action)
{
	switch (action.Type)
	{
	case ParsingActionType::Shift:
		o << "s" << action.StateIndex;
		break;
	case ParsingActionType::Reduce:
			o << "r" << action.ProductionHead << ", " << action.RuleIndex;
			break;
	case ParsingActionType::Accept:
			o << "Accept";
			break;
	default:
		break;
	}

	return o;
}

ostream& operator<<(ostream& o, const Parser& parser)
{
	for (auto& pair : parser.actionTable)
	{
		o << "<" << pair.first.first << ", " << pair.first.second << ">: " << pair.second << endl;
	}

	return o;
}