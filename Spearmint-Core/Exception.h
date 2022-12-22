#pragma once

#include <string>
#include <iostream>
class Exception {
public:
    std::string message;

    Exception(std::string message) {
        this->message = message;
    }

    Exception(std::string message, char charToAppend) {
        this->message = message + charToAppend;
    }

    std::string getMessage() {
        return this->message;
    }
};