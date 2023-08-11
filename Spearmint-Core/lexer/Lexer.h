#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

#include "Token.h"
#include "exception/Exception.h"


class Lexer {
private:
    std::string fn;
    std::string text;
    int index;
    char curChar;
    Position curPos;

public:
    Lexer(std::string fn, std::string text) {
        this->fn = fn;
        this->text = text;
        this->index = -1;
        this->curPos = Position(fn);
        getNext();
    }

private:
    bool hasNext(int stepsAhead=1) {
        return this->index + stepsAhead < (int)text.length();
    }

    void getNext() {
        if (hasNext()) {
            index++;
            this->curPos.advance(curChar);
            this->curChar = this->text.at(index);
        }
        else {
            this->curChar = '\0';
        }
    }

    char lookAhead(int stepsAhead) {
        return hasNext(stepsAhead) ? this->text.at(this->index + stepsAhead) : '\0';
    }

public:
    std::vector<Token> getTokens() {
        std::vector<Token> tokens;
        std::string operators = "+-*/^%=<>!&";
        std::unordered_set<std::string> keywords = { 
            "import", "const", "var",
            "if", "else", "for", "while", "break", "continue",
            "fn", "return", "type", "new", "api"
        };

        while (curChar != '\0') {
            std::string next2chars = std::string(1, curChar) + lookAhead(1);

            if (curChar == ' ' || curChar == '\t' || curChar == '\n') { // Skip whitespace
            }
            else if (curChar == '#') { // Skip Comments
                while (curChar != '\n' && curChar != '\0') {
                    getNext();
                }
            }
            else if (isalpha(curChar) || curChar == '_') { // Keywords and Identifiers
                Position startPos = curPos.copy();
                std::string str = "";

                while (isalpha(curChar) || isdigit(curChar) || curChar == '_') {
                    str += curChar;
                    getNext();
                }

                if (keywords.find(str) != keywords.end()) {
                    tokens.push_back(Token(KEYWORD, str, startPos));
                }
                else {
                    tokens.push_back(Token(ID, str, startPos));
                }
                continue;
            }
            else if (isdigit(curChar)) { // Numbers
                Position startPos = curPos.copy();
                std::string number = "";

                int decimal_count = 0;
                while (isdigit(curChar) || curChar == '.') {
                    if (curChar == '.') {
                        if (decimal_count == 1) break;
                        decimal_count++;
                    }
                    number += curChar;
                    getNext();
                }

                if (decimal_count == 0) {
                    tokens.push_back(Token(INT, number));
                }
                else {
                    tokens.push_back(Token(FLOAT, number));
                }

                continue;
            }
            else if (curChar == '"') { // Strings
                Position startPos = curPos.copy();
                getNext();
                std::unordered_map<char, std::string> escapeChars = {
                    {'"', "\""},
                    {'n', "\n"},
                    {'t', "\t"},
                    {'\\', "\\"}
                };

                std::string str;
                while (curChar != '"') {
                    if (curChar == '\0') {
                        throw Exception("Unterminated string " + startPos.toString());
                    }

                    if (curChar == '\\') {
                        if (escapeChars.find(lookAhead(1)) == escapeChars.end()) {
                            throw Exception("Unescaped slash in string " + curPos.toString());
                        }

                        str += escapeChars.at(lookAhead(1));
                        getNext();
                        getNext();
                    }
                    else {
                        str += curChar;
                        getNext();
                    }
                }
                tokens.push_back(Token(STRING, str, startPos));
            }
            else if (next2chars == "!=" || next2chars == "==" || next2chars == "<=" || next2chars == ">=" ||
                next2chars == "&&" || next2chars == "||") { // 2 character operators
                tokens.push_back(Token(OP, next2chars, curPos));
                getNext();
            }
            else if (next2chars == "->") { // Right Arrow
                tokens.push_back(Token(RARROW, "->"));
                getNext();
            }
            else if (operators.find(curChar) != std::string::npos) { // 1 character operators
                tokens.push_back(Token(OP, curChar, curPos));
            }
            else if (curChar == '.') { // Dot
                tokens.push_back(Token(DOT, curChar, curPos));
            }
            else if (curChar == ',') { // Comma
                tokens.push_back(Token(COMMA, curChar, curPos));
            }
            else if (curChar == ':') { // Colon
                tokens.push_back(Token(COLON, curChar, curPos));
            }
            else if (curChar == ';') { // Semicolons
                tokens.push_back(Token(SEMICOLON, curChar, curPos));
            }
            else if (curChar == '(') { // Left Parenthesis
                tokens.push_back(Token(LPAREN, curChar, curPos));
            }
            else if (curChar == ')') { // Right Parenthesis
                tokens.push_back(Token(RPAREN, curChar, curPos));
            }
            else if (curChar == '{') { // Left Brace
                tokens.push_back(Token(LBRACE, curChar, curPos));
            }
            else if (curChar == '}') { // Right Brace
                tokens.push_back(Token(RBRACE, curChar, curPos));
            }
            else if (curChar == '[') { // Left Bracket
                tokens.push_back(Token(LBRACKET, curChar, curPos));
            }
            else if (curChar == ']') { // Right Bracket
                tokens.push_back(Token(RBRACKET, curChar, curPos));
            }
            else { // Invalid Character Exception 
                throw Exception("Invalid character: '" + std::string(1, curChar) + "' " + curPos.toString());
            }
            getNext();
        }

        tokens.push_back(Token(END, "END"));
        return tokens;
    }
};