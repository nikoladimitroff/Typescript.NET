#include <iostream>

#include "gtest.h"

#include "LexerTests.h"
#include "GrammarTests.h"


using namespace std;



int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	int result = RUN_ALL_TESTS();

	cout << "Press Enter to continue" << endl;
	cin.get();
	return result;
}