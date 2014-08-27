#include <algorithm>
#include <sstream>
#include <iterator>
#include <functional>

#include "Grammar.h"
#include "Utilities.h"


using namespace std;


Item::Item(const string& productionHead, const string& lookahead, const int ruleIndex, const int dotIndex)
{
	this->ProductionHead = productionHead;
	this->Lookahead = lookahead;
	this->RuleIndex = ruleIndex;
	this->DotIndex = dotIndex;
}


bool operator<(const Item& first, const Item& second)
{
	if (first.ProductionHead != second.ProductionHead) return first.ProductionHead < second.ProductionHead;
	if (first.Lookahead != second.Lookahead) return first.Lookahead < second.Lookahead;
	if (first.RuleIndex != second.RuleIndex) return first.RuleIndex < second.RuleIndex;
	return first.DotIndex < second.DotIndex;
}

bool operator==(const Item& first, const Item& second)
{
	return first.ProductionHead == second.ProductionHead &&
		first.Lookahead == second.Lookahead &&
		first.RuleIndex == second.RuleIndex &&
		first.DotIndex == second.DotIndex;
}

bool operator!=(const Item& first, const Item& second)
{
	return !(first == second);
}


inline const string EPSILON()
{
	const std::string epsilon = "EPSILON";
	return epsilon;
}

inline const string ENDMARKER()
{
	const std::string endmarker = "ENDMARKER";
	return endmarker;
}

inline const string AUGMENTED_START()
{
	const std::string endmarker = "AUGMENTED_START";
	return endmarker;
}


Grammar::Grammar(string start, map<string, RuleList> rules, bool shouldAugment)
{
	this->startSymbol = start;
	this->rules = rules;

	set<string> symbols;

	for (auto& pair : this->rules)
	{
		this->nonterminals.insert(pair.first);
		symbols.insert(pair.first);

		for (auto& rhs : pair.second)
		{
			for (auto& rhsSymbol : rhs)
			{
				symbols.insert(rhsSymbol);
			}
		}
	}
	set_difference(symbols.begin(), symbols.end(),
		this->nonterminals.begin(), this->nonterminals.end(),
		inserter(this->terminals, terminals.begin()));


	if (shouldAugment)
	{
		auto& augmented = AUGMENTED_START();
		vector<string> rule = { this->startSymbol };
		this->rules[augmented] = { rule };
		this->nonterminals.insert(augmented);
		this->terminals.insert(ENDMARKER());
		this->startSymbol = augmented;
	}

	this->ComputeFirst();
	this->ComputeFollow();
	this->ComputeLR1Items();
}


int Grammar::ComputeFirstStep(string symbol)
{
	RuleList& symbolRules = this->rules[symbol];
	if (this->first.find(symbol) == this->first.end())
	{
		auto equalsEpsilon = [&symbol](const vector<string>& rhs) -> bool {
			return rhs.size() == 1 && rhs[0] == EPSILON();
		};
		this->first[symbol] = set<string>();
		if (any_of(symbolRules.begin(), symbolRules.end(), equalsEpsilon))
		{
			this->first[symbol].insert(EPSILON());
		}
		return 1;
	}
	int currentSize = this->first[symbol].size();
	for (auto& rhs : symbolRules)
	{
		int index = 0;
		string current = rhs[index];
		this->first[symbol].insert(this->first[current].begin(), first[current].end());

		while (first[current].find(EPSILON()) != first[current].end() && index < rhs.size() - 1)
		{
			string& next = rhs[index++];
			first[symbol].insert(first[next].begin(), first[next].end());
			current = next;
		}
		if (index == rhs.size() - 1 && first[current].find(EPSILON()) != first[current].end())
			first[symbol].insert(first[current].begin(), first[current].end());
		else
		{
			// If we are here, some symbol on the rhs doesn't derive Epsilon, thus remove Epsilon if
			// it has been added by another symbol
			auto iter = first[symbol].find(EPSILON());
			if (iter != first[symbol].end())
				first[symbol].erase(iter);
		}
	}
	return first[symbol].size() - currentSize;
}

void Grammar::ComputeFirst()
{
	for (auto& terminal : this->terminals)
	{
		this->first[terminal] = { terminal };
	}

	int added = 0;
	do
	{
		added = 0;
		for (auto& nonterminal : this->nonterminals)
		{
			added += this->ComputeFirstStep(nonterminal);
		}
	} while (added != 0);
}

ostream& operator<<(ostream& stream, const Grammar& g)
{
	stream << "FIRST: " << endl;
	for (auto pair : g.first)
	{
		stream << pair.first << " -> ";
		JoinCollection(pair.second, stream) << endl;
	}

	stream << "FOLLOW: " << endl;
	for (auto pair : g.follow)
	{
		stream << pair.first << " -> ";
		JoinCollection(pair.second, stream) << endl;
	}

	return stream;
}


set<string> Grammar::ComputeFirstWord(const vector<string>& word)
{
	if (word.size() == 0) return set<string>();

	set<string> wordFirst;
	int index = 0;
	string current = word[index];
	wordFirst.insert(this->first[current].begin(), first[current].end());

	while (this->first[current].find(EPSILON()) != this->first[current].end() && index < word.size() - 1)
	{
		const string& next = word[index++];
		wordFirst.insert(this->first[next].begin(), this->first[next].end());
		current = next;
	}
	if (index == word.size() - 1 && this->first[current].find(EPSILON()) != this->first[current].end())
		wordFirst.insert(this->first[current].begin(), this->first[current].end());
	else
	{
		// If we are here, some symbol on the rhs doesn't derive Epsilon, thus remove Epsilon if
		// it has been added by another symbol
		auto iter = wordFirst.find(EPSILON());
		if (iter != wordFirst.end())
			wordFirst.erase(iter);
	}
	return wordFirst;
}


int Grammar::ComputeFollowStep(std::string symbol)
{
	RuleList& symbolRules = this->rules[symbol];
	if (this->follow.find(symbol) == this->follow.end())
	{
		this->follow[symbol] = set<string>();
		return 1;
	}
	int added = 0;
	for (auto& rhs : symbolRules)
	{
		for (auto it = rhs.begin(); it != rhs.end(); ++it)
		{
			if (this->nonterminals.find(*it) != this->nonterminals.end())
			{
				int currentSize = this->follow[*it].size();
				bool isLast = it + 1 == rhs.end();
				vector<string> beta(it + 1, rhs.end());
				auto firstBeta = this->ComputeFirstWord(beta);
				// Case 2
				if (!isLast)
				{
					this->follow[*it].insert(firstBeta.begin(), firstBeta.end());
				}

				// Case 3
				bool followedByEpsilon = firstBeta.find(EPSILON()) != firstBeta.end();
				if (isLast || followedByEpsilon)
				{
					this->follow[*it].insert(this->follow[symbol].begin(), this->follow[symbol].end());
				}
				added += this->follow[*it].size() - currentSize;
			}
		}
	}
	return added;
}

void Grammar::ComputeFollow()
{
	this->follow[this->startSymbol] = { ENDMARKER() };
	set<string> epsilonSet = { EPSILON() };

	int added = 0;
	do
	{
		added = 0;
		for (auto& nonterminal : this->nonterminals)
		{
			added += this->ComputeFollowStep(nonterminal);
		}
	} while (added != 0);

	// remove all epsilons
	for (auto& pair : this->follow)
	{
		auto iter = pair.second.find(EPSILON());
		if (iter != pair.second.end())
			pair.second.erase(iter);
	}
}


set<Item>& Grammar::Closure(set<Item>& setOfItems)
{
	// See page 261
	int added;
	do
	{
		added = 0;
		int currentSize = setOfItems.size();
		for (auto& item : setOfItems)
		{
			RuleBody& body = this->rules[item.ProductionHead][item.RuleIndex];
			if (item.DotIndex >= body.size()) continue;

			const string& nonterminal = body[item.DotIndex];
			if (this->nonterminals.find(nonterminal) == this->nonterminals.end())
			{
				continue;
			}

			RuleBody tail(body.begin() + item.DotIndex + 1, body.end());
			tail.push_back(item.Lookahead);
			auto firstTail = this->ComputeFirstWord(tail);
			auto& bodies = this->rules[nonterminal];
			for (auto it = bodies.begin(); it != bodies.end(); it++)
			{
				for (auto& terminal : firstTail)
				{
					int ruleIndex = distance(bodies.begin(), it);
					Item newItem(nonterminal, terminal, ruleIndex, 0);
					setOfItems.insert(newItem);/*t,jgrtjhrthgrtj*/
				}
			}
		}
		added += setOfItems.size() - currentSize;

	} while (added != 0);
	return setOfItems;
}


void PrintClosure(const std::set<Item>& items, Grammar& g)
{
	for (auto& i : items)
	{
		vector<string> body = g.rules[i.ProductionHead][i.RuleIndex];
		vector<string> firstHalf(body.begin(), body.begin() + i.DotIndex);
		vector<string> secondHalf(body.begin() + i.DotIndex, body.end());
		cout << i.ProductionHead << " -> ";
		JoinCollection(secondHalf, JoinCollection(firstHalf, cout) << "@") << " | " << i.Lookahead << endl;
	}
}

set<Item> Grammar::GoTo(const std::set<Item>& setOfItems, const std::string& symbol)
{
	set<Item> gotoset;
	for (const Item& item : setOfItems)
	{
		auto& ruleBody = this->rules[item.ProductionHead][item.RuleIndex];
		if (item.DotIndex >= ruleBody.size()) continue;
		string& beforeDotSymbol = ruleBody[item.DotIndex];
		if (beforeDotSymbol == symbol)
		{
			Item newItem(item.ProductionHead, item.Lookahead, item.RuleIndex, item.DotIndex + 1);
			gotoset.insert(newItem);
		}
	}
	return this->Closure(gotoset);
}

set<string> Grammar::GetSymbols() const
{
	set<string> symbols;
	auto inserter = std::inserter(symbols, symbols.end());
	copy(this->nonterminals.begin(), this->nonterminals.end(), inserter);
	copy(this->terminals.begin(), this->terminals.end(), inserter);

	return symbols;
}

void Grammar::ComputeItems()
{
	Item i(AUGMENTED_START(), ENDMARKER(), 0, 0);
	i.DotIndex = 0;
	i.Lookahead = ENDMARKER();
	i.ProductionHead = AUGMENTED_START();
	i.RuleIndex = 0;
	set<Item> startSet = { i };
	this->items.clear();
	this->items.push_back(this->Closure(startSet));

	set<string> symbols = this->GetSymbols();

	int added;
	do
	{
		added = 0;
		int size = this->items.size();

		for (int i = 0; i < size; i++)
		{
			for (auto& symbol : symbols)
			{
				auto gotoset = this->GoTo(this->items[i], symbol);
				if (gotoset.size() != 0)
				{
					auto it = find(this->items.begin(), this->items.end(), gotoset);
					int index = std::distance(this->items.begin(), it);
					if (it == this->items.end())
					{
						this->items.push_back(gotoset);
						index = this->items.size() - 1;
					}
					auto pair = make_pair(i, symbol);
					this->gotoTable[pair] = index;
				}
			}
		}

		added = this->items.size() - size;
	} while (added != 0);
}

int ComputeItemSetCoreHash(const set<Item>& items)
{
	static hash<string> hashString;
	set<unsigned long> totalHash;

	for (const Item& item : items)
	{
		unsigned long hash = 23;
		hash = hash * 31 + hashString(item.ProductionHead);
		hash = hash * 31 + item.RuleIndex;
		hash = hash * 31 + item.DotIndex;

		totalHash.insert(hash);
	}

	unsigned long finalHash = 23;
	for (auto hash : totalHash)
	{
		finalHash = finalHash * 31 + hash;
	}
	return finalHash;
}

void Grammar::ComputeLR1Items()
{
	this->ComputeItems();
	map<unsigned long, vector<int>> groups;
	// Start from the 2nd item since we want to preserve the I0 at index 0 in the merged set
	for (auto i = 0; i < this->items.size(); i++)
	{
		auto hash = ComputeItemSetCoreHash(this->items[i]);
		groups[hash].push_back(i);
	}

	decltype(this->items) mergedItems;
	map<int, int> oldStatesToMerged;
	for (const auto& group : groups)
	{
		set<Item> merged;
		for (int i : group.second)
		{
			merged.insert(this->items[i].begin(), this->items[i].end());
			oldStatesToMerged[i] = mergedItems.size();
		}
		mergedItems.push_back(merged);
	}

	decltype(this->gotoTable) mergedGoto;
	set<string> symbols = this->GetSymbols();

	auto iterator = groups.begin();
	for (auto i = 0; i < mergedItems.size(); i++)
	{
		vector<int>& group = (iterator++)->second;
		for (const string& symbol : symbols)
		{
			for (int j : group)
			{
				auto key = make_pair(j, symbol);
				if (this->gotoTable.find(key) != this->gotoTable.end())
				{
					auto newKey = make_pair(i, symbol);
					auto transitionIndex = oldStatesToMerged[this->gotoTable[key]];
					if (mergedGoto.find(newKey) != mergedGoto.end() && mergedGoto[newKey] != transitionIndex)
						throw new std::invalid_argument("Grammar is not LR(1)!");

					mergedGoto[newKey] = transitionIndex;
				}
			}
		}
	}

	this->items = mergedItems;
	this->gotoTable = mergedGoto;
}


void Grammar::Save(ostream& output) const
{
	output << this->startSymbol << endl;
	for (auto& entry : this->rules)
	{
		const string& head = entry.first;
		output << head << ' ' <<  entry.second.size() << endl;
		for (const RuleBody& rule : entry.second)
		{
			for (const string& symbol : rule)
			{
				output << symbol << ' ';
			}
			output << "ENDBODY" << endl;
		}
	}
	output << "ENDRULES" << endl;

	for (auto& entry : this->gotoTable)
	{
		output << entry.first.first << " " << entry.first.second << " " << entry.second << endl;
	}

	output << "-1" << endl;
}

void Grammar::Load(istream& input)
{
	this->rules.clear();
	this->gotoTable.clear();
	this->items.clear();
	input >> this->startSymbol;
	while (true)
	{
		string head;
		input >> head;
		if (head == "ENDRULES")
			break;
		int ruleCount;
		input >> ruleCount;

		for (int i = 0; i < ruleCount; i++)
		{
			vector<string> body;
			while (true)
			{
				string symbol;
				input >> symbol;
				if (symbol == "ENDBODY")
				{
					this->rules[head].push_back(body);
					break;
				}
				body.push_back(symbol);
			}
		}
	}

	while (true)
	{
		int state;
		input >> state;
		if (state < 0) break;
		string symbol;
		input >> symbol;
		int nextState;
		input >> nextState;
		this->gotoTable[make_pair(state, symbol)] = nextState;
	}

}