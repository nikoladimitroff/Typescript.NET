#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>

#undef CLIENT_CODE

#include "LexicalAnalyzer.h"
#include "Grammar.h"
#include "Utilities.h"
#include "Parser.h"
#include "TsToCSharp.h"

using namespace std;

int main()
{
	map<string, vector<vector<string>>> ifGrammar =
	{
		//{ "Statement", { { "s", "Statement" }, { EPSILON() } } }
		{
			"Statement",
			{
				{ "if", "(", "Bool", ")", "Statement" },
				{ "if", "(", "Bool", ")", "Statement", "else", "Statement" },
				{ "while", "(", "Bool", ")", "Statement" },
				{ "{", "}" }
			}
		},
		{ "Bool", { { "BOOL_LITERAL" }, { "NUMBER", "RELATIVE_OP", "NUMBER" }, { "Bool", "BOOL_OP", "Bool" } } },
	};


	string code = R"( 
		class A extends B implements I1, I2 {
			private x: number;
			
			public myMethod(x: number, y: string): void {
				for (var i: number; i ; i++) {
					y += y;
				}
			}
		}
	)";

	code = R"( 
		class A extends B implements I1, I2, I3 {
			private x: number;
			public computeMe(x: number, txt: string, flag: boolean): number {
				var y: number = 5;
				for (var i: number = 0; i < y; i++) {
					var x: string = "texty";
					txt = x[y + i];
				}
			}
		}
	)";

	Translators::TsToCSharp translator(true);

	try
	{
		cout << translator.Translate(code);
	}
	catch (invalid_argument& e)
	{
		cout << e.what();
	}
	cout << endl << "Press any key to continue" << endl;
	cin.get();

	return 0;
}