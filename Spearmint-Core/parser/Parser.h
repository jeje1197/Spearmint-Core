#pragma once

#include <string>
#include <vector>
#include <unordered_set>

#include "exception/Exception.h"
#include "lexer/Token.h"
#include "AstNode.h"

class Parser {
private:
    std::vector<Token>* tokens;
    int index;
    Token curTok;

    std::vector<AstNode> importStatements;

public:
    Parser(std::vector<Token>& tokens) {
        this->tokens = &tokens;
        this->index = -1;
        this->curTok = getNext();
    }

private:
    bool hasNext(int steps = 1) {
        return index + steps < (int) tokens->size();
    }

    Token getNext() {
        if (hasNext()) {
            index++;
            curTok = tokens->at(index);
        }
        else {
            curTok = Token::getNullToken();
        }
        return curTok;
    }

    Token lookAhead(int steps = 1) {
        return hasNext(steps) ? tokens->at((int)(index + steps)) : Token::getNullToken();
    }

    void returnToIndex(int i) {
        index = i-1;
        getNext();
    }

    void skipSemis() {
        while (curTok.type == SEMICOLON) {
            getNext();
        }
    }

public:
    // Parse entry point
    std::vector<AstNode> parse() {
        std::vector<AstNode> ast = statements(END);
        if (!curTok.matches(END)) {
            throw Exception("Did not reach end of input! " + curTok.pos.toString());
        }
        return ast;
    }

private:
    std::vector<AstNode> statements(int ENDTYPE) {
        std::vector<AstNode> statements;

        skipSemis();
        while (!curTok.matches(ENDTYPE) || !curTok.matches(END)) {
            AstNode statement_node = statement();
            if (statement_node == nullptr) break;

            if (!curTok.matches(SEMICOLON)) {
                throw Exception("Expected semicolon after statement " + curTok.pos.toString());
            }
            statements.push_back(statement_node);
            skipSemis();
        }

        return statements;
    }

    AstNode statement() {
        while (curTok.matches(KEYWORD, "import")) {
            importStatement();
        }

        if ((curTok.matches(KEYWORD, "const") || curTok.matches(KEYWORD, "var")) &&
            lookAhead().matches(ID)) {
            return varDeclaration();
        }
        else if (curTok.matches(ID) && lookAhead().matches(OP, "=")) {
            return varAssign();
        }
        else if (curTok.matches(KEYWORD, "if")) {
            return ifStatement();
        }
        else if (curTok.matches(KEYWORD, "for")) {
            return forStatement();
        }
        else if (curTok.matches(KEYWORD, "while")) {
            return whileStatement();
        }
        else if (curTok.matches(KEYWORD, "break")) {
            return breakStatement();
        }
        else if (curTok.matches(KEYWORD, "continue")) {
            return continueStatement();
        }
        else if (curTok.matches(KEYWORD, "fn")) {
            return functionDef();
        }
        else if (curTok.matches(KEYWORD, "return")) {
            return returnStatement();
        }
        else if (curTok.matches(KEYWORD, "type")) {
            return structureDef();
        }

        return expr(); // Can return nullptr
    }

    void importStatement() {
        if (!curTok.matches(KEYWORD, "import")) {
            throw Exception("Expected keyword 'import' " + curTok.pos.toString());
        }
        getNext();

        if (!curTok.matches(STRING)) {
            throw Exception("Expected module name " + curTok.pos.toString());
        }
        Token fileNameTok = curTok;
        getNext();

        this->importStatements.push_back(AstNode(new ImportNode(fileNameTok)));
    }

    // Parse a type name
    // id(.id)*(\[\])*
    AstNode typeExpr() {
        AstNode typeExprNode = nullptr;
        int resetIndex = this->index;

        if (!curTok.matches(ID)) {
            returnToIndex(resetIndex);
            return nullptr;
        }
        getNext();

        while (curTok.matches(DOT) && lookAhead().matches(ID)) {
            // Access subtype
            getNext();
            getNext();
        }

        while (curTok.matches(LBRACKET) && lookAhead().matches(RBRACKET)) {
            // Array of type
            getNext();
            getNext();
        }
        return typeExprNode;
    }

    AstNode varDeclaration() {
        bool isConstant = false;
        
        if (curTok.matches(KEYWORD, "const")) {
            isConstant = true;
            getNext();
        }

        if (!curTok.matches(KEYWORD, "var")) {
            throw Exception("Expected 'var keyword' " + curTok.pos.toString());
        }
        getNext();

        if (!curTok.matches(ID)) {
            throw Exception("Expected identifier " + curTok.pos.toString());
        }
        Token varNameTok = curTok;
        getNext();

        // Optional type declaration
        if (curTok.matches(COLON)) {
            getNext();

            AstNode typeExprNode = typeExpr();
            if (typeExprNode == nullptr) {
                throw Exception("Expected type after ':' " + curTok.pos.toString());
            }
        }

        if (!curTok.matches(OP, "=")) {
            throw Exception("Expected '=' " + curTok.pos.toString());
        }
        getNext();

        AstNode expr_node = expr();
        if (expr_node == nullptr) {
            throw Exception("Expected expression " + curTok.pos.toString());
        }

        return AstNode(new VarDeclarationNode(varNameTok, expr_node, isConstant));
    }

    AstNode varAssign() {
        if (!curTok.matches(ID)) {
            throw Exception("Expected identifier");
        }
        Token varNameTok = curTok;
        getNext();

        if (!curTok.matches(OP, "=")) {
            throw Exception("Expected '='");
        }
        getNext();

        AstNode expr_node = expr();
        if (expr_node == nullptr) {
            throw Exception("Expected expression");
        }

        return AstNode(new VarAssignNode(varNameTok, expr_node));
    }

    AstNode ifStatement() {
        std::vector<AstNode> caseConditions;
        std::vector<std::vector<AstNode>> caseStatements;
        std::vector<AstNode> elseCaseStatements;

        // if
        if (!curTok.matches(KEYWORD, "if")) {
            throw Exception("Expected keyword 'if'");
        }
        getNext();

        if (!curTok.matches(LPAREN)) {
            throw Exception("Expected '('");
        }
        getNext();

        AstNode cond_node = expr();
        if (cond_node == nullptr) {
            throw Exception("Expected conditional expression");
        }

        caseConditions.push_back(cond_node);

        if (!curTok.matches(RPAREN)) {
            throw Exception("Expected ')'");
        }
        getNext();

        if (!curTok.matches(LBRACE)) {
            throw Exception("Expected '{'");
        }
        getNext();

        caseStatements.push_back(statements(RBRACE));

        if (!curTok.matches(RBRACE)) {
            throw Exception("Expected '}'");
        }
        getNext();


        // else if
        while (curTok.matches(KEYWORD, "else") && lookAhead().matches(KEYWORD, "if")) {
            getNext();
            getNext();

            if (!curTok.matches(LPAREN)) {
                throw Exception("Expected '('");
            }
            getNext();

            cond_node = expr();
            if (cond_node == nullptr) {
                throw Exception("Expected conditional expression");
            }

            caseConditions.push_back(cond_node);

            if (!curTok.matches(RPAREN)) {
                throw Exception("Expected ')'");
            }
            getNext();

            if (!curTok.matches(LBRACE)) {
                throw Exception("Expected '{'");
            }
            getNext();

            caseStatements.push_back(statements(RBRACE));

            if (!curTok.matches(RBRACE)) {
                throw Exception("Expected '}'");
            }
            getNext();
        }


        // else
        if (curTok.matches(KEYWORD, "else")) {
            getNext();

            if (!curTok.matches(LBRACE)) {
                throw Exception("Expected '{'");
            }
            getNext();

            elseCaseStatements = statements(RBRACE);

            if (!curTok.matches(RBRACE)) {
                throw Exception("Expected '}'");
            }
            getNext();
        }

        return AstNode(new IfNode(caseConditions, caseStatements, elseCaseStatements));
    }

    AstNode forStatement() {
        if (!curTok.matches(KEYWORD, "for")) {
            throw Exception("Expected keyword 'for'");
        }
        getNext();

        if (!curTok.matches(LPAREN)) {
            throw Exception("Expected '('");
        }
        getNext();

        AstNode init_statement = statement();
        if (init_statement == nullptr) {
            throw Exception("Expected statement");
        }

        if (!curTok.matches(SEMICOLON)) {
            throw Exception("Expected ';'");
        }
        getNext();

        AstNode cond_node = expr();
        if (cond_node == nullptr) {
            throw Exception("Expected conditional expression");
        }

        if (!curTok.matches(SEMICOLON)) {
            throw Exception("Expected ';'");
        }
        getNext();

        AstNode update_statement = statement();
        if (update_statement == nullptr) {
            throw Exception("Expected statement");
        }

        if (!curTok.matches(RPAREN)) {
            throw Exception("Expected ')'");
        }
        getNext();

        if (!curTok.matches(LBRACE)) {
            throw Exception("Expected '{'");
        }
        getNext();

        std::vector<AstNode> statement_list = statements(RBRACE);

        if (!curTok.matches(RBRACE)) {
            throw Exception("Expected '}'");
        }
        getNext();

        return AstNode(new ForNode(init_statement, cond_node, update_statement, statement_list));
    }

    AstNode whileStatement() {
        if (!curTok.matches(KEYWORD, "while")) {
            throw Exception("Expected keyword 'while'");
        }
        getNext();

        if (!curTok.matches(LPAREN)) {
            throw Exception("Expected '('");
        }
        getNext();

        AstNode cond_node = expr();
        if (cond_node == nullptr) {
            throw Exception("Expected conditional expression");
        }

        if (!curTok.matches(RPAREN)) {
            throw Exception("Expected ')'");
        }
        getNext();

        if (!curTok.matches(LBRACE)) {
            throw Exception("Expected '{'");
        }
        getNext();

        std::vector<AstNode> statement_list = statements(RBRACE);

        if (!curTok.matches(RBRACE)) {
            throw Exception("Expected '}'");
        }
        getNext();

        return AstNode(new WhileNode(cond_node, statement_list));
    }

    AstNode functionDef() {
        if (!curTok.matches(KEYWORD, "fn")) {
            throw Exception("Expected keyword 'fun'");
        }
        getNext();

        if (!curTok.matches(ID)) {
            throw Exception("Expected identifier");
        }
        Token functionNameTok = curTok;
        getNext();

        std::vector<std::string> argNames;
        if (!curTok.matches(LPAREN)) {
            throw Exception("Expected '('");
        }
        getNext();

        if (curTok.matches(ID)) {
            argNames.push_back(curTok.value);
            getNext();

            while (curTok.matches(COMMA)) {
                getNext();

                if (!curTok.matches(ID)) {
                    throw Exception("Expected identifier after ','");
                }
                argNames.push_back(curTok.value);
                getNext();
            }
        }

        if (!curTok.matches(RPAREN)) {
            throw Exception("Expected ')'");
        }
        getNext();

        if (!curTok.matches(LBRACE)) {
            throw Exception("Expected '{'");
        }
        getNext();

        std::vector<AstNode> statement_list = statements(RBRACE);

        if (!curTok.matches(RBRACE)) {
            throw Exception("Expected '}'");
        }
        getNext();

        return AstNode(new FunctionDefNode(functionNameTok, argNames, statement_list));
    }

    AstNode returnStatement() {
        if (!curTok.matches(KEYWORD, "return")) {
            throw Exception("Expected keyword 'return'");
        }
        getNext();

        AstNode expr_node = expr();
        return AstNode(new ReturnNode(expr_node));
    }

    AstNode breakStatement() {
        if (!curTok.matches(KEYWORD, "break")) {
            throw Exception("Expected keyword 'break'");
        }
        getNext();

        return AstNode(new BreakNode());
    }

    AstNode continueStatement() {
        if (!curTok.matches(KEYWORD, "continue")) {
            throw Exception("Expected keyword 'continue'");
        }
        getNext();

        return AstNode(new ContinueNode());
    }

    AstNode structureDef() {
        if (!curTok.matches(KEYWORD, "type")) {
            throw Exception("Expected keyword 'type'");
        }
        getNext();

        if (!curTok.matches(ID)) {
            throw Exception("Expected structure name");
        }
        Token classNameTok = curTok;
        getNext();

        if (!curTok.matches(LBRACE)) {
            throw Exception("Expected '{'");
        }
        getNext();

        std::vector<AstNode> classStatements = statements(RBRACE);

        if (!curTok.matches(RBRACE)) {
            throw Exception("Expected '}'");
        }
        getNext();

        return AstNode(new StructureDefNode(classNameTok, classStatements));
    }

    // Expression Parsing
    AstNode expr() {
        return comp_expr();
    }

    AstNode comp_expr() {
        return binOp(&Parser::comp_expr2, { "&&", "||" }, &Parser::comp_expr2);
    }

    AstNode comp_expr2() {
        return binOp(&Parser::arith_expr, { "<", ">", "<=", ">=", "==", "!=" }, &Parser::arith_expr);
    }

    AstNode arith_expr() {
        return binOp(&Parser::term, { "+", "-" }, &Parser::term);
    }

    AstNode term() {
        return binOp(&Parser::power, { "*", "/", "%" }, &Parser::power);
    }

    AstNode power() {
        return binOp(&Parser::modifier, { "^" }, &Parser::power);
    }

    AstNode modifier() {
        AstNode returnNode = atom();

        // Match function call, attribute access, index access or attribute assignment
        while (curTok.matches(LPAREN) || curTok.matches(DOT) || curTok.matches(LBRACKET) ||
            curTok.matches(OP, "=")) {
            returnNode = call(returnNode);
            returnNode = attributeAccess(returnNode);
            //returnNode = indexAccess(returnNode);
            returnNode = attributeAssign(returnNode);
        }

        return returnNode;
    }

    AstNode call(AstNode node) {
        while (curTok.matches(LPAREN)) {
            getNext();
            std::vector<AstNode> argNodes;

            AstNode expr_node = expr();
            if (expr_node != nullptr) {
                argNodes.push_back(expr_node);
            }

            while (curTok.matches(COMMA)) {
                getNext();

                expr_node = expr();
                if (expr_node == nullptr) {
                    throw Exception("Expected expression after ','");
                }

                argNodes.push_back(expr_node);
            }

            if (!curTok.matches(RPAREN)) {
                throw Exception("Expected ')'");
            }
            getNext();

            node = AstNode(new FunctionCallNode(node, argNodes));
        }
        return node;
    }

    AstNode attributeAccess(AstNode node) {
        while (curTok.matches(DOT)) {
            getNext();

            if (!curTok.matches(ID)) {
                throw Exception("Expected attribute name after .");
            }
            Token attributeToken = curTok;
            getNext();

            node = AstNode(new AttributeAccessNode(node, attributeToken));
        }
        return node;
    }

    /*AstNode indexAccess(AstNode node) {
        while (curTok.matches(LBRACKET)) {
            Token startTok = curTok;
            getNext();

            AstNode exprNode = expr();
            if (exprNode == nullptr) {
                throw new Exception("Expected value after " + startTok.value);
            }

            if (!curTok.matches(RBRACKET)) {
                throw new Exception("Unclosed bracket");
            }
            getNext();

            node = AstNode(new IndexAccessNode(node, exprNode));
        }
        return node;
    }*/

    AstNode attributeAssign(AstNode node) {
        if (curTok.matches(OP, "=")) {
            getNext();

            AstNode valueNode = expr();
            if (valueNode == nullptr) {
                throw Exception("Expected value after '='");
            }

            node = AstNode(new AttributeAssignNode(node, valueNode));
        }
        return node;
    }

    /*AstNode indexAssign(AstNode node) {
        if (curTok.matches(OP, "=")) {
            getNext();

            AstNode valueNode = expr();
            if (valueNode == nullptr) {
                throw Exception("Expected value after '='");
            }

            node = AstNode(new AttributeAssignNode(node, valueNode));
        }
        return node;
    }*/

    AstNode atom() {
        Token tok = curTok;
        std::unordered_set<std::string> unaryOps = { "+", "-", "!"};

        if (tok.matches(OP, unaryOps)) { // Unary Operation
            getNext();

            AstNode node = atom();
            if (node == nullptr) {
                throw Exception("Expected atom after unary operator");
            }
            return AstNode(new UnaryOpNode(tok, node));
        }
        else if (curTok.matches(KEYWORD, "new")) { // Contructor Call
            getNext();

            AstNode structDefNode = expr();
            if (structDefNode == nullptr) {
                throw new Exception("Expected constructor call after new keyword");
            }
            return AstNode(new ConstructorCallNode(structDefNode));
        }
        else if (tok.matches(INT)) { // Integer
            getNext();
            return AstNode(new IntNode(tok));
        }
        else if (tok.matches(FLOAT)) { // Float
            getNext();
            return AstNode(new FloatNode(tok));
        }
        else if (tok.matches(STRING)) { // String
            getNext();
            return AstNode(new StringNode(tok));
        }
        else if (tok.matches(ID)) { // Variable Access
            getNext();
            return AstNode(new VarAccessNode(tok));
        }
        else if (tok.matches(LBRACKET)) { // List Creation
            getNext();
            std::vector<AstNode> listValueNodes;

            AstNode expr_node = expr();
            if (expr_node != nullptr) {
                listValueNodes.push_back(expr_node);
            }

            while (curTok.matches(COMMA)) {
                getNext();

                expr_node = expr();
                if (expr_node == nullptr) {
                    throw Exception("Expected expression after ','");
                }

                listValueNodes.push_back(expr_node);
            }

            if (!curTok.matches(RBRACKET)) {
                throw Exception("Expected ')'");
            }
            getNext();
            return AstNode(new ListNode(listValueNodes));
        }
        else if (tok.matches(LPAREN)) { // Parenthesis
            getNext();
            AstNode exprNode = expr();
            if (exprNode == nullptr) {
                throw Exception("Expected expr after parenthesis");
            }

            if (!curTok.matches(RPAREN)) {
                throw Exception("Expected ')'");
            }
            getNext();
            return exprNode;
        }

        return nullptr;
    }

    typedef AstNode(Parser::* ParserFunction)();
    AstNode binOp(ParserFunction func1, std::unordered_set<std::string> ops, ParserFunction func2) {
        AstNode left = ((*this).*func1)();

        while (curTok.matches(OP, ops)) {
            Token opTok = curTok;
            getNext();

            AstNode right = ((*this).*func2)();
            if (right == nullptr) {
                throw Exception("Expected expr after operator");
            }
            left = AstNode(new BinOpNode(left, opTok, right));
        }

        return left;
    }
};