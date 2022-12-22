#pragma once

#include <string>
#include <unordered_set>
#include <sstream>

enum TokenType {
    NEWLINE,
    KEYWORD,
    ID,
    INT,
    DOUBLE,
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
    ARROW,
    OP,
    END,
    NULLTYPE
};

class Position {
public:
    std::string fn;
    int ln;
    int col;

    Position() {
        this->ln = 0;
        this->col = 0;
    }

    Position(std::string fn) {
        this->fn = fn;
        this->ln = 0;
        this->col = 0;
    }

    void advance(char c) {
        this->col++;
        if (c == '\n') {
            this->ln++;
            this->col = 0;
        }
    }

    Position copy() {
        Position p = Position();
        p.fn = this->fn;
        p.ln = this->ln;
        p.col = this->col;
        return p;
    }

    std::string toString() {
        return "at Line: " + std::to_string(ln) + " Col: " + std::to_string(col) + " in File: '" +
            fn + "'";
    }
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

    Token(int type, std::string value) {
        this->type = type;
        this->value = value;
    }

    Token(int type, std::string value, Position& pos) {
        this->type = type;
        this->value = value;
        this->pos = pos.copy();
    }

    Token(int type, char c) {
        this->type = type;
        this->value = c;
    }

    Token(int type, char c, Position& pos) {
        this->type = type;
        this->value = c;
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
};

std::ostream& operator<<(std::ostream& s, const Token& tok) {
    s << "(" << tok.type << ", " << tok.value << ")";
    return s;
}