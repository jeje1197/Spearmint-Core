#pragma once

#include <string>
#include <vector>
#include <memory>

#include "lexer/Token.h"

enum NodeType {
    NODE_IMPORT,
    NODE_VECTOR_WRAPPER,
    NODE_INT,
    NODE_FLOAT,
    NODE_STRING,
    NODE_UNARY_OP,
    NODE_BINARY_OP,
    NODE_VAR_DECLARATION,
    NODE_VAR_ASSIGN,
    NODE_VAR_ACCESS,
    NODE_IF,
    NODE_FOR,
    NODE_WHILE,
    NODE_FUNCTION_DEF,
    NODE_FUNCTION_CALL,
    NODE_RETURN,
    NODE_BREAK,
    NODE_CONTINUE,
    NODE_STRUCT_DEF,
    NODE_CONSTRUCTOR_CALL,
    NODE_ATTRIBUTE_ACCESS,
    NODE_ATTRIBUTE_ASSIGN,
    NODE_INDEX_ACCESS,
    NODE_INDEX_ASSIGN,
    NODE_LIST
};

class AstNodeBase {
public:
    int type;
    Position pos;

    AstNodeBase() {
        this->type = -1;
    }

    virtual ~AstNodeBase() = default;

    virtual std::string toString() {
        return "Error: AstNode.toString()";
    }
};

typedef std::shared_ptr<AstNodeBase> AstNode;

std::string stringListToString(std::vector<std::string>& list) {
    std::string output;

    for (int i = 0; i < (int)list.size(); i++) {
        output += list.at(i) + ", ";
    }
    return output;
}

std::string astListToString(std::vector<AstNode>& ast) {
    std::string output;

    for (int i = 0; i < (int)ast.size(); i++) {
        output += ast.at(i)->toString() + " ";
    }
    return output;
}

// This purpose of this is to be able to pass a vector to the
// visit method of the Interpreter
class VectorWrapperNode : public AstNodeBase {
public:
    std::vector<AstNode> vec;
    
    VectorWrapperNode(std::vector<AstNode> vec) {
        this->type = NODE_VECTOR_WRAPPER;
        this->vec = vec;
    }
};


class IntNode : public AstNodeBase {
public:
    int value;

    IntNode(Token& tok) {
        this->type = NODE_INT;
        this->value = stoi(tok.value);
    }
};

class FloatNode : public AstNodeBase {
public:
    float value;

    FloatNode(Token& tok) {
        this->type = NODE_FLOAT;
        this->value = stof(tok.value);
    }
};

class StringNode : public AstNodeBase {
public:
    std::string value;

    StringNode(Token& tok) {
        this->type = NODE_STRING;
        this->value = tok.value;
    }
};

class UnaryOpNode : public AstNodeBase {
public:
    std::string op;
    AstNode exprNode;

    UnaryOpNode(Token& opTok, AstNode exprNode) {
        this->type = NODE_UNARY_OP;
        this->op = opTok.value;
        this->exprNode = exprNode;
    }
};

class BinOpNode : public AstNodeBase {
public:
    AstNode left;
    std::string op;
    AstNode right;

    BinOpNode(AstNode left, Token& opTok, AstNode right) {
        this->type = NODE_BINARY_OP;
        this->left = left;
        this->op = opTok.value;
        this->right = right;
    }
};

class ImportNode : public AstNodeBase {
public:
    std::string fileToImport;

    ImportNode(Token& fileNameTok) {
        this->type = NODE_IMPORT;
        this->fileToImport = fileNameTok.value;
    }
};

class VarDeclarationNode : public AstNodeBase {
public:
    std::string varName;
    AstNode exprNode;
    bool isConstant;
    
    VarDeclarationNode(Token& varNameTok, AstNode exprNode, bool isConstant) {
        this->type = NODE_VAR_DECLARATION;
        this->varName = varNameTok.value;
        this->exprNode = exprNode;
        this->isConstant = isConstant;
    }
};


class VarAssignNode : public AstNodeBase {
public:
    std::string varName;
    AstNode exprNode;

    VarAssignNode(Token& varNameTok, AstNode exprNode) {
        this->type = NODE_VAR_ASSIGN;
        this->varName = varNameTok.value;
        this->exprNode = exprNode;
    }
};


class VarAccessNode : public AstNodeBase {
public:
    std::string varName;

    VarAccessNode(Token& tok) {
        this->type = NODE_VAR_ACCESS;
        this->varName = tok.value;
    }
};


class IfNode : public AstNodeBase {
public:
    std::vector<AstNode> caseConditions;
    std::vector<std::vector<AstNode>> caseStatements;
    std::vector<AstNode> elseCaseStatements;

    IfNode(std::vector<AstNode>& caseConditions, std::vector<std::vector<AstNode>>& caseStatements,
        std::vector<AstNode>& elseCaseStatements) {
        this->type = NODE_IF;
        this->caseConditions = caseConditions;
        this->caseStatements = caseStatements;
        this->elseCaseStatements = elseCaseStatements;
    }
};


class ForNode : public AstNodeBase {
public:
    AstNode initStatement, condNode, updateStatement;
    std::vector<AstNode> statements;

    ForNode(AstNode initStatement, AstNode condNode, AstNode updateStatement, std::vector<AstNode>& statements) {
        this->type = NODE_FOR;
        this->initStatement = initStatement;
        this->condNode = condNode;
        this->updateStatement = updateStatement;
        this->statements = statements;
    }
};


class WhileNode : public AstNodeBase {
public:
    AstNode condNode;
    std::vector<AstNode> statements;

    WhileNode(AstNode condNode, std::vector<AstNode>& statements) {
        this->type = NODE_WHILE;
        this->condNode = condNode;
        this->statements = statements;
    }
};


class FunctionDefNode : public AstNodeBase {
public:
    std::string name;
    std::vector<std::string> argNames;
    std::vector<AstNode> statements;

    FunctionDefNode(Token& functionNameTok, std::vector<std::string>& argNames, std::vector<AstNode>& statements) {
        this->type = NODE_FUNCTION_DEF;
        this->name = functionNameTok.value;
        this->argNames = argNames;
        this->statements = statements;
    }
};


class FunctionCallNode : public AstNodeBase {
public:
    AstNode nodeToCall;
    std::vector<AstNode> argNodes;

    FunctionCallNode(AstNode nodeToCall, std::vector<AstNode>& argNodes) {
        this->type = NODE_FUNCTION_CALL;
        this->nodeToCall = nodeToCall;
        this->argNodes = argNodes;
    }
};


class ReturnNode : public AstNodeBase {
public:
    AstNode exprNode;

    ReturnNode(AstNode exprNode) {
        this->type = NODE_RETURN;
        this->exprNode = exprNode;
    }
};


class BreakNode : public AstNodeBase {
public:
    BreakNode() {
        this->type = NODE_BREAK;
    }
};


class ContinueNode : public AstNodeBase {
public:
    ContinueNode() {
        this->type = NODE_CONTINUE;
    }
};


class StructureDefNode : public AstNodeBase {
public:
    std::string name;
    std::vector<AstNode> statements;

    StructureDefNode(Token& structNameTok, std::vector<AstNode>& statements) {
        this->type = NODE_STRUCT_DEF;
        this->name = structNameTok.value;
        this->statements = statements;
    }
};

class ConstructorCallNode : public AstNodeBase {
public:
    AstNode structureNode;

    ConstructorCallNode(AstNode structureNode) {
        this->type = NODE_CONSTRUCTOR_CALL;
        this->structureNode = structureNode;
    }
};

class AttributeAccessNode : public AstNodeBase {
public:
    AstNode exprNode;
    std::string name;

    AttributeAccessNode(AstNode exprNode, Token& attributeTok) {
        this->type = NODE_ATTRIBUTE_ACCESS;
        this->exprNode = exprNode;
        this->name = attributeTok.value;
    }
};

class ListNode : public AstNodeBase {
public:
    std::vector<AstNode> listValueNodes;

    ListNode(std::vector<AstNode> listValueNodes) {
        this->type = NODE_LIST;
        this->listValueNodes = listValueNodes;
    }
};

class IndexAccessNode : public AstNodeBase {
public:
    AstNode node;
    AstNode indexNode;

    IndexAccessNode(AstNode node, AstNode indexNode) {
        this->type = NODE_INDEX_ACCESS;
        this->node = node;
        this->indexNode = indexNode;
    }
};

class AttributeAssignNode : public AstNodeBase {
public:
    AstNode attrNode;
    AstNode exprNode;

    AttributeAssignNode(AstNode attrNode, AstNode exprNode) {
        this->type = NODE_ATTRIBUTE_ASSIGN;
        this->attrNode = attrNode;
        this->exprNode = exprNode;
    }
};

class IndexAssignNode : public AstNodeBase {
public:
    AstNode attrNode;
    AstNode exprNode;

    IndexAssignNode(AstNode attrNode, AstNode exprNode) {
        this->type = NODE_ATTRIBUTE_ASSIGN;
        this->attrNode = attrNode;
        this->exprNode = exprNode;
    }
};