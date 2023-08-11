#pragma once

#include <string>
#include <iostream>

class Exception {
public:
    std::string message;

    Exception(std::string message) {
        this->message = message;
    }

    void show() {
        std::cout << "Exception: " + this->message << std::endl;
    }
};