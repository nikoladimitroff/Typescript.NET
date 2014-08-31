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
		"Program",
		{
			{ "Module", },
			{ "Class", },
		},
	},
	{
		"Statement",
		{
			{ "If" },
			{ "While" },
			{ "For" },
			{ "VarDeclaration" },
			{ "ExpressionHeader", ";" },
			{ "StatementList" },
			// break / continue
			{ "OtherControlStatements", ";" },
		},
	},
	{
		"StatementList",
		{
			{ "{", "}" },
			{ "{", "StatementListBody", "}" },
		},
	},
	{
		"StatementListBody",
		{
			{ "Statement", "StatementListBody" },
			{ "Statement", },
		},
	},
	{
		"If",
		{
			{ "if", "(", "ExpressionHeader", ")", "Statement" },
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
			{ "while", "(", "ExpressionHeader", ")", "Statement" },
		},
	},
	{
		"For",
		{
			{ "for", "(", "ForHead", ")", "Statement" },
		},
	},
	{
		"ForHead",
		{
			{ "VarDeclaration", "ExpressionHeader", ";", "ExpressionHeader" },
			{ ";", "ExpressionHeader", ";", "ExpressionHeader" },
			{ ";", ";", "ExpressionHeader" },
			{ ";", ";" },
			{ "VarDeclaration", ";", "ExpressionHeader" },
			{ "VarDeclaration", ";" },
			{ "VarDeclaration", "ExpressionHeader", ";" },
			{ ";", "ExpressionHeader", ";" },
		},
	},
	{
		"OtherControlStatements",
		{
			{ "break" },
			{ "continue" },
		}
	},
	{
		"VarDeclaration",
		{
			{ "var", "id", ":", "id", ";" },
			{ "var", "id", ":", "id", "=", "ExpressionHeader", ";" },
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
		"Module",
		{
			{ "module", "id", "{", "Program", "}" }
		}
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
			{ "VisibilitySpecifier", "id", "(", "ArgumentList", ")", ":", "TypeId", "StatementList" },
			{ "VisibilitySpecifier", "id", "(", ")", ":", "TypeId", "StatementList" },
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
		"ExpressionHeader",
		{
			{ "Expression" }
		}
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
			{ "(", "ExpressionHeader", ")" },
		},
	},
	{
		"LValue",
		{
			{ "id" },
			{ "id", "[", "ExpressionHeader", "]" },
		}
	},
};


TsToCSharp::TsToCSharp() : grammar("Program", ifGrammar, true), parser(this->grammar)
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
	void RemoveLastSymbol(ostream& stream)
	{
		stream.seekp(static_cast<int>(stream.tellp()) - 1);
	}

	void TranslateStatement(const SyntaxTree& tree, ostream& stream, int indentation)
	{
		if (tree.Children.size() == 1)
		{
			// Statement -> If/while/for/statementlist;
			for (auto& child : tree.Children)
				TranslateSubtree(*child, stream, indentation);
		}
		if (tree.Children.size() == 2)
		{
			// Statement -> Expression ; | continue ; | break ;
			TranslateSubtree(*tree.Children[0], stream, indentation);
			stream << tree.Children[1]->Node.GetLexeme();
		}
	}

	void TranslateStatementListBody(const SyntaxTree& tree, ostream& stream, int indentation)
	{
		stream << endl;
		if (tree.Children.size() == 1)
		{
			TranslateStatement(*tree.Children[0], stream, indentation);
		}
		else
		{
			TranslateStatement(*tree.Children[0], stream, indentation);
			TranslateStatementListBody(*tree.Children[1], stream, indentation);
		}
	}

	void TranslateStatementList(const SyntaxTree& tree, ostream& stream, int indentation)
	{
		string indent(indentation * 4, ' ');
		stream << indent << tree.Children[0]->Node.GetLexeme(); // {
		if (tree.Children.size() == 3)
		{
			TranslateStatementListBody(*tree.Children[1], stream, indentation + 1);
			stream << endl << indent;
		}
		stream << tree.Children[tree.Children.size() - 1]->Node.GetLexeme(); // }
	}

	void TranslateControlFlow(const SyntaxTree& tree, ostream& stream, int indentation)
	{
		if (tree.Children.size() == 5)
		{
			// Control Flow -> if/while/for ( Bool / ForHead ) Statement
			stream << string(indentation * 4, ' ') << tree.Children[0]->Node.GetLexeme();/* keyword */
			stream << ' ';
			stream << tree.Children[1]->Node.GetLexeme();  /* ( */
			TranslateSubtree(*tree.Children[2], stream, 0); /* condition / ForHead*/
			stream << tree.Children[3]->Node.GetLexeme() << endl; /* ) */
			TranslateSubtree(*tree.Children[4], stream, indentation); /* Statement */
		}

		if (tree.Children.size() == 2)
		{
			// Control Flow -> If Else
			TranslateSubtree(*tree.Children[0], stream, indentation);
			TranslateSubtree(*tree.Children[1], stream, indentation);
		}
	}

	void TranslateForHead(const SyntaxTree& tree, ostream& stream, int indentation)
	{
		for (auto it = tree.Children.begin(); it != tree.Children.end(); ++it)
		{
			auto& child = *it;
			// Place a space before every subtree but the first
			if (child->Node.GetTag() == TokenTag::Nonterminal && it != tree.Children.begin())
				stream << ' ';

			TranslateSubtree(*child, stream, indentation);
		}
	}

	void TranslateElse(const SyntaxTree& tree, ostream& stream, int indentation)
	{
		stream << string(indentation * 4, ' ') << tree.Children[0]->Node.GetLexeme() << '\n';
		TranslateSubtree(*tree.Children[1], stream, indentation + 1);
	}

	void TranslateOtherControlFlow(const SyntaxTree& tree, ostream& stream, int indentation)
	{
		stream << string(indentation * 4, ' ') << tree.Children[0]->Node.GetLexeme();
	}

	void TranslateExpression(const SyntaxTree& tree, ostream& stream, int indentation)
	{
		stream << string(indentation * 4, ' ');
		bool isUnary = tree.Children.size() == 2 &&
			(tree.Children[0]->Node.GetTag() == TokenTag::UnaryOp ||
			tree.Children[1]->Node.GetTag() == TokenTag::UnaryOp);

		if (isUnary)
		{
			if (tree.Children[0]->Node.GetTag() == TokenTag::UnaryOp)
			{
				stream << tree.Children[0]->Node.GetLexeme();
				TranslateSubtree(*tree.Children[1], stream, 0);
			}
			else
			{
				TranslateSubtree(*tree.Children[0], stream, 0);
				RemoveLastSymbol(stream);
				stream << tree.Children[1]->Node.GetLexeme();
				stream << ' ';
			}
			return;
		}

		for (auto& child : tree.Children)
		{
			if (child->Node.GetTag() == TokenTag::Nonterminal)
			{
				TranslateSubtree(*child, stream, 0);
			}
			else
			{
				stream << child->Node.GetLexeme();
				stream << ' ';
			}
		}
	}

	void TranslateExpressionHeader(const SyntaxTree& tree, ostream& stream, int indentation)
	{
		TranslateExpression(*tree.Children[0], stream, indentation);
		RemoveLastSymbol(stream);
	}

	void TranslateLvalue(const SyntaxTree& tree, ostream& stream, int indentation)
	{
		if (tree.Children.size() == 1)
		{
			stream << tree.Children[0]->Node.GetLexeme() << ' ';
		}
		else
		{
			stream << tree.Children[0]->Node.GetLexeme();
			stream << tree.Children[1]->Node.GetLexeme();
			TranslateExpressionHeader(*tree.Children[2], stream, 0);
			stream << tree.Children[3]->Node.GetLexeme();
			stream << ' ';
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
			stream << ' ' << tree.Children[4]->Node.GetLexeme() << ' '; // =
			TranslateExpressionHeader(*tree.Children[5], stream, 0); // expression
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
		auto typeId = tree.Children.size() == 8 ? tree.Children[6] : tree.Children[5];

		TranslateTypeId(*typeId, stream, 0);
		stream << ' ' << tree.Children[1]->Node.GetLexeme();
		stream << "(";
		if (tree.Children.size() == 8)
			// We've got args!
			TranslateArgList(*tree.Children[3], stream, 0);

		stream << ")" << endl;
		TranslateStatementList(*tree.Children[tree.Children.size() - 1], stream, indentation);
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

	void TranslateModule(const SyntaxTree& tree, ostream& stream, int indentation)
	{
		string indent(indentation * 4, ' ');
		stream << indent << "namespace" << ' ' << tree.Children[1]->Node.GetLexeme() << endl;
		stream << indent << '{' << endl;
		TranslateSubtree(*tree.Children[3], stream, indentation + 1);
		stream << indent << '}' << endl;
	}
}

map<string, function<void(const SyntaxTree&, ostream&, int)>> TranslatorFunctions =
{
	{ "StatementList", translate::TranslateStatementList },
	{ "If", translate::TranslateControlFlow },
	{ "Else", translate::TranslateElse },
	{ "While", translate::TranslateControlFlow },
	{ "For", translate::TranslateControlFlow },
	{ "ForHead", translate::TranslateForHead },
	{ "OtherControlStatements", translate::TranslateOtherControlFlow },
	{ "VarDeclaration", translate::TranslateVarDeclaration },
	{ "Class", translate::TranslateClass },
	{ "ClassBody", translate::TranslateClassBody },
	{ "ClassMethod", translate::TranslateClassMethod },
	{ "ClassField", translate::TranslateClassField },
	{ "IdList", translate::TranslateIdList },
	{ "Module", translate::TranslateModule },

	{ "Expression", translate::TranslateExpression },
	{ "ExpressionHeader", translate::TranslateExpressionHeader },
	{ "LValue", translate::TranslateLvalue }
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
		translate::TranslateStatement(tree, stream, indentation);
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