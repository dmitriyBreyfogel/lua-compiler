#pragma once

#include "ast/expression/ExpressionNode.h"

namespace lua::ast {

class IdentifierExpression : public ExpressionNode {
public:
    explicit IdentifierExpression(std::string name);
};

class VarargExpression : public ExpressionNode {
public:
    explicit VarargExpression(std::string name = {});
};

class UnaryExpression : public ExpressionNode {
public:
    explicit UnaryExpression(std::string operation);

    const Node* operand() const { return child("operand"); }
};

class BinaryExpression : public ExpressionNode {
public:
    explicit BinaryExpression(std::string operation);

    const Node* left() const { return child("left"); }
    const Node* right() const { return child("right"); }
};

class IndexExpression : public ExpressionNode {
public:
    IndexExpression();

    const Node* object() const { return child("object"); }
    const Node* index() const { return child("index"); }
};

class FieldExpression : public ExpressionNode {
public:
    explicit FieldExpression(std::string kind = {});

    const Node* object() const { return child("object"); }
    const Node* field() const { return child("field"); }
};

class CallExpression : public ExpressionNode {
public:
    CallExpression();

    const Node* function() const { return child("function"); }
    const Node* arguments() const { return child("arguments"); }
};

class MethodCallExpression : public ExpressionNode {
public:
    explicit MethodCallExpression(std::string method);

    const Node* object() const { return child("object"); }
    const Node* arguments() const { return child("arguments"); }
};

class TableConstructorExpression : public ExpressionNode {
public:
    TableConstructorExpression();

    const Node* fields() const { return child("fields"); }
};

class TableFieldExpression : public ExpressionNode {
public:
    TableFieldExpression();

    const Node* key() const { return child("key"); }
    const Node* value() const { return child("value"); }
};

class FunctionExpression : public ExpressionNode {
public:
    FunctionExpression();

    const Node* parameters() const { return child("parameters"); }
    const Node* body() const { return child("body"); }
};

} // namespace lua::ast
