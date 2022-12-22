

#include "Token.h"
#include "Lexer.h"
#include "Exception.h"

#include <iostream>
#include <memory>
#include <fstream>
#include <vector>

int main()
{
	Lexer l("Console", "print(5);");
	auto v = l.getTokens();
	for (int i = 0; i < (int) v.size(); i++) {
		std::cout << (Token) v.at(i) << std::endl;
	}
	return 0;
}