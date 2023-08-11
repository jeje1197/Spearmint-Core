#include "exception/Exception.h"
#include "lexer/Token.h"
#include "lexer/Lexer.h"

#include "parser/AstNode.h"
#include "parser/Parser.h"

#include "interpreter/Context.h"
#include "interpreter/Interpreter.h"
#include "interpreter/BuiltInFunctions.h"

#include <iostream>
#include <memory>
#include <fstream>
#include <vector>
#include <chrono>

// ...

using namespace std::chrono;

void showWelcomeMessage();
std::string getFileText(std::string fileName);
void run(std::string filename, std::string input);

int main()
{
    showWelcomeMessage();

    // Shell loop
    while (true) {
        std::string input;
        std::cout << "\nSpearmint>";
        std::getline(std::cin, input);

        if (input.size() == 0) { // Ignore empty input
            continue;
        }
        else if (input == "-e") { // Exit Program
            break;
        }
        else if (input.find("-r ") != std::string::npos) { // Read from file
            int len = (int)input.size();
            int startIndex = (int)input.find("-r ") + 3;
            if (startIndex < len) {
                std::string filename = input.substr(startIndex, len - startIndex);
                try {
                    input = getFileText(filename);
                }
                catch (...) {
                    std::cout << "File: '" << filename << "' not found.";
                }
                run(filename, input);
            }
            continue;
        }
        else { // Read input as text (No flags)
            run("Console", input);
        }
    }
	return 0;
}

void showWelcomeMessage() {
    std::cout << "----- Spearmint Interpreter -----" << std::endl;
    std::cout << "Type '-e' or '-exit' to close the shell.\nType -help to see a list of available commands." << std::endl;
}

void run(std::string filename, std::string input) {
    // Lexical Analysis
    Lexer lexer(filename, input);
    std::vector<Token> tokens; 
    try {
        tokens = lexer.getTokens();
    }
    catch (Exception e) {
        e.show();
        return;
    }

    if ((int)tokens.size() <= 1) return;

    // Syntactical Analysis
    Parser parser(tokens);
    std::vector<AstNode> ast;
    try {
        ast = parser.parse();
    }
    catch (Exception e) {
        e.show();
        return;
    }

    if ((int)ast.size() == 0) return;

    // Interpreting
    // Put vector in AstWrapper to pass through as AstNode Argument
    AstNode programStatements = AstNode(new VectorWrapperNode(ast));

    
    Object_sPtr truePrimitive(new Boolean(true));
    Object_sPtr falsePrimitive(new Boolean(false));
    Object_sPtr nullPrimitive(NullType::getNullType());

    Interpreter interpreter("Console");
    Object_sPtr result = nullPrimitive;
    Context ctx("Base Context", SymbolTable_sPtr(new SymbolTable()));

    // Add variables to global symbol table
    ctx.symbol_table->addLocal("true", Object_sPtr(new VariableWrapper(truePrimitive, true)));
    ctx.symbol_table->addLocal("false", Object_sPtr(new VariableWrapper(falsePrimitive, true)));
    ctx.symbol_table->addLocal("null", Object_sPtr(new VariableWrapper(nullPrimitive, true)));

    // Add built-in-variables to global symbol table
    addBuiltInFunctions(ctx.symbol_table);
    try {
        int msBefore = (int) duration_cast<milliseconds>(
            system_clock::now().time_since_epoch()
        ).count();
        interpreter.visit(programStatements, ctx);
        int msAfter = duration_cast<milliseconds>(
            system_clock::now().time_since_epoch()
        ).count();
        std::cout << "Program Time Elapsed: " << msAfter - msBefore << std::endl;
    }
    catch (Exception e) {
        e.show();
    }
}

// Method to read text from file
std::string getFileText(std::string fileName) {
    std::string fileText, line;
    std::ifstream MyReadFile(fileName);

    while (getline(MyReadFile, line)) {
        fileText += line + "\n";
    }

    MyReadFile.close();

    if (fileText.size() > 0) {
        return fileText.erase(fileText.length() - 1, 1);
    }
    return "";
}
