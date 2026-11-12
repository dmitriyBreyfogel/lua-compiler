#pragma once

#include "ast/expression/ExpressionNode.h"

namespace lua::ast {

class LiteralExpression : public ExpressionNode {
protected:
    LiteralExpression(NodeType type, std::string value = {});
};

class IntegerLiteralExpression : public LiteralExpression {
public:
    explicit IntegerLiteralExpression(long long value);

    long long integerValue() const;

private:
    long long integerValue_;
};

class FloatLiteralExpression : public LiteralExpression {
public:
    explicit FloatLiteralExpression(double value);

    double floatValue() const;

private:
    double floatValue_;
};

class StringLiteralExpression : public LiteralExpression {
public:
    explicit StringLiteralExpression(std::string value);

    const std::string& text() const;
};

class BooleanLiteralExpression : public LiteralExpression {
public:
    explicit BooleanLiteralExpression(bool value);

    bool booleanValue() const;

private:
    bool booleanValue_;
};

class NilLiteralExpression : public LiteralExpression {
public:
    NilLiteralExpression();
};

} // namespace lua::ast
