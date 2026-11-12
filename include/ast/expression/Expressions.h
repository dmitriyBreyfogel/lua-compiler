#pragma once

#include "ast/expression/ExpressionNode.h"

namespace lua::ast {

class LiteralExpression : public ExpressionNode {
public:
    LiteralExpression(NodeType type, std::string value = {});
};

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
};

class BinaryExpression : public ExpressionNode {
public:
    explicit BinaryExpression(std::string operation);
};

class IndexExpression : public ExpressionNode {
public:
    IndexExpression();
};

class FieldExpression : public ExpressionNode {
public:
    explicit FieldExpression(std::string kind = {});
};

class CallExpression : public ExpressionNode {
public:
    CallExpression();
};

class MethodCallExpression : public ExpressionNode {
public:
    explicit MethodCallExpression(std::string method);
};

class TableConstructorExpression : public ExpressionNode {
public:
    TableConstructorExpression();
};

class TableFieldExpression : public ExpressionNode {
public:
    TableFieldExpression();
};

class FunctionExpression : public ExpressionNode {
public:
    FunctionExpression();
};

} // namespace lua::ast
