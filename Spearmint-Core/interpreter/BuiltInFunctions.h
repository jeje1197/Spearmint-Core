#pragma once

#include "Classes.h"
#include "Context.h"

typedef std::shared_ptr<Function> Function_sPtr;

// Print function
Object_sPtr print(Context* ctx) {
    Object_sPtr varWrapper = ctx->symbol_table->get("text");
    Object_sPtr strObj = varWrapper->getObject();

    std::cout << strObj->toString();
    return NullType::getNullType();
}
Function_sPtr printFunction(new Function("print", { "text" }, (Object_sPtr(*)(void*))& print));

// Println function
Object_sPtr println(Context* ctx) {
    Object_sPtr varWrapper = ctx->symbol_table->get("text");
    Object_sPtr strObj = varWrapper->getObject();

    std::cout << strObj->toString() << "\n";
    return NullType::getNullType();
}
Function_sPtr printlnFunction(new Function("println", { "text" }, (Object_sPtr(*)(void*))& println));

// Get type
Object_sPtr typeof(Context* ctx) {
    Object_sPtr varWrapper = ctx->symbol_table->get("object");
    Object_sPtr obj = varWrapper->getObject();

    return Object_sPtr(new String(obj->getType()));
}
Function_sPtr typeFunction(new Function("typeof", { "object" }, (Object_sPtr(*)(void*))& typeof));

Object_sPtr intToFloat(Context* ctx) {
    Object_sPtr varWrapper = ctx->symbol_table->get("int");
    Object_sPtr obj = varWrapper->getObject();

    return Object_sPtr(new Float(obj->getFloatValue()));
}
Function_sPtr toIntFunction(new Function("intToFloat", { "int" }, (Object_sPtr(*)(void*))& intToFloat));

Object_sPtr floatToInt(Context* ctx) {
    Object_sPtr varWrapper = ctx->symbol_table->get("float");
    Object_sPtr obj = varWrapper->getObject();

    return Object_sPtr(new Int(obj->getIntValue()));
}
Function_sPtr toFloatFunction(new Function("floatToInt", { "float" }, (Object_sPtr(*)(void*))& floatToInt));

Object_sPtr stoi(Context* ctx) {
    Object_sPtr varWrapper = ctx->symbol_table->get("string");
    Object_sPtr obj = varWrapper->getObject();

    return Object_sPtr(new Int(stoi(obj->toString())));
}
Function_sPtr stoiFunction(new Function("stoi", {"string"}, (Object_sPtr(*)(void*))& stoi));

Object_sPtr stof(Context* ctx) {
    Object_sPtr varWrapper = ctx->symbol_table->get("string");
    Object_sPtr obj = varWrapper->getObject();

    return Object_sPtr(new Float(stof(obj->toString())));
}
Function_sPtr stofFunction(new Function("stof", {"string"}, (Object_sPtr(*)(void*))& stof));

// Is Object null 
Object_sPtr isNull(Context* ctx) {
    Object_sPtr varWrapper = ctx->symbol_table->get("object");
    Object_sPtr obj = varWrapper->getObject();

    return Object_sPtr(new Boolean(varWrapper->getObject()->getType() == "Null"));
}
Function_sPtr isNullFunction(new Function("isNull", { "object" }, (Object_sPtr(*)(void*))& isNull));

// Get len
Object_sPtr len(Context* ctx) {
    Object_sPtr varWrapper = ctx->symbol_table->get("object");
    Object_sPtr obj = varWrapper->getObject();

    return Object_sPtr(new Int(varWrapper->getObject()->getLength()));
}
Function_sPtr lenFunction(new Function("len", { "object" }, (Object_sPtr(*)(void*))& len));

// Read console input
Object_sPtr input(Context* ctx) {
    std::string input;
    getline(std::cin, input);
    return Object_sPtr(new String(input));
}
Function_sPtr inputFunction(new Function("input", {}, (Object_sPtr(*)(void*))& input));

// Exit program function
Object_sPtr closeProgram(Context* ctx) {
    exit(0);
    return NullType::getNullType();
}
Function_sPtr exitFunction(new Function("exit", {}, (Object_sPtr(*)(void*))& closeProgram));

// Function to add all built-in functions to SymbolTable
std::vector<Function_sPtr> BUILTINFUNCTIONS = { 
    printFunction, printlnFunction, typeFunction, stoiFunction, stofFunction, isNullFunction,
    lenFunction, inputFunction, exitFunction 
};

void addBuiltInFunctions(SymbolTable_sPtr symbol_table) {
    for (Function_sPtr funPtr : BUILTINFUNCTIONS) {
        symbol_table->addLocal(funPtr->name, Object_sPtr(new VariableWrapper(funPtr, true)));
    }
}