#pragma once

#include <string>
#include <iostream>
#include <memory>

#include "exception/Exception.h"
#include "Classes.h"
#include "Context.h"

class Interpreter {
private:
	std::string fileName;

	Object_sPtr return_value = nullptr;
	bool should_return = false;
	bool should_break = false;
	bool should_continue = false;

    Object_sPtr Null_sPtr = NullType::getNullType();

public:
    Interpreter() {}
    Interpreter(std::string fileName) {
        this->fileName = fileName;
    }

    Object_sPtr visit(AstNode node, Context& ctx) {
        switch (node->type) {
        case NODE_VECTOR_WRAPPER:
            return visit_VectorWrapperNode(node, ctx);
        case NODE_INT:
            return visit_IntNode(node, ctx);
        case NODE_FLOAT:
            return visit_FloatNode(node, ctx);
        case NODE_STRING:
            return visit_StringNode(node, ctx);
        case NODE_UNARY_OP:
           return visit_UnaryOpNode(node, ctx);
        case NODE_BINARY_OP:
            return visit_BinOpNode(node, ctx);
        case NODE_VAR_DECLARATION:
            return visit_VarDeclarationNode(node, ctx);
        case NODE_VAR_ASSIGN:
            return visit_VarAssignNode(node, ctx);
        case NODE_VAR_ACCESS:
            return visit_VarAccessNode(node, ctx);
        case NODE_IF:
            return visit_IfNode(node, ctx);
        case NODE_FOR:
            return visit_ForNode(node, ctx);
        case NODE_WHILE:
            return visit_WhileNode(node, ctx);
        case NODE_FUNCTION_DEF:
            return visit_FunctionDefNode(node, ctx);
        case NODE_FUNCTION_CALL:
            return visit_FunctionCallNode(node, ctx);
        case NODE_RETURN:
            return visit_ReturnNode(node, ctx);
        case NODE_BREAK:
            return visit_BreakNode(node, ctx);
        case NODE_CONTINUE:
            return visit_ContinueNode(node, ctx);
        case NODE_STRUCT_DEF:
            return visit_StructDefNode(node, ctx);
        case NODE_CONSTRUCTOR_CALL:
            return visit_ConstructorCallNode(node, ctx);
        case NODE_ATTRIBUTE_ACCESS:
            return visit_AttributeAccessNode(node, ctx);
        case NODE_INDEX_ACCESS:
            return visit_IndexAccessNode(node, ctx);
        case NODE_ATTRIBUTE_ASSIGN:
            return visit_AttributeAssignNode(node, ctx);
        case NODE_LIST:
            return visit_ListNode(node, ctx);
        default:
            throw Exception("No visit_" + std::to_string(node->type) + " method defined.");
        }
        return Null_sPtr;
    }

    Object_sPtr visit_VectorWrapperNode(AstNode node, Context& ctx) {
        std::vector<AstNode> v = std::static_pointer_cast<VectorWrapperNode>(node)->vec;
        for (AstNode a : v) {
            visit(a, ctx);
            if (this->should_return) {
                return this->return_value;
            }
            else if (this->should_break || this->should_continue) {
                return Null_sPtr;
            }
        }

        return Null_sPtr;
    }

    Object_sPtr visit_IntNode(AstNode node, Context& ctx) {
        std::shared_ptr<IntNode> intNode = std::static_pointer_cast<IntNode>(node);
        return Object_sPtr(new Int(intNode->value));
    }

    Object_sPtr visit_FloatNode(AstNode node, Context& ctx) {
        std::shared_ptr<FloatNode> floatNode = std::static_pointer_cast<FloatNode>(node);
        return Object_sPtr(new Float(floatNode->value));
    }

    Object_sPtr visit_StringNode(AstNode node, Context& ctx) {
        std::shared_ptr<StringNode> strNode = std::static_pointer_cast<StringNode>(node);
        return Object_sPtr(new String(strNode->value));
    }

    Object_sPtr visit_VarDeclarationNode(AstNode node, Context& ctx) {
        std::shared_ptr<VarDeclarationNode> varNode = std::static_pointer_cast<VarDeclarationNode>(node);
        if (ctx.symbol_table->containsLocalKey(varNode->varName)) {
            throw Exception("'" + varNode->varName + "' is already in scope.");
        }

        Object_sPtr value = visit(varNode->exprNode, ctx);
        Object_sPtr varWrapper = Object_sPtr(new VariableWrapper(value, varNode->isConstant));
        ctx.symbol_table->addLocal(varNode->varName, varWrapper);
        return value;
    }

    Object_sPtr visit_VarAssignNode(AstNode node, Context& ctx) {
        std::shared_ptr<VarAssignNode> varNode = std::static_pointer_cast<VarAssignNode>(node);
        if (!ctx.symbol_table->containsKeyAnywhere(varNode->varName)) {
            throw Exception("'" + varNode->varName + "' has not been declared.");
        }

        Object_sPtr varWrapper = ctx.symbol_table->get(varNode->varName);
        if (varWrapper->isConstant()) {
            throw Exception("Value cannot be reassigned. Variable '" + varNode->varName + "' is declared as constant.");
        }

        Object_sPtr value = visit(varNode->exprNode, ctx);
        varWrapper->storeObject(value);
        return value;
    }

    Object_sPtr visit_VarAccessNode(AstNode node, Context& ctx) {
        std::shared_ptr<VarAccessNode> varNode = std::static_pointer_cast<VarAccessNode>(node);
        if (!ctx.symbol_table->containsKeyAnywhere(varNode->varName)) {
            throw Exception("'" + varNode->varName + "' has not been declared.");
        }

        Object_sPtr varWrapper = ctx.symbol_table->get(varNode->varName);
        return varWrapper->getObject();
    }

    Object_sPtr visit_UnaryOpNode(AstNode node, Context& ctx) {
        std::shared_ptr<UnaryOpNode> unaryOpNode = std::static_pointer_cast<UnaryOpNode>(node);
        Object_sPtr res = visit(unaryOpNode->exprNode, ctx);

        if (unaryOpNode->op.compare("-") == 0) {
            res = res->mul(Object_sPtr(new Int(-1)));
        }
        else if (unaryOpNode->op.compare("!") == 0) {
            res = res->notted();
        }
        return res;
    }

    Object_sPtr visit_BinOpNode(AstNode node, Context& ctx) {
        std::shared_ptr<BinOpNode> binOpNode = std::static_pointer_cast<BinOpNode>(node);

        Object_sPtr left = visit(binOpNode->left, ctx);
        Object_sPtr right = visit(binOpNode->right, ctx);

        Object_sPtr res = Null_sPtr;
        if (binOpNode->op.compare("+") == 0) {
            res = left->add(right);
        }
        else if (binOpNode->op.compare("-") == 0) {
            res = left->sub(right);
        }
        else if (binOpNode->op.compare("*") == 0) {
            res = left->mul(right);
        }
        else if (binOpNode->op.compare("/") == 0) {
            res = left->div(right);
        }
        else if (binOpNode->op.compare("^") == 0) {
            res = left->pow(right);
        }
        else if (binOpNode->op.compare("%") == 0) {
            res = left->mod(right);
        }
        else if (binOpNode->op.compare("<") == 0) {
            res = left->compare_lt(right);
        }
        else if (binOpNode->op.compare(">") == 0) {
            res = left->compare_gt(right);
        }
        else if (binOpNode->op.compare("<=") == 0) {
            res = left->compare_lte(right);
        }
        else if (binOpNode->op.compare(">=") == 0) {
            res = left->compare_gte(right);
        }
        else if (binOpNode->op.compare("==") == 0) {
            res = left->compare_ee(right);
        }
        else if (binOpNode->op.compare("!=") == 0) {
            res = left->compare_ne(right);
        }
        else if (binOpNode->op.compare("&&") == 0) {
            res = left->anded_by(right);
        }
        else if (binOpNode->op.compare("||") == 0) {
            res = left->ored_by(right);
        }
        else {
            res = left->pow(right);
        }

        return res;
    }

    Object_sPtr visit_IfNode(AstNode node, Context& ctx) {
        std::shared_ptr<IfNode> ifNode = std::static_pointer_cast<IfNode>(node);
        Context newCtx = ctx.generateNewContext("If statement in " + ctx.name);

        for (int i = 0; i < (int)ifNode->caseConditions.size(); i++) {
            Object_sPtr cond = visit(ifNode->caseConditions.at(i), ctx);
            if (cond->is_true()) {
                Object_sPtr res = visit(AstNode(new VectorWrapperNode(ifNode->caseStatements.at(i))), newCtx);
                return res;
            }
        }

        AstNode vw = AstNode(new VectorWrapperNode(ifNode->elseCaseStatements));
        Object_sPtr res = visit(vw, newCtx);
        return res;
    }

    Object_sPtr visit_ForNode(AstNode node, Context& ctx) {
        std::shared_ptr<ForNode> forNode = std::static_pointer_cast<ForNode>(node);
        Context initCtx = ctx.generateNewContext("For loop initializer");
        visit(forNode->initStatement, initCtx);

        while (visit(forNode->condNode, initCtx)->is_true()) {
            Context iterCtx = initCtx.generateNewContext("For loop iteration");
            visit(AstNode(new VectorWrapperNode(forNode->statements)), iterCtx);
            if (this->should_break) {
                this->should_break = false;
                break;
            }
            else if (this->should_continue) {
                this->should_continue = false;
            }
            visit(forNode->updateStatement, iterCtx);
        }

        return Null_sPtr;
    }

    Object_sPtr visit_WhileNode(AstNode node, Context& ctx) {
        std::shared_ptr<WhileNode> whileNode = std::static_pointer_cast<WhileNode>(node);

        while (visit(whileNode->condNode, ctx)->is_true()) {
            Context iterCtx = ctx.generateNewContext("While loop iteration");
            visit(AstNode(new VectorWrapperNode(whileNode->statements)), iterCtx);
            if (this->should_break) {
                this->should_break = false;
                break;
            }
            else if (this->should_continue) {
                this->should_continue = false;
            }
        }

        return Null_sPtr;
    }

    Object_sPtr visit_FunctionDefNode(AstNode node, Context& ctx) {
        std::shared_ptr<FunctionDefNode> funDefNode = std::static_pointer_cast<FunctionDefNode>(node);
        if (ctx.symbol_table->containsLocalKey(funDefNode->name)) {
            throw Exception("Cannot define function. '" + funDefNode->name + "' is already in scope.");
        }

        Object_sPtr newFunction(new Function(funDefNode->name, funDefNode->argNames, funDefNode->statements));
        Object_sPtr varWrapper = Object_sPtr(new VariableWrapper(newFunction, false));
        ctx.symbol_table->addLocal(funDefNode->name, varWrapper);
        return newFunction;
    }

    Object_sPtr visit_FunctionCallNode(AstNode node, Context& ctx) {
        std::shared_ptr<FunctionCallNode> funCallNode = std::static_pointer_cast<FunctionCallNode>(node);
        std::shared_ptr<Function> functionObj = std::static_pointer_cast<Function>(visit(funCallNode->nodeToCall, ctx));
        functionObj->isCallable();
        functionObj->checkNumArgs(funCallNode->argNodes);

        Context funCtx = ctx.generateNewContext("Function '" + functionObj->name + "'");
        for (int i = 0; i < (int)functionObj->argNames.size(); i++) {
            Object_sPtr arg_value = visit(funCallNode->argNodes.at(i), ctx);
            Object_sPtr varWrapper = Object_sPtr(new VariableWrapper(arg_value));
            funCtx.symbol_table->addLocal(functionObj->argNames.at(i), varWrapper);
        }

        if (functionObj->isBuiltIn()) {
            return functionObj->executeWrapper(&funCtx);
        }

        visit(AstNode(new VectorWrapperNode(functionObj->statements)), funCtx);

        if (this->should_return) {
            Object_sPtr retValue = this->return_value;
            this->return_value = Null_sPtr;
            this->should_return = false;
            return retValue;
        }

        return Null_sPtr;
    }

    Object_sPtr visit_ReturnNode(AstNode node, Context& ctx) {
        std::shared_ptr<ReturnNode> returnNode = std::static_pointer_cast<ReturnNode>(node);
        if (returnNode->exprNode != nullptr) {
            this->return_value = visit(returnNode->exprNode, ctx);
        }
        this->should_return = true;
        return Null_sPtr;
    }

    Object_sPtr visit_BreakNode(AstNode node, Context& ctx) {
        this->should_break = true;
        return Null_sPtr;
    }

    Object_sPtr visit_ContinueNode(AstNode node, Context& ctx) {
        this->should_continue = true;
        return Null_sPtr;
    }

    Object_sPtr visit_StructDefNode(AstNode node, Context& ctx) {
        std::shared_ptr<StructureDefNode> structDefNode = std::static_pointer_cast<StructureDefNode>(node);

        if (ctx.symbol_table->containsKeyAnywhere(structDefNode->name)) {
            throw Exception("Struct '" + structDefNode->name + "' is already defined.");
        }

        std::shared_ptr<StructureDefinition> newClass = std::shared_ptr<StructureDefinition>(new StructureDefinition(structDefNode->name));
        Object_sPtr varWrapper = Object_sPtr(new VariableWrapper(newClass, true));
        ctx.symbol_table->addLocal(structDefNode->name, varWrapper);

        for (AstNode a : structDefNode->statements) {
            if (a->type == NODE_VAR_DECLARATION) {
                std::shared_ptr<VarDeclarationNode> varNode = std::static_pointer_cast<VarDeclarationNode>(a);
                newClass->addField(varNode->varName, Object_sPtr(new VariableWrapper(visit(varNode->exprNode, ctx), false)));
            }
            else if (a->type == NODE_FUNCTION_DEF) {
                std::shared_ptr<FunctionDefNode> funDefNode = std::static_pointer_cast<FunctionDefNode>(a);
                Object_sPtr newFunction(new Function(funDefNode->name, funDefNode->argNames, funDefNode->statements));
                newClass->addField(funDefNode->name, Object_sPtr(new VariableWrapper(newFunction, false)));
            }
            else {
                throw Exception(a->type + " cannot be used in a structure definition.");
            }
        }

        return newClass;
    }

    Object_sPtr visit_ConstructorCallNode(AstNode node, Context& ctx) {
        std::shared_ptr<ConstructorCallNode> constructorCallNode = std::static_pointer_cast<ConstructorCallNode>(node);
        Object_sPtr structureDef = visit(constructorCallNode->structureNode, ctx);
        return structureDef->createInstance();
    }

    Object_sPtr visit_AttributeAccessNode(AstNode node, Context& ctx) {
        std::shared_ptr<AttributeAccessNode> attrAccessNode = std::static_pointer_cast<AttributeAccessNode>(node);
        Object_sPtr structure = visit(attrAccessNode->exprNode, ctx);

        Object_sPtr varWrapper = structure->getField(attrAccessNode->name);
        Object_sPtr obj = varWrapper->getObject();
        return varWrapper->getObject();
    }

    Object_sPtr visit_IndexAccessNode(AstNode node, Context& ctx) {
        std::shared_ptr<IndexAccessNode> indexAccessNode = std::static_pointer_cast<IndexAccessNode>(node);
        Object_sPtr list = visit(indexAccessNode->node, ctx);
        Object_sPtr index = visit(indexAccessNode->indexNode, ctx);
        
        return list->getIndex(index);
    }

    Object_sPtr visit_AttributeAssignNode(AstNode node, Context& ctx) {
        std::shared_ptr<AttributeAssignNode> attrAssignNode = std::static_pointer_cast<AttributeAssignNode>(node);
        std::shared_ptr<AttributeAccessNode> attrAccessNode = std::static_pointer_cast<AttributeAccessNode>(attrAssignNode->attrNode);
        
        Object_sPtr obj = visit(attrAccessNode->exprNode, ctx);

        Object_sPtr varWrapper = obj->getField(attrAccessNode->name);
        Object_sPtr value = visit(attrAssignNode->exprNode, ctx);
        varWrapper->storeObject(value);

        return value;
    }

    Object_sPtr visit_ListNode(AstNode node, Context& ctx) {
        std::shared_ptr<ListNode> listNode = std::static_pointer_cast<ListNode>(node);
        Object_sPtr listObj = Object_sPtr(new List());

        for (AstNode n : listNode->listValueNodes) {
            listObj->add(visit(n, ctx));
        }

        return listObj;
    }
};