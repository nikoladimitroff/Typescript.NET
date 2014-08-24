#include <sstream>
#include <functional>

#include "TsToCSharp.h"
#include "LexicalAnalyzer.h"
#include "Grammar.h"
#include "Parser.h"

using namespace std;
using namespace Translators;


map<string, vector<vector<string>>> ifGrammar =
{
	{
		"Statement",
		{
			{ "If" },
			{ "While" },
			{ "VarDeclaration" },
			{ "Class" },
			{ "{", "Statement", "Statement", "}" },
			{ "{", "}", },
		},
	},
	{
		"If",
		{
			{ "if", "(", "Bool", ")", "Statement" },
			{ "If", "Else" },
		},
	},
	{
		"Else",
		{
			{ "else", "Statement" },
		},
	},
	{
		"While",
		{
			{ "while", "(", "Bool", ")", "Statement" },
		},
	},
	{
		"VarDeclaration",
		{
			{ "var", "id", ":", "id", ";" },
			{ "var", "id", ":", "id", "=", "Expression", ";" },
		},
	},
	{
		"Class",
		{
			{ "class", "id", "{", "ClassBody", "}" },
		},
	},
	{
		"ClassBody",
		{
			{ "ClassBody", "ClassField" },
			{ "ClassBody", "ClassMethod" },
			{ "ClassField" },
			{ "ClassMethod" },
		},
	},
	{
		"ClassField",
		{
			{ "VisibilitySpecifier", "id", ":", "TypeId", ";"},
		},
	},
	{
		"ClassMethod",
		{
			{ "VisibilitySpecifier", "id", "(", "ArgumentList", ")", ":", "TypeId", "{", "Statement", "}" },
			{ "VisibilitySpecifier", "id", "(", ")", ":", "TypeId", "{", "Statement", "}" },
		},
	},
	{
		"ArgumentList",
		{
			{ "id", ":", "TypeId" },
			{ "id", ":", "TypeId", ",", "ArgumentList" },
		},
	},
	{
		"VisibilitySpecifier",
		{
			{ "private" },
			{ "public" },
		},
	},
	{
		"TypeId",
		{
			{ "id" },
		},
	},
	{
		"Expression",
		{
			{ "Bool" },
		},
	},
	{ "Bool", { { "BOOL_LITERAL" }, { "NUMBER", "RELATIVE_OP", "NUMBER" }, { "Bool", "BOOL_OP", "Bool" } } },

};


TsToCSharp::TsToCSharp() : grammar("Statement", ifGrammar, true)
{
}

void TranslateSubtree(const SyntaxTree&, ostream&, int);


namespace translate
{
	void TranslateStatement(const SyntaxTree& tree, ostream& stream, int indentation)
	{
		if (tree.Children.size() == 1)
		{
			for (auto& child : tree.Children)
				TranslateSubtree(*child, stream, indentation);
		}
		if (tree.Children.size() == 2)
		{
			stream << string(indentation * 4, ' ') << tree.Children[0]->Node.GetLexeme();
			stream << ' ';
			stream << tree.Children[1]->Node.GetLexeme() << endl;
		}

		if (tree.Children.size() == 4)
		{
			stream << string(indentation * 4, ' ') << tree.Children[0]->Node.GetLexeme() << endl;

			TranslateSubtree(*tree.Children[1], stream, indentation + 1);
			TranslateSubtree(*tree.Children[2], stream, indentation + 1);
			stream << string(indentation * 4, ' ') << tree.Children[3]->Node.GetLexeme() << endl;
		}
	}

	void TranslateControlFlow(const SyntaxTree& tree, ostream& stream, int indentation)
	{
		if (tree.Children.size() == 5)
		{
			stream << string(indentation * 4, ' ') << tree.Children[0]->Node.GetLexeme();
			stream << ' ';
			stream << tree.Children[1]->Node.GetLexeme();
			TranslateSubtree(*tree.Children[2], stream, 0);
			stream << tree.Children[3]->Node.GetLexeme() << endl;
			TranslateSubtree(*tree.Children[4], stream, indentation + 1);
		}

		if (tree.Children.size() == 2)
		{
			TranslateSubtree(*tree.Children[0], stream, indentation);
			TranslateSubtree(*tree.Children[1], stream, indentation);
		}
	}

	void TranslateBlock(const SyntaxTree& tree, ostream& stream, int indentation)
	{
	}

	void TranslateElse(const SyntaxTree& tree, ostream& stream, int indentation)
	{
		stream << string(indentation * 4, ' ') << tree.Children[0]->Node.GetLexeme() << '\n';
		TranslateSubtree(*tree.Children[1], stream, indentation + 1);
	}

	void TranslateExpression(const SyntaxTree& tree, ostream& stream, int indentation)
	{
		for (auto& child : tree.Children)
		{
			if (child->Node.GetTag() == TokenTag::Nonterminal)
				TranslateExpression(*child, stream, 0);
			else
				stream << child->Node.GetLexeme() << ' ';
		}
	}


	void TranslateTypeId(const SyntaxTree& tree, ostream& stream, int indentation)
	{
		static map<string, string> renamingTable =
		{
			{ "number", "double" },
			{ "boolean", "bool" },
			{ "any", "dynamic" },
			{ "Array", "List" },
		};
		string typeId = tree.Children.size() == 0 ? tree.Node.GetLexeme() : tree.Children[0]->Node.GetLexeme();
		if (renamingTable.find(typeId) != renamingTable.end())
		{
			stream << renamingTable[typeId];
		}
		else
		{
			stream << typeId;
		}
	}

	void TranslateVarDeclaration(const SyntaxTree& tree, ostream& stream, int indentation)
	{
		stream << string(indentation * 4, ' ');
		TranslateTypeId(*tree.Children[3], stream, 0); 
		stream << ' ';
		stream << tree.Children[1]->Node.GetLexeme();
		if (tree.Children.size() == 7)
		{
			// Initialization
			stream << ' ' << tree.Children[4]->Node.GetLexeme() << ' ';
			TranslateExpression(*tree.Children[5], stream, 0);
		}
		stream << tree.Children[tree.Children.size() - 1]->Node.GetLexeme() << endl;
	}


	void TranslateClassField(const SyntaxTree& tree, ostream& stream, int indentation)
	{
		string indent(indentation * 4, ' ');
		stream << indent << tree.Children[0]->Children[0]->Node.GetLexeme() << ' ';
		TranslateTypeId(*tree.Children[3], stream, 0);
		stream << ' ' << tree.Children[1]->Node.GetLexeme() << ';' << endl;
	}

	void TranslateArgList(const SyntaxTree& tree, ostream& stream, int indentation)
	{


	}

	void TranslateClassMethod(const SyntaxTree& tree, ostream& stream, int indentation)
	{
		string indent(indentation * 4, ' ');
		stream << indent << tree.Children[0]->Children[0]->Node.GetLexeme() << ' ';
		auto typeId = tree.Children.size() == 10 ? tree.Children[6] : tree.Children[5];

		TranslateTypeId(*typeId, stream, 0);
		stream << ' ' << tree.Children[1]->Node.GetLexeme();
		stream << "(";
		if (tree.Children.size() == 10)
			// We've got args!
			TranslateArgList(*tree.Children[3], stream, 0);

		stream << ")" << endl;
		stream << indent << "{" << endl;
		TranslateStatement(*tree.Children[tree.Children.size() - 2], stream, indentation + 1);
		stream << endl << indent << "}";
	}

	void TranslateClassBody(const SyntaxTree& tree, ostream& stream, int indentation)
	{
		if (tree.Children.size() == 2)
		{
			TranslateClassBody(*tree.Children[0], stream, indentation);
			TranslateSubtree(*tree.Children[1], stream, indentation);
		}
		else
		{
			TranslateSubtree(*tree.Children[0], stream, indentation);
		}
	}

	void TranslateClass(const SyntaxTree& tree, ostream& stream, int indentation)
	{
		string indent(indentation * 4, ' ');
		stream << indent << "class ";
		TranslateTypeId(*tree.Children[1], stream, 0);
		stream << endl << indent << "{" << endl;
		TranslateSubtree(*tree.Children[3], stream, indentation + 1);
		stream << endl << indent << "}" << endl;
	}
}

map<string, function<void(const SyntaxTree&, ostream&, int)>> TranslatorFunctions =
{
	{ "Block", translate::TranslateBlock },
	{ "If", translate::TranslateControlFlow },
	{ "Else", translate::TranslateElse },
	{ "While", translate::TranslateControlFlow },
	{ "For", translate::TranslateControlFlow },
	{ "VarDeclaration", translate::TranslateVarDeclaration },
	{ "Class", translate::TranslateClass },
	{ "ClassBody", translate::TranslateClassBody },
	{ "ClassMethod", translate::TranslateClassMethod },
	{ "ClassField", translate::TranslateClassField },

	{ "Expression", translate::TranslateExpression },
	{ "Bool", translate::TranslateExpression },
};



void TranslateSubtree(const SyntaxTree& tree, ostream& stream, int indentation)
{
	if (tree.Node.GetTag() != TokenTag::Nonterminal)
	{
		stream << tree.Node.GetLexeme();
		return;
	}
	auto txt = tree.Node.GetLexeme();
	if (TranslatorFunctions.find(txt) != TranslatorFunctions.end())
		TranslatorFunctions[txt](tree, stream, indentation);
	else
		translate::TranslateStatement(tree, stream, indentation + 1);
}

string TsToCSharp::Translate(string source)
{
	LexicalAnalyzer lexer;
	auto tokenizedSource = lexer.Tokenize(source, true);
	Parser parser(this->grammar);
	auto parseTree = parser.Parse(tokenizedSource);
	cout << *parseTree;
	stringstream stream;
	TranslateSubtree(*parseTree, stream, 0);
	return stream.str();
}