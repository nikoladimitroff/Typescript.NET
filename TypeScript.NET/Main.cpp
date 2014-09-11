#include <iostream>
#include <fstream>
#include <streambuf>
#include <sstream>
#include <iterator>

#undef CLIENT_CODE

#include "LexicalAnalyzer.h"
#include "Grammar.h"
#include "Utilities.h"
#include "Parser.h"
#include "TsToCSharp.h"

using namespace std;

int main(int argc, char** argv)
{
	if (argc == 1)
	{
		cout << "Source file not specified. Call the compiler using 'tsnet <path_to_source>'\n";
		return -1;
	}

	string sourcePath = argv[1];
	Translators::TsToCSharp translator;


	auto dot = sourcePath.find_last_of('.');
	string outputPath = sourcePath.substr(0, dot) + ".cs";
	ifstream inputFile(sourcePath);
	string code((istreambuf_iterator<char>(inputFile)),
				 istreambuf_iterator<char>());
	inputFile.close();
	ofstream outputFile(outputPath);
		
	try {
		cout << "Translating..." << endl;
		outputFile << translator.Translate(code);
	}
	catch (invalid_argument& e)
	{
		cout << e.what();
	}
	outputFile.close();
	cout << "Done." << endl;
	cout << endl << "Press ENTER to continue" << endl;
	cin.get();

	return 0;
}