#pragma once

#include <string>
#include <unordered_map>

#include "exception/Exception.h"
#include "Classes.h"

class SymbolTable {
    std::unordered_map<std::string, Object_sPtr> symbol_table = std::unordered_map<std::string, Object_sPtr>();
    SymbolTable* parent = nullptr;

public:
    SymbolTable() {}

    SymbolTable(SymbolTable* parent) {
        this->parent = parent;
    }

    bool containsLocalKey(std::string key) {
        return symbol_table.find(key) != symbol_table.end();
    }

    bool containsKeyAnywhere(std::string key) {
        SymbolTable* cur = this;
        while (cur != nullptr) {
            if (cur->symbol_table.find(key) != cur->symbol_table.end()) {
                return true;
            }
            cur = cur->parent;
        }
        return false;
    }

    void addLocal(std::string key, Object_sPtr value) {
        symbol_table[key] = value;
    }

    void addGlobal(std::string key, Object_sPtr value) {
        SymbolTable* cur = this;
        while (cur->parent != nullptr) {
            cur = cur->parent;
        }
        cur->addLocal(key, value);
    }

    void update(std::string key, Object_sPtr value) {
        SymbolTable* cur = this;
        while (cur != nullptr) {
            if (cur->symbol_table.find(key) != cur->symbol_table.end()) {
                cur->symbol_table[key] = value;
                return;
            }
            cur = cur->parent;
        }
    }

    Object_sPtr get(std::string key) {
        SymbolTable* cur = this;
        while (cur != nullptr) {
            if (cur->symbol_table.find(key) != cur->symbol_table.end()) {
                return cur->symbol_table.at(key);
            }
            cur = cur->parent;
        }
        throw Exception("'" + key + "' is not in scope.");
        return NullType::getNullType();
    }

    void remove(std::string key) {
        this->symbol_table.erase(key);
    }
};

typedef std::shared_ptr<SymbolTable> SymbolTable_sPtr;

class Context {
public:
    std::string name;
    SymbolTable_sPtr symbol_table;

    Context(std::string name, SymbolTable_sPtr symbol_table) {
        this->name = name;
        this->symbol_table = symbol_table;
    }

    Context generateNewContext(std::string name) {
        // Use shared_ptr variable to allocate space so the SymbolTable object doesn't go out of scope
        // and give us a nullptr when we return from the function
        return Context(name, SymbolTable_sPtr(new SymbolTable(symbol_table.get())));
    }
};
