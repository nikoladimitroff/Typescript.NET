#include <sstream>
#include <functional>
#include <istream>

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
			{ "For" },
			{ "VarDeclaration" },
			{ "Expression", ";" },
			{ "Class" },
			{ "{", "Statement", "}" },
			{ "Statement", "Statement" },
			{ "{", "}", },
		},
	},
	{
		"If",
		{
			{ "if", "(", "Expression", ")", "Statement" },
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
			{ "while", "(", "Expression", ")", "Statement" },
		},
	},
	{
		"For",
		{
			{ "for", "(", "ForBody", ")", "Statement" },
		},
	},
	{
		"ForBody",
		{
			{ "VarDeclaration", "Expression", ";", "Expression" },
			//{ ";", "Expression", ";", "Expression" },
			//{ ";", ";", "Expression" },
			//{ ";", ";" },
			////{ "VarDeclaration", ";", "Expression" },
			//{ "VarDeclaration", ";" },
			//{ "VarDeclaration", "Expression", ";" },
			//{ ";", "Expression", ";" },
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
			{ "class", "id", "BaseClasses", "{", "ClassBody", "}" },
		},
	},
	{
		"BaseClasses",
		{
			{ "extends", "id" },
			{ "implements", "IdList" },
			{ "extends", "id", "implements", "IdList" },
			{ "implements", "IdList", "extends", "id" },
		},
	},
	{
		"IdList",
		{
			{ "id", ",", "IdList" },
			{ "id" },
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
			{ "LValue" },
			{ "NUMBER" },
			{ "STRING_LITERAL" },
			{ "BOOL_LITERAL" },
			{ "LValue", "UNARY_OP" },
			{ "UNARY_OP", "LValue" },
			{ "LValue", "=", "Expression" },
			{ "LValue", "BINARY_ASSIGNMENT", "Expression" },
			{ "Expression", "BINARY_OP", "Expression" },
			{ "(", "Expression", ")" },

		},
	},
	{
		"LValue",
		{
			{ "id" },
			{ "id", "[", "Expression", "]" },
		}
	},
};


TsToCSharp::TsToCSharp() : grammar("Statement", ifGrammar, true), parser(this->grammar)
{
	ofstream file("tscs.txt");
	this->parser.Save(file);
	file.close();
}

TsToCSharp::TsToCSharp(bool load)
{
	ifstream input("tscs.txt");
	this->parser.Load(input);
	input.close();
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
			if (tree.Children[0]->Node.GetTag() == TokenTag::Nonterminal)
			{
				if (tree.Children[1]->Node.GetTag() == TokenTag::Nonterminal)
				{
					// Statement -> Statement Statement
					TranslateSubtree(*tree.Children[0], stream, indentation);
					TranslateSubtree(*tree.Children[1], stream, indentation);
				}
				else
				{
					// Statement -> Expression ;
					TranslateSubtree(*tree.Children[0], stream, indentation);
					stream << tree.Children[1]->Node.GetLexeme() << endl;
				}
			}
			else
			{
				// Statement -> { } (empty block)
				stream << string(indentation * 4, ' ') << tree.Children[0]->Node.GetLexeme();
				stream << ' ';
				stream << tree.Children[1]->Node.GetLexeme() << endl;
			}
		}

		if (tree.Children.size() == 3)
		{
			// Statement -> { Statement }
			stream << string(indentation * 4, ' ') << tree.Children[0]->Node.GetLexeme() << endl;
			TranslateSubtree(*tree.Children[1], stream, indentation + 1);
			stream << string(indentation * 4, ' ') << tree.Children[2]->Node.GetLexeme() << endl;
		}
	}

	void TranslateControlFlow(const SyntaxTree& tree, ostream& stream, int indentation)
	{
		stream << endl;
		if (tree.Children.size() == 5)
		{
			// Control Flow -> if/while/for ( Bool / ForBody ) Statement
			stream << string(indentation * 4, ' ') << tree.Children[0]->Node.GetLexeme();/* keyword */
			stream << ' ';
			stream << tree.Children[1]->Node.GetLexeme();  /* ( */
			TranslateSubtree(*tree.Children[2], stream, 0); /* condition / forbody*/
			stream << tree.Children[3]->Node.GetLexeme() << endl; /* ) */
			TranslateSubtree(*tree.Children[4], stream, indentation + 1); /* Statement */
		}

		if (tree.Children.size() == 2)
		{
			// Control Flow -> If Else
			TranslateSubtree(*tree.Children[0], stream, indentation);
			TranslateSubtree(*tree.Children[1], stream, indentation);
		}
	}

	void TranslateForBody(const SyntaxTree& tree, ostream& stream, int indentation)
	{
		for (auto& child : tree.Children)
		{
			TranslateSubtree(*child, stream, indentation);
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
		stream << string(indentation * 4, ' ');
		bool isUnary = tree.Children.size() == 2 &&
			(tree.Children[0]->Node.GetTag() == TokenTag::UnaryOp ||
			tree.Children[1]->Node.GetTag() == TokenTag::UnaryOp);


		for (auto& child : tree.Children)
		{
			if (child->Node.GetTag() == TokenTag::Nonterminal)
				TranslateExpression(*child, stream, 0);
			else
			{
				if (!isUnary)
					stream << ' ';
				stream << child->Node.GetLexeme();
			}
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
		stream << tree.Children[tree.Children.size() - 1]->Node.GetLexeme();
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
		/*"ArgumentList",
		{
			{ "id", ":", "TypeId" },
			{ "id", ":", "TypeId", ",", "ArgumentList" },
		},*/

		TranslateTypeId(*tree.Children[2], stream, 0);
		stream << ' ' << tree.Children[0]->Node.GetLexeme();
		if (tree.Children.size() == 5)
		{
			stream << ", ";
			TranslateArgList(*tree.Children[4], stream, indentation);
		}


	}

	void TranslateIdList(const SyntaxTree& tree, ostream& stream, int indentation)
	{
		stream << tree.Children[0]->Node.GetLexeme();

		if (tree.Children.size() > 1)
		{
			stream << ", ";
			TranslateSubtree(*tree.Children[2], stream, 0);
		}
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

	void TranslateBaseClasses(const SyntaxTree& tree, ostream& stream, int indentation)
	{
		/*"BaseClasses",
		{
			{ "extends", "id" },
			{ "implements", "IdList" },
			{ "extends", "id", "implements", "IdList" },
			{ "implements", "IdList", "extends", "id" },
		},*/

		stream << " : ";
		TranslateSubtree(*tree.Children[1], stream, indentation);
		if (tree.Children.size() > 2)
		{
			stream << ", ";
			TranslateSubtree(*tree.Children[3], stream, indentation);
		}
	}

	void TranslateClass(const SyntaxTree& tree, ostream& stream, int indentation)
	{
		/*"Class",
		{
			{ "class", "id", "{", "ClassBody", "}" },
			{ "class", "id", "BaseClasses", "{", "ClassBody", "}" },
		},*/
		string indent(indentation * 4, ' ');
		stream << indent << "class ";
		TranslateTypeId(*tree.Children[1], stream, 0);

		if (tree.Children.size() == 6)
			TranslateBaseClasses(*tree.Children[2], stream, 0);

		stream << endl << indent << "{" << endl;
		TranslateSubtree(*tree.Children[tree.Children.size() - 2], stream, indentation + 1);
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
	{ "ForBody", translate::TranslateForBody },
	{ "VarDeclaration", translate::TranslateVarDeclaration },
	{ "Class", translate::TranslateClass },
	{ "ClassBody", translate::TranslateClassBody },
	{ "ClassMethod", translate::TranslateClassMethod },
	{ "ClassField", translate::TranslateClassField },
	{ "IdList", translate::TranslateIdList },

	{ "Expression", translate::TranslateExpression },
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
	auto parseTree = this->parser.Parse(tokenizedSource);
	cout << *parseTree;
	stringstream stream;
	TranslateSubtree(*parseTree, stream, 0);
	return stream.str();
}