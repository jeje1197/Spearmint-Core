#pragma once

#include <string>
#include <unordered_set>

#include "Position.h"

enum TokenType {
    NEWLINE,
    KEYWORD,
    ID,
    INT,
    FLOAT,
    STRING,
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    LBRACKET,
    RBRACKET,
    COLON,
    DOT,
    COMMA,
    SEMICOLON,
    RARROW,
    OP,
    END,
    NULLTYPE
};

class Token {
public:
    int type;
    std::string value;
    Position pos;

    Token() {
        this->type = -1;
        this->value = "NULLTOK";
    }

    Token(int type, char c) {
        this->type = type;
        this->value = c;
    }

    Token(int type, std::string value) {
        this->type = type;
        this->value = value;
    }

    Token(int type, char c, Position& pos) {
        this->type = type;
        this->value = c;
        this->pos = pos.copy();
    }

    Token(int type, std::string value, Position& pos) {
        this->type = type;
        this->value = value;
        this->pos = pos.copy();
    }

    static Token getNullToken() {
        return Token(NULLTYPE, "NULL");
    }

    bool matches(int type) {
        return this->type == type;
    }

    bool matches(int type, std::string value) {
        return this->type == type && (this->value == value);
    }

    bool matches(int type, std::unordered_set<std::string> valueSet) {
        return this->type == type && (valueSet.find(this->value) != valueSet.end());
    }

    std::string toString() {
        return "(" + std::to_string(this->type) + ", " + this->value + ")";
    }
};