#pragma once

#include <string>

class Position {
private:
    std::string fn;
    int ln;
    int col;

public:
    Position() {
        this->fn = "N/A";
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
        return "at line: " + std::to_string(ln) + ", col: " + std::to_string(col) + 
            " in File: '" + fn + "'";
    }
};