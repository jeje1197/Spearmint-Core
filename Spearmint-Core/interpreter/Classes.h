#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <cmath>
#include <unordered_map>
#include <iterator>

#include "parser/AstNode.h"

class Object;
typedef std::shared_ptr<Object> Object_sPtr;

// Base Object in Spearmint
class Object {
private:
    std::string type;

public:
    Object(std::string type) {
        this->type = type;
    }

    virtual ~Object() = default;

    std::string getType() {
        return type;
    }

    bool isInstance(Object_sPtr obj, std::string type) {
        return obj->type.compare(type) == 0;
    }

    Object_sPtr illegalOperation() {
        throw Exception("Operation cannot be performed on " + this->type);
        return nullptr;
    }

    virtual int getIntValue() {
        illegalOperation();
        return 0;
    }

    virtual float getFloatValue() {
        illegalOperation();
        return 0.0f;
    }

    virtual bool getBooleanValue() {
        illegalOperation();
        return true;
    }

    // Operator methods
    virtual Object_sPtr add(Object_sPtr other) {
        return illegalOperation();
    }

    virtual Object_sPtr sub(Object_sPtr other) {
        return illegalOperation();
    }

    virtual Object_sPtr mul(Object_sPtr other) {
        return illegalOperation();
    }

    virtual Object_sPtr div(Object_sPtr other) {
        return illegalOperation();
    }

    virtual Object_sPtr pow(Object_sPtr other) {
        return illegalOperation();
    }

    virtual Object_sPtr mod(Object_sPtr other) {
        return illegalOperation();
    }

    virtual Object_sPtr compare_lt(Object_sPtr other) {
        return illegalOperation();
    }

    virtual Object_sPtr compare_gt(Object_sPtr other) {
        return illegalOperation();
    }

    virtual Object_sPtr compare_lte(Object_sPtr other) {
        return illegalOperation();
    }

    virtual Object_sPtr compare_gte(Object_sPtr other) {
        return illegalOperation();
    }

    virtual Object_sPtr compare_ee(Object_sPtr other) {
        return illegalOperation();
    }

    virtual Object_sPtr compare_ne(Object_sPtr other) {
        return illegalOperation();
    }

    virtual Object_sPtr anded_by(Object_sPtr other) {
        return illegalOperation();
    }

    virtual Object_sPtr ored_by(Object_sPtr other) {
        return illegalOperation();
    }

    virtual Object_sPtr notted() {
        return illegalOperation();
    }

    Object_sPtr getRef();

    virtual bool is_true() {
        return true;
    }

    std::string getAddress() {
        const void* address = static_cast<const void*>(this);
        std::stringstream ss;
        ss << address;
        return ss.str();
    }

    uint64_t getAddressAsInt() {
        return (uint64_t) this;
    }

    virtual std::string toString() {
        return type + " at " + getAddress();
    }

    virtual Object_sPtr copy() {
        return (Object_sPtr)this;
    }

    // Extra for VariableWrappers
    virtual void storeObject(Object_sPtr obj) {
        illegalOperation();
    }

    virtual Object_sPtr getObject() {
        return illegalOperation();
    }

    virtual bool isConstant() {
        illegalOperation();
        return false;
    }

    virtual int getLength() {
        illegalOperation();
        return 0;
    }

    virtual Object_sPtr getIndex(Object_sPtr index) {
        return illegalOperation();
    }

    virtual Object_sPtr getField(std::string name) {
        return illegalOperation();
    }

    virtual Object_sPtr createInstance() {
        return illegalOperation();
    }
};

class NullType : public Object {
private:
    NullType() : Object("Null") {}

public:
    static Object_sPtr getNullType() {
        return Object_sPtr(new NullType());
    }

    bool isTrue() {
        return false;
    }
};

class Boolean : public Object {
private:
    bool value;

public:
    Boolean(bool value) : Object("Boolean") {
        this->value = value;
    }

    Boolean(float value) : Object("Boolean") {
        this->value = value != 0;
    }

    std::string toString() {
        return this->value ? "true" : "false";
    }

    Object_sPtr anded_by(Object_sPtr other) {
        return Object_sPtr(new Boolean(is_true() && other->is_true()));
    }

    Object_sPtr ored_by(Object_sPtr other) {
        return Object_sPtr(new Boolean(is_true() || other->is_true()));
    }

    Object_sPtr notted() {
        return Object_sPtr(new Boolean(!is_true()));
    }

    bool is_true() {
        return this->value;
    }

    Object_sPtr copy() {
        return Object_sPtr(new Boolean(this->value));
    }
};

class String : public Object {
private:
    std::string value;

public:
    String(std::string value) : Object("String") {
        this->value = value;
    }

    std::string toString() {
        return value;
    }

    // Operations
    Object_sPtr add(Object_sPtr other) {
        return Object_sPtr(new String(this->toString() + other->toString()));
    }

    Object_sPtr compare_lt(Object_sPtr other) {
        if (isInstance(other, "String")) {
            return Object_sPtr(new Boolean(this->toString().compare(other->toString()) < 0));
        }
        return illegalOperation();
    }

    Object_sPtr compare_gt(Object_sPtr other) {
        if (isInstance(other, "String")) {
            return Object_sPtr(new Boolean(this->toString().compare(other->toString()) > 0));
        }
        return illegalOperation();
    }

    Object_sPtr compare_lte(Object_sPtr other) {
        if (isInstance(other, "String")) {
            return Object_sPtr(new Boolean(this->toString().compare(other->toString()) <= 0));
        }
        return illegalOperation();
    }

    Object_sPtr compare_gte(Object_sPtr other) {
        if (isInstance(other, "String")) {
            return Object_sPtr(new Boolean(this->toString().compare(other->toString()) >= 0));
        }
        return illegalOperation();
    }

    Object_sPtr compare_ee(Object_sPtr other) {
        if (isInstance(other, "String")) {
            return Object_sPtr(new Boolean(this->toString().compare(other->toString()) == 0));
        }
        return illegalOperation();
    }

    Object_sPtr compare_ne(Object_sPtr other) {
        if (isInstance(other, "String")) {
            return Object_sPtr(new Boolean(this->toString().compare(other->toString()) != 0));
        }
        return illegalOperation();
    }

    Object_sPtr notted() {
        return Object_sPtr(new Boolean(this->toString().compare("") == 0));
    }

    bool is_true() {
        return (int)this->toString().length() != 0;
    }

    int getLength() {
        return (int)value.length();
    }

    Object_sPtr getSize();

    Object_sPtr getIndex(Object_sPtr other) {
        if (!isInstance(other, "Int")) {
            throw Exception("Index get method requires an int as argument.");
        }
        int index = other->getIntValue();
        if (index < 0 || index >= (int)toString().size()) {
            throw Exception("Index " + std::to_string(index) + " is out of list bounds.");
        }

        return Object_sPtr(new String(std::string(1, this->toString().at(index))));
    }

    Object_sPtr copy() {
        return Object_sPtr(new String(this->toString()));
    }
};

class Float : public Object {
private:
    float value;

public:
    Float(float value) : Object("Float") {
        this->value = value;
    }

    std::string toString() {
        return std::to_string(this->value);
    }

    int getIntValue() {
        return (int) this->value;
    }

    float getFloatValue() {
        return this->value;
    }

    Object_sPtr add(Object_sPtr other) {
        if (isInstance(other, "Int") || isInstance(other, "Float")) {
            return Object_sPtr(new Float(this->getFloatValue() + other->getFloatValue()));
        }
        else if (isInstance(other, "String")) {
            return Object_sPtr(new String(this->toString() + other->toString()));
        }
        return illegalOperation();
    }

    Object_sPtr sub(Object_sPtr other) {
        if (isInstance(other, "Int") || isInstance(other, "Float")) {
            return Object_sPtr(new Float(this->getFloatValue() - other->getFloatValue()));
        }
        return illegalOperation();
    }

    Object_sPtr mul(Object_sPtr other) {
        if (isInstance(other, "Int") || isInstance(other, "Float")) {
            return Object_sPtr(new Float(this->getFloatValue() * other->getFloatValue()));
        }
        return illegalOperation();
    }

    Object_sPtr div(Object_sPtr other) {
        if (isInstance(other, "Int") || isInstance(other, "Float")) {
            return Object_sPtr(new Float(this->getFloatValue() / other->getFloatValue()));
        }
        return illegalOperation();
    }

    Object_sPtr pow(Object_sPtr other) {
        if (isInstance(other, "Int")) {
            return Object_sPtr(new Float((float)std::pow(this->getFloatValue(), other->getIntValue())));
        }
        return illegalOperation();
    }

    Object_sPtr mod(Object_sPtr other) {
        if (isInstance(other, "Int") || isInstance(other, "Float")) {
            return Object_sPtr(new Float(std::fmod(this->getFloatValue(), other->getFloatValue())));
        }
        return illegalOperation();
    }

    Object_sPtr compare_lt(Object_sPtr other) {
        if (isInstance(other, "Int") || isInstance(other, "Float")) {
            return Object_sPtr(new Boolean(this->getFloatValue() < other->getFloatValue()));
        }
        return illegalOperation();
    }

    Object_sPtr compare_gt(Object_sPtr other) {
        if (isInstance(other, "Int") || isInstance(other, "Float")) {
            return Object_sPtr(new Boolean(this->getFloatValue() > other->getFloatValue()));
        }
        return illegalOperation();
    }

    Object_sPtr compare_lte(Object_sPtr other) {
        if (isInstance(other, "Int") || isInstance(other, "Float")) {
            return Object_sPtr(new Boolean(this->getFloatValue() <= other->getFloatValue()));
        }
        return illegalOperation();
    }

    Object_sPtr compare_gte(Object_sPtr other) {
        if (isInstance(other, "Int") || isInstance(other, "Float")) {
            return Object_sPtr(new Boolean(this->getFloatValue() >= other->getFloatValue()));
        }
        return illegalOperation();
    }

    Object_sPtr compare_ee(Object_sPtr other) {
        if (isInstance(other, "Int") || isInstance(other, "Float")) {
            return Object_sPtr(new Boolean(this->getFloatValue() == other->getFloatValue()));
        }
        return illegalOperation();
    }

    Object_sPtr compare_ne(Object_sPtr other) {
        if (isInstance(other, "Int") || isInstance(other, "Float")) {
            return Object_sPtr(new Boolean(this->getFloatValue() != other->getFloatValue()));
        }
        return illegalOperation();
    }

    Object_sPtr notted() {
        return Object_sPtr(new Boolean(!is_true()));
    }

    bool is_true() {
        return this->getFloatValue() != 0;
    }

    Object_sPtr copy() {
        return Object_sPtr(new Float(this->getFloatValue()));
    }
};

class Int : public Object {
private:
    int value;
public:
    Int(int value) : Object("Int") {
        this->value = value;
    }

    std::string toString() {
        return std::to_string(this->value);
    }

    int getIntValue() {
        return this->value;
    }

    float getFloatValue() {
        return (float) this->value;
    }

    Object_sPtr add(Object_sPtr other) {
        if (isInstance(other, "Int")) {
            return Object_sPtr(new Int(this->getIntValue() + other->getIntValue()));
        } else if (isInstance(other, "Float")) {
            return Object_sPtr(new Float(this->getFloatValue() + other->getFloatValue()));
        }
        else if (isInstance(other, "String")) {
            return Object_sPtr(new String(this->toString() + other->toString()));
        }
        return illegalOperation();
    }

    Object_sPtr sub(Object_sPtr other) {
        if (isInstance(other, "Int")) {
            return Object_sPtr(new Int(this->getIntValue() - other->getIntValue()));
        } else if (isInstance(other, "Float")) {
            return Object_sPtr(new Float(this->getFloatValue() - other->getFloatValue()));
        }
        return illegalOperation();
    }

    Object_sPtr mul(Object_sPtr other) {
        if (isInstance(other, "Int")) {
            return Object_sPtr(new Int(this->getIntValue() * other->getIntValue()));
        } else if (isInstance(other, "Float")) {
            return Object_sPtr(new Float(this->getFloatValue() * other->getFloatValue()));
        }
        return illegalOperation();
    }

    Object_sPtr div(Object_sPtr other) {
        if (isInstance(other, "Int")) {
            return Object_sPtr(new Int(this->getIntValue() / other->getIntValue()));
        }
        else if (isInstance(other, "Float")) {
            return Object_sPtr(new Float(this->getFloatValue() / other->getFloatValue()));
        }
        return illegalOperation();
    }

    Object_sPtr pow(Object_sPtr other) {
        if (isInstance(other, "Int")) {
            return Object_sPtr(new Float((float)std::pow(this->getFloatValue(), other->getIntValue())));
        }
        return illegalOperation();
    }

    Object_sPtr mod(Object_sPtr other) {
        if (isInstance(other, "Int")) {
            return Object_sPtr(new Float(std::fmod(this->getFloatValue(), other->getFloatValue())));
        }
        return illegalOperation();
    }

    Object_sPtr compare_lt(Object_sPtr other) {
        if (isInstance(other, "Int") || isInstance(other, "Float")) {
            return Object_sPtr(new Boolean(this->getFloatValue() < other->getFloatValue()));
        }
        return illegalOperation();
    }

    Object_sPtr compare_gt(Object_sPtr other) {
        if (isInstance(other, "Int") || isInstance(other, "Float")) {
            return Object_sPtr(new Boolean(this->getFloatValue() > other->getFloatValue()));
        }
        return illegalOperation();
    }

    Object_sPtr compare_lte(Object_sPtr other) {
        if (isInstance(other, "Int") || isInstance(other, "Float")) {
            return Object_sPtr(new Boolean(this->getFloatValue() <= other->getFloatValue()));
        }
        return illegalOperation();
    }

    Object_sPtr compare_gte(Object_sPtr other) {
        if (isInstance(other, "Int") || isInstance(other, "Float")) {
            return Object_sPtr(new Boolean(this->getFloatValue() >= other->getFloatValue()));
        }
        return illegalOperation();
    }

    Object_sPtr compare_ee(Object_sPtr other) {
        if (isInstance(other, "Int") || isInstance(other, "Float")) {
            return Object_sPtr(new Boolean(this->getFloatValue() == other->getFloatValue()));
        }
        return illegalOperation();
    }

    Object_sPtr compare_ne(Object_sPtr other) {
        if (isInstance(other, "Int") || isInstance(other, "Float")) {
            return Object_sPtr(new Boolean(this->getFloatValue() != other->getFloatValue()));
        }
        return illegalOperation();
    }

    Object_sPtr notted() {
        return Object_sPtr(new Boolean(!is_true()));
    }

    bool is_true() {
        return this->getIntValue() != 0;
    }

    Object_sPtr copy() {
        return Object_sPtr(new Int(this->getIntValue()));
    }
};

Object_sPtr String::getSize() {
    return Object_sPtr(new Int((int)value.length()));
}

class VariableWrapper : public Object {
private:
    Object_sPtr obj = nullptr;
    bool constant_modifier = false; // 0 - none, 1 - const

public:
    VariableWrapper(Object_sPtr obj) : Object("VariableWrapper") {
        this->obj = obj;
    }

    VariableWrapper(Object_sPtr obj, bool isConstant) : Object("VariableWrapper") {
        this->obj = obj;
        this->constant_modifier = isConstant;
    }

    void storeObject(Object_sPtr obj) {
        this->obj = obj;
    }

    Object_sPtr getObject() {
        if (obj == nullptr) {
            throw Exception("Nullptr in VariableWrapper");
        }
        return obj;
    }

    bool isConstant() {
        return constant_modifier;
    }

    std::string toString() {
        if (obj == nullptr) {
            return "Null VariableWrapper";
        }
        return obj->toString();
    }

    Object_sPtr copy() {
        return illegalOperation();
    }
};

class List : public Object {
private:
    std::vector<Object_sPtr> myList;

public:
    List() : Object("List") {}

    int getLength() {
        return (int)this->myList.size();
    }

    Object_sPtr add(Object_sPtr newObj) {
        this->myList.push_back(newObj);
        return NullType::getNullType();
    }

    Object_sPtr sub(Object_sPtr other) {
        this->myList.erase(this->myList.begin() + other->getIntValue());
        return NullType::getNullType();
    }

    Object_sPtr notted() {
        return Object_sPtr(new Boolean(!is_true()));
    }

    bool is_true() {
        return !myList.empty();
    }

    Object_sPtr getInternal(int index) {
        return this->myList.at(index);
    }

    Object_sPtr getSize() {
        return Object_sPtr(new Int((int)myList.size()));
    }

    Object_sPtr getIndex(Object_sPtr numObj) {
        if (!isInstance(numObj, "Int")) {
            throw Exception("List get method requires a number object as argument.");
        }
        int index = numObj->getIntValue();
        if ((index < 0) || (index >= (int)myList.size())) {
            throw Exception("Index " + std::to_string(index) + " is out of list bounds.");
        }
        return this->myList.at(index);;
    }

    std::string toString() {
        std::string str = "[";
        for (int i = 0; i < (int)myList.size(); i++) {
            str += myList.at(i)->toString();
            if (i < (int)myList.size() - 1) {
                str += ", ";
            }
        }
        str += "]";
        return str;
    }
};

class Function : public Object {
public:
    std::string name;
    std::vector<std::string> argNames;
    std::vector<AstNode> statements;
    bool builtIn = false;
    Object_sPtr(*execute)(void*) = nullptr;

    bool bindToObject = false;
    Object_sPtr boundObject = nullptr;

    Function(std::string name, std::vector<std::string> argNames, std::vector<AstNode> statements) : Object("Function") {
        this->name = name;
        this->argNames = argNames;
        this->statements = statements;
    }

    Function(std::string name, std::vector<std::string> argNames, Object_sPtr(*execute)(void*)) : Object("Function") {
        this->name = name;
        this->argNames = argNames;
        this->execute = execute;
        this->builtIn = true;
    }

    bool isCallable() {
        return true;
    }

    bool checkNumArgs(std::vector<AstNode> other) {
        int numArgs = (int)argNames.size();
        int numPassedArgs = (int)other.size();

        if (numArgs != numPassedArgs) {
            throw Exception("Function '" + name + "' expected " + std::to_string(numArgs) + " args, but received " +
                std::to_string(numPassedArgs) + " args");
        }
        return true;
    }

    bool isBuiltIn() {
        return builtIn;
    }

    Object_sPtr executeWrapper(void* ctx) {
        if (execute == nullptr) {
            throw Exception("Built in method not defined for " + name);
        }

        // Should call function from function pointer (Object_sPtr (*)(void *))
        return execute(ctx);
    }

    std::string toString() {
        return "Function '" + name + "' (" + std::to_string((int)argNames.size()) + ") at address:" + getAddress();
    }

    Object_sPtr add(Object_sPtr other) {
        if (isInstance(other, "String")) {
            return Object_sPtr(new String(this->toString() + other->toString()));
        }
        return illegalOperation();
    }

    Object_sPtr compare_ee(Object_sPtr other) {
        if (isInstance(other, "Function")) {
            return Object_sPtr(new Boolean(this->getAddress() == other->getAddress()));
        }
        return illegalOperation();
    }

    Object_sPtr compare_ne(Object_sPtr other) {
        if (isInstance(other, "Function")) {
            return Object_sPtr(new Boolean(this->getAddress() != other->getAddress()));
        }
        return illegalOperation();
    }
};

class StructureDefinition : public Object {
public:
    std::string name;
    std::unordered_map<std::string, Object_sPtr> fields;

    StructureDefinition(std::string name) : Object(name) {
        this->name = name;
    }

    bool hasField(std::string key) {
        return fields.find(key) != fields.end();
    }

    void addField(std::string key, Object_sPtr value) {
        if (fields.find(key) != fields.end()) {
            throw Exception("Class '" + name + "' already has a '" + key + "' field.");
        }
        fields[key] = value;
    }

    Object_sPtr getField(std::string key) {
        if (fields.find(key) == fields.end()) {
            throw Exception(name + " does not have a '" + key + "' field.");
        }

        return fields.at(key);
    }

    std::string toString() {
        return "Structure <" + name + ">";
    }

    Object_sPtr createInstance() {
        std::shared_ptr<StructureDefinition> newInstance(new StructureDefinition(name));
        for (auto entry : fields) {

            newInstance->addField(entry.first, Object_sPtr(new VariableWrapper(entry.second->getObject(), entry.second->isConstant())));
        }
        return (Object_sPtr) newInstance;
    }

    Object_sPtr add(Object_sPtr other) {
        if (isInstance(other, "String")) {
            return Object_sPtr(new String(this->toString() + other->toString()));
        }
        return illegalOperation();
    }

    Object_sPtr compare_ee(Object_sPtr other) {
        if (isInstance(other, getType())) {
            return Object_sPtr(new Boolean(this->getAddress() == other->getAddress()));
        }
        return illegalOperation();
    }

    Object_sPtr compare_ne(Object_sPtr other) {
        if (isInstance(other, getType())) {
            return Object_sPtr(new Boolean(this->getAddress() != other->getAddress()));
        }
        return illegalOperation();
    }

    Object_sPtr notted() {
        return Object_sPtr(new Boolean(false));
    }
};